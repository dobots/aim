/**
 * @brief 
 * @file Odometry.h
 *
 * This file is created at Almende B.V. It is open-source software and part of the Common 
 * Hybrid Agent Platform (CHAP). A toolbox with a lot of open-source tools, ranging from 
 * thread pools and TCP/IP components to control architectures and learning algorithms. 
 * This software is published under the GNU Lesser General Public license (LGPL).
 *
 * It is not possible to add usage restrictions to an open-source license. Nevertheless,
 * we personally strongly object to this software being used by the military, in factory 
 * farming, for animal experimentation, or anything that violates the Universal 
 * Declaration of Human Rights.
 *
 * Copyright © 2012 Anne van Rossum <anne@almende.com>
 *
 * @author  Anne C. van Rossum
 * @date    Sep 28, 2012
 * @project Replicator FP7
 * @company Almende B.V.
 * @case    modular robotics / sensor fusion
 */


#ifndef ODOMETRY_H_
#define ODOMETRY_H_

// General files

/* **************************************************************************************
 * Interface of Odometry
 * **************************************************************************************/

/**
 * From two sets of features we want to derive how much we moved forwards and in how much
 * we turned left or right.
 */
class Odometry {
public:
	//! Constructor Odometry
	Odometry();

	//! Destructor ~Odometry
	virtual ~Odometry();
protected:

private:

};

#endif /* ODOMETRY_H_ */
