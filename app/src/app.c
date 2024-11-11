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

#include "main.h"
#include "cmsis_os.h"
#include "logger.h"
#include "dwt.h"
#include "board.h"

#include "task_button.h"
#include "task_led.h"
#include "task_ui.h"

#include "ao_api.h"

/********************** macros and definitions *******************************/

/********************** internal data declaration ****************************/

/********************** internal functions declaration ***********************/

/********************** internal data definition *****************************/

/********************** external data declaration *****************************/
ao_t ao_ui;
ao_t ao_led_r;
ao_t ao_led_g;
ao_t ao_led_b;

/********************** external functions definition ************************/
void app_init(void)
{
    BaseType_t status;

    // Initialize user interface
    ao_ui = ao_ui_init();
    // Initialize 'n' AO_leds
    ao_led_r = ao_led_init(LED_RED_PORT, LED_RED_PIN);
    ao_led_g = ao_led_init(LED_GREEN_PORT, LED_GREEN_PIN);
    ao_led_b = ao_led_init(LED_BLUE_PORT, LED_BLUE_PIN);

    // Create task button
    status = xTaskCreate(task_button, "task_button", 128, NULL, tskIDLE_PRIORITY + 3, NULL);

    while (pdPASS != status)
    {
        // error
    }

    LOGGER_INFO("Application initialized");

    cycle_counter_init();
}

/********************** end of file ******************************************/
