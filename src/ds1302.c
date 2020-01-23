
/**************************************************************************
*
* @brief DS1302 DIGITAL CLOCK LIBRARY
         for Real time digital clock 
Version: 		2.1 BETA
Release date:	

@author:
(C)2018 Adeyemi Adekorede

@modified:
(C)2019 Bytehub Embedded Lab

@re-authored for STM32
(C)2020 Ayinde Olayiwola

@file:     	DS1302.h
Dependencies:	None
Compiler:   	CloudX C Compiler | STM32
Company:        www.cloudx.ng | www.bytehubembed.com 
Product:        CloudX microcontrollers | STM32

You may NOT distribute or sell this code without explicit authorization

* Warranty
* 
* THE SOFTWARE AND DOCUMENTATION ARE PROVIDED "AS IS" WITHOUT
* WARRANTY OF ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT
* LIMITATION, ANY WARRANTY OF MERCHANTABILITY, FITNESS FOR A
* PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO EVENT SHALL
* WE ARE LIABLE FOR ANY INCIDENTAL, SPECIAL, INDIRECT OR
* CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA, COST OF
* PROCUREMENT OF SUBSTITUTE GOODS, TECHNOLOGY OR SERVICES, ANY CLAIMS
* BY THIRD PARTIES (INCLUDING BUT NOT LIMITED TO ANY DEFENSE
* THEREOF), ANY CLAIMS FOR INDEMNITY OR CONTRIBUTION, OR OTHER
* SIMILAR COSTS, WHETHER ASSERTED ON THE BASIS OF CONTRACT, TORT
* (INCLUDING NEGLIGENCE), BREACH OF WARRANTY, OR OTHERWISE.
*
**************************************************************************/




/********************************************************************
/*****EXAMPLE:
/****** WARNING: *****************************************
 Check the CloudX website page for more examples

   char  Lcd_VoltScreen[6],
      Lcd_CurScreen[4],
      Lcd_TimeScreen[]= "00:00:00",
      Lcd_DateScreen[]= "00/00/00";

typedef struct{
       uint8_t Seconds;
       uint8_t Minutes;
       uint8_t Hours;
}Time_TypeDef;

typedef struct{
       uint8_t Date;
       uint8_t WeekDay;
       uint8_t Month;
       uint8_t Year;    
}Date_TypeDef;

Time_TypeDef    gTime;
Date_TypeDef    gDate;

int main()
{
   DS1302_init();  
   Lcd_Init(); 
   LCD_Cmd(clear);
   
   LCD_writeText(1, 1, "Time:");
   LCD_writeText(2, 1, "Date:");
   
   DS1302_WriteDateTime(6, 7, 18, 30, 9, 30, 0); //day(1-7),month(1-12),year(0-99),date(1-31),hr(0-23),min(0-59),sec(0-59)
   //DS1302_writeDate(27, 9, 18); //date(1-31), month(1-12), year(0-99)
   //DS1302_writeTime(9, 30, 15); //hr,min,seconds
   
   //write and read from DS1302RAM
   DS1302RAM_write(0, 7);
   minutes = DS1302RAM_read(0);
   
   loop(){
  
       DS1302_readTime(&hours, &minutes, &seconds);   
       DS1302_readDate(&day, &month, &year, &date);

        time[0]= BCD2UpperChar(hours);
        time[1]= BCD2LowerChar(hours);
        time[3]= BCD2UpperChar(minutes);
        time[4]= BCD2LowerChar(minutes);
        time[6]= BCD2UpperChar(seconds);
        time[7]= BCD2LowerChar(seconds);

        Date[0]= BCD2UpperChar(date);
        Date[1]= BCD2LowerChar(date);
        Date[3]= BCD2UpperChar(month);
        Date[4]= BCD2LowerChar(month);
        Date[6]= BCD2UpperChar(year);
        Date[7]= BCD2LowerChar(year);


        LCD_writeText(1, 7, time);
        LCD_writeText(2, 7, Date);


   }
    

}
****************************************************************************/


#include "ds1302.h"



void DS1302_Init(void)
{
   byte x;

     DS1302_GPIO_MODE(GPIO_Pin_CLK, GPIO_MODE_OUTPUT_PP);
     DS1302_GPIO_MODE(GPIO_Pin_RST, GPIO_MODE_OUTPUT_PP);
     DS1302_GPIO_MODE(GPIO_Pin_DAT, GPIO_MODE_INPUT);

    HAL_GPIO_WritePin(GPIO_PORT, GPIO_Pin_CLK | GPIO_Pin_RST, GPIO_PIN_RESET);

//basic house keeping 
//Write protect disable bit must be disabled.
    DS1302_Write(0x8E,0);
 //enable trickle charger
 // enable one diode mode
 // enable 2k charging resistor and charging current calculated is 2.2mA PG 7.
    DS1302_Write(0x90,0xA5);
 //we read clock disable bit and ensure the clock is enabled
    x = DS1302_Read(0x81);

    if((x & 0x80)!=0)
       DS1302_Write(0x80, 0);
}


