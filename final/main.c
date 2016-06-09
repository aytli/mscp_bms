// BPMS final code
// Authors: Andy Li, Stefan Dumitrescu, Mhamad Salih
// Copyright 2016, McMaster Solar Car Project
// Monitors voltage and performs charge balancing for 30 cells

// Includes
#include "main.h"
#include "stdlib.h"
#include "math.h"
#include "pec.c"
#include "ltc6804.c"
#include "adc.c"
#include "fan.c"
#include "lcd.c"
#include "hall_sensor.c"
#include "eeprom.c"
#include "can_telem.h"
#include "can_PIC24.c"

// PIC internal register addresses
#word IFS0 = 0x0084
#word TRISF = 0x02DE

// Macros to disable timers and clear flags
#define CLEAR_T2_FLAG IFS0  &= 0xFF7F

// Telemetry packet IDs
#define VOLTAGE_ID  0x5A
#define TEMP_ID     0x69
#define BALANCE_ID  0x41
#define CURRENT_ID  0xE7
#define STATUS_ID   0x77

// Kilovac control
#define KILOVAC_ON        \
    gb_connected = true;  \
    output_high(RX_LED);  \
    output_high(KVAC_PIN);

#define KILOVAC_OFF       \
    gb_connected = false; \
    output_low(RX_LED);   \
    output_low(KVAC_PIN);

// Protection limits
#define VOLTAGE_MAX            42000 // 4.20V
#define VOLTAGE_MIN            27500 // 2.75V
#define TEMP_WARNING              60 // 60°C, actual value to be determined
#define TEMP_CRITICAL             70 // 70°C, actual value to be determined
#define CURRENT_DISCHARGE_LIMIT 3137 // -20A, actual value to be determined
#define CURRENT_CHARGE_LIMIT    2758 // +10A, actual value to be determined

// Status LED blink period
#define HEARTBEAT_PERIOD_MS 500

// Telemetry data sending period
#define TELEMETRY_PERIOD_MS 200

// Balancing discharge period
#define BALANCE_PERIOD_MS  2000

// Voltage threshold for balancing to occur (BALANCE_THRESHOLD / 10) mV
#define BALANCE_THRESHOLD   500

// Timeout period for PMS response
#define PMS_RESPONSE_TIMEOUT_MS 1000

// Timeout period for the balancing command
#define BALANCING_TIMEOUT_MS 500

// MPPT turn off time
#define MPPT_DELAY_MS 10

// CAN bus defines
#define TX_PRI 3
#define TX_EXT 0
#define TX_RTR 0

#define CAN_SEND_DATA_PACKET(i) \
    can_putd(g_can_id[i],gp_can_data_address[i],g_can_len[i],TX_PRI,TX_EXT,TX_RTR)

// Creates an array of CAN packet IDs
static int16 g_can_id[N_CAN_ID] =
{
    CAN_ID_TABLE(EXPAND_AS_CAN_ID_ARRAY)
};

// Creates an array of CAN packet lengths
static int16 g_can_len[N_CAN_ID] =
{
    CAN_ID_TABLE(EXPAND_AS_CAN_LEN_ARRAY)
};

// Telemetry data pages
TELEM_ID_TABLE(EXPAND_AS_TELEM_PAGE_DECLARATIONS)

// Creates an array of CAN packet addresses
static int * gp_can_data_address[N_CAN_ID] =
{
    CAN_ID_TABLE(EXPAND_AS_DATA_ADDRESS_ARRAY)
};

static cell_t         g_cell[N_CELLS];
static temperature_t  g_temperature[N_ADC_CHANNELS];
static current_t      g_current;
static int            g_highest_voltage_cell_index;
static int            g_lowest_voltage_cell_index;
static int            g_highest_temperature_cell_index;
static unsigned int16 g_highest_voltage;
static unsigned int16 g_lowest_voltage;
static float          g_highest_temperature;
static int1           gb_connected;
static int1           gb_balance_enable;
static int1           gb_pms_response_received;
static bps_state      g_state;

// Initializes the cells, clears all flags, resets highest and lowest cells
void main_init(void)
{
    int i;
    for (i = 0 ; i < N_CELLS ; i++)
    {
        g_cell[i].voltage = 0;
    }
    g_current.average = 0;
    g_highest_voltage_cell_index = 0;
    g_lowest_voltage_cell_index = 0;
    g_highest_temperature_cell_index = 0;
    g_highest_voltage = 0;
    g_lowest_voltage = 0;
    g_highest_temperature = 0;
    gb_connected = false;
    g_state = SAFETY_CHECK;
}

