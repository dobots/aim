/**
 * @file RingBuffer.cpp
 * @brief 
 *
 * This file is created at Almende B.V. It is open-source software and part of the Common 
 * Hybrid Agent Platform (CHAP). A toolbox with a lot of open-source tools, ranging from 
 * thread pools and TCP/IP components to control architectures and learning algorithms. 
 * This software is published under the GNU Lesser General Public license (LGPL).
 *
 * It is not possible to add usage restrictions to an open-source license. Nevertheless,
 * we personally strongly object against this software being used by the military, in the
 * bio-industry, for animal experimentation, or anything that violates the Universal
 * Declaration of Human Rights.
 *
 * Copyright © 2010 Anne van Rossum <anne@almende.com>
 *
 * @author  Anne C. van Rossum
 * @date    Aug 9, 2012
 * @project Replicator FP7
 * @company Almende B.V.
 * @case    Artificial Intelligence Framework
 */

#include <RingBuffer.h>
#include <string.h>
#include <algorithm>
#include <iostream>
#include <assert.h>

using namespace std;

/**
 *
 */
void RingBuffer::advance(int count) {
	pbuf_r += count;
	pbuf_r %= length;
}

void RingBuffer::set_pbuf_r(int pbuf_r) {
	assert (pbuf_r < length);
	this->pbuf_r = pbuf_r;
}

/**
 * Will return a series of bytes in consecutive fashion, it will use a copy for that.
 */
char *RingBuffer::getLast(int count) {
	char *result;
	if (!available(count)) {
		int c = readN(count);
		if (c < count) {
			return NULL;
		}
	}
	if ((pbuf_r+count) >= length) {
		int wrap_around = (pbuf_r+count) % length;
		int till_end = length - pbuf_r;
		memcpy(result, &buffer[pbuf_r], till_end);
		memcpy(result+till_end, buffer, wrap_around);
	} else {
		memcpy(result, &buffer[pbuf_r], pbuf_r+count);
	}
	pbuf_r = (pbuf_r + count) % length;
	return result;
}

/**
 * The read pointer needs not always to be "smaller" than the write pointer, because the
 * write pointer can wrap around to the first byte again. Example: with size = 100, and
 * chunk count = 20, read = 99 and write = 30, should be fine. However, read = 99 and
 * write = 10 should return false.
 */
bool RingBuffer::available(int count) {
	return ((pbuf_r + count) % length) < pbuf_w;
}

/**
 * Return the number of bytes available.
 */
int RingBuffer::available() {
	int av = (pbuf_r + length - pbuf_w) % length;
	assert (av >= 0);
	return av;
}

int RingBuffer::find(char ch, int index, int max_len) {
	if (max_len == -1) max_len = length;
	assert (max_len <= length);
	int i = index;
	do {
		if (buffer[i] == ch) return i;
		i++;
	} while ((i < max_len) && (i != pbuf_w));
	return -1;
}

/**
 * Read a series of bytes, namely "count" and return total number of bytes available for
 * reading.
 */
int RingBuffer::readN(int count) {
	int bytes_read = 0;
	if ((pbuf_w+count) >= length) {
		unsigned int wrap_around = (pbuf_w+count) % length;
		unsigned int till_end = length - pbuf_w;
		bytes_read += read(fd, &buffer[pbuf_w], till_end);
		bytes_read += read(fd, buffer, wrap_around);
	} else {
		bytes_read = read(fd, &buffer[pbuf_w], pbuf_w+count);
	}
	pbuf_w = (pbuf_w + bytes_read) % length;
	return available();
}
