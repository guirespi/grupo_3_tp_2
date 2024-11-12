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

#ifndef TASK_UI_H_
#define TASK_UI_H_

/********************** CPP guard ********************************************/
#ifdef __cplusplus
extern "C" {
#endif

/********************** inclusions *******************************************/

#include "ao_api.h"
#include "cmsis_os.h"
#include <stdbool.h>

/********************** macros ***********************************************/

/********************** typedef **********************************************/

typedef enum {
  AO_UI_PRESS_NONE = 0, /*< No event happened */
  AO_UI_PRESS_PULSE,    /*< Button pressed pulse */
  AO_UI_PRESS_SHORT,    /*< Button pressed short*/
  AO_UI_PRESS_LONG,     /*< Button pressed long */
  AO_UI_PRESS_IDLE,     /*< Button was not press for to long */
  AO_UI_PRESS_DESTROY,  /*< Destroy AO ui and leds*/
} ao_ui_message_t;

typedef enum {
  AO_UI_IDLE = 0,     /*< Idle UI, not valid to use */
  AO_UI_READY,        /*< UI ready */
  AO_UI_LED_RED_ON,   /*< UI red led is on*/
  AO_UI_LED_GREEN_ON, /*< UI green led is on*/
  AO_UI_LED_BLUE_ON,  /*< UI blue led is on*/
} ao_ui_state_t;

/********************** external data declaration ****************************/

/********************** external functions declaration ***********************/

/**
 * @brief Initialize UI AO.
 *
 * @note This functions initialize the related leds to the application.
 *
 * @return ao_t AO UI instance.
 */
ao_t ao_ui_init(void);
/**
 * @brief Get AO UI state.
 *
 * @return ao_ui_state_t UI state.
 */
ao_ui_state_t ao_ui_get_state(void);

/********************** End of CPP guard *************************************/
#ifdef __cplusplus
}
#endif

#endif /* TASK_UI_H_ */
/********************** end of file ******************************************/
