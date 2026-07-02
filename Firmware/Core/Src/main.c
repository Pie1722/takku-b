/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
typedef enum
{
    SCREEN_TIME,
    SCREEN_DATE,
    SCREEN_YEAR,
	SCREEN_ALARM
} DisplayScreen_t;

typedef enum
{
    MODE_NORMAL,
    MODE_SET_HOUR,
    MODE_SET_MINUTE,
    MODE_SET_WEEKDAY,
    MODE_SET_DATE,
    MODE_SET_MONTH,
    MODE_SET_YEAR,
	MODE_SET_ALARM_HOUR,
	MODE_SET_ALARM_MINUTE
} WatchMode_t;
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
uint8_t matrix[4][4]; // 2D array for display

uint8_t hour_tens;
uint8_t hour_ones;
uint8_t min_tens;
uint8_t min_ones;

uint8_t day_tens;
uint8_t day_ones;
uint8_t date_tens;
uint8_t date_ones;

uint8_t year_tens;
uint8_t year_ones;
uint8_t month_tens;
uint8_t month_ones;

uint8_t editHour;
uint8_t editMinute;
uint8_t editWeekday;
uint8_t editDate;
uint8_t editMonth;
uint8_t editYear;

uint8_t alarmHour = 0;
uint8_t alarmMinute = 0;
uint8_t editAlarmHour;
uint8_t editAlarmMinute;
uint8_t alarmEnabled = 0;
uint8_t alarmActive = 0;        // Alarm currently sounding
uint32_t alarmStartTime = 0;    // When the alarm started
uint32_t alarmTimer = 0;        // Timing between beeps
uint8_t beepCount = 0;          // 0..3 beeps in the current burst
uint8_t buzzerOn = 0;           // 0 = OFF, 1 = ON

uint32_t displayTimeout = 0;
uint8_t displayEnabled = 0;

uint32_t buttonPressTime[3] = {0};
uint8_t buttonPressed[3] = {0};
uint32_t lastButtonInterrupt[3] = {0};   // debounce timer

uint8_t settingMode = 0;
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
RTC_HandleTypeDef hrtc;

TIM_HandleTypeDef htim2;
TIM_HandleTypeDef htim21;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_RTC_Init(void);
static void MX_TIM2_Init(void);
static void MX_TIM21_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/*
 * Store the pattern in memory matrix
 * The digit and the column where to print is sent.
 * Then the row is selected by the AND operation and check if the output is true or false (1 or 0)
 * 		digit = 6 = 0110
 * 		0110 * 1000 = 0 so the 8th bit row will turn off
 * 		0110 * 0100 = 1 so the 4th bit row will turn on
 * 		0110 * 0010 = 1 so the 2nd bit row will turn on
 * 		0110 * 0001 = 0 so the 1st bit row will turn off
 */
void SetColumnMem(uint8_t col, uint8_t digit)
{
    matrix[0][col] = (digit & 0x08) ? 1 : 0; // 8
    matrix[1][col] = (digit & 0x04) ? 1 : 0; // 4
    matrix[2][col] = (digit & 0x02) ? 1 : 0; // 2
    matrix[3][col] = (digit & 0x01) ? 1 : 0; // 1
}

