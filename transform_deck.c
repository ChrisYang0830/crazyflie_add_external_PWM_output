/**
 *    ||          ____  _ __
 * +------+      / __ )(_) /_______________ _____  ___
 * | 0xBC |     / __  / / __/ ___/ ___/ __ `/_  / / _ \
 * +------+    / /_/ / / /_/ /__/ /  / /_/ / / /_/  __/
 *  ||  ||    /_____/_/\__/\___/_/   \__,_/ /___/\___/
 *
 * Crazyflie control firmware
 *
 * Copyright (C) 2015 BitCraze AB
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, in version 3.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 *
 * transform.c - Deck driver for the transformation function
 */

#include <stdint.h>
#include <stdlib.h>
#include "stm32fxxx.h"

#include "FreeRTOS.h"
#include "timers.h"

#include "deck.h"
#include "param.h"
#include "Delay.h"

//equivalent to piezo
#include "transform.h"

#define transform_expand HIGH
#define transform_shrink LOW

//The transform function port is mapped to the deck expansion pin IO1
#define transform_IN DECK_GPIO_IO1
static uint8_t tfmEnable = 0; //the variable to switch transformation
typedef enum{
	expand = 0,
	shrink = 1
} Transforms;
static Transforms transform;

//Parameters for controlling PWM
//
static void transformDeckPWM(uint8_t power1, uint8_t power2)
{
	transformDriverSetRatio(power1,power2);
	transformDriverSetFreq(50);
}

static void transformDeckOff()
{
  transformDriverSetRatio(25,25);
}


static void transformSwitch(bool on)
{
	if (on)
		{
		transformDeckPWM(10,10);
		}
	else
		{
		transformDeckOff();
		}
	}

// Timer loop and handle
static xTimerHandle timer;
static void transformTimer(xTimerHandle timer)
{
	transformSwitch(tfmEnable);
	}


static void transformDeckInit(DeckInfo *info)
{
	transformInit();
	pinMode(transform_IN, OUTPUT);
//	digitalWrite(transform_IN, transform_shrink);

	timer = xTimerCreate( "transformTimer", M2T(10),
			pdTRUE, NULL, transformTimer);
	xTimerStart(timer, 100);
	}

PARAM_GROUP_START(transform_deck)
PARAM_ADD(PARAM_UINT8, state, &transform)
PARAM_ADD(PARAM_UINT8, tfmEnable, &tfmEnable)
PARAM_GROUP_STOP(transform_deck)

static const DeckDriver transform_deck = {
		.vid = 0,
		.pid = 0,
		.name = "transformDeck",
		//Timer5 is used by the pwm in transform.c
		//TX2 is the PWM signals
		//PB8 is IO1
		.usedPeriph = DECK_USING_TIMER5,
		.usedGpio =  DECK_USING_TX2 | DECK_USING_RX2 | DECK_USING_PB5 | DECK_USING_PB8,

		.init = transformDeckInit,
};
DECK_DRIVER(transform_deck);
