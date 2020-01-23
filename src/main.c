/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */


/* Includes ------------------------------------------------------------------*/
#include <stdio.h>
#include "main.h"
#include "adc.h"
#include "tim.h"
#include "gpio.h"
#include "lcd.h"
#include "i2c.h"
#include "ds1302.h"
#include "dma.h"
#include "usart.h"



float ADCValue;
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

typedef struct{
       uint8_t One;
       uint8_t Two;
       uint8_t Three;
       uint8_t Four;
       uint8_t Five;
       uint8_t Six;
       uint8_t Seven;
       uint8_t Eight;
       uint8_t Nine;
       uint8_t Zero;
       uint8_t Harsh;
       uint8_t Star;

}Keypad_TypeDef;



Time_TypeDef    gTime;
Date_TypeDef    gDate;
Keypad_TypeDef  KeyPadx;

uint8_t keyPress, tempHour;
uint8_t Date_Rd_Buffer[6];
uint8_t Date_Wr_Buffer[6];

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
//void RTC_Config(void);

//void GetClock(void);
void KeyPad_Init(void);
uint8_t KeyPad_Scan();
void RTC_Set_TimeDate(void);
void Terminal_Send_Logs(void);


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
  MX_ADC1_Init();
 // MX_TIM3_Init();
  MX_I2C2_Init();
  MX_DMA_Init();
  MX_USART2_UART_Init();
  DS1302_Init();
  KeyPad_Init();
  Lcd_Init();
  Lcd_Cmd(clear);
  Lcd_Cmd(cursorOff);


  
   HAL_ADC_Start(&hadc1);
   Lcd_PrintString(2,8,(uint8_t *) "Micro");
   Lcd_PrintString(3,2,(uint8_t *) "Energy Monitoring");
   Lcd_PrintString(4,7,(uint8_t *) "System");
   HAL_Delay(2000);
   Lcd_Cmd(clear);
  
   Lcd_PrintString(1,2,(uint8_t *) "Storage Device");
   Lcd_PrintString(4,1,(uint8_t *) "Detecting......");
   HAL_Delay(1000);
   Lcd_Cmd(clear);
   if(HAL_I2C_IsDeviceReady(&hi2c2, 0xA0, 5, 1000) != HAL_OK)
   {
     Lcd_Cmd(clear);
     Lcd_PrintString(1,8,(uint8_t *) "Error ");
     Lcd_PrintString(2,2,(uint8_t *) "Connecting Device");
     Error_Handler();
   }

   Lcd_PrintString(1,2,(uint8_t *) "Storage Device");
   Lcd_PrintString(3,8,(uint8_t *) "Found");
   HAL_Delay(1000);
   Lcd_Cmd(clear);


  while (1)
  {
    ADCValue  =  ADC_Sampling(ADC_CHANNEL_0);
    ADCValue *=  0.00080586081 * 23.32066 * 9.97355 ;
    //float currentRMS = getCurrent();
   // float power = 230.0 * currentRMS;
  

    gcvt(ADCValue, 5, Lcd_VoltScreen);
    Lcd_VoltScreen[5]=0;
    Lcd_PrintString(2,1,(uint8_t *) "AC: ");
    Lcd_PrintString_CurPos((uint8_t *) Lcd_VoltScreen);

    Lcd_PrintString(2,13,(uint8_t *) "C1: ");
    Lcd_PrintString(3,1, (uint8_t *) "C2: ");
    Lcd_PrintString(3,13,(uint8_t *) "C3: ");
   
    if(tempHour != gTime.Hours)
    {
       tempHour = gTime.Hours;
       char EEADDR[2];
       HAL_I2C_Mem_Write(&hi2c2, 0xA0, ‭0x7FFE‬, I2C_MEMADD_SIZE_16BIT, EEADDR, 0x0001, 100);
       HAL_Delay(50);
       HAL_I2C_Mem_Write(&hi2c2, 0xA0, 0x0000U, I2C_MEMADD_SIZE_16BIT, Lcd_TimeScreen, 0x0008, 100);
       HAL_Delay(50);
       HAL_I2C_Mem_Write(&hi2c2, 0xA0, 0x0009U, I2C_MEMADD_SIZE_16BIT, Lcd_DateScreen, 0x0011, 100);
    }
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
    char keyFind = KeyPad_Scan();
    if(keyFind == KeyPadx.Harsh) 
           RTC_Set_TimeDate();
    if(keyFind== KeyPadx.Star) 
           Terminal_Send_Logs();

  }
 
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState       = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState       = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState   = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource  = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL     = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_ClkInitStruct.ClockType      = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                                     |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource   = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider  = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}


