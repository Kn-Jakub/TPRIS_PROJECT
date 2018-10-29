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
#include <ctype.h>
#include <assert.h>


lpsci_config_t user_config;

// Redirection printf https://community.nxp.com/thread/389140

uint8_t shadowBuffer[_bufferUartSize];
uint8_t shadowOutputBuffer[_bufferUartSize];
buffer_t cBuffer;
buffer_t outBuffer;

static callBack_struct_t* callBack_struct_pt;

void UART0_IRQHandler(void) {
	UART0_Type *base = UART0;
	uint8_t pcBuffer;

//	/* If RX overrun. */
//	if (UART0_S1_OR_MASK & base->S1) {
//		while (UART0_S1_RDRF_MASK & base->S1) {
//			(void) base->D;
//		}
//		LPSCI_ClearStatusFlags(base, kLPSCI_RxOverrunFlag);
//	}

	/* Send data register empty and the interrupt is enabled. */
	if ((base->S1 & kLPSCI_TxDataRegEmptyFlag)) {
		int c = bufferRead(&cBuffer, &pcBuffer, 1);
		if (c > 0) {
//			LPSCI_WriteBlocking(UART0, &pcBuffer, 1);
			UART0->D = pcBuffer;
		} else {
			/* Disable TX register empty interrupt. */
//			base->C2 &= ~UART0_C2_TIE_MASK;
			LPSCI_DisableInterrupts(base, kLPSCI_TxDataRegEmptyInterruptEnable);
		}
		LPSCI_ClearStatusFlags(base, kLPSCI_TxDataRegEmptyFlag);
	}
	/* If RX overrun. */
	if (UART0_S1_OR_MASK & base->S1) {
		while (UART0_S1_RDRF_MASK & base->S1) {
			(void) base->D;
		}

		LPSCI_ClearStatusFlags(base, kLPSCI_RxOverrunFlag);
	}

	/* Receive data register full */
	if ((UART0_S1_RDRF_MASK & base->S1) && (UART0_C2_RIE_MASK & base->C2)) {
		uint8_t rxData;
		static uint8_t size = 0;
		rxData = base->D;
		if (rxData == '\n' || rxData == '\r') {
			callBack_struct_pt->callbackHandler(&outBuffer, size,
					callBack_struct_pt->data);
			size = 0;
		} else {
			size++;
			if (isalnum(rxData))

				bufferWrite(&outBuffer, &rxData, 1);
		}

	}

}

// Redefine __sys_write to handle printf output
int __sys_write(int iFileHandle, char *pcBuffer, int iLength) {
	// Write String to BUFFER
	int size = bufferWrite(&cBuffer, (uint8_t *) pcBuffer, iLength);
	LPSCI_EnableInterrupts(UART0,
			kLPSCI_TxDataRegEmptyInterruptEnable
					| kLPSCI_RxOverrunInterruptEnable /* kLPSCI_TransmissionCompleteInterruptEnable | kLPSCI_RxOverrunInterruptEnable*/);
	return iLength - size;

}

void CBufferUart_Init(callBack_struct_t* _callBack_struct_pt) {
	bufferInit(&cBuffer, shadowBuffer, sizeof(shadowBuffer));
	bufferInit(&outBuffer, shadowOutputBuffer, sizeof(shadowOutputBuffer));

	assert(_callBack_struct_pt != NULL);
	callBack_struct_pt = _callBack_struct_pt;

	LPSCI_GetDefaultConfig(&user_config);
	user_config.baudRate_Bps = 115200U;
	user_config.enableTx = true;
	user_config.enableRx = true;

	LPSCI_Init(UART0, &user_config, CLOCK_GetFreq(kCLOCK_PllFllSelClk));
	LPSCI_DisableInterrupts(UART0, kLPSCI_AllInterruptsEnable);

	/* Enable LPSCI RX IRQ if previously enabled. */
	LPSCI_EnableInterrupts(UART0, kLPSCI_RxDataRegFullInterruptEnable);

	/* Enable interrupt in NVIC. */
	EnableIRQ(UART0_IRQn);

}

void setNewCommandCallBack(callBack_struct_t* _callBack_struct_pt) {
	assert(_callBack_struct_pt != NULL);
	callBack_struct_pt = _callBack_struct_pt;
}
