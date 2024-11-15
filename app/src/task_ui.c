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

#include "task_led.h"
#include "task_ui.h"

/********************** macros and definitions *******************************/

#define AO_UI_QUEUE_LENGTH_ (3)
#define AO_UI_QUEUE_ITEM_SIZE_ (sizeof(ao_ui_message_t))

/********************** internal data declaration ****************************/

/********************** internal functions declaration ***********************/

/********************** internal data definition *****************************/

static ao_ui_state_t ao_ui_state = AO_UI_IDLE;

/********************** external data definition *****************************/

ao_t ao_led_r;
ao_t ao_led_b;
ao_t ao_led_g;

extern SemaphoreHandle_t os_sem_h;

/********************** internal functions definition ************************/

static void ao_ui_turn_off_previous_led(ao_t ao_ui,
                                        ao_ui_state_t ao_previous_state) {
  ao_led_message_t ao_led_msg = AO_LED_MESSAGE_OFF;
  if (ao_previous_state == AO_UI_LED_RED_ON)
    ao_send_message(ao_led_r, ao_ui, &ao_led_msg, sizeof(ao_led_msg));

  if (ao_previous_state == AO_UI_LED_GREEN_ON)
    ao_send_message(ao_led_g, ao_ui, &ao_led_msg, sizeof(ao_led_msg));

  if (ao_previous_state == AO_UI_LED_BLUE_ON)
    ao_send_message(ao_led_b, ao_ui, &ao_led_msg, sizeof(ao_led_msg));
}

/********************** external functions definition ************************/

static void ao_ui_ev_f(ao_msg_t *ao_msg) {
  ao_ui_message_t ao_message = *(ao_ui_message_t *)ao_msg->ao_msg;
  // For this example we will send to turn on the LED
  ao_led_message_t ao_led_msg = AO_LED_MESSAGE_ON;
  bool need_turn_off = false, need_destroy = false;
  ao_ui_state_t ao_ui_previous = ao_ui_state;
  ao_t ao_led_target = NULL;

  // The AO receiver is the same AO for user interface (UI)
  switch (ao_message) {
  case AO_UI_PRESS_PULSE: {
    if (ao_ui_state != AO_UI_LED_RED_ON) {
      need_turn_off = true;
      ao_led_target = ao_led_r;
      ao_ui_state = AO_UI_LED_RED_ON;
    }
    break;
  }
  case AO_UI_PRESS_SHORT: {
    if (ao_ui_state != AO_UI_LED_GREEN_ON) {
      need_turn_off = true;
      ao_led_target = ao_led_g;
      ao_ui_state = AO_UI_LED_GREEN_ON;
    }
    break;
  }
  case AO_UI_PRESS_LONG: {
    if (ao_ui_state != AO_UI_LED_BLUE_ON) {
      need_turn_off = true;
      ao_led_target = ao_led_b;
      ao_ui_state = AO_UI_LED_BLUE_ON;
    }
    break;
  }
  case AO_UI_PRESS_IDLE: {
    xSemaphoreTake(
        os_sem_h,
        portMAX_DELAY); // Critical section. Start resource destruction
    {
      LOGGER_INFO("User interface idle. Start destruction");
      need_turn_off = true;
      need_destroy = true;
    }
    break;
  }
  case AO_UI_PRESS_DESTROY: {
    ao_deinit(ao_led_r);
    ao_deinit(ao_led_g);
    ao_deinit(ao_led_b);
    ao_t ao_ui = ao_msg->receiver;
    ao_generic_free_message(ao_msg); // The message can be free here because the
                                     // receiver is the sender (UI).
    ao_ui_state = AO_UI_IDLE;        // AO is in idle state.

    xSemaphoreGive(os_sem_h); // Finally leave control to task button again as
                              // we almos finis destroying resources.

    LOGGER_INFO("Finish destroying User Interface");

    ao_deinit(ao_ui); /*< End of task. No more execution after this point*/
    break;
  }
  default: {
    LOGGER_INFO("Unknown event for UI object");
    return;
  }
  }

  // Turn off previous led based in previous UI state.
  if (need_turn_off)
    ao_ui_turn_off_previous_led(ao_msg->receiver, ao_ui_previous);

  // Destroy user interface to save resources.
  if (need_destroy) {
    ao_ui_message_t ui_msg = AO_UI_PRESS_DESTROY;
    ao_send_message(ao_msg->receiver, NULL, (uint8_t *)&ui_msg, sizeof(ui_msg));
  }

  // Send message to led AO. If ao_led_target is null this part does/send
  // nothing. The receiver is the ao_ui.
  ao_send_message(ao_led_target, ao_msg->receiver, &ao_led_msg,
                  sizeof(ao_led_msg));
}

static void ao_ui_free_f(ao_msg_t *ao_msg) { ao_generic_free_message(ao_msg); }

ao_ui_state_t ao_ui_get_state(void) { return ao_ui_state; }

ao_t ao_ui_init(void) {
  // Initialize User Interface AO.
  ao_t ao = ao_init(NULL, 0, ao_ui_ev_f, ao_ui_free_f, 0);
  // User Interface has the task of initialize necessary led.
  ao_led_r = ao_led_init(LED_RED_PORT, LED_RED_PIN);
  ao_led_g = ao_led_init(LED_GREEN_PORT, LED_GREEN_PIN);
  ao_led_b = ao_led_init(LED_BLUE_PORT, LED_BLUE_PIN);
  // Ready to go.
  ao_ui_state = AO_UI_READY;
  return ao;
}

/********************** end of file ******************************************/
