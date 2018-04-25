#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ds18b20.h"  

#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof(arr)[0])

typedef unsigned  char  uchar;
typedef unsigned  int   uint;
typedef unsigned  short uint16;
typedef signed    short int16;

uint16 readCGData();
uint16 readCOData();
void initClock();
void initUart();
void delayMSecond(uint msecond);
void uartSendString(char *Data, int len);

void main()
{
	char precdictValue[30];

	uint16 CGData;
	uint16 COData;
        
	float temperature;
        //float co = 0.0;
        //float cg = 0.0;

	initClock();
	initUart();
	P0SEL &= 0x7f;  //init the DS18B20 I/O pin

	while (1)
	{
		memset(precdictValue, 0, ARRAY_SIZE(precdictValue));

		temperature = floatReadDs18B20();
                
		CGData = readCGData();
		COData = readCOData();

		sprintf(precdictValue, "TM%.02f|CG%d|CO%d", temperature, CGData, COData);
                if(17 <= strlen(precdictValue))
                {
			uartSendString(precdictValue, strlen(precdictValue));
                }
		delayMSecond(800);
	}
}

uint16 readCGData(void)
{
	uint16 reading = 0;
	/* Enable channel */
	ADCCFG |= 0x40;
	/* writing to this register starts the extra conversion */
	ADCCON3 = 0x86;// AVDD5 引脚  00：64 抽取率(7 位 ENOB)  0110：AIN6 
	/* Wait for the conversion to be done */
	while (!(ADCCON1 & 0x80));
	/* Disable channel after done conversion */
	ADCCFG &= (0x40 ^ 0xFF); //按位异或。如 1010^1111=0101（二进制） 
	/* Read the result */
	reading = ADCL;
	reading |= (int16)(ADCH << 8);
	reading >>= 8;
	return (reading);
}

uint16 readCOData(void)
{
	uint16 reading = 0;
	/* Enable channel */
	ADCCFG |= 0x40;
	/* writing to this register starts the extra conversion */
	ADCCON3 = 0x85;// AVDD5 引脚  00：64 抽取率(7 位 ENOB)  0101：AIN5 
	/* Wait for the conversion to be done */
	while (!(ADCCON1 & 0x80));
	/* Disable channel after done conversion */
	ADCCFG &= (0x40 ^ 0xFF); //按位异或。如 1010^1111=0101（二进制） 
	/* Read the result */
	reading = ADCL;
	reading |= (int16)(ADCH << 8);
	reading >>= 8;
	return (reading);
}

void delayMSecond(uint msecond)
{
	for (int i = 0; i < msecond; i++)
	{
		for (int j = 0; j < 1070; j++);
	}
}

void initClock()
{
	CLKCONCMD &= ~0x40;             //设置系统时钟源为32MHZ晶振
	while (CLKCONSTA & 0x40);        //等待晶振稳定为32M
	CLKCONCMD &= ~0x47;             //设置系统主时钟频率为32MHZ   
}

void initUart()
{
	PERCFG = 0x00;                  //位置1 P0口
	P0SEL = 0x0c;                   //P0用作串口
	P2DIR &= ~0xc0;                 //P0优先作为UART0 
	U0CSR |= 0x80;                  //串口设置为UART方式
	U0GCR |= 11;
	U0BAUD |= 216;                  //波特率设为115200
	U0CSR |= 0x40;                  //UART接收器使能
	UTX0IF = 0;                     //UART0 TX中断标志初始置位0
}

void uartSendString(char *Data, int len)
{
	uint i;

	for (i = 0; i < len; i++)
	{
		U0DBUF = *Data++;
		while (UTX0IF == 0);
		UTX0IF = 0;
	}
}