// Turn off LEDs before the next pattern
void DisableAllColumns(void)
{
    HAL_GPIO_WritePin(GPIOA, COL_1_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(GPIOA, COL_2_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(GPIOA, COL_3_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(GPIOA, COL_4_Pin, GPIO_PIN_SET);
}

//------ Convert the patter stored in memory to GPIO ---------

/*
 * Check each matrix to see which row is to be set or reset.
 * if matrix[0][1] is set then set the row 4 pin
 */
void SetRows(uint8_t col)
{
    HAL_GPIO_WritePin(GPIOB, ROW_1_Pin,
        matrix[0][col] ? GPIO_PIN_SET : GPIO_PIN_RESET);

    HAL_GPIO_WritePin(GPIOB, ROW_2_Pin,
        matrix[1][col] ? GPIO_PIN_SET : GPIO_PIN_RESET);

    HAL_GPIO_WritePin(GPIOB, ROW_3_Pin,
        matrix[2][col] ? GPIO_PIN_SET : GPIO_PIN_RESET);

    HAL_GPIO_WritePin(GPIOB, ROW_4_Pin,
        matrix[3][col] ? GPIO_PIN_SET : GPIO_PIN_RESET);
}

// Reset the column based on the user column entry
void SetColumn(uint8_t col)
{
    switch(col)
    {
        case 0:
            HAL_GPIO_WritePin(GPIOA, COL_1_Pin, GPIO_PIN_RESET);
            break;

        case 1:
            HAL_GPIO_WritePin(GPIOA, COL_2_Pin, GPIO_PIN_RESET);
            break;

        case 2:
            HAL_GPIO_WritePin(GPIOA, COL_3_Pin, GPIO_PIN_RESET);
            break;

        case 3:
            HAL_GPIO_WritePin(GPIOA, COL_4_Pin, GPIO_PIN_RESET);
            break;
    }
}

void getTime(void)
{
	RTC_TimeTypeDef sTime;
	RTC_DateTypeDef sDate;

    /* Get the RTC current Time */
	HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BIN);
	/* Get the RTC current Date */
    HAL_RTC_GetDate(&hrtc, &sDate, RTC_FORMAT_BIN);

    hour_tens = sTime.Hours / 10;
    hour_ones = sTime.Hours % 10;
    min_tens  = sTime.Minutes / 10;
    min_ones  = sTime.Minutes % 10;

    // HH : MM
    SetColumnMem(0, hour_tens);
    SetColumnMem(1, hour_ones);
    SetColumnMem(2, min_tens);
    SetColumnMem(3, min_ones);
}

void getDate(void)
{
	RTC_TimeTypeDef sTime;
	RTC_DateTypeDef sDate;

    /* Get the RTC current Time */
	HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BIN);
	/* Get the RTC current Date */
    HAL_RTC_GetDate(&hrtc, &sDate, RTC_FORMAT_BIN);

    day_tens = sDate.WeekDay / 10;
    day_ones = sDate.WeekDay % 10;
    date_tens  = sDate.Date / 10;
    date_ones  = sDate.Date % 10;

    // WD : DD
    SetColumnMem(0, day_tens);
    SetColumnMem(1, day_ones);
    SetColumnMem(2, date_tens);
    SetColumnMem(3, date_ones);
}

void getYear(void)
{
	RTC_TimeTypeDef sTime;
	RTC_DateTypeDef sDate;

    /* Get the RTC current Time */
	HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BIN);
	/* Get the RTC current Date */
    HAL_RTC_GetDate(&hrtc, &sDate, RTC_FORMAT_BIN);

    month_tens = sDate.Month / 10;
    month_ones = sDate.Month % 10;
    year_tens  = sDate.Year / 10;
    year_ones  = sDate.Year % 10;

    // MM : YY
    SetColumnMem(0, month_tens);
    SetColumnMem(1, month_ones);
    SetColumnMem(2, year_tens);
    SetColumnMem(3, year_ones);
}

/*
 * Initialize the enum with default screen and mode and change the enum to enter the different modes.
 * See the functions below for more info
 */
DisplayScreen_t currentScreen = SCREEN_YEAR;
WatchMode_t currentMode = MODE_NORMAL;

