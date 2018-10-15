/*
 * CircularBufferUart.c
 *
 *  Created on: 14. 10. 2018
 *      Author: Martin
 */

#include <CircularBufferUart.h>
#include <fsl_clock.h>
#include <fsl_lpsci.h>
#include <stdint.h>
#include "utils/CircularBuffer.h"

lpsci_config_t user_config;

// Redirection printf https://community.nxp.com/thread/389140

uint8_t shadowBuffer[_bufferUartSize];
buffer_t cBuffer;

void UART0_IRQHandler(void) {
	UART0_Type *base = UART0;
	uint8_t pcBuffer;

	/* Send data register empty and the interrupt is enabled. */
	if ((base->S1 & kLPSCI_TxDataRegEmptyFlag)) {
		int c = bufferRead(&cBuffer, &pcBuffer, 1);
		if (c > 0) {
			LPSCI_WriteBlocking(UART0, &pcBuffer, 1);
		}
		LPSCI_ClearStatusFlags(base, kLPSCI_TxDataRegEmptyFlag);
	}

	if(base->S1 & kLPSCI_RxOverrunFlag){

	}
	LPSCI_ClearStatusFlags(base, kLPSCI_AllInterruptsEnable);
}

// Redefine __sys_write to handle printf output
int __sys_write(int iFileHandle, char *pcBuffer, int iLength) {
	// Write String to BUFFER
	int size = bufferWrite(&cBuffer, (uint8_t *)pcBuffer, iLength);
	bufferRead(&cBuffer, (uint8_t *) pcBuffer, 1);
	LPSCI_WriteBlocking(UART0, (const uint8_t *) pcBuffer, 1);

	return iLength - size;

}

void CBufferUart_Init() {
	bufferInit(&cBuffer, shadowBuffer, sizeof(shadowBuffer));

	LPSCI_GetDefaultConfig(&user_config);
	user_config.baudRate_Bps = 115200U;
	user_config.enableTx = true;
	user_config.enableRx = true;

	LPSCI_Init(UART0, &user_config, CLOCK_GetFreq(kCLOCK_PllFllSelClk));
	LPSCI_DisableInterrupts(UART0, kLPSCI_AllInterruptsEnable);
	LPSCI_EnableInterrupts(UART0, kLPSCI_TransmissionCompleteInterruptEnable | kLPSCI_RxOverrunInterruptEnable);

	/* Enable interrupt in NVIC. */
	EnableIRQ(UART0_IRQn);

}