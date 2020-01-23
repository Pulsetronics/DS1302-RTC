#include <stdio.h>
#include "main.h"

#include "ds1302.h"

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



/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
//void RTC_Config(void);

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* Configure the system clock */
  SystemClock_Config();

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  DS1302_Init();
  Lcd_Init();
  Lcd_Cmd(clear);
  Lcd_Cmd(cursorOff);

   /* You can also decide to use the write function below for both Date and Time
      day(1-7),month(1-12),year(0-99),date(1-31),hr(0-23),min(0-59),sec(0-59)
   */
    //DS1302_WriteDateTime(6, 7, 18, 30, 9, 30, 0); 

    DS1302_WriteTime(9, 30, 15); //hr,min,seconds
    DS1302_WriteDate(27, 9, 18); //date(1-31), month(1-12), year(0-99)

 
  while (1)
  {

    DS1302_ReadTime(&gTime.Hours, &gTime.Minutes, &gTime.Seconds);   
    DS1302_ReadDate(&gDate.WeekDay, &gDate.Month, &gDate.Year, &gDate.Date);
    
    Lcd_TimeScreen[0] = BCD2UpperChar(gTime.Hours);
    Lcd_TimeScreen[1] = BCD2LowerChar(gTime.Hours);
    Lcd_TimeScreen[3] = BCD2UpperChar(gTime.Minutes);
    Lcd_TimeScreen[4] = BCD2LowerChar(gTime.Minutes);
    Lcd_TimeScreen[6] = BCD2UpperChar(gTime.Seconds);
    Lcd_TimeScreen[7] = BCD2LowerChar(gTime.Seconds);

    Lcd_DateScreen[0] = BCD2UpperChar(gDate.Date);
    Lcd_DateScreen[1] = BCD2LowerChar(gDate.Date);
    Lcd_DateScreen[3] = BCD2UpperChar(gDate.Month);
    Lcd_DateScreen[4] = BCD2LowerChar(gDate.Month);
    Lcd_DateScreen[6] = BCD2UpperChar(gDate.Year);
    Lcd_DateScreen[7] = BCD2LowerChar(gDate.Year);

    Lcd_PrintString(1,1, (uint8_t *) Lcd_TimeScreen);
    Lcd_PrintString(1,13, (uint8_t *) Lcd_DateScreen);
    HAL_Delay(100);

  }
}