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
#include "lcd.c"
#include "hall_sensor.c"
#include "eeprom.c"
#include "can_telem.h"
#include "can_PIC24.c"

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
#define TEMP_WARNING              60 // 60°C charge limit
#define TEMP_CRITICAL             70 // 70°C discharge limit
#define DISCHARGE_LIMIT_AMPS      65 // Current discharge limit (exiting the pack)
#define CHARGE_LIMIT_AMPS         50 // Current charge limit (entering the pack)
#define CURRENT_DISCHARGE_LIMIT CURRENT_ZERO+(CURRENT_SLOPE*DISCHARGE_LIMIT_AMPS)
#define CURRENT_CHARGE_LIMIT    CURRENT_ZERO-(CURRENT_SLOPE*CHARGE_LIMIT_AMPS)

// Delay periods
#define HEARTBEAT_PERIOD_MS      500 // Status LED blink period
#define TELEMETRY_PERIOD_MS      200 // Telemetry data sending period
#define BALANCE_PERIOD_MS       2000 // Balancing discharge period
#define PMS_RESPONSE_TIMEOUT_MS 1000 // Timeout period for PMS response
#define BALANCING_TIMEOUT_MS     500 // Timeout period for the balancing command
#define MPPT_DELAY_MS             10 // MPPT turn off time
#define BLINKER_WAIT_TIME_MS      50 // Time the blinker needs to process the trip signal

// Misc defines
#define BALANCE_THRESHOLD        500 // Voltage threshold for balancing to occur (BALANCE_THRESHOLD / 10) mV
#define N_BAD_SAMPLES             30 // Number of bad data samples required to trip

// CAN bus defines
#define TX_PRI 3
#define TX_EXT 0
#define TX_RTR 0

// Sends a packet of telemetry data over CAN bus
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
static int1           gb_connected;
static int1           gb_balance_enable;
static int1           gb_pms_response_received;
static int1           gb_motor_connected;
static int1           gb_mppt_connected;
static bps_state_t    g_state;

// Initializes voltage and temperature error counts, current, and other flags
void main_init(void)
{
    int i;
    
    // Resets average voltages and error counts
    for (i = 0 ; i < N_CELLS ; i++)
    {
        g_cell[i].average_voltage  = 0;
        g_cell[i].ov_count         = 0;
        g_cell[i].uv_count         = 0;
    }
    
    // Resets average temperatures and error counts
    for (i = 0 ; i < N_ADC_CHANNELS ; i++)
    {
        g_temperature[i].average  = 0;
        g_temperature[i].ot_count = 0;
        g_temperature[i].wt_count = 0;
    }
    
    // Resets average current and error counts
    g_current.average  = 0;
    g_current.oc_count = 0;
    g_current.uc_count = 0;
    
    gb_connected = false;
    g_state = SAFETY_CHECK;
}

// Returns the index for the lowest voltage cell
int get_lowest_voltage_cell_index(void)
{
    int i;
    int lowest = 0;
    for (i = 0 ; i < N_CELLS ; i++)
    {
        if (g_cell[i].voltage <= g_cell[lowest].voltage)
        {
            lowest = i;
        }
    }
    return lowest;
}

