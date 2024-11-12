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
#include "task_led.h"

/********************** macros and definitions *******************************/

#define QUEUE_LENGTH_ (3)
#define QUEUE_ITEM_SIZE_ (sizeof(ao_led_message_t))
#define AO_LED_BLINK_TIME (1000) /*< milliseconds */

/********************** internal data declaration ****************************/

/********************** internal functions declaration ***********************/

/********************** internal data definition *****************************/

typedef struct {
  GPIO_TypeDef *led_port;
  uint16_t led_pin;
} ao_led_data_t;

/********************** external data definition *****************************/

/********************** internal functions definition ************************/

static void ao_led_ev_f(ao_msg_t *ao_msg) {
  ao_led_data_t *ao_data = (ao_led_data_t *)ao_get_data(ao_msg->receiver);
  ao_led_message_t msg = *((ao_led_message_t *)ao_msg->ao_msg);

  if (AO_LED_MESSAGE_ON == msg) {
	LOGGER_INFO("Turning on AO led [Port:%p][Pin:%d]", ao_data->led_port, (int)ao_data->led_pin);
    HAL_GPIO_WritePin((GPIO_TypeDef *)ao_data->led_port,
                      (uint16_t)ao_data->led_pin, GPIO_PIN_SET);
  }
  if (AO_LED_MESSAGE_OFF == msg) {
	LOGGER_INFO("Turning off AO led [Port:%p][Pin:%d]", ao_data->led_port, (int)ao_data->led_pin);
    HAL_GPIO_WritePin((GPIO_TypeDef *)ao_data->led_port,
                      (uint16_t)ao_data->led_pin, GPIO_PIN_RESET);
  }

  // Free AO message from sender.
  ao_sender_free_method(ao_msg->sender, ao_msg);
}

/********************** external functions definition ************************/

ao_t ao_led_init(GPIO_TypeDef *led_port, uint16_t led_pin) {
  ao_led_data_t ao_led_data = {.led_pin = led_pin, .led_port = led_port};
  ao_t ao = ao_init((uint8_t *)&ao_led_data, sizeof(ao_led_data), ao_led_ev_f,
                    NULL, (AO_OP_NO_QUEUE | AO_OP_NO_TASK));
  return ao;
}

/********************** end of file ******************************************/