/*
 * Entering to setting mode based on the screens
 * So if the currentScreen is SCREEN_TIME
 * then it should go into MODE_SET_HOUR to change the hour.
*/
void EnterSettingMode(void)
{
    RTC_TimeTypeDef sTime;
    RTC_DateTypeDef sDate;

    HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BIN);
    HAL_RTC_GetDate(&hrtc, &sDate, RTC_FORMAT_BIN);

    editHour = sTime.Hours;
    editMinute = sTime.Minutes;

    editWeekday = sDate.WeekDay;
    editDate = sDate.Date;

    editMonth = sDate.Month;
    editYear = sDate.Year;

    switch(currentScreen) // Check the currentScreen typedef and enter the appropriate setting modes
    {
        case SCREEN_TIME:
            currentMode = MODE_SET_HOUR; // Change the Hours
            break;

        case SCREEN_DATE:
            currentMode = MODE_SET_WEEKDAY; // Change the Weekday
            break;

        case SCREEN_YEAR:
            currentMode = MODE_SET_MONTH; // Change the month
            break;

        case SCREEN_ALARM:

            editAlarmHour = alarmHour;
            editAlarmMinute = alarmMinute;
            currentMode = MODE_SET_ALARM_HOUR;
            break;
    }
}

/*
 * Once we are inside setting a particular mode then we need to change the mode
 * to set the other field.
 * So after setting the hour we need to set the minutes.
 */
void NextField(void)
{
    switch(currentMode) // Check the currentMode typedef and go into the next mode to change the num
    {
        case MODE_SET_HOUR: // is current mode is in setting hour
            currentMode = MODE_SET_MINUTE; // then enter setting minute once this function is called
            break;
        case MODE_SET_MINUTE:
            currentMode = MODE_SET_HOUR;
            break;

        case MODE_SET_WEEKDAY:
            currentMode = MODE_SET_DATE;
            break;
        case MODE_SET_DATE:
            currentMode = MODE_SET_WEEKDAY;
            break;

        case MODE_SET_MONTH:
            currentMode = MODE_SET_YEAR;
            break;
        case MODE_SET_YEAR:
            currentMode = MODE_SET_MONTH;
            break;

        case MODE_SET_ALARM_HOUR:
            currentMode = MODE_SET_ALARM_MINUTE;
            break;
        case MODE_SET_ALARM_MINUTE:
            currentMode = MODE_SET_ALARM_HOUR;
            break;

        default:
            currentMode = MODE_NORMAL;
            break;
    }
}

/*
 * This function is to increment the values in each and every mode based on the
 * current mode.
 *
 * SCREEN_TIME:
 * 			MODE_SET_HOUR
 * 			MODE_SET_MINUTE
 * SCREEN_DAY:
 * 			MODE_SET_WEEKDAY
 * 			MODE_SET_DAY
 * SCREEN_YEAR:
 * 			MODE_SET_MONTH
 * 			MODE_SET_YEAR
 *
 * All the changes to the time is done to temp variables.
 */

void IncrementCurrentField(void)
{
    switch(currentMode)
    {
        case MODE_SET_HOUR:

            editHour++;

            if(editHour > 23)
                editHour = 0;

            break;

        case MODE_SET_MINUTE:

            editMinute++;

            if(editMinute > 59)
                editMinute = 0;

            break;

        case MODE_SET_WEEKDAY:

            editWeekday++;

            if(editWeekday > 7)
                editWeekday = 1;

            break;

        case MODE_SET_DATE:

            editDate++;

            if(editDate > 31)
                editDate = 1;

            break;

        case MODE_SET_MONTH:

            editMonth++;

            if(editMonth > 12)
                editMonth = 1;

            break;

        case MODE_SET_YEAR:

            editYear++;

            if(editYear > 99)
                editYear = 0;

            break;

        case MODE_SET_ALARM_HOUR:

            editAlarmHour++;

            if(editAlarmHour > 23)
            {
                editAlarmHour = 0;
            }

            break;

        case MODE_SET_ALARM_MINUTE:

            editAlarmMinute++;

            if(editAlarmMinute > 59)
            {
                editAlarmMinute = 0;
            }

            break;

        default:
            break;
    }
}

