// CRC polynomial
#define CRC15_POLY 0x4599

void init_PEC15_Table(void);
unsigned int16 pec15(char *, int);
