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
#include <stdlib.h>
#include <stdio.h>
#include <algorithm>
#include <iostream>
#include <assert.h>

using namespace std;

/**
 *
 */
void ring_buffer::advance(int count) {
	pbuf_r += count;
	pbuf_r %= length;
}

void ring_buffer::set_pbuf_r(int pbuf_r) {
	assert (pbuf_r < length);
	this->pbuf_r = pbuf_r;
}

/**
 * Will return a series of bytes in consecutive fashion, it will use a copy for that.
 */
BUFFER_TYPE *ring_buffer::getLast(int count) {
	BUFFER_TYPE *result = new BUFFER_TYPE[count];
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
		memcpy(result, &buffer[pbuf_r], count);
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
bool ring_buffer::available(int count) {
	return ((pbuf_r + count) % length) < pbuf_w;
}

/**
 * Return the number of bytes available for reading.
 */
int ring_buffer::available() {
	int av = (pbuf_r + length - pbuf_w) % length;
	cout << "Number of bytes available: " << av << endl;
	assert (av >= 0);
	return av;
}

int ring_buffer::find(BUFFER_TYPE ch, int index, int max_len) {
	if (max_len == -1) max_len = length;
	assert (max_len <= length);
	int i = index;
	do {
		if (buffer[i] == ch) {
			cout << "Found start byte at " << i << endl;
			return i;
		}
		i++;
		if (i == pbuf_w) {
			cout << "Started at " << index << " and ran into writing pointer at " << pbuf_w << endl;
			break;
		}
	} while (i < max_len);
	return -1;
}

/**
 * Read a series of bytes, namely "count" and return total number of bytes available for
 * reading.
 */
int ring_buffer::readN(int count) {
	if (available() < count) {
		return 0;
	}
	int bytes_read = 0;
	if ((pbuf_w+count) >= length) {
		unsigned int wrap_around = (pbuf_w+count) % length;
		unsigned int till_end = length - pbuf_w;
		cout << "Read [0] " << till_end << " bytes" << endl;
		bytes_read += read(fd, &buffer[pbuf_w], till_end);
		cout << "Read [1] " << wrap_around << " bytes" << endl;
		bytes_read += read(fd, buffer, wrap_around);
	} else {
		cout << "Read " << count << " bytes" << endl;
		bytes_read = read(fd, &buffer[pbuf_w], count);

		cout << "Buffer: ";
		for (int i = 0; i < count; ++i) {
//			int i =
//			char buf[2]; buf[1] = '\0';
//			itoa (buffer[pbuf_w+i],buf,2);
//			buf[0] = buffer[pbuf_w+i];
//			printf("0x%X ", buffer[pbuf_w+i]);
			cout << (unsigned int)buffer[pbuf_w+i] << ' ';
		}
		cout << endl;
	}
	pbuf_w = (pbuf_w + bytes_read) % length;
	cout << "Set writer pointer to " << pbuf_w << endl;
	return available();
}