void KeyPad_Init(void)
{
    GPIO_InitTypeDef GPIOx;

    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();

    GPIOx.Mode  = GPIO_MODE_OUTPUT_PP;
    GPIOx.Pin   = GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14 ;
    GPIOx.Pull  = GPIO_NOPULL;
    GPIOx.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOB, &GPIOx);

    GPIOx.Mode  = GPIO_MODE_INPUT;
    GPIOx.Pin   = GPIO_PIN_15;
    GPIOx.Pull  = GPIO_PULLDOWN;
    GPIOx.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOB, &GPIOx);

    GPIOx.Mode  = GPIO_MODE_INPUT;
    GPIOx.Pin   = GPIO_PIN_11 | GPIO_PIN_9 | GPIO_PIN_10;
    GPIOx.Pull  = GPIO_PULLDOWN;
    GPIOx.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOA, &GPIOx);

   KeyPadx.Zero  = '0';
   KeyPadx.One   = '1';
   KeyPadx.Two   = '2';
   KeyPadx.Three = '3';
   KeyPadx.Four  = '4';
   KeyPadx.Five  = '5';
   KeyPadx.Six   = '6';
   KeyPadx.Seven = '7';
   KeyPadx.Eight = '8';
   KeyPadx.Nine  = '9';
   KeyPadx.Harsh = '#';
   KeyPadx.Star  = '*';

}


uint8_t KeyPad_Scan(void)
{
   keyPress = 0;
   HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_SET);
   HAL_GPIO_WritePin(GPIOB, GPIO_PIN_13, GPIO_PIN_RESET);
   HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_RESET);

   if(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_15) == GPIO_PIN_SET) 
      { while(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_15) == GPIO_PIN_SET); keyPress = KeyPadx.Harsh;}
   if(HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_11) == GPIO_PIN_SET) 
      { while(HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_11) == GPIO_PIN_SET); keyPress = KeyPadx.Nine;}
   if(HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_9) == GPIO_PIN_SET) 
      { while(HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_9) == GPIO_PIN_SET); keyPress = KeyPadx.Six;}
   if(HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_10) == GPIO_PIN_SET) 
      { while(HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_10) == GPIO_PIN_SET); keyPress = KeyPadx.Three;}

    HAL_Delay(5);

   HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_RESET);
   HAL_GPIO_WritePin(GPIOB, GPIO_PIN_13, GPIO_PIN_SET);
   HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_RESET);

   if(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_15) == GPIO_PIN_SET) 
      { while(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_15) == GPIO_PIN_SET); keyPress = KeyPadx.Zero;}
   if(HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_11) == GPIO_PIN_SET) 
      { while(HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_11) == GPIO_PIN_SET); keyPress = KeyPadx.Eight;}
   if(HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_9) == GPIO_PIN_SET) 
      { while(HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_9) == GPIO_PIN_SET); keyPress = KeyPadx.Five;}
   if(HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_10) == GPIO_PIN_SET) 
      { while(HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_10) == GPIO_PIN_SET); keyPress = KeyPadx.Two;}

      HAL_Delay(5);

   HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_RESET);
   HAL_GPIO_WritePin(GPIOB, GPIO_PIN_13, GPIO_PIN_RESET);
   HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_SET);

   if(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_15) == GPIO_PIN_SET) 
      { while(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_15) == GPIO_PIN_SET); keyPress = KeyPadx.Star;}
   if(HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_11) == GPIO_PIN_SET) 
      { while(HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_11) == GPIO_PIN_SET); keyPress = KeyPadx.Seven;}
   if(HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_9) == GPIO_PIN_SET) 
      { while(HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_9) == GPIO_PIN_SET); keyPress = KeyPadx.Four;}
   if(HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_10) == GPIO_PIN_SET) 
      { while(HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_10) == GPIO_PIN_SET); keyPress = KeyPadx.One;}

   HAL_Delay(5);

   HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_RESET);
   HAL_GPIO_WritePin(GPIOB, GPIO_PIN_13, GPIO_PIN_RESET);
   HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14, GPIO_PIN_RESET);

  return keyPress;

}



