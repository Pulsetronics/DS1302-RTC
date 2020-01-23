/**
  ************************************************************************
  * @file:      lcd.c
  * @author:    Ayinde Olayiwola
  * @brief:     Alphanumeric LCD driver
  * @support:   16x1, 16x2, 16x4, 20x4
  * @date:      22-12-2019
  *
  *
  ************************************************************************
  ************************************************************************
  **/

#include "stm32f1xx_hal.h"
#include "lcd.h"


int8_t _row, _col;



/**
  * @brief:   Write bits to LCD
  * @return:  None
  */
void Lcd_ShiftOut(uint8_t MSB1, uint8_t LSB1)
{

      HAL_GPIO_WritePin(GPIOB, D4, (MSB1 & 1));
      MSB1 = MSB1 >> 1;   HAL_GPIO_WritePin(GPIOB, D5, (MSB1 & 1));
      MSB1 = MSB1 >> 1;   HAL_GPIO_WritePin(GPIOB, D6, (MSB1 & 1));
      MSB1 = MSB1 >> 1;   HAL_GPIO_WritePin(GPIOB, D7, (MSB1 & 1));
      Lcd_Clock();

      HAL_GPIO_WritePin(GPIOB, D4, (LSB1 & 1));
      LSB1 = LSB1 >> 1;  HAL_GPIO_WritePin(GPIOB, D5, (LSB1 & 1));
      LSB1 = LSB1 >> 1;  HAL_GPIO_WritePin(GPIOB, D6, (LSB1 & 1));
      LSB1 = LSB1 >> 1;  HAL_GPIO_WritePin(GPIOB, D7, (LSB1 & 1));
      Lcd_Clock();
}

/**
  * @brief:   Write commands to LCD
  * @return:  None
  */
void Lcd_Cmd(uint8_t _command)
{
      uint8_t MSB2, LSB2;
      HAL_GPIO_WritePin(GPIOB, RS, 0);
      MSB2 = (_command & 0xF0) >> 4; LSB2 = _command & 0x0F ;
      Lcd_ShiftOut(MSB2, LSB2);
}

/**
  * @brief:   Display or Print single character on LCD current
  *           position.
  * @return:  None
  */
void Lcd_Print_CurPos(uint8_t _char)
{
	  uint8_t MSB, LSB;
      HAL_GPIO_WritePin(GPIOB, RS, 1);
      MSB = (_char & 0xF0) >> 4; LSB = _char & 0x0F ;
      Lcd_ShiftOut(MSB, LSB);
	  _col++;
}


/**
  * @brief:   Display or Print single character on LCD
  * @return:  None
  */
void Lcd_Print(uint8_t row, uint8_t column, uint8_t _char)
{
      _col = column;
	  _row = row;
      if(row == 1) Lcd_Cmd(0x80 + (column - 1));
      if(row == 2) Lcd_Cmd(0xC0 + (column - 1));
      if(row == 3) Lcd_Cmd(0x94 + (column - 1));
      if(row == 4) Lcd_Cmd(0xD4 + (column - 1));
      Lcd_Print_CurPos(_char);
}


/**
  * @brief:   Display or Print string on LCD current
  *           position.
  * @return:  None
  */
void Lcd_PrintString_CurPos(uint8_t *_str)
{
	   uint8_t pnt=0;

       while(_str[pnt] != '\0')
	        {
              Lcd_Print_CurPos(_str[pnt]);
              pnt++;
              HAL_Delay(2);
            }
}

/**
  * @brief:   Sets cursor position to print characters
  *           on LCD.
  * @return:  None
  */
void Lcd_SetCursor(uint8_t row, uint8_t column)
{
       if(row == 1) Lcd_Cmd(0x80 + (column - 1));
       if(row == 2) Lcd_Cmd(0xC0 + (column - 1));
       if(row == 3) Lcd_Cmd(0x94 + (column - 1));
       if(row == 4) Lcd_Cmd(0xD4 + (column - 1));
}


/**
  * @brief:   Display or Print string on LCD
  * @return:  None
  */
void Lcd_PrintString(uint8_t row, uint8_t column, uint8_t *_str)
{
	   uint8_t pnt;
	   _col = column;
       _row = row;
       if(row == 1) Lcd_Cmd(0x80 + (column - 1));
       if(row == 2) Lcd_Cmd(0xC0 + (column - 1));
       if(row == 3) Lcd_Cmd(0x94 + (column - 1));
       if(row == 4) Lcd_Cmd(0xD4 + (column - 1));
          
       pnt =0;
       while(_str[pnt] != '\0')
		 {
            Lcd_Print_CurPos(_str[pnt]);
            pnt++;
            HAL_Delay(2);
         }								  
}

/**
  * @brief:   Initialises the LCD
  * @return:  None
  */
void Lcd_Init(void)
{
      Lcd_Cmd(0x33);
      HAL_Delay(1);
      Lcd_Cmd(0x32);
      HAL_Delay(1);
      Lcd_Cmd(0x28);
      HAL_Delay(1);
      Lcd_Cmd(0x08);
      HAL_Delay(1);
      Lcd_Cmd(0x0C);
      HAL_Delay(1);
      Lcd_Cmd(0x06);
      HAL_Delay(1);
}


/**
  * @brief:   Creates custom characters on LCD
  * @return:  None
  */
void Lcd_PrintCustomChar(uint8_t addressCGRAM, uint8_t row, uint8_t column, int8_t *customCharArray)
{
	uint8_t  pnt;
    Lcd_Cmd(64 + (addressCGRAM *  8));
    for (pnt = 0; pnt <= 7; pnt++)	
        Lcd_Print_CurPos(customCharArray[pnt]);
    Lcd_Cmd(returnHome);
    
    if((!row) && (!column))
		Lcd_Print(_row, _col - 8, addressCGRAM); 
    else
        Lcd_Print(row, column, addressCGRAM);
 }   
 
 
