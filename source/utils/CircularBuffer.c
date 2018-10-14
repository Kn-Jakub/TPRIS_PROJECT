/*
 * CircularBuffer.c
 *
 *  Created on: 8. 10. 2018
 *      Author: Martin
 */
#include "CircularBuffer.h"
#include <string.h>
#include <assert.h>

// Static function prototypes
static inline uint8_t* incrementAddresBy(uint8_t* address, const buffer_t* buffer, size_t addNum) {
	return (uint8_t *) (buffer->buf + (((address - buffer->buf) + addNum) % buffer->size));
}

static inline size_t calcWriteCount(const buffer_t* buffer) {

	if (buffer->head >= buffer->tail) {
		return buffer->size - (buffer->head - buffer->buf);
	} else {
		return (size_t) (buffer->tail - buffer->head);
	}
}

static inline size_t calcReadCount(const buffer_t* buffer) {
	if (buffer->head > buffer->tail) {
		return buffer->head - buffer->tail;
	} else {
		return ((buffer->buf + buffer->size) - buffer->tail);
	}
}

/**
 * Copy data in the buffer until is full
 * @param buff - circular buffer handle
 * @param data_pt - pointer to data buffer
 * @param count -  size of data
 * @return number of copied bytes
 */
int16_t bufferWrite(buffer_t* buff, uint8_t* data_pt, uint16_t count) {
	int16_t size = 0;

	assert((data_pt != NULL) && "Pointer data_pt is NULL");
	assert((count <= buff->size) && "Write count is bigger than capacity of Circual Buffer");

	if (buff->full == BUFF_FULL)
		return 0;

	size_t wCount = calcWriteCount(buff);
	if (wCount == 0)
		return 0;  // Buffer is full

	if (wCount > count) {
		wCount = count;
	}
	memcpy(buff->head, data_pt, wCount);
	size = wCount;
	buff->full = BUFF_FREE;

	if (wCount > 0) {
		buff->head = incrementAddresBy(buff->head, buff, wCount);
		if (buff->head == buff->tail) {
			buff->full = BUFF_FULL;
			return size;
		}
	}

	if (wCount < count) {
		size += bufferWrite(buff, (data_pt + wCount), (count - wCount));
	}
	return size;
}
/**
 * Retrieve a value from the buffer
 * @param buff
 * @param data_pt
 * @param count
 * @return
 */
int16_t bufferRead(buffer_t* buff, uint8_t* data_pt, uint16_t count) {
	int16_t size = 0;

	assert((data_pt != NULL) && "Output data buffer is NULL!");

	if ((buff->tail == buff->head) && (buff->full != BUFF_FULL))
		return 0; // No data, empty buffer

	size_t rCount = calcReadCount(buff);
	if (rCount == 0) {
		buff->full = BUFF_EMPTY;
		return 0; // Nothing to read
	}

	if (rCount > count) {
		rCount = count;
	}

	memcpy(data_pt, buff->tail, rCount);
	buff->tail = incrementAddresBy(buff->tail, buff, rCount);

	if (buff->tail == buff->head)
		buff->full = BUFF_EMPTY;
	else
		buff->full = BUFF_FREE;

	size = rCount;
	if (rCount < count) {
		size += bufferRead(buff, (data_pt + rCount), (count - rCount));
	}
	return size;
}
/**
 * Initialize the circular buffer handler
 * @param buff
 * @param bufferPt
 * @param size
 */
void bufferInit(buffer_t* buff, uint8_t* bufferPt, uint16_t size) {

	buff->buf = bufferPt;
	buff->tail = bufferPt;
	buff->head = bufferPt;
	buff->size = size;
	buff->full = BUFF_EMPTY;
}

/**
 * Returns the maximum capacity of the buffer
 * @param buff
 * @return
 */
size_t bufferCapacity(buffer_t* buff) {
	return buff->size;
}

/**
 * Returns the current number of free bytes in the buffer
 * @param rb
 * @return
 */
size_t bufferBytesFree(const buffer_t *rb) {
	if (rb->head >= rb->tail)
		return rb->full != BUFF_FULL ? ((rb->size) - (rb->head - rb->tail)) : 0;
	else
		return rb->tail - rb->head;
}