void RTC_Set_TimeDate(void)
{
      Lcd_Cmd(clear);
      Lcd_PrintString(1,1, (uint8_t *) "Set Time: ");
      Lcd_Cmd(cursorBlink);
      while(KeyPad_Scan()==0);
      char _temp = keyPress - 0x30;
      Lcd_Print_CurPos(keyPress);
      while(KeyPad_Scan()==0);
      gTime.Hours =  (_temp <<4) | (keyPress-0x30);
      Lcd_Print_CurPos(keyPress);
      Lcd_Print_CurPos(':');
       while(KeyPad_Scan()==0);
      _temp = keyPress - 0x30;
      Lcd_Print_CurPos(keyPress);
      while(KeyPad_Scan()==0);
      gTime.Minutes =  (_temp <<4) | (keyPress-0x30);
      Lcd_Print_CurPos(keyPress);
      Lcd_Print_CurPos(':');
 
      while(KeyPad_Scan()==0);
      _temp = keyPress - 0x30;
      Lcd_Print_CurPos(keyPress);
      while(KeyPad_Scan()==0);
      gTime.Seconds =  (_temp <<4) | (keyPress-0x30);
      Lcd_Print_CurPos(keyPress);

      Lcd_PrintString(2,1, (uint8_t *) "Set Date: ");
      while(KeyPad_Scan()==0);
      _temp = keyPress - 0x30;
      Lcd_Print_CurPos(keyPress);
      while(KeyPad_Scan()==0);
      gDate.Date =  (_temp <<4) | (keyPress-0x30);
      Lcd_Print_CurPos(keyPress);
      Lcd_Print_CurPos('/');
      while(KeyPad_Scan()==0);
      _temp = keyPress - 0x30;
      Lcd_Print_CurPos(keyPress);
      while(KeyPad_Scan()==0);
      gDate.Month =  (_temp <<4) | (keyPress-0x30);
      Lcd_Print_CurPos(keyPress);
      Lcd_Print_CurPos('/');
      while(KeyPad_Scan()==0);
      _temp = keyPress - 0x30;
      Lcd_Print_CurPos(keyPress);
      while(KeyPad_Scan()==0);
      gDate.Year =  (_temp <<4) | (keyPress-0x30);
      Lcd_Print_CurPos(keyPress);
      keyPress=0x00U;
      HAL_Delay(1000);
      
      Lcd_Cmd(clear);
      Lcd_Cmd(cursorOff);
      Lcd_PrintString(4,1, (uint8_t *) "Writing to RTC chip");
     
      gTime.Hours   = HexToDec(gTime.Hours);
      gTime.Minutes = HexToDec(gTime.Minutes);
      gTime.Seconds = HexToDec(gTime.Seconds);

      gDate.Date    = HexToDec(gDate.Date);
      gDate.Month   = HexToDec(gDate.Month);
      gDate.Year    = HexToDec(gDate.Year);

      DS1302_WriteTime(gTime.Hours, gTime.Minutes, gTime.Seconds); //hr,min,seconds
      DS1302_WriteDate(gDate.Date, gDate.Month, gDate.Year); //date(1-31), month(1-12), year(0-99)
    HAL_Delay(1000);
    Lcd_Cmd(clear);
}


void Terminal_Send_Logs(void)
{     
      char buffer[50];
      char EEADDR[2];
      HAL_I2C_Mem_Read(&hi2c2, 0xA1, ‭0x7FFE‬,I2C_MEMADD_SIZE_16BIT, EEADDR, 0x0001, 100);
      HAL_I2C_Mem_Read(&hi2c2, 0xA1, 0x0000U,I2C_MEMADD_SIZE_16BIT, Lcd_TimeScreen, 0x0008U, 100);
      HAL_I2C_Mem_Read(&hi2c2, 0xA1, 0x0009U,I2C_MEMADD_SIZE_16BIT, Lcd_DateScreen, 0x0011U, 100);
      sprintf(buffer, "Time: %s  Date: %s  %d\r\n", Lcd_TimeScreen, Lcd_DateScreen, EEADDR);
      HAL_UART_Transmit(&huart2, buffer, 34,100);
}
/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  while(1)
  {

  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{ 
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
