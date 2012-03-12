/**
 * @file rur_dummy.h
 * @brief Example file for a to-be-generated header from IPL to a dummy compilable class
 *
 * This file is created at Almende B.V. It is open-source software and part of the Common
 * Hybrid Agent Platform (CHAP). A toolbox with a lot of open-source tools, ranging from
 * thread pools and TCP/IP components to control architectures and learning algorithms.
 * This software is published under the GNU Lesser General Public license (LGPL).
 *
 * It is not possible to add usage restrictions to an open-source license. Nevertheless,
 * we personally strongly object against this software used by the military, in the
 * bio-industry, for animal experimentation, or anything that violates the Universal
 * Declaration of Human Rights.
 *
 * Copyright © 2010 Anne van Rossum <anne@almende.com>
 *
 * @author 	Anne C. van Rossum
 * @date    Jun 8, 2011
 * @project Replicator FP7
 * @company Almende B.V.
 * @case    Architecture
 */

class $Foo$Module {
private:
	Audio *dummyAudio;
public:

	$Foo$Module() {
		dummyAudio = new Audio();
	}

	~$Foo$Module() { }

	void Init() { }

	//! Audio inout="in", so only a read function
	inline Audio *read$Mic$(int index = 0) {
		return dummyAudio;
	}

	//! Distance inout="out", so only a write function
	inline void write$Distance$(const $int$ data, int index = 0) {

	}
};

