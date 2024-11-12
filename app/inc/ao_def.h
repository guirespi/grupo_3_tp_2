/*
 * ao_def.h
 *
 *  Created on: Nov 11, 2024
 *      Author: guirespi
 */

#ifndef INC_AO_DEF_H_
#define INC_AO_DEF_H_

/* AO general configuration */

/*< AO max message size to send */
#define AO_MAX_MSG_SIZE (4)
/*< AO max aditional data size */
#define AO_MAX_DATA_SIZE (8)
/*< AO max static object allowed */
#define AO_MAX_OBJECTS (4)
/*< AO max events received */
#define AO_MAX_QUEUE_MSG (3)

/* AO option flags for initialize objects */

/*< AO does not create a queue for events */
#define AO_OP_NO_QUEUE (1 << 0)
/*< AO does not create a task for events */
#define AO_OP_NO_TASK (1 << 1)

#endif /* INC_AO_DEF_H_ */