// Returns the index for the highest voltage cell
int get_highest_voltage_cell_index(void)
{
    int i;
    int highest = 0;
    for (i = 0 ; i < N_CELLS ; i++)
    {
        if (g_cell[i].voltage >= g_cell[highest].average_voltage)
        {
            highest = i;
        }
    }
    return highest;
}

// Returns the index for the lowest voltage cell
int get_lowest_voltage_cell_index(void)
{
    int i;
    int lowest = 0;
    for (i = 0 ; i < N_CELLS ; i++)
    {
        if (g_cell[i].average_voltage <= g_cell[lowest].average_voltage)
        {
            lowest = i;
        }
    }
    return lowest;
}

int get_highest_temperature_cell_index(void)
{
    int i;
    int highest = 0;
    for (i = 0 ; i < N_ADC_CHANNELS ; i++)
    {
        if (g_temperature[i].average >= g_temperature[highest].average)
        {
            highest = i;
        }
    }
    return highest;
}

// Use the simplified Steinhart-Hart equation to approximate temperatures
void convert_adc_data_to_temps(void)
{
    int i;
    float resistance;
    float temperature;
    for (i = 0; i < N_ADC_CHANNELS; i++)
    {
        resistance = THERMISTOR_SERIES * (float)g_temperature[i].average /
           (LSBS_PER_VOLT * THERMISTOR_SUPPLY - (float)g_temperature[i].average);
        temperature = resistance / THERMISTOR_NOMINAL;
        temperature = log(temperature);
        temperature /= B_COEFF;
        temperature += 1.0 / (TEMPERATURE_NOMINAL + 273.15);
        temperature = 1.0 / temperature;
        temperature -= 273.15;
        g_temperature[i].converted = temperature;
    }
}

void disable_balancing(void)
{
    g_discharge1 = 0x000;
    g_discharge2 = 0x000;
    g_discharge3 = 0x000;
    output_low(CSBI1);
    ltc6804_write_config(g_discharge1);
    output_high(CSBI1);
    output_low(CSBI2);
    ltc6804_write_config(g_discharge2);
    output_high(CSBI2);
    output_low(CSBI3);
    ltc6804_write_config(g_discharge3);
    output_high(CSBI3);
}

void average_voltage(void)
{
    int i;
    int j;
    unsigned int32 sum;
    for (i = 0 ; i < N_CELLS ; i++)
    {
        sum = 0;
        for (j = 0 ; j < N_VOLTAGE_SAMPLES-1 ; j++)
        {
            sum += g_cell[i].samples[j];
            g_cell[i].samples[j] = g_cell[i].samples[j+1];
        }
        sum += g_cell[i].voltage;
        g_cell[i].samples[N_VOLTAGE_SAMPLES-1] = g_cell[i].voltage;
        g_cell[i].average_voltage = (unsigned int16) (sum/N_VOLTAGE_SAMPLES);
    }
}

void average_temperature(void)
{
    int i;
    int j;
    unsigned int32 sum;
    for (i = 0 ; i < N_ADC_CHANNELS ; i++)
    {
        sum = 0;
        for (j = 0 ; j < N_TEMPERATURE_SAMPLES-1 ; j++)
        {
            sum += g_temperature[i].samples[j];
            g_temperature[i].samples[j] = g_temperature[i].samples[j+1];
        }
        sum += g_temperature[i].raw;
        g_temperature[i].samples[N_TEMPERATURE_SAMPLES-1] = g_temperature[i].raw;
        g_temperature[i].average = (unsigned int16) (sum/N_TEMPERATURE_SAMPLES);
    }
}

void average_current(void)
{
    int i;
    unsigned int32 sum = 0;
    for (i = 0 ; i < N_CURRENT_SAMPLES-1 ; i++)
    {
        sum += g_current.samples[i];
        g_current.samples[i] = g_current.samples[i+1];
    }
    sum += g_current.raw;
    g_current.samples[N_CURRENT_SAMPLES-1] = g_current.raw;
    g_current.average = (unsigned int16) (sum/N_CURRENT_SAMPLES);
}

void update_voltage_data(void)
{
    int i;
    for (i = 0 ; i < N_CELLS ; i ++)
    {
        g_bps_voltage_page[2*i]   = (int8)(g_cell[i].average_voltage >> 8);
        g_bps_voltage_page[2*i+1] = (int8)(g_cell[i].average_voltage & 0x00FF);
    }
}

void update_temperature_data(void)
{
    int i;
    for (i = 0 ; i < N_ADC_CHANNELS ; i++)
    {
        g_bps_temperature_page[i] = (unsigned int8) (g_temperature[i].converted);
    }
}

