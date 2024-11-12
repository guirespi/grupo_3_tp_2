/*
 * Copyright (c) 2023 Sebastian Bedin <sebabedin@gmail.com>.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its
 *    contributors may be used to endorse or promote products derived from
 *    this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 * @author : Sebastian Bedin <sebabedin@gmail.com>
 */

/********************** inclusions *******************************************/

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#include "board.h"
#include "cmsis_os.h"
#include "dwt.h"
#include "logger.h"
#include "main.h"

#include "ao_api.h"
#include "task_ui.h"

/********************** macros and definitions *******************************/

#define TASK_PERIOD_MS_ (50)

#define BUTTON_PERIOD_MS_ (TASK_PERIOD_MS_)
#define BUTTON_MAX_IDLE_MS_ 	(10*1000)
#define BUTTON_PULSE_TIMEOUT_ 	(200)
#define BUTTON_SHORT_TIMEOUT_ 	(1000)
#define BUTTON_LONG_TIMEOUT_ 	(2000)

/********************** internal data declaration ****************************/

/********************** internal functions declaration ***********************/

/********************** internal data definition *****************************/

/********************** external data definition *****************************/

extern ao_t ao_ui;
extern SemaphoreHandle_t os_sem_h;

/********************** internal functions definition ************************/

typedef enum {
  BUTTON_TYPE_NONE,
  BUTTON_TYPE_PULSE,
  BUTTON_TYPE_SHORT,
  BUTTON_TYPE_LONG,
  BUTTON_TYPE__N,
} button_type_t;

static struct {
  uint32_t counter;
  uint32_t counter_idle;
} button;

static void button_init_(void) { button.counter = 0; }

static button_type_t button_process_state_(bool value) {
  button_type_t ret = BUTTON_TYPE_NONE;
  if (value) {
    button.counter += BUTTON_PERIOD_MS_;
  } else {
    if (BUTTON_LONG_TIMEOUT_ <= button.counter) {
      ret = BUTTON_TYPE_LONG;
    } else if (BUTTON_SHORT_TIMEOUT_ <= button.counter) {
      ret = BUTTON_TYPE_SHORT;
    } else if (BUTTON_PULSE_TIMEOUT_ <= button.counter) {
      ret = BUTTON_TYPE_PULSE;
    }
    button.counter = 0;
  }
  return ret;
}

/********************** external functions definition ************************/

void task_button(void *argument) {
  button_init_();

  while (true) {

	xSemaphoreTake(os_sem_h, portMAX_DELAY); // Critical section as this task can create and start destruction of resources.
	{
	    GPIO_PinState button_state;
	    button_state = HAL_GPIO_ReadPin(BUTTON_PORT, BUTTON_PIN);

	    button_type_t button_type;
	    button_type = button_process_state_(button_state);
	    ao_ui_message_t ui_msg = AO_UI_PRESS_NONE;


	    switch (button_type) {
	    case BUTTON_TYPE_NONE:
	    {
	        button.counter_idle += BUTTON_PERIOD_MS_;
	        if(button.counter_idle >= BUTTON_MAX_IDLE_MS_)
	        {
	        	if(ao_ui_get_state() != AO_UI_IDLE) //Avoid double destruction of UI object.
	        	{
	        		LOGGER_INFO("Button idle. Starting shutdown to save resources");
	        		ui_msg = AO_UI_PRESS_IDLE;
	        	}
	        }
	        break;
	    }
	    case BUTTON_TYPE_PULSE:
	    {
	      LOGGER_INFO("Button pulse");
	      ui_msg = AO_UI_PRESS_PULSE;
	      break;
	    }
	    case BUTTON_TYPE_SHORT:
	    {
	      LOGGER_INFO("Button short");
	      ui_msg = AO_UI_PRESS_SHORT;
	      break;
	    }
	    case BUTTON_TYPE_LONG:
	    {
	      LOGGER_INFO("Button long");
	      ui_msg = AO_UI_PRESS_LONG;
	      break;
	    }
	    default:
	    {
	      LOGGER_INFO("Button error");
	      break;
	    }
	    }

	    if(button_type != BUTTON_TYPE_NONE)
	    {
	    	//We receive a new external event. Re-allocate resources.
	    	button.counter_idle = 0;
	        if(ao_ui_get_state() == AO_UI_IDLE)
	        {
	        	LOGGER_INFO("Creating OS resources as external event happened");
	        	ao_ui = ao_ui_init();
	        }
	    }

	    if(ui_msg != AO_UI_PRESS_NONE)
	    {
	        ao_send_message(ao_ui, NULL, (uint8_t *)&ui_msg, sizeof(ui_msg));
	    }

		xSemaphoreGive(os_sem_h);
	}

    vTaskDelay((TickType_t)(TASK_PERIOD_MS_ / portTICK_PERIOD_MS));
  }
}

/********************** end of file ******************************************/
