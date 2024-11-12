/*
 * ao_api.h
 *
 *  Created on: Nov 10, 2024
 *      Author: guirespi
 */

#ifndef INC_AO_API_H_
#define INC_AO_API_H_

#include "ao_def.h"
#include <stdint.h>

typedef uint8_t ao_op_t;

struct ao_t;
typedef struct ao_t *ao_t;

typedef struct {
  ao_t sender;                     /*< AO send of this message */
  ao_t receiver;                   /*< AO receiving this message */
  uint8_t ao_msg_size;             /*< AO message size */
  uint8_t ao_msg[AO_MAX_MSG_SIZE]; /*< AO message pointer*/
} ao_msg_t;

/**
 * @brief AO error types
 *
 */
typedef enum {
  AO_OK = 0,
  AO_E_SENDER,
  AO_E_RECEIVER,
  AO_E_NO_MEM,
  AO_E_SIZE,
  AO_E_ARG,
  AO_E_OS,
} ao_err_t;

/**
 * @brief AO event handler.
 */
typedef void (*ao_ev_handler_t)(ao_msg_t *ao_msg);

/**
 * @brief Free handler used commonly by a message's AO sender.
 */
typedef void (*ao_free_handler_t)(ao_msg_t *ao_msg);

/**
 * @brief Initialize AO object.
 *
 * @note An AO with (AO_OP_NO_QUEUE) needs the  (AO_OP_NO_TASK) option. This
 * makes that the AO doesnt create these OS resources. An AO with this options
 * enable must need a sender in 'ao_send_message' method to receive events.
 *
 * @param ao_data AO aditional data.
 * @param ao_data_size AO aditional data size.
 * @param ao_ev_f AO event handler.
 * @param ao_free_f AO free message handler (if sender)
 * @param ao_op AO flag operations.
 * @return ao_t Allocated AO object.
 */
ao_t ao_init(uint8_t *ao_data, uint8_t ao_data_size, ao_ev_handler_t ao_ev_f,
             ao_free_handler_t ao_free_f, ao_op_t ao_op);
/**
 * @brief Deinit an AO object.
 *
 * @note You can deinit an AO inside its handler but its the last line it will
 * execute.
 *
 * @param ao AO object to deinitialize.
 */
void ao_deinit(ao_t ao);
/**
 * @brief Get AO aditional data pointer.
 *
 * @param ao AO handler.
 * @return uint8_t* Aditional data pointer.
 */
uint8_t *ao_get_data(ao_t ao);
/**
 * @brief Get AO aditional data size.
 *
 * @param ao AO handler.
 * @return uint8_t AO aditional data size.
 */
uint8_t ao_get_data_size(ao_t ao);
/**
 * @brief Send a message for an AO.
 *
 * @note For this functions, the receiver is a MUST. The sender is optional. The
 * event handler of the receiver is called after the message arrives to the AO
 * context.
 *
 * If this function receives an invalid AO receiver created with no
 * queue and task, it will always returns error as the AO is no suited for this
 * method; in this case a sender with queue and task implemented is a MUST.
 *
 * @param receiver Receiver AO.
 * @param sender Sender AO.
 * @param ao_msg AO message pointer.
 * @param ao_msg_size AO message size.
 * @return int
 * 				- AO_OK if no error.
 */
int ao_send_message(ao_t receiver, ao_t sender, uint8_t *ao_msg,
                    uint8_t ao_msg_size);
/**
 * @brief Call the free message method of an AO.
 *
 * @param ao AO instance.
 * @param ao_msg Pointer of AO message to be free.
 */
void ao_sender_free_method(ao_t ao, ao_msg_t *ao_msg);
/**
 * @brief Generic free for an AO message.
 *
 * @param ao_msg AO message.
 */
void ao_generic_free_message(ao_msg_t *ao_msg);

#endif /* INC_AO_API_H_ */