/**
  * @brief  Sends read command to the DS1302 RTC.
  * @param  cmd a byte that contains read command
  * @param  Timeout Timeout duration
  * @retval DS1302 status
  */
byte DS1302_Read(byte cmd)
{
     DS1302_GPIO_MODE(GPIO_Pin_DAT, GPIO_MODE_INPUT);

     byte dataRead=0x00;
     HAL_GPIO_WritePin(GPIO_PORT, GPIO_Pin_RST, HIGH);  //reset high
     Write_DS1302_byte(cmd);
     dataRead = shift_in_byte();

     DS1302_GPIO_MODE(GPIO_Pin_DAT, GPIO_MODE_OUTPUT_PP);

     HAL_GPIO_WritePin(GPIO_PORT, GPIO_Pin_RST,LOW);
     return dataRead;
     }


/**
  * @brief  Sends write command to the DS1302 RTC.
  * @param  cmd a byte that contains read command
  * @param  Timeout Timeout duration
  * @retval None
  */
void Write_DS1302_byte(byte cmd)
{
    DS1302_GPIO_MODE(GPIO_Pin_DAT, GPIO_MODE_OUTPUT_PP);
    HAL_GPIO_WritePin(GPIO_PORT, GPIO_Pin_DAT, LOW);
    Shift_out_byte_right(cmd);
    DS1302_GPIO_MODE(GPIO_Pin_DAT, GPIO_MODE_INPUT);
}


void DS1302_Write(byte cmd, byte data)
{
    HAL_GPIO_WritePin(GPIO_PORT, GPIO_Pin_RST, HIGH);
    Write_DS1302_byte(cmd);
    Write_DS1302_byte(data);
    HAL_GPIO_WritePin(GPIO_PORT, GPIO_Pin_RST, LOW);
    HAL_GPIO_WritePin(GPIO_PORT, GPIO_Pin_CLK, LOW);
}


// ADDRESS IS THE BITS PLUS INITIAL START ADDRESS OF THE RAM.  
//RAM IS 31BIT WIDE  ADDRESS WOULD RANGE FROM 0-31;
void DS1302RAM_Write(byte address, byte data){
    data = Binary2BCD(data);
    DS1302_Write( (address|0xC0), data);
}

byte DS1302RAM_Read(byte address){
    byte data = 0x00;
    data = DS1302_Read(address|0xC1);
    data = BCD2Binary(data);
    return data;
    
}



byte shift_in_byte(){
byte temp, data=0x00;
    for(byte i=1;i<=8;i++)
    {
        temp = HAL_GPIO_ReadPin(GPIO_PORT, GPIO_Pin_DAT);
        temp= (temp<<7);
        data=(data>>1);
        data = (temp | data);
        HAL_GPIO_WritePin(GPIO_PORT, GPIO_Pin_CLK, HIGH);
        HAL_GPIO_WritePin(GPIO_PORT, GPIO_Pin_CLK, LOW);
    }
return data;
}



void Shift_out_byte_right(byte data){
    char i;
    for (i=0; i<8; i++)
    {
        if(data & 0x01)
         HAL_GPIO_WritePin(GPIO_PORT, GPIO_Pin_DAT, HIGH);
        else
         HAL_GPIO_WritePin(GPIO_PORT, GPIO_Pin_DAT, LOW);
    
       HAL_GPIO_WritePin(GPIO_PORT, GPIO_Pin_CLK, HIGH);
       HAL_GPIO_WritePin(GPIO_PORT, GPIO_Pin_CLK, LOW);
       data = data>>1;    
    }

}

/**
  * @brief  Sends write and read command to the DS1302 RTC.
  * @param  day   a byte that contains value day   to write
  * @param  month a byte that contains value Month to write
  * @param  date  a byte that contains value date  to write
  * @param  hr    a byte that contains value hr    to write
  * @param  min   a byte that contains value min   to write
  * @param  sec   a byte that contains value sec   to write
  * @retval None
  */