void ShowSettingValue(void)
{
    switch(currentScreen)
    {
        case SCREEN_TIME:

            SetColumnMem(0, editHour / 10);
            SetColumnMem(1, editHour % 10);
            SetColumnMem(2, editMinute / 10);
            SetColumnMem(3, editMinute % 10);

            break;

        case SCREEN_DATE:

            SetColumnMem(0, editWeekday / 10);
            SetColumnMem(1, editWeekday % 10);
            SetColumnMem(2, editDate / 10);
            SetColumnMem(3, editDate % 10);

            break;

        case SCREEN_YEAR:

            SetColumnMem(0, editMonth / 10);
            SetColumnMem(1, editMonth % 10);
            SetColumnMem(2, editYear / 10);
            SetColumnMem(3, editYear % 10);

            break;

        case SCREEN_ALARM:

            SetColumnMem(0, editAlarmHour / 10);
            SetColumnMem(1, editAlarmHour % 10);
            SetColumnMem(2, editAlarmMinute / 10);
            SetColumnMem(3, editAlarmMinute % 10);

            break;
    }
}

void beep()
{

    HAL_TIM_PWM_Start(&htim21, TIM_CHANNEL_2);

    HAL_Delay(20);

    HAL_TIM_PWM_Stop(&htim21, TIM_CHANNEL_2);
}

void getAlarm(void)
{
    SetColumnMem(0, alarmHour / 10);
    SetColumnMem(1, alarmHour % 10);
    SetColumnMem(2, alarmMinute / 10);
    SetColumnMem(3, alarmMinute % 10);

    if(alarmEnabled)
    {
        matrix[0][0] = 1;
    }
    else
    {
        matrix[0][0] = 0;
    }
}
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_RTC_Init();
  MX_TIM2_Init();
  MX_TIM21_Init();
  /* USER CODE BEGIN 2 */
  HAL_TIM_Base_Start_IT(&htim2);
  __HAL_TIM_SET_COMPARE(&htim21, TIM_CHANNEL_2, 50);

  // read the alarm  from eeprom
  alarmHour = *(uint8_t *)0x08080000;
  alarmMinute = *(uint8_t *)0x08080001;
