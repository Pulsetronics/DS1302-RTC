#include "stm32f1xx_hal_gpio.h"

// Define LCD Commands

#define clear               0x01
#define returnHome          0x02
#define moveCursorRight     0x14
#define moveCursorLeft      0x10
#define shiftDisplayRight   0x1C
#define shiftDisplayLeft    0x18
#define cursorBlink         0x0F
#define cursorOff           0x0C
#define cursorOn            0x0E
#define Function_set_4bit   0x28
#define Function_set_8bit   0x38
#define Entry_mode          0x06
#define Function_8_bit      0x32



// Define LCD Pins

#define  RS   GPIO_PIN_3
#define  EN	  GPIO_PIN_4
#define  D4	  GPIO_PIN_5
#define  D5	  GPIO_PIN_6
#define  D6	  GPIO_PIN_7
#define  D7	  GPIO_PIN_8

//Define LCD Enable PORT
#define  LCD_ENABLE_PORT    GPIOB

// Define LCD Internal Functions

#define Lcd_Clock()  ({ \
	                 HAL_GPIO_WritePin(LCD_ENABLE_PORT, EN, GPIO_PIN_SET); \
					 HAL_Delay(2); \
					 HAL_GPIO_WritePin(LCD_ENABLE_PORT, EN, GPIO_PIN_RESET);\
                      })


//Functions Declarations

void Lcd_ShiftOut(uint8_t MSB1, uint8_t LSB1);
void Lcd_Cmd(uint8_t _command);
void Lcd_Print_CurPos(uint8_t _char);
void Lcd_Print(uint8_t row, uint8_t column, uint8_t _char);
void Lcd_PrintString_CurPos(uint8_t *_str);
void Lcd_SetCursor(uint8_t row, uint8_t column);
void Lcd_PrintString(uint8_t row, uint8_t column, uint8_t *_str);
void Lcd_Init(void);
void Lcd_PrintCustomChar(uint8_t addressCGRAM, uint8_t row, uint8_t column, int8_t *customCharArray);