void update_current_data(void)
{
    g_bps_current_page[0] = (int8) ((g_current.average>>8)&0xFF);
    g_bps_current_page[1] = (int8) (g_current.average&0xFF);
}

void update_balancing_bits(void)
{
    int32 discharge = ((((int32)(g_discharge1))<< 0)&0x00000FFF)
                     |((((int32)(g_discharge2))<<12)&0x00FFF000)
                     |((((int32)(g_discharge3))<<24)&0x3F000000);
    g_bps_balancing_page[0] = (int8) (((int32)(discharge>> 24))&0xFF);
    g_bps_balancing_page[1] = (int8) (((int32)(discharge>> 16))&0xFF);
    g_bps_balancing_page[2] = (int8) (((int32)(discharge>>  8))&0xFF);
    g_bps_balancing_page[3] = (int8) (((int32)(discharge>>  0))&0xFF);
}

void update_pack_status(void)
{
    g_bps_status_page[0] = gb_connected;
}

int1 check_voltage(void)
{
    // Find highest and lowest cell voltages
    ltc6804_read_cell_voltages(g_cell);
    average_voltage();
    g_highest_voltage_cell_index = get_highest_voltage_cell_index();
    g_lowest_voltage_cell_index = get_lowest_voltage_cell_index();
    g_highest_voltage = g_cell[g_highest_voltage_cell_index].average_voltage;
    g_lowest_voltage = g_cell[g_lowest_voltage_cell_index].average_voltage;
    
    if (g_highest_voltage >= VOLTAGE_MAX)
    {
        // over voltage protection
        // shut off pack and write OV error and cell id to eeprom
        eeprom_set_ov_error((int8)(g_highest_voltage_cell_index));
        return 0;
    }
    else if (g_lowest_voltage <= VOLTAGE_MIN)
    {
        // under voltage protection
        // shut off pack and write UV error and cell if to eeprom
        eeprom_set_uv_error((int8)(g_lowest_voltage_cell_index));
        return 0;
    }
    else
    {
        // cell voltages are fine
        return 1;
    }
}

int1 check_temperature(void)
{
    // Find highest temperature reading
    ads7952_read_all_channels(g_temperature);
    average_temperature();
    convert_adc_data_to_temps();
    g_highest_temperature_cell_index = get_highest_temperature_cell_index();
    g_highest_temperature = g_temperature[g_highest_temperature_cell_index].converted;
    
    if (g_highest_temperature >= TEMP_CRITICAL)
    {
        // temperature discharge protection
        // shut off pack, write OT error and cell id to eeprom
        fan_set_speed(FAN_MAX);
        eeprom_set_ot_error((int8)(g_highest_temperature_cell_index));
        return 0;
    }
    else if (g_highest_temperature >= TEMP_WARNING)
    {
        // temperature charge protection
        // disable charging
        //if (motor controller and mppt are connected)
        if (true)
        {
            //turn off array, tell motor controller to disable regen
            fan_set_speed(FAN_HIGH);
            eeprom_set_ot_error((int8)(g_highest_temperature_cell_index));
            return 0;
        }
        else
        {
            return 1;
        }
    }
    else
    {
        // cell temperatures are fine
        return 1;
    }
}

int1 check_current(void)
{
    // Read the pack current
    g_current.raw = hall_sensor_read_data();
    average_current();
    
    if (g_current.average >= CURRENT_DISCHARGE_LIMIT)
    {
        // discharge current protection
        // shut off pack, write OC error to eeprom
        eeprom_set_current_error(OC_ERROR);
        return 0;
    }
    else if (g_current.average <= CURRENT_CHARGE_LIMIT)
    {
        // charge current protection
        // shut off pack, write UC error to eeprom
        eeprom_set_current_error(UC_ERROR);
        return 0;
    }
    else
    {
        // current is fine
        return 1;
    }
}

// Reads error data from the eeprom and displays it on the lcd
void display_errors(void)
{
    char str[3];
    unsigned int8 errors[N_ERROR_BYTES];
    eeprom_read(errors);
    
    lcd_init();
    
    lcd_set_cursor_position(0,0);
    lcd_write("OV: ");
    itoa(errors[0],10,str);
    lcd_write(str);
    
    lcd_set_cursor_position(1,0);
    lcd_write("UV: ");
    itoa(errors[1],10,str);
    lcd_write(str);
    
    lcd_set_cursor_position(2,0);
    lcd_write("OT: ");
    itoa(errors[2],10,str);
    lcd_write(str);
    
    lcd_set_cursor_position(3,0);
    lcd_write("CURRENT: ");
    itoa(errors[3],10,str);
    lcd_write(str);
    
    // Clear the eeprom after errors are read
    eeprom_clear();
}