void DS1302_WriteDateTime(byte day, byte month, byte year, byte date, byte hr, byte min, byte seconds)
{  
min      = Binary2BCD(min);
hr       = Binary2BCD(hr);
seconds  = Binary2BCD(seconds);
day      = Binary2BCD(day);
month    = Binary2BCD(month);
year     = Binary2BCD(year);
date     = Binary2BCD(date);

DS1302_Write(minuteWrite, min);
DS1302_Write(hourWrite, hr);
DS1302_Write(dayWrite, day);
DS1302_Write(monthWrite, month);
DS1302_Write(dateWrite, date);
DS1302_Write(yearWrite, year);
DS1302_Write(secondsWrite, seconds);

}

/**
  * @brief  Sends write Time command to the DS1302 RTC.
  * @param  hr  a byte that contains value Hours to write
  * @param  min a byte that contains value Minute to write
  * @param  sec a byte that contains value Second to write
  * @retval None
  */
void DS1302_WriteTime(byte hr,byte min,byte seconds)
{
   min     = Binary2BCD(min);
   hr      = Binary2BCD(hr);
   seconds = Binary2BCD(seconds);

   DS1302_Write(minuteWrite,min);
   DS1302_Write(hourWrite, hr);
   DS1302_Write(secondsWrite,seconds);
}


void DS1302_WriteDate(byte date,byte month,byte year)
{
   date  = Binary2BCD(date);
   month = Binary2BCD(month);
   year  = Binary2BCD(year);


   DS1302_Write(dateWrite,date);
   DS1302_Write(monthWrite,month);
   DS1302_Write(yearWrite,year);
}


void DS1302_ReadTime( byte *hr,byte *min,byte *sec)
{
   *sec = DS1302_Read(secondsRead );
   *min = DS1302_Read(minuteRead);
   *hr  = DS1302_Read(hourRead);
}
 
void DS1302_ReadDate(byte *day, byte *month, byte *year, byte *date){
     
    *day   =  DS1302_Read(dayRead );
    *month =  DS1302_Read(monthRead);
    *year  =  DS1302_Read(yearRead);
    *date  =  DS1302_Read(dateRead);
      
}



// reason for this is cause my output is in form of binary 
//which is an integer..
char IntToChar(byte x)
{
 x = Binary2BCD(x);
}


char BCD2Binary(byte BCDValue)
{
   char c,d;
   d        = BCDValue & 0x0F;
   c        = d;
   BCDValue = 0xF0 & BCDValue;
   d        = BCDValue >> 4;
   d        = 0x0F & d;
   c        = d * 10 + c;
   return c;
}


char Binary2BCD(byte binaryValue)
{
   char a, b;
   a          = binaryValue % 10;
   a          = a & 0x0F;
   binaryValue = binaryValue / 10;
   b          = binaryValue % 10;
   b          = 0x0F & b;
   b          = b << 4;
   b          = 0xF0 & b;
   a          = a | b;
   return a;
}

uint8_t BCD2UpperChar(uint8_t bcd)
{
  return ((bcd >> 4) + '0');
}

uint8_t BCD2LowerChar(uint8_t bcd)
{
  return ((bcd & 0x0F) + '0');
}

uint16_t HexToDec(uint8_t h2d)
{
  return ((((h2d & 0xF0)>>4)*10) + (h2d & 0x0F));
}

uint16_t DecToHex(uint8_t h2d)
{
  return (((h2d/16)<<4) + (h2d%16));
}


void DS1302_GPIO_MODE(uint16_t GPIO_Pin_t, uint16_t GPIO_MODE_t)
{
    GPIO_InitTypeDef GPIOx;

#if defined   DS1302_GPIO_PORT_GPIOA
     __HAL_RCC_GPIOA_CLK_ENABLE();
#elif defined DS1302_GPIO_PORT_GPIOB
      __HAL_RCC_GPIOB_CLK_ENABLE();
#elif defined DS1302_GPIO_PORT_GPIOC
      __HAL_RCC_GPIOC_CLK_ENABLE();
#elif defined DS1302_GPIO_PORT_GPIOD
      __HAL_RCC_GPIOD_CLK_ENABLE();
#endif

     GPIOx.Mode  = GPIO_MODE_t;
     GPIOx.Pin   = GPIO_Pin_t;
     GPIOx.Pull  = GPIO_NOPULL;
     GPIOx.Speed = GPIO_SPEED_FREQ_LOW;

     HAL_GPIO_Init(GPIO_PORT, &GPIOx);
}

/*example to write and read from the RAM*/

  /*
   char *time;
   byte minutes =0; 
   DS1302RAM_write(0,7);
   minutes = DS1302RAM_read(0);


   
   //  
    intTostr(time,minutes,DEC);
 Lcd_writeText(1,1,time);
*/