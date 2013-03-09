/**
 * @file rur_yarp.h
 * @brief Example file for a to-be-generated header from IPL to a YARP module
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
 * The RUR compiler (a set of back-ends for the omniidl compiler) generates a reference
 * header file for different languages (Java, C++). This reference header is subsequently
 * re-generated for a specific targeted middleware solution (YARP, ROS, AgentScape).
 *
 * The IDL (component specification) file does not contain all information, for example,
 * it does not need to define the Init() functions, etc. Basically, it only contains a
 * very selected subset of elements that are needed to create this header file (besides
 * knowledge from the yarp library), nothing more. The intention of the IDL file is to
 * have a stable/standardized interface, even if the yarp library changes. It is not
 * meant to be able to generate a yarp header file from the IDL without any additional
 * information.
 *
 * Exogenous coordination
 * An important design decision is to allow only for exogenous coordination. A component
 * is not allowed to set up its wiring to other components itself. This is the
 * responsibility of another/third entity. In practice, this means that if you have
 * already a YARP component and wants to use it in the RUR framework, you'll have to
 * remove the "startup" and "shutdown" code w.r.t. connect() and disconnect() calls and
 * create a separate Wirer IDL description (about which later).
 */

// Types defined by the RUR framework

//! Just a raw audio buffer, additional information should be sent over ioctl channels
struct Audio {
	float *buffer;
	int len;
};

/**
 * Field in IDL file      specific instance
 * @component_name@       $Foo$
 * @port_name@            $Mic$
 *   @type@               $Audio$
 *   @nof_ports@          $2$           (parameter)
 *   @inout@              $in$          (parameter)
 * @port_name@            $Distance$
 *   @type@               $int$
 *   @nof_ports@          $1$
 *   @inout@              $out$
 */

class $Foo$Module {
private:
	// the YARP Module class is not sooo convenient for our purposes
	// later on, we might decide to have it as a member or even derive from it
    // yarp::os::RFModule module;

	std::vector< BufferedPort<$Audio$> > port$Mic$;
	std::vector< BufferedPort<$int$> > port$Distance$;

public:

	$Foo$Module() {
		Network yarp;

		for (int i = 0; i < $2$; ++i) {
			port$Mic$.push_back(new BufferedPort<$Audio$>() );
		}
		for (int i = 0; i < $1$; ++i) {
			port$Distance$.push_back(new BufferedPort<$int$>() );
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
		return port$Mic$[index].read();
	}

	//! Distance inout="out", so only a write function
	inline void write$Distance$($int$ data, int index = 0) {
		port$Distance$[index].write(data);
	}
};

