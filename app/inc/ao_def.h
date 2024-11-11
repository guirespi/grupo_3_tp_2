/*
 * ao_def.h
 *
 *  Created on: Nov 11, 2024
 *      Author: guirespi
 */

#ifndef INC_AO_DEF_H_
#define INC_AO_DEF_H_

/* AO general configuration */

#define AO_MAX_MSG_SIZE 	(4)
#define AO_MAX_DATA_SIZE 	(8)
#define AO_MAX_OBJECTS 		(4)
#define AO_MAX_QUEUE_MSG	(3)

/* AO option flags for initialize objects */

#define AO_OP_NO_QUEUE 		(1<<0)
#define AO_OP_NO_TASK 		(1<<1)

#endif /* INC_AO_DEF_H_ */
