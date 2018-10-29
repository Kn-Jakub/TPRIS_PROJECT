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

#define _bufferUartSize 20
extern buffer_t cBuffer;


typedef void (*fn_new_command_callack)(buffer_t* commandBuffer,
		uint8_t newCommandSize);

void CBufferUart_Init();
void setNewCommandCallBack(fn_new_command_callack _fn);

#endif /* CIRCULARBUFFERUART_H_ */
