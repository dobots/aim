/**
 * @file rur_ros.h
 * @brief Example file for a to-be-generated header from IPL to a ROS module
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

/**
 * This allows to send messages using ROS. There are publishers and subscribers
 * which reflect the way of communication and not so much the components. Hence,
 * these will be encapsulated as "Wirer" entity (and not component).
 */

/**
 * Actually the publish-subscribe architecture screams for a circular buffer,
 * like e.g. #include <boost/circular_buffer.hpp>. However, let's make this
 * header file as lightweight as possible. We won't include additional header
 * files (like a custom-made C++ circular buffer class) or Boost headers.
 */
/**
 * Add the proper PATHs, e.g.:
 *  /opt/ros/diamondback/stacks/ros_comm/clients/cpp/roscpp/include/./ros/subscriber.h
 */
#include <ros/publisher.h>

class $Foo$Module {
private:
	int BUFFERSIZE = 1000;

	std::vector< ros::Subscriber > port$Mic$;
	std::vector< ros::Publisher > port$Distance$;

	std::vector < std::queue < $Audio$ > > buffer$Audio$;
protected:
	// Following function we need Nx, because there is no way of adding parameters
	// to the callback function in NodeHandle::subscribe ...
	void Fill$Audio$Buffer$i$(const $Audio$ & data) { Fill$Audio$Buffer(data, $i$); }

    void Fill$Audio$Buffer(const $Audio$ & data, int index) {
		buffer$Audio$[index].push_back();

		// if the buffer is full, we start to pop the oldest item
		// this would be done "automatically" with a circular buffer
		// we can also use the policy to drop new messages...
		if (buffer$Audio$[index].size() > BUFFERSIZE)
			buffer$Audio$[index].pop();
	}

public:

	$Foo$Module() {
		ros::init(argc, argv, "$Foo$");
		ros::NodeHandle n;

		for (int i = 0; i < $2$; ++i) {
			// Something like this... but we also send the parameter...
			// we do not want to rely on $Mic$[i], putting to much logic there, seems not right
			ros::Subscriber sub = nh.subscribe("$Mic$[i]", 1, &$Foo$Module::Fill$Audio$Buffer$i$, &this);
			port$Mic$.push_back( sub );
			buffer$Audio$.push_back ( *new std::queue < $Audio$ >() );
		}

		for (int i = 0; i < $2$; ++i) {
			buffer$Audio$[index].push_back( *new $Audio$() );
		}

		for (int i = 0; i < $1$; ++i) {
			// Publisher is reference counted, and should be usable in std containers
			// when the last reference is gone, it is automatically shutdown
			// http://www.ros.org/wiki/roscpp/Overview/Publishers%20and%20Subscribers
			ros::Publisher pub = nh.advertise<$int$>("$Distance$[i]", BUFFERSIZE);
			port$Distance$.push_back ( pub );
		}

	}

	~$Foo$Module() {
		// remove all ports and close them if still open
	}

	void Init() {
		std::vector< BufferedPort<$Audio$> >::iterator it;
		for (it = port$Mic$.begin(); it != port$Mic$.end(); ++it) {
			*it.open();
		}
		std::vector< BufferedPort<$int$> >::iterator it;
		for (it = port$Distance$.begin(); it != port$Distance$.end(); ++it) {
			*it.open();
		}
	}

	//! Audio inout="in", so only a read function
	inline Audio *read$Mic$(int index = 0) {
		port$Mic$[index].pop(); // pop also deallocates
		while (buffer$Audio$[index].empty()) {
			// wait...
			usleep(1000); // release control for 1/1000 of a second
		}
		return port$Mic$[index].front();
	}

	//! Distance inout="out", so only a write function
	inline void write$Distance$(const $int$ data, int index = 0) {
		port$Distance$[index].publish(data);
	}
};