// Timer 2 blinks heartbeat LED, checks for status of LCD
#int_timer2 level = 4
void isr_timer2(void)
{
    static int i;
    static int1 b_lcd_connected = false;
    
    output_toggle(STATUS);
    
    // If the LCD is connected, display errors
    if ((input_state(LCD_SIG) == 1) && (b_lcd_connected == false))
    {
        // LCD connected, debounce the pin
        for (i = 0 ; i < LCD_DEBOUNCE_MS ; i++)
        {
            delay_ms(1);
        }
        
        if (input_state(LCD_SIG) == 1)
        {
            // LCD still connected, set flag to true, read and display errors
            b_lcd_connected = true;
            display_errors();
        }
    }
    else if (input_state(LCD_SIG) == 0)
    {
        // LCD not connected, clear flag
        b_lcd_connected = false;
    }
}

// Timer 4 sends telemetry data over CANbus
#int_timer4 level = 4
void isr_timer4(void)
{
    static int ms = 0;
    static int i = 0;
    
    if ((ms >= TELEMETRY_PERIOD_MS) && can_tbe())
    {
        ms = 0;
        output_toggle(TX_LED);
        
        // Update telemetry data pages
        update_voltage_data();
        update_temperature_data();
        update_current_data();
        update_balancing_bits();
        update_pack_status();
        
        // Send a packet of CAN data
        CAN_SEND_DATA_PACKET(i);
        if (i == (N_CAN_ID-1))
        {
            i = 0;
        }
        else
        {
            i++;
        }
    }
    else
    {
        ms++;
    }
}

// C1RX triggers when data is received on the CAN bus
#int_c1rx
void isr_c1rx(void)
{
    struct rx_stat rxstat;
    int32 rx_id;
    int in_data[8];
    int8 rx_len;
    
    if (can_getd(rx_id, in_data, rx_len, rxstat))
    {
        // Data was received, raise a flag corresponding to the data received
        switch(rx_id)
        {
            case COMMAND_ENABLE_BALANCING_ID:
                gb_balance_enable = true;
                break;
            case COMMAND_PMS_DISCONNECT_ARRAY_ID:
                gb_pms_response_received = true;
                break;
            default:
                break;
        }
    }
}

void safety_check_state(void)
{
    int1 b_success = true;
    
    b_success &= check_voltage();
    b_success &= check_temperature();
    b_success &= check_current();
    
    if (b_success == true)
    {
        if (gb_balance_enable == true)
        {
            // All parameters within safe range, balance the cells
            gb_balance_enable = false;
            g_state = BEGIN_BALANCE;
        }
        else
        {
            // Balancing disabled, continue monitoring cell status
            g_state = SAFETY_CHECK;
        }
    }
    else
    {
        // Something went wrong, disconnect the array
        g_state = SEND_ARRAY_DISCONNECT;
    }
}

void begin_balance_state(void)
{
    int i;
    int min_idx = get_lowest_voltage_cell_index();

    /*for (i = 0 ; i < 3 ; i++)
    {
        if ((g_cell[i].average_voltage - g_cell[min_idx].average_voltage)
            > BALANCE_THRESHOLD)
        {
            g_discharge1 |= 1 << i;
        }
        else
        {
            g_discharge1 &= ~(1 << i);
        }
    }
    
    // hardcoded to 0 for safety
    g_discharge1 = 0x000;
    g_discharge2 = 0x000;
    g_discharge3 = 0x000;*/
    
    for (i = 0 ; i < 12 ; i++)
    {
        if ((g_cell[i].average_voltage - g_cell[min_idx].average_voltage)
            > BALANCE_THRESHOLD)
        {
            g_discharge1 |= 1 << i;
        }
        else
        {
            g_discharge1 &= ~(1 << i);
        }
    }

    for (i = 12 ; i < 24 ; i++)
    {
        if ((g_cell[i].average_voltage - g_cell[min_idx].average_voltage)
            > BALANCE_THRESHOLD)
        {
            g_discharge2 |= 1 << (i - 12);
        }
        else
        {
            g_discharge2 &= ~(1 << (i - 12));
        }
    }
    
    for (i = 24 ; i < 30 ; i++)
    {
        if ((g_cell[i].average_voltage - g_cell[min_idx].average_voltage)
            > BALANCE_THRESHOLD)
        {
            g_discharge3 |= 1 << (i - 24);
        }
        else
        {
            g_discharge3 &= ~(1 << (i - 24));
        }
    }

    /*output_low(CSBI1);
    ltc6804_write_config(g_discharge1);
    output_high(CSBI1);
    output_low(CSBI2);
    ltc6804_write_config(g_discharge2);
    output_high(CSBI2);
    output_low(CSBI3);
    ltc6804_write_config(g_discharge3);
    output_high(CSBI3);*/
    
    g_state = BALANCING;
}

