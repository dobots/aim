/**
 * @file BinaryStream.h
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

#ifndef DBINARYSTREAM_H_
#define DBINARYSTREAM_H_

#include <RingBuffer.h>

#include <assert.h>
#include <list>

/**
 * Read a binary stream from a socket or file
 */
class BinaryStream: public ring_buffer {
public:
	BinaryStream(int size): ring_buffer(size) {
		packets.clear();
	}

	~BinaryStream() { }

	inline void open(int fd) {
		ring_buffer::open(fd);
	}

	/**
	 * The final read function
	 */
	BUFFER_TYPE *Read();

	//! In the dotty protocol, the delimiter is \xa5 (decimal: 165)
	inline void SetStartByte(BUFFER_TYPE start_byte) { this->start_byte = start_byte; }

	//! Set the packet size to work with
	inline void ExpectedPacketSize(unsigned int size) {
		this->packet_size = size;
	}

protected:
	/**
	 * Sync such that the next getLast() with packet_size as argument will exactly return
	 * a packet.
	 */
	void Sync();

	//! Check if the proper start byte is used
	bool check(BUFFER_TYPE *packet);
private:
	//! Start byte
	BUFFER_TYPE start_byte;

	//! The size of an individual packet
	unsigned int packet_size;

	//! Number of packets to read per time
	unsigned char packet_count_per_read;

	//! Packet
	std::list<BUFFER_TYPE*> packets;

};


#endif /* DBINARYSTREAM_H_ */