// Use the simplified Steinhart-Hart equation to approximate temperatures
void convert_adc_data_to_temps(void)
{
    int i;
    float resistance;
    float temperature;
    for (i = 0; i < N_ADC_CHANNELS; i++)
    {
        resistance = THERMISTOR_SERIES * (float)g_temperature[i].raw /
           (LSBS_PER_VOLT * THERMISTOR_SUPPLY - (float)g_temperature[i].raw);
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
    for (i = 0 ; i < N_CELLS ; i++)
    {
        g_bps_voltage_page[i]   = (int8)(g_cell[i].voltage >> 8);
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

void update_cur_bal_stat_data(void)
{
    // Current, balancing bits, and pack status are stored in the same CAN packet and telemetry page
    
    // BPMS CAN heartbeat bit
    static int1 b_heartbeat = 0;
    
    // Update current data
    g_bps_cur_bal_stat_page[0] = (int8) ((g_current.raw>>8)&0xFF);
    g_bps_cur_bal_stat_page[1] = (int8) (g_current.raw&0xFF);
    
    // Update balancing bits
    int32 discharge = ((((int32)(g_discharge1))<< 0)&0x00000FFF)
                     |((((int32)(g_discharge2))<<12)&0x00FFF000)
                     |((((int32)(g_discharge3))<<24)&0x3F000000);
    g_bps_cur_bal_stat_page[2] = (int8) (((int32)(discharge>> 24))&0xFF);
    g_bps_cur_bal_stat_page[3] = (int8) (((int32)(discharge>> 16))&0xFF);
    g_bps_cur_bal_stat_page[4] = (int8) (((int32)(discharge>>  8))&0xFF);
    g_bps_cur_bal_stat_page[5] = (int8) (((int32)(discharge>>  0))&0xFF);
    
    // Update the pack status
    g_bps_cur_bal_stat_page[6] = gb_connected;
    
    // Update CAN heartbeat bit
    g_bps_cur_bal_stat_page[7] = b_heartbeat;
    b_heartbeat = !b_heartbeat;
}

int1 check_voltage(void)
{
    int i;
    
    // Read the cell voltages, compute a moving average of each cell voltage
    ltc6804_read_cell_voltages(g_cell);
    //average_voltage();
    
    for (i = 0 ; i < N_CELLS ; i++)
    {
        if (g_cell[i].voltage >= VOLTAGE_MAX)
        {
            // Voltage is too high, increment the OV count
            g_cell[i].ov_count++;
        }
        else if (g_cell[i].voltage <= VOLTAGE_MIN)
        {
            // Voltage is too low, increment the UV count
            g_cell[i].uv_count++;
        }
        else
        {
            // Voltage is within the safe operating range, clear OV and UV counts
            g_cell[i].ov_count = 0;
            g_cell[i].uv_count = 0;
        }
        
        if (g_cell[i].ov_count >= N_BAD_SAMPLES)
        {
            // Too many OV errors, write OV error to eeprom and return false
            eeprom_set_ov_error(i);
            output_high(STATUS);
            return 0;
        }
        else if (g_cell[i].uv_count >= N_BAD_SAMPLES)
        {
            // Too many UV errors, write UV error to eeprom and return false
            eeprom_set_uv_error(i);
            output_high(STATUS);
            return 0;
        }
        else
        {
            // The voltage is within the safe range, continue checking cells
        }
    }
    
    // All cells are within the safe range, return true
    return 1;
}

int1 check_temperature(void)
{
    int i;
    
    // Find highest temperature reading
    ads7952_read_all_channels(g_temperature);
    //average_temperature();
    convert_adc_data_to_temps();
    
    for (i = 0 ; i < N_ADC_CHANNELS ; i++)
    {
        if (g_temperature[i].converted >= TEMP_CRITICAL)
        {
            // Temperature is critical, increment the OT count
            g_temperature[i].ot_count++;
        }
        else if ((g_temperature[i].converted >= TEMP_WARNING)
                  && (gb_motor_connected == true) && (gb_mppt_connected == true))
        {
            // Temperature is above the warning threshold, motor and MPPT are connected
            // Increment the temperature warning count
            g_temperature[i].wt_count++;
        }
        else
        {
            // Temperature is within the safe range, clear the error counts
            g_temperature[i].ot_count = 0;
            g_temperature[i].wt_count = 0;
        }
        
        if (g_temperature[i].ot_count >= N_BAD_SAMPLES)
        {
            // Too many OT errors, write OT error to eeprom and return false
            eeprom_set_ot_error(i);
            return 0;
        }
        else if (g_temperature[i].wt_count >= N_BAD_SAMPLES)
        {
            // Too many temperature warning errors
            // PMS monitors BPS temperatures over CAN bus and controls the MPPT relay accordingly
        }
        else
        {
            // The temperature is within the safe range, continue checking
        }
    }
    
    // All temperature values are within the safe range, return true
    return 1;
}

int1 check_current(void)
{
    // Read the pack current
    g_current.raw = hall_sensor_read_data();
    //average_current();
    
    if (g_current.raw >= CURRENT_DISCHARGE_LIMIT)
    {
        // Current is above the allowed discharge limit
        g_current.oc_count++;
    }
    else if (g_current.raw <= CURRENT_CHARGE_LIMIT)
    {
        // Current is below the allowed charge limit
        g_current.uc_count++;
    }
    else
    {
        // current is fine
        g_current.oc_count = 0;
        g_current.uc_count = 0;
    }
    
    if (g_current.oc_count >= N_BAD_SAMPLES)
    {
        // Too many overcurrent errors, write OC error to eeprom, return false
        eeprom_set_current_error(OC_ERROR);
        return 0;
    }
    else if (g_current.uc_count >= N_BAD_SAMPLES)
    {
        // Too many undercurrent errors, write UC error to eeprom, return false
        eeprom_set_current_error(UC_ERROR);
        return 0;
    }
    else
    {
        // The current is within the safe range, return true
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
    switch(errors[3])
    {
        case OC_ERROR:
            lcd_write("OC");
            break;
        case UC_ERROR:
            lcd_write("UC");
            break;
        default:
            lcd_write("SUCCESS");
            break;
    }
}

// Timer 2 blinks heartbeat LED, checks for status of LCD
#int_timer2 level = 4
void isr_timer2(void)
{
    static int8 i;
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
    static int16 ms = 0;
    static int8  i = 0;
    
    if ((ms >= TELEMETRY_PERIOD_MS) && can_tbe())
    {
        ms = 0;
        output_toggle(TX_LED);
        
        // Update telemetry data pages
        update_voltage_data();
        update_temperature_data();
        update_cur_bal_stat_data();
        
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
    int32  rx_id;
    int8   rx_len;
    int8   in_data[8];
    
    if (can_getd(rx_id, in_data, rx_len, rxstat))
    {
        // Data was received, raise a flag corresponding to the data received
        switch(rx_id)
        {
            case COMMAND_ENABLE_BALANCING_ID:
                gb_balance_enable = true;
                break;
            case RESPONSE_PMS_DISCONNECT_ARRAY_ID:
                gb_pms_response_received = true;
                break;
            case COMMAND_EVDC_DRIVE_ID:
                gb_motor_connected = true;
                break;
            // If any of the MPPTs respond, raise the flag
            case RESPONSE_MPPT1_ID:
            case RESPONSE_MPPT2_ID:
            case RESPONSE_MPPT3_ID:
            case RESPONSE_MPPT4_ID:
                gb_mppt_connected = true;
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
        // Something went wrong, signal PMS to disconnect the array
        // Wait for response
        can_putd(COMMAND_PMS_DISCONNECT_ARRAY_ID,0,0,TX_PRI,TX_EXT,TX_RTR);
        g_state = PMS_RESPONSE_PENDING;
    }
}

void begin_balance_state(void)
{
    int i;
    int lowest = get_lowest_voltage_cell_index();
    
    for (i = 0 ; i < 12 ; i++)
    {
        if ((g_cell[i].average_voltage - g_cell[lowest].average_voltage)
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
        if ((g_cell[i].average_voltage - g_cell[lowest].average_voltage)
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
        if ((g_cell[i].average_voltage - g_cell[lowest].average_voltage)
            > BALANCE_THRESHOLD)
        {
            g_discharge3 |= 1 << (i - 24);
        }
        else
        {
            g_discharge3 &= ~(1 << (i - 24));
        }
    }
    
    // Enable/disable the discharge pins on the LTC6804
    output_low(CSBI1);
    ltc6804_write_config(g_discharge1);
    output_high(CSBI1);
    output_low(CSBI2);
    ltc6804_write_config(g_discharge2);
    output_high(CSBI2);
    output_low(CSBI3);
    ltc6804_write_config(g_discharge3);
    output_high(CSBI3);
    
    g_state = BALANCING;
}

void balancing_state(void)
{
    static int16 balance_time_ms = 0;
    
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

void pms_response_pending_state(void)
{
    static int16 timeout_ms = 0;
    
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
    eeprom_write_errors();
    can_putd(COMMAND_BPS_TRIP_SIGNAL_ID,0,0,TX_PRI,TX_EXT,TX_RTR);
    delay_ms(BLINKER_WAIT_TIME_MS); // Wait a bit for the blinker to process the trip signal
    KILOVAC_OFF;
}

// Main
void main()
{
    int8 i;
    
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
    eeprom_clear_flags();
    output_high(FAN_PIN); // Turn on the fan
    
    // Initialize CANbus, configure outputs, enable transfer buffers
    can_init();
    set_tris_f((*0x02DE&0xFFFD)|0x01); // set F0 to CANRX, F1 to CANTX
    for (i = 0 ; i < 8 ; i++)
    {
        can_enable_b_transfer(1);
    }
    
    // Populate running averages
    /*for (i = 0 ; i < N_VOLTAGE_SAMPLES ; i++)
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
    }*/
    
    // Perform startup test
    if ((check_voltage() & check_temperature() & check_current()) == true)
    {
        // Voltage, temperature, and current are all safe
        // Clear the eeprom and connect the pack
        eeprom_clear_memory();
        KILOVAC_ON;
    }
    else
    {
        // Something went wrong, do not connect the pack
        eeprom_write_errors();
        can_putd(COMMAND_BPS_TRIP_SIGNAL_ID,0,0,TX_PRI,TX_EXT,TX_RTR);
        delay_ms(BLINKER_WAIT_TIME_MS); // Wait a bit for the blinker to process the trip signal
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
                break;
            case BALANCING:
                balancing_state();
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

