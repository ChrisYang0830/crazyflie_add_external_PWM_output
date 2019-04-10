#include <stdbool.h>

/* ST includes */
#include "stm32fxxx.h"

#include "transform.h"

//FreeRTOS includes
#include "FreeRTOS.h"
#include "task.h"

// TIMER5, CH3 and CH4
// HW defines
#define transform_TIM_PERIF       RCC_APB1Periph_TIM5
#define transform_TIM             TIM5
#define transform_TIM_DBG         DBGMCU_TIM2_STOP
#define transform_TIM_SETCOMPARE  TIM_SetCompare2
#define transform_TIM_GETCAPTURE  TIM_GetCapture2

#define transform_GPIO_1_PERIF         RCC_AHB1Periph_GPIOA
#define transform_GPIO_1_PORT          GPIOA
#define transform_GPIO_1_PIN           GPIO_Pin_2 // TIM5_CH3
#define transform_GPIO_AF_1_PIN        GPIO_PinSource2
#define transform_GPIO_AF_1            GPIO_AF_TIM5

#define transform_GPIO_2_PERIF         RCC_AHB1Periph_GPIOA
#define transform_GPIO_2_PORT          GPIOA
#define transform_GPIO_2_PIN           GPIO_Pin_3 // TIM5_CH4
#define transform_GPIO_AF_2_PIN        GPIO_PinSource3
#define transform_GPIO_AF_2            GPIO_AF_TIM5

#define transform_PWM_BITS      (8)
#define transform_PWM_PERIOD    ((1<<transform_PWM_BITS) - 1)
#define transform_PWM_PRESCALE  (0)

/* This should be calculated.. */
#define transform_BASE_FREQ (329500)

static bool isInit = false;

/* Public functions */

void transformInit()
{
  if (isInit)
    return;

  //Init structures
  GPIO_InitTypeDef GPIO_InitStructure;
  TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
  TIM_OCInitTypeDef  TIM_OCInitStructure;

  //Clock the gpio and the timers
  RCC_AHB1PeriphClockCmd(transform_GPIO_1_PERIF | transform_GPIO_2_PERIF, ENABLE);
  RCC_APB1PeriphClockCmd(transform_TIM_PERIF, ENABLE);

  // Configure the GPIO for the timer output
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_DOWN;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;

  GPIO_InitStructure.GPIO_Pin = transform_GPIO_1_PIN;
  GPIO_Init(transform_GPIO_1_PORT, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin = transform_GPIO_2_PIN;
  GPIO_Init(transform_GPIO_2_PORT, &GPIO_InitStructure);

  //Map timers to alternate functions
  GPIO_PinAFConfig(transform_GPIO_1_PORT, transform_GPIO_AF_1_PIN, transform_GPIO_AF_1);
  GPIO_PinAFConfig(transform_GPIO_2_PORT, transform_GPIO_AF_2_PIN, transform_GPIO_AF_2);

  //Timer configuration
  TIM_TimeBaseStructure.TIM_Period = transform_PWM_PERIOD;
  TIM_TimeBaseStructure.TIM_Prescaler = transform_PWM_PRESCALE;
  TIM_TimeBaseStructure.TIM_ClockDivision = 0;
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
  TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;
  TIM_TimeBaseInit(transform_TIM, &TIM_TimeBaseStructure);

  // PWM channels configuration
  TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
  TIM_OCInitStructure.TIM_Pulse = 0;
  TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;
  TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Set;

  // Configure OC3
  TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
  TIM_OC3Init(transform_TIM, &TIM_OCInitStructure);
  TIM_OC3PreloadConfig(transform_TIM, TIM_OCPreload_Enable);

  // Configure OC4 inverted
  TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
  TIM_OC4Init(transform_TIM, &TIM_OCInitStructure);
  TIM_OC4PreloadConfig(transform_TIM, TIM_OCPreload_Enable);

  //Enable the timer PWM outputs
  TIM_CtrlPWMOutputs(transform_TIM, ENABLE);
  TIM_SetCompare3(transform_TIM, 0x00);
  TIM_SetCompare4(transform_TIM, 0x00);

  //Enable the timer
  TIM_Cmd(transform_TIM, ENABLE);

  isInit = true;
}

bool transformDriverTest(void)
{
  return isInit;
}

//set the PWM duty cycle from 0 to 255
void transformDriverSetRatio(uint8_t ratio1, uint8_t ratio2)
{
  TIM_SetCompare3(transform_TIM, ratio1);
  TIM_SetCompare4(transform_TIM, ratio2);
}

//set PWM frequency
void transformDriverSetFreq(uint16_t freq)
{
  TIM_PrescalerConfig(transform_TIM, (transform_BASE_FREQ/freq), TIM_PSCReloadMode_Update);
}
