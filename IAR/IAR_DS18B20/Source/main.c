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
	ADCCON3 = 0x86;// AVDD5 ����  00��64 ��ȡ��(7 λ ENOB)  0110��AIN6 
	/* Wait for the conversion to be done */
	while (!(ADCCON1 & 0x80));
	/* Disable channel after done conversion */
	ADCCFG &= (0x40 ^ 0xFF); //��λ����� 1010^1111=0101�������ƣ� 
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
	ADCCON3 = 0x85;// AVDD5 ����  00��64 ��ȡ��(7 λ ENOB)  0101��AIN5 
	/* Wait for the conversion to be done */
	while (!(ADCCON1 & 0x80));
	/* Disable channel after done conversion */
	ADCCFG &= (0x40 ^ 0xFF); //��λ����� 1010^1111=0101�������ƣ� 
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
	CLKCONCMD &= ~0x40;             //����ϵͳʱ��ԴΪ32MHZ����
	while (CLKCONSTA & 0x40);        //�ȴ������ȶ�Ϊ32M
	CLKCONCMD &= ~0x47;             //����ϵͳ��ʱ��Ƶ��Ϊ32MHZ   
}

void initUart()
{
	PERCFG = 0x00;                  //λ��1 P0��
	P0SEL = 0x0c;                   //P0��������
	P2DIR &= ~0xc0;                 //P0������ΪUART0 
	U0CSR |= 0x80;                  //��������ΪUART��ʽ
	U0GCR |= 11;
	U0BAUD |= 216;                  //��������Ϊ115200
	U0CSR |= 0x40;                  //UART������ʹ��
	UTX0IF = 0;                     //UART0 TX�жϱ�־��ʼ��λ0
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
