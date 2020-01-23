

#ifndef _DS1302_H_
#define _DS1302_H_

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------*/
#include <stdlib.h>
#include <math.h>
#include "main.h"


/*Definitions for backward-compatibility*/
#define RAM_write DS1302RAM_write
#define RAM_read  DS1302RAM_read

#define secondsWrite 0x80
#define minuteWrite  0x82
#define hourWrite    0x84
#define dateWrite    0x86
#define monthWrite   0x88
#define dayWrite     0x8A
#define yearWrite    0x8C


#define secondsRead   0x81
#define minuteRead    0x83
#define hourRead      0x85
#define dateRead      0x87
#define monthRead     0x89
#define dayRead       0x8B
#define yearRead      0x8D

#define byte uint8_t

#define DS1302_GPIO_PORT_GPIOA
//#define DS1302_GPIO_PORT_GPIOB
//#define DS1302_GPIO_PORT_GPIOC
//#define DS1302_GPIO_PORT_GPIOD
//#define DS1302_GPIO_PORT_GPIOE

#if defined   DS1302_GPIO_PORT_GPIOA
#define GPIO_PORT GPIOA
#elif defined DS1302_GPIO_PORT_GPIOB
#define GPIO_PORT GPIOB
#elif defined DS1302_GPIO_PORT_GPIOC
#define GPIO_PORT GPIOC
#elif defined DS1302_GPIO_PORT_GPIOD
#define GPIO_PORT GPIOD
#endif
 

#define  GPIO_Pin_CLK  GPIO_PIN_4
#define  GPIO_Pin_DAT  GPIO_PIN_5
#define  GPIO_Pin_RST  GPIO_PIN_6

typedef enum{
      LOW,
      HIGH
}state;


char BCD2Binary(byte BCDValue);
char Binary2BCD(byte BCDValue);
void DS1302_GPIO_MODE(uint16_t GPIO_Pin_t, uint16_t GPIO_MODE_t);
void Shift_out_byte_right(byte data);
byte shift_in_byte();
void Write_DS1302_byte(byte cmd);
byte DS1302_Read(byte cmd);
void DS1302_Init();

 
 // THESE ARE THE FUNCTIONS THAT CONCERN THE USER...
void DS1302_ReadTime(byte *hr,byte *min,byte *sec);
void DS1302_ReadDate(byte *day, byte *month, byte *year, byte *date);
void DS1302RAM_Write(byte address, byte data);
byte DS1302RAM_Read(byte address);
void DS1302_WriteDateTime(byte day, byte month,byte year,byte date,byte hr,byte min,byte seconds);
void DS1302_WriteTime(byte hr,byte min,byte seconds);
void DS1302_WriteDate(byte date,byte month,byte year);
char IntToChar(byte x);
unsigned char BCD2UpperChar(unsigned char bcd);
unsigned char BCD2LowerChar(unsigned char bcd);


#endif  //#ifndef _DS1302_H_