//beep();
//  __HAL_TIM_SET_COMPARE(&htim21, TIM_CHANNEL_2, 250);
//  HAL_TIM_PWM_Start(&htim21, TIM_CHANNEL_2);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
	  if(alarmActive && buttonPressed[0])
	  {
	      alarmActive = 0;
	      HAL_TIM_PWM_Stop(&htim21, TIM_CHANNEL_2);
	      buttonPressed[0] = 0;
	  }

	  if(alarmActive)
	  {
	      uint32_t now = HAL_GetTick();

	      // Stop after 20 seconds
	      if((now - alarmStartTime) >= 20000)
	      {
	          alarmActive = 0;
	          HAL_TIM_PWM_Stop(&htim21, TIM_CHANNEL_2);
	      }
	      else
	      {
	          if(buzzerOn)
	          {
	              // ON for 80ms
	              if((now - alarmTimer) >= 80)
	              {
	                  HAL_TIM_PWM_Stop(&htim21, TIM_CHANNEL_2);

	                  buzzerOn = 0;
	                  alarmTimer = now;

	                  beepCount++;
	              }
	          }
	          else
	          {
	              if(beepCount < 4)
	              {
	                  // OFF between beeps (80ms)
	                  if((now - alarmTimer) >= 80)
	                  {
	                      HAL_TIM_PWM_Start(&htim21, TIM_CHANNEL_2);

	                      buzzerOn = 1;
	                      alarmTimer = now;
	                  }
	              }
	              else
	              {
	                  // Pause after 4 beeps (250ms)
	                  if((now - alarmTimer) >= 500)
	                  {
	                      beepCount = 0;

	                      HAL_TIM_PWM_Start(&htim21, TIM_CHANNEL_2);

	                      buzzerOn = 1;
	                      alarmTimer = now;
	                  }
	              }
	          }
	      }
	  }

	  if(currentMode == MODE_NORMAL)
	  {
	      if(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_2) == GPIO_PIN_RESET)
	      {
	          uint32_t now = HAL_GetTick();

	          if(buttonPressed[2] == 0 &&
	             (now - lastButtonInterrupt[2]) > 100)
	          {
	              if(currentScreen != SCREEN_ALARM)
	              {
	                  currentScreen = SCREEN_ALARM;
	                  getAlarm();
	              }
	              else
	              {
	                  currentScreen = SCREEN_TIME;
	                  getTime();
	              }

	              displayEnabled = 1;
	              displayTimeout = HAL_GetTick();

	              beep();

	              buttonPressed[2] = 1;
	              lastButtonInterrupt[2] = now;
	          }
	      }
	      else
	      {
	          buttonPressed[2] = 0;
	      }
	  }

	  if(currentMode == MODE_NORMAL && currentScreen == SCREEN_ALARM)
	  {
	      if(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_1) == GPIO_PIN_RESET)
	      {
	          uint32_t now = HAL_GetTick();

	          if(buttonPressed[1] == 0 &&
	              (now - lastButtonInterrupt[1]) > 100)
	          {
	              alarmEnabled = !alarmEnabled;

	              if(alarmEnabled)
	              {
	                  RTC_AlarmTypeDef sAlarm = {0};

	                  sAlarm.AlarmTime.Hours = alarmHour;
	                  sAlarm.AlarmTime.Minutes = alarmMinute;
	                  sAlarm.AlarmTime.Seconds = 0;
	                  sAlarm.AlarmTime.SubSeconds = 0;
	                  sAlarm.AlarmTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
	                  sAlarm.AlarmTime.StoreOperation = RTC_STOREOPERATION_RESET;
	                  sAlarm.AlarmMask = RTC_ALARMMASK_DATEWEEKDAY;
	                  sAlarm.AlarmSubSecondMask = RTC_ALARMSUBSECONDMASK_ALL;
	                  sAlarm.AlarmDateWeekDaySel = RTC_ALARMDATEWEEKDAYSEL_DATE;
	                  sAlarm.AlarmDateWeekDay = 1;
	                  sAlarm.Alarm = RTC_ALARM_A;

	                  HAL_RTC_SetAlarm_IT(&hrtc, &sAlarm, RTC_FORMAT_BIN);

	                  matrix[0][0] = 1;
	              }
	              else
	              {
	                  HAL_RTC_DeactivateAlarm(&hrtc, RTC_ALARM_A);
	                  matrix[0][0] = 0;
	              }

	              beep();

	              buttonPressed[1] = 1;
	              lastButtonInterrupt[1] = now;
	          }
	      }
	      else
	      {
	          buttonPressed[1] = 0;
	      }
	  }

	  if(currentMode != MODE_NORMAL)
	  {
	      displayEnabled = 1;
	      ShowSettingValue();

	      if(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_1) == GPIO_PIN_RESET)
	      {
	          uint32_t now = HAL_GetTick();

	          if(buttonPressed[1] == 0 &&
	             (now - lastButtonInterrupt[1]) > 100)
	          {
	              IncrementCurrentField();

	              buttonPressed[1] = 1;
	              lastButtonInterrupt[1] = now;
	          }
	      }
	      else
	      {
	          buttonPressed[1] = 0;
	      }


	      if(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_2) == GPIO_PIN_RESET)
	      {
	          uint32_t now = HAL_GetTick();

	          if(buttonPressed[2] == 0 &&
	             (now - lastButtonInterrupt[2]) > 50)
	          {
	              NextField();
	              beep();

	              buttonPressed[2] = 1;
	              lastButtonInterrupt[2] = now;
	          }
	      }
	      else
	      {
	          buttonPressed[2] = 0;
	      }


	      if(buttonPressed[0])
	      {
	          if(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_0) == GPIO_PIN_RESET)
	          {
	              if((HAL_GetTick() - buttonPressTime[0]) >= 1000)
	              {
	            	  RTC_TimeTypeDef sTime;
	            	  RTC_DateTypeDef sDate;

	            	  HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BIN);
	            	  HAL_RTC_GetDate(&hrtc, &sDate, RTC_FORMAT_BIN);

	            	  sTime.Hours = editHour;
	            	  sTime.Minutes = editMinute;

	            	  sDate.WeekDay = editWeekday;
	            	  sDate.Date = editDate;
	            	  sDate.Month = editMonth;
	            	  sDate.Year = editYear;

	            	  HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BIN);
	            	  HAL_RTC_SetDate(&hrtc, &sDate, RTC_FORMAT_BIN);

	            	  if(currentScreen == SCREEN_ALARM)
	            	  {
	            	      alarmHour = editAlarmHour;
	            	      alarmMinute = editAlarmMinute;

	            	      // save into eeprom so the memory retains even after powerloss
	            	      HAL_FLASHEx_DATAEEPROM_Unlock();

	            	      HAL_FLASHEx_DATAEEPROM_Program(FLASH_TYPEPROGRAMDATA_BYTE,
	            	                                     0x08080000,
	            	                                     alarmHour);

	            	      HAL_FLASHEx_DATAEEPROM_Program(FLASH_TYPEPROGRAMDATA_BYTE,
	            	                                     0x08080001,
	            	                                     alarmMinute);

	            	      HAL_FLASHEx_DATAEEPROM_Lock();
	            	  }

            	      currentMode = MODE_NORMAL;
            	      settingMode = 0;

            	      buttonPressed[0] = 0;
            	      buttonPressTime[0] = 0;

            	      displayEnabled = 1;
            	      displayTimeout = HAL_GetTick();

            	      beep();
	              }
	          }
	      }
	      continue;
	  }

	  if(buttonPressed[0])
	  {
	      if(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_0) == GPIO_PIN_RESET)
	      {
	    	  if(displayEnabled){
		          if((HAL_GetTick() - buttonPressTime[0]) >= 1000)
		          {
		              buttonPressed[0] = 0;   // prevent repeated entry
		              settingMode = 1;
		              beep();
		              EnterSettingMode();
		          }
	    	  }
	      }
	      else
	      {

	    	  displayEnabled = 1;
	          displayTimeout = HAL_GetTick();

	          switch(currentScreen)
	          {
	          	  case SCREEN_TIME:
	          		  currentScreen = SCREEN_DATE;
	                  getDate();
	                  //beep();
	                  break;

	              case SCREEN_DATE:
	                  currentScreen = SCREEN_YEAR;
	                  getYear();
	                  //beep();
	                  break;

	              case SCREEN_YEAR:
	                  currentScreen = SCREEN_TIME;
	                  getTime();
	                  beep();
	                  break;

	              case SCREEN_ALARM:
	                  // Do nothing.
	                  // PB0 press has no function on the alarm screen.
	                  break;
	          }
	          buttonPressed[0] = 0;
	      }
	  }

	  // Sleep timeout only in normal mode, in setting mode disable timeout

	  if(currentMode == MODE_NORMAL)
	  {
		  if(displayEnabled)
		  {
			  if((HAL_GetTick() - displayTimeout) >= 5000)
			  {
				  displayEnabled = 0;
				  DisableAllColumns();
				  currentScreen = SCREEN_YEAR; // SHOW SCREEN_TIME AFTER WAKEUP. SEE EXTI_CALLBACK() FOR MORE INFO

				  HAL_SuspendTick();

				  HAL_TIM_Base_Stop_IT(&htim2);
				  HAL_TIM_PWM_Stop(&htim21, TIM_CHANNEL_2);

				  __HAL_RCC_TIM2_CLK_DISABLE();
				  __HAL_RCC_TIM21_CLK_DISABLE();

				  HAL_PWREx_EnableUltraLowPower();
				  HAL_PWREx_EnableFastWakeUp();

				  HAL_PWR_EnterSTOPMode(PWR_LOWPOWERREGULATOR_ON,PWR_STOPENTRY_WFI);

				  // RESUME CPU

				  SystemClock_Config();

				  HAL_ResumeTick();

				  __HAL_RCC_TIM2_CLK_ENABLE();
				  __HAL_RCC_TIM21_CLK_ENABLE();

				  HAL_TIM_Base_Start_IT(&htim2);
			  }
		  }
	  }
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Configure LSE Drive Capability
  */
  HAL_PWR_EnableBkUpAccess();
  __HAL_RCC_LSEDRIVE_CONFIG(RCC_LSEDRIVE_LOW);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI|RCC_OSCILLATORTYPE_LSE;
  RCC_OscInitStruct.LSEState = RCC_LSE_ON;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_RTC;
  PeriphClkInit.RTCClockSelection = RCC_RTCCLKSOURCE_LSE;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief RTC Initialization Function
  * @param None
  * @retval None
  */
