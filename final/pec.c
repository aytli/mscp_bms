#ifndef PEC_C
#define PEC_C

// CRC polynomial
#define CRC15_POLY 0x4599

static unsigned int16 pec15Table[256];

void init_PEC15_Table(void)
{
    int i;
    int bit;
    unsigned int16 remainder;
    for (i = 0; i < 256; i++)
    {
        remainder = i << 7;
        for (bit = 8; bit > 0; --bit)
        {
            if (remainder & 0x4000)
            {
                remainder = ((remainder << 1));
                remainder = (remainder ^ CRC15_POLY);
            }
            else
            {
                remainder = ((remainder << 1));
            }
        }
        pec15Table[i] = remainder & 0xFFFF;
    }
}

unsigned int16 pec15(char *data , int len)
{
    unsigned int16 remainder, address;
    remainder = 16;
    int i;
    for (i = 0; i < len; i++) {
        address = ((remainder >> 7) ^ data[i]) & 0xff;
        remainder = (remainder << 8 ) ^ pec15Table[address];
    }
    return (remainder * 2);
}

#endif
