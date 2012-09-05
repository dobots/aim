/**
 * @file RingBuffer.h
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

#define BUFFER_TYPE unsigned char

/**
 * Just ordinary ring buffer of
 */
//template <typename T>
class ring_buffer {
public:
	/**
	 * Create a ring buffer. Default size is 128.
	 */
	ring_buffer(int size = 128): length(size), buffer(new BUFFER_TYPE[size]) {
		this->length = size;
//		buffer = new BUFFER_TYPE[size];
		pbuf_w = 0;
		pbuf_r = length - 1;
	}

	~ring_buffer() {
		delete [] buffer;
	}

	//! Open file
	inline void open(int fd) { this->fd = fd; }

protected:

//	inline char *getBuffer() { return buffer; }

	//! Returns number of bytes that can be read (can be larger than "count")
	int readN(int count);

	//! Get the last count bytes
	BUFFER_TYPE *getLast(int count);

	//! Advance the pointer
	void advance(int count);

	//! Return the number of bytes that is available
	int available();

	//! Check if there is data available
	bool available(int count);

	//! Only protected use allowed, be careful with it
	inline int get_pbuf_r() { return pbuf_r; }

	//! Only protected use allowed, be careful with it
	void set_pbuf_r(int pbuf_r);

	inline int get_length() { return length; }

	//! Find a certain character from a certain index respecting circularity of buffer
	int find(BUFFER_TYPE ch, int index, int max_len = -1);
private:
	//! File descriptor to be used (or socket descriptor)
	int fd;

	//! The buffer itself
	BUFFER_TYPE* buffer;

	//! Buffer length
	int length;

	//! Pointer (writing) to buffer
	unsigned int pbuf_w;

	//! Pointer (reading) from buffer
	unsigned int pbuf_r;

};