static void MX_RTC_Init(void)
{

  /* USER CODE BEGIN RTC_Init 0 */

  /* USER CODE END RTC_Init 0 */

  RTC_TimeTypeDef sTime = {0};
  RTC_DateTypeDef sDate = {0};
  RTC_AlarmTypeDef sAlarm = {0};

  /* USER CODE BEGIN RTC_Init 1 */

  /* USER CODE END RTC_Init 1 */

  /** Initialize RTC Only
  */
  hrtc.Instance = RTC;
  hrtc.Init.HourFormat = RTC_HOURFORMAT_24;
  hrtc.Init.AsynchPrediv = 127;
  hrtc.Init.SynchPrediv = 255;
  hrtc.Init.OutPut = RTC_OUTPUT_DISABLE;
  hrtc.Init.OutPutRemap = RTC_OUTPUT_REMAP_NONE;
  hrtc.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;
  hrtc.Init.OutPutType = RTC_OUTPUT_TYPE_OPENDRAIN;
  if (HAL_RTC_Init(&hrtc) != HAL_OK)
  {
    Error_Handler();
  }

  /* USER CODE BEGIN Check_RTC_BKUP */

  /* USER CODE END Check_RTC_BKUP */

  /** Initialize RTC and set the Time and Date
  */
  sTime.Hours = 0;
  sTime.Minutes = 0;
  sTime.Seconds = 0;
  sTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
  sTime.StoreOperation = RTC_STOREOPERATION_RESET;
  if (HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BIN) != HAL_OK)
  {
    Error_Handler();
  }
  sDate.WeekDay = RTC_WEEKDAY_MONDAY;
  sDate.Month = RTC_MONTH_JANUARY;
  sDate.Date = 1;
  sDate.Year = 0;

  if (HAL_RTC_SetDate(&hrtc, &sDate, RTC_FORMAT_BIN) != HAL_OK)
  {
    Error_Handler();
  }

  /** Enable the Alarm A
  */
  sAlarm.AlarmTime.Hours = 0;
  sAlarm.AlarmTime.Minutes = 0;
  sAlarm.AlarmTime.Seconds = 0;
  sAlarm.AlarmTime.SubSeconds = 0;
  sAlarm.AlarmTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
  sAlarm.AlarmTime.StoreOperation = RTC_STOREOPERATION_RESET;
  sAlarm.AlarmMask = RTC_ALARMMASK_NONE;
  sAlarm.AlarmSubSecondMask = RTC_ALARMSUBSECONDMASK_ALL;
  sAlarm.AlarmDateWeekDaySel = RTC_ALARMDATEWEEKDAYSEL_DATE;
  sAlarm.AlarmDateWeekDay = 1;
  sAlarm.Alarm = RTC_ALARM_A;
  if (HAL_RTC_SetAlarm_IT(&hrtc, &sAlarm, RTC_FORMAT_BIN) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN RTC_Init 2 */

  /* USER CODE END RTC_Init 2 */

}

