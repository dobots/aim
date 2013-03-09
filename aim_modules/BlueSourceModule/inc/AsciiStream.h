/**
 * @file AsciiStream.h
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

#ifndef ASCIISTREAM_H_
#define ASCIISTREAM_H_

#include <stdio.h>
#include <string>

class AsciiStream {
public:
	AsciiStream(int size) {
		length = size;
		buf = new char[size];
	}

	~AsciiStream() {
		if (buf != NULL)
			delete [] buf;
	}

	//! Returns 0 if there is nothing to read
	int Read(std::string &buffer);

	inline void open(int fd) {
		this->fd = fd;
	}

private:
	//! File descriptor
	int fd;

	int length;

	char *buf;
};


#endif /* ASCIISTREAM_H_ */
