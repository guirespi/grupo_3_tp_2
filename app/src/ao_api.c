/*
 * ao_api.c
 *
 *  Created on: Nov 10, 2024
 *      Author: guirespi
 */
#include "ao_api.h"
#include "cmsis_os.h"
#include <stdbool.h>
#include <string.h>

struct ao_t {
  bool used;
  QueueHandle_t ao_queue;
  TaskHandle_t ao_task;
  ao_ev_handler_t ao_ev_f;
  ao_free_handler_t ao_free_f;
  uint8_t ao_data_size;
  uint8_t ao_data[AO_MAX_DATA_SIZE];
};

typedef struct {
  struct ao_t ao_ins[AO_MAX_OBJECTS];
} ao_sys_t;

static ao_sys_t ao_sys;

static void ao_task(void *pv_parameters) {
  ao_t ao = (ao_t)pv_parameters;
  for (;;) {
    ao_msg_t *ao_msg = NULL;
    if (xQueueReceive(ao->ao_queue, &ao_msg, portMAX_DELAY)) {
      ao_msg->receiver->ao_ev_f(ao_msg);
    }
  }
}

static int ao_create_object(struct ao_t *ao, uint8_t *ao_data,
                            uint8_t ao_data_size, ao_ev_handler_t ao_ev_f,
                            ao_free_handler_t ao_free_f, ao_op_t ao_op) {
  if (ao_data_size > AO_MAX_DATA_SIZE)
    return AO_E_SIZE;
  if (ao_data_size > 0 && ao_data == NULL)
    return AO_E_ARG;

  ao->ao_data_size = ao_data_size;
  memcpy(ao->ao_data, ao_data, ao->ao_data_size);

  ao->ao_ev_f = ao_ev_f;
  ao->ao_free_f = ao_free_f;

  // Create queue if necessary
  if ((ao_op & AO_OP_NO_QUEUE) != AO_OP_NO_QUEUE) {
    ao->ao_queue = xQueueCreate(AO_MAX_QUEUE_MSG, sizeof(void *));
    if (ao->ao_queue == NULL)
      return AO_E_OS;
  } else {
    ao->ao_queue = NULL;
  }

  // Create task if necessary. If fails, destroy previous queue
  if ((ao_op & AO_OP_NO_TASK) != AO_OP_NO_TASK) {
    BaseType_t rt = xTaskCreate(ao_task, "ao_task", 128, (void *const)ao,
                                tskIDLE_PRIORITY + 1, &ao->ao_task);
    if (rt == pdFAIL) {
      if (ao->ao_queue != NULL) {
        vQueueDelete(ao->ao_queue);
        ao->ao_queue = NULL;
      }
      return AO_E_OS;
    }
  } else
    ao->ao_task = NULL;

  ao->used = true;

  return AO_OK;
}

ao_t ao_init(uint8_t *ao_data, uint8_t ao_data_size, ao_ev_handler_t ao_ev_f,
             ao_free_handler_t ao_free_f, ao_op_t ao_op) {
  for (uint8_t i = 0; i < AO_MAX_OBJECTS; i++) {
    if (false == ao_sys.ao_ins[i].used) {
      ao_t ao = &ao_sys.ao_ins[i];
      int rt = ao_create_object(ao, ao_data, ao_data_size, ao_ev_f, ao_free_f,
                                ao_op);
      if (rt != AO_OK)
        return NULL;
      return ao;
    }
  }
  return NULL;
}

void ao_deinit(ao_t ao) {
  if (ao->ao_queue) {
    vQueueDelete(ao->ao_queue);
    ao->ao_queue = NULL;
  }
  if (ao->ao_task) {
    vTaskDelete(ao->ao_task);
    ao->ao_task = NULL;
  }
  ao->used = false;
  memset(ao->ao_data, 0, ao->ao_data_size);
  ao->ao_data_size = 0;
}

uint8_t *ao_get_data(ao_t ao) {
  if (ao == NULL)
    return NULL;
  if (ao->ao_data_size == 0)
    return NULL;
  return ao->ao_data;
}

uint8_t ao_get_data_size(ao_t ao) {
  if (ao == NULL)
    return 0;
  return ao->ao_data_size;
}

int ao_send_message(ao_t receiver, ao_t sender, uint8_t *ao_msg,
                    uint8_t ao_msg_size) {
  if (!receiver)
    return AO_E_ARG; // Sender its optional
  if ((receiver->ao_queue == NULL && !sender) && sender->ao_queue == NULL)
    return AO_E_SENDER; // If receiver does not use queue we must need a sender
                        // with queue in use.
  if (ao_msg == NULL)
    return AO_E_ARG;
  if (ao_msg_size == 0)
    return AO_E_ARG;
  if (ao_msg_size > 0 && ao_msg == NULL)
    return AO_E_SIZE;

  int err = AO_OK;
  ao_msg_t *ao_msg_o = pvPortMalloc(sizeof(*ao_msg_o));
  memset(ao_msg_o, 0, sizeof(*ao_msg_o));

  ao_msg_o->sender = sender;
  ao_msg_o->receiver = receiver;
  memcpy(ao_msg_o->ao_msg, ao_msg, ao_msg_size);
  ao_msg_size = ao_msg_size;

  QueueHandle_t hqueue =
      receiver->ao_queue == NULL ? sender->ao_queue : receiver->ao_queue;

  BaseType_t rt = xQueueSend(hqueue, &ao_msg_o, 0);
  if (rt == pdFAIL)
    err = AO_E_OS;

  return err;
}

void ao_sender_free_method(ao_t ao, ao_msg_t *ao_msg) {
  if (ao == NULL || ao->ao_free_f == NULL)
    return;
  ao->ao_free_f(ao_msg);
}

void ao_generic_free_message(ao_msg_t *ao_msg) { vPortFree(ao_msg); }