void balancing_state(void)
{
    static int balance_time_ms = 0;
    
    if (balance_time_ms >= BALANCE_PERIOD_MS)
    {
        // Balancing period over, disable balancing
        balance_time_ms = 0;
        disable_balancing();
        g_state = SAFETY_CHECK;
    }
    else
    {
        // Continue balancing
        delay_ms(1);
        balance_time_ms++;
        g_state = BALANCING;
    }
}

void send_array_disconnect_state(void)
{
    // Signal the PMS to disconnect the array, wait for response
    can_putd(COMMAND_PMS_DISCONNECT_ARRAY_ID,0,0,TX_PRI,TX_EXT,TX_RTR);
    g_state = PMS_RESPONSE_PENDING;
}

void pms_response_pending_state(void)
{
    static int timeout_ms = 0;
    
    if (timeout_ms >= PMS_RESPONSE_TIMEOUT_MS)
    {
        // Response timed out, proceed to disconnect pack
        g_state = DISCONNECT_PACK;
    }
    else if (gb_pms_response_received == true)
    {
        // Response received from PMS, disconnect the pack
        timeout_ms = 0;
        gb_pms_response_received = false;
        g_state = DISCONNECT_PACK;
    }
    else
    {
        // No timeout, no CAN packet, increment timeout counter
        delay_ms(1);
        timeout_ms++;
        g_state = PMS_RESPONSE_PENDING;
    }
}

void disconnect_pack_state(void)
{
    delay_ms(MPPT_DELAY_MS);
    KILOVAC_OFF;
}

// Main
void main()
{
    int i;
    struct rx_stat rxstat;
    int32 rx_id;
    int in_data[8];
    int8 rx_len;
    int8 timeout_ms;
    
    // Kilovac is initially disabled
    KILOVAC_OFF;
    
    // Set up and enable timer 2 with a period of HEARTBEAT_PERIOD_MS
    setup_timer2(TMR_INTERNAL|TMR_DIV_BY_256,39*HEARTBEAT_PERIOD_MS);
    enable_interrupts(INT_TIMER2);
    
    // Set up and enable timer 4 with a period of 1ms
    setup_timer4(TMR_INTERNAL|TMR_DIV_BY_256,39);
    enable_interrupts(INT_TIMER4);
    
    // Enable CAN receive interrupt
    enable_interrupts(INT_C1RX);
    
    main_init();
    ltc6804_init();
    ads7952_init();
    hall_sensor_init();
    fan_init();
    fan_set_speed(FAN_LOW);
    
    // Initialize CANbus, configure outputs, enable transfer buffers
    can_init();
    set_tris_f((*0x02DE&0xFFFD)|0x01); // set F0 to CANRX, F1 to CANTX
    for (i = 0 ; i < 8 ; i++)
    {
        can_enable_b_transfer(1);
    }
    
    // Populate running averages
    for (i = 0 ; i < N_VOLTAGE_SAMPLES ; i++)
    {
        ltc6804_read_cell_voltages(g_cell);
        average_voltage();
    }
    
    for (i = 0 ; i < N_TEMPERATURE_SAMPLES ; i++)
    {
        ads7952_read_all_channels(g_temperature);
        average_temperature();
    }
    
    for (i = 0 ; i < N_CURRENT_SAMPLES ; i++)
    {
        g_current.raw = hall_sensor_read_data();
        average_current();
    }
    
    // Perform startup test
    if ((check_voltage() & check_temperature() & check_current()) == true)
    {
        // Voltage, temperature, and current are all safe, connect the pack
        delay_ms(500);
        KILOVAC_ON;
    }
    else
    {
        // Something went wrong, do not connect the pack
        eeprom_write_errors();
        KILOVAC_OFF;
    }
    
    while (true)
    {
        switch(g_state)
        {
            case SAFETY_CHECK:
                safety_check_state();
                break;
            case BEGIN_BALANCE:
                begin_balance_state();
            case BALANCING:
                balancing_state();
                break;
            case SEND_ARRAY_DISCONNECT:
                send_array_disconnect_state();
                break;
            case PMS_RESPONSE_PENDING:
                pms_response_pending_state();
                break;
            case DISCONNECT_PACK:
                disconnect_pack_state();
                break;
            default:
                break;
        }
    }
}

