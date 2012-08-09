/**
 * @file AsciiStream.cpp
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

#include <AsciiStream.h>

#include <string>
#include <iostream>

/**
 * Just read the bytes from a buffer and drops into a string. If there is nothing to read
 * it returns 0, else way the number of bytes as returned.
 */
int AsciiStream::Read(std::string &buffer) {
	int bytes_read = read(fd, buf, sizeof(buf));
	if( bytes_read > 0 ) {
#ifdef DEBUG
		printf("received [%s]\n", buf);
#endif
		buffer = std::string(buf);
	} else {
		buffer = "";
	}
	return bytes_read;
}
