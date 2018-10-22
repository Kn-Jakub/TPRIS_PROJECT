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

void CBufferUart_Init();


#endif /* CIRCULARBUFFERUART_H_ */
