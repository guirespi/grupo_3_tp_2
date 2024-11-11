/*
 * ao_api.h
 *
 *  Created on: Nov 10, 2024
 *      Author: guirespi
 */

#ifndef INC_AO_API_H_
#define INC_AO_API_H_

#include <stdint.h>
#include "ao_def.h"

typedef  uint8_t ao_op_t;

struct ao_t;
typedef struct ao_t * ao_t;

typedef struct{
	ao_t sender;
	ao_t receiver;
	uint8_t ao_msg_size;
	uint8_t ao_msg[AO_MAX_MSG_SIZE];
}ao_msg_t;

typedef enum
{
	AO_OK = 0,
	AO_E_SENDER,
	AO_E_RECEIVER,
	AO_E_NO_MEM,
	AO_E_SIZE,
	AO_E_ARG,
	AO_E_OS,
}ao_err_t;

/**
 * @brief AO event handler.
 */
typedef void (* ao_ev_handler_t)(ao_msg_t * ao_msg);

/**
 * @brief Free handler used commonly by a message's AO sender.
 */
typedef void (* ao_free_handler_t)(ao_msg_t * ao_msg);

ao_t ao_init(uint8_t * ao_data, uint8_t ao_data_size, ao_ev_handler_t ao_ev_f, ao_free_handler_t ao_free_f, ao_op_t ao_op);

void ao_deinit(ao_t ao);

uint8_t * ao_get_data(ao_t ao);

uint8_t ao_get_data_size(ao_t ao);

int ao_send_message(ao_t receiver, ao_t sender, uint8_t * ao_msg, uint8_t ao_msg_size);

void ao_sender_free_method(ao_t ao, ao_msg_t * ao_msg);

void ao_generic_free_message(ao_msg_t * ao_msg);

#endif /* INC_AO_API_H_ */
