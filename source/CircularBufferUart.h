/*
 * CircularBufferUart.h
 *
 *  Created on: 14. 10. 2018
 *      Author: Martin
 */

#ifndef CIRCULARBUFFERUART_H_
#define CIRCULARBUFFERUART_H_
#include <stdio.h>
#include <utils/CircularBuffer.h>

#define _bufferUartSize 50
extern buffer_t cBuffer;


typedef void (*fn_new_command_callack)(buffer_t* commandBuffer,
		uint8_t newCommandSize, void* data);

typedef struct{
	fn_new_command_callack callbackHandler;
	void* data;
}callBack_struct_t;

void CBufferUart_Init();
void setNewCommandCallBack(callBack_struct_t* _callBack_struct_pt);

#endif /* CIRCULARBUFFERUART_H_ */