/**
  * @brief TIM2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM2_Init(void)
{

  /* USER CODE BEGIN TIM2_Init 0 */

  /* USER CODE END TIM2_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM2_Init 1 */

  /* USER CODE END TIM2_Init 1 */
  htim2.Instance = TIM2;
  htim2.Init.Prescaler = 16-1;
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = 1000-1;
  htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
  if (HAL_TIM_Base_Init(&htim2) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim2, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM2_Init 2 */

  /* USER CODE END TIM2_Init 2 */

}

/**
  * @brief TIM21 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM21_Init(void)
{

  /* USER CODE BEGIN TIM21_Init 0 */

  /* USER CODE END TIM21_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};

  /* USER CODE BEGIN TIM21_Init 1 */

  /* USER CODE END TIM21_Init 1 */
  htim21.Instance = TIM21;
  htim21.Init.Prescaler = 7;
  htim21.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim21.Init.Period = 499;
  htim21.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim21.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
  if (HAL_TIM_Base_Init(&htim21) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim21, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_Init(&htim21) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim21, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 0;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  if (HAL_TIM_PWM_ConfigChannel(&htim21, &sConfigOC, TIM_CHANNEL_2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM21_Init 2 */

  /* USER CODE END TIM21_Init 2 */
  HAL_TIM_MspPostInit(&htim21);

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  /* USER CODE BEGIN MX_GPIO_Init_1 */

  /* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, ROW_1_Pin|ROW_2_Pin|ROW_3_Pin|ROW_4_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, COL_1_Pin|COL_2_Pin|COL_3_Pin|COL_4_Pin, GPIO_PIN_SET);

  /*Configure GPIO pins : PC0 PC13 */
  GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_13;
  GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pins : PH0 PH1 */
  GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1;
  GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOH, &GPIO_InitStruct);

  /*Configure GPIO pins : PA0 PA1 PA2 PA4
                           PA5 PA6 PA7 PA12
                           PA13 PA14 PA15 */
  GPIO_InitStruct.Pin = GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_4
                          |GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7|GPIO_PIN_12
                          |GPIO_PIN_13|GPIO_PIN_14|GPIO_PIN_15;
  GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pin : PB0 */
  GPIO_InitStruct.Pin = GPIO_PIN_0;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pins : PB1 PB2 */
  GPIO_InitStruct.Pin = GPIO_PIN_1|GPIO_PIN_2;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pins : PB10 PB11 PB3 PB4
                           PB5 PB6 PB7 PB8
                           PB9 */
  GPIO_InitStruct.Pin = GPIO_PIN_10|GPIO_PIN_11|GPIO_PIN_3|GPIO_PIN_4
                          |GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7|GPIO_PIN_8
                          |GPIO_PIN_9;
  GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pins : ROW_1_Pin ROW_2_Pin ROW_3_Pin ROW_4_Pin */
  GPIO_InitStruct.Pin = ROW_1_Pin|ROW_2_Pin|ROW_3_Pin|ROW_4_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pins : COL_1_Pin COL_2_Pin COL_3_Pin COL_4_Pin */
  GPIO_InitStruct.Pin = COL_1_Pin|COL_2_Pin|COL_3_Pin|COL_4_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI0_1_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI0_1_IRQn);

  /* USER CODE BEGIN MX_GPIO_Init_2 */

  /* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */
volatile uint8_t currentColumn = 0;

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if(htim->Instance == TIM2)
    {
        if(!displayEnabled)
        {
            DisableAllColumns();
            return;
        }

        DisableAllColumns();

        SetRows(currentColumn);

        SetColumn(currentColumn);

        currentColumn++;

        if(currentColumn >= 4)
        {
            currentColumn = 0;
        }
    }
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    if(GPIO_Pin == GPIO_PIN_0)
    {
        uint32_t now = HAL_GetTick();

        // 100ms debounce
        if((now - lastButtonInterrupt[0]) < 100)
            return;

        lastButtonInterrupt[0] = now;

        buttonPressTime[0] = now;
        buttonPressed[0] = 1;
    }
}

void HAL_RTC_AlarmAEventCallback(RTC_HandleTypeDef *hrtc)
{
    displayEnabled = 1;
    displayTimeout = HAL_GetTick();

    alarmActive = 1;

    alarmStartTime = HAL_GetTick();
    alarmTimer = HAL_GetTick();

    beepCount = 0;
    buzzerOn = 0;
    //beep();
}

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}
#ifdef USE_FULL_ASSERT
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
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
