/**
 * @file BeliefModule.cpp
 * @brief ...
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
 * Copyright © 2012 Anne van Rossum <anne@almende.com>
 *
 * @author  ...
 * @date    ...
 * @company Almende B.V.
 * @case    Artificial Intelligence Framework
 */

#include <BeliefModule.h>

#include <graph.hpp>

#include <graph2matrix.hpp>
#include <beliefpropagation.hpp>

#include <iostream>
#include <cassert>

using namespace rur;
using namespace std;
using namespace dobots;

typedef float ValueType;

//! Define the type of implementation we want
const ClassImplType impl_type = CIT_CHECK;

/**
 * Observation
 */
class ObsFactor: public vertex<ValueType> {
public:
	ObsFactor(): vertex<ValueType>(VT_FACTOR) {};

	ValueType function() {
		assert (from_size() == 1);
		assert (to_size() == 1);
		return abs(*to_at(0).second - *from_at(0).second);
	}
};

class SmoothFactor: public vertex<ValueType> {
public:
	SmoothFactor(): vertex<ValueType>(VT_FACTOR) {};

	ValueType function() {
		assert (from_size() == 1);
		assert (to_size() == 1);
		//		for (int i = 0; i < 0; ++i) {		}
		return abs(*to_at(0).second - *from_at(0).second);
	}
};

void simple_example() {
	graph<ValueType, impl_type> g;
	vertex<ValueType> v0(VT_VARIABLE);
	vertex<ValueType> v1(VT_VARIABLE);
	vertex<ValueType> v2(VT_FACTOR);
	g.push(v0);
	g.push(v1);
	g.push(v2);
	g.push(&v0, &v2);
	g.push(&v1, &v2);
	v0.setValue(0.3);
	v1.setValue(0.4);
	v2.setValue(0.5);
}


void BeliefModule::Tick() {
	graph<ValueType, impl_type> g;

	// create and load vertices with values from an image
	int pixel_count_sqrt = 2;
	int pixel_count = pixel_count_sqrt*pixel_count_sqrt;
	std::vector< vertex<ValueType> * > image;
	cout << "Create variable nodes per pixel" << endl;
	for (int i = 0; i < pixel_count; ++i) {
		vertex<ValueType> *pixel = new vertex<ValueType>(VT_VARIABLE);
		image.push_back(pixel);
		g.push(*pixel);
		ValueType value = (i < (pixel_count/2) ? 100 : 0); // // test image
		if (rand() % 20) value = 100 - value; // noise
		pixel->setValue(value);
	}

	// create a set of vertices parallel to the one before, set default values at random(?)
	cout << "Create variable nodes per label" << endl;
	std::vector< vertex<ValueType> * > labels;
	for (int i = 0; i < pixel_count; ++i) {
		vertex<ValueType> *label = new vertex<ValueType>(VT_VARIABLE);
		g.push(*label);
		labels.push_back(label);
		label->setValue(1); // set label everywhere to "1"
	}

	// create a set of factors that connect
	//	std::vector< vertex<ValueType> * > factors;
	for (int i = 0; i < pixel_count_sqrt; ++i) {
		for (int j = 0; j < pixel_count_sqrt; ++j) {
			ObsFactor *obs_factor = new ObsFactor();
			g.push(*obs_factor);
			int p = i+j*pixel_count_sqrt;
			g.push(image[p], obs_factor);
			g.push(obs_factor, labels[p]);

			SmoothFactor *factor;
			factor = new SmoothFactor();
			g.push(*factor);
		}
	}
	for (int i = 0; i < pixel_count_sqrt; ++i) {
		for (int j = 0; j < pixel_count_sqrt; ++j) {
			SmoothFactor *factor;
			int p_i_min1 = (i-1)+j*pixel_count_sqrt;
			int p_i_plus1 = (i+1)+j*pixel_count_sqrt;
			int p_j_min1 = i+(j-1)*pixel_count_sqrt;
			int p_j_plus1 = i+(j+1)*pixel_count_sqrt;
			if (i > 0) {
				factor = new SmoothFactor();
				g.push(labels[p_i_min1], factor);
				g.push(factor, labels[i]);
			}
			if ((i+1) < pixel_count_sqrt) {
				factor = new SmoothFactor();
				g.push(labels[p_i_plus1], factor);
				g.push(factor, labels[i]);
			}
			//			if (j > 0) {
			//				factor = new SmoothFactor();
			//				g.push(labels[p_j_min1], factor);
			//				g.push(factor, labels[i]);
			//			}
			//			if ((j+1) < pixel_count_sqrt) {
			//				factor = new SmoothFactor();
			//				g.push(labels[p_j_plus1], factor);
			//				g.push(factor, labels[i]);
			//			}
		}
	}

	// i have to check if the references are actually properly updated after a copy
	// because they should now refer to the newly created vertices
	//	vertex<ValueType> v3(VT_VARIABLE);
	//	v3 = v2;
	//	cout << "Vertex copy: " << v3.index() << endl;

	//  print graph
	cout << "Original graph " << endl << endl;
	cout << g << endl;

	cout << "Make the graph into a tree" << endl << endl;
	tree<ValueType, impl_type> t(g);

	cout << t << endl;

#ifdef BUILD_EIGEN
	cout << "Build matrix from graph" << endl << endl;
	graph2matrix<ValueType> g2m;
	graph2matrix<ValueType>::matrix *m;
	m = g2m.copy(g);
	cout << *m << endl;

	cout << "Build matrix from tree" << endl << endl;
	m = g2m.copy(t);
	cout << *m << endl;
#endif

	cout << "Propagate beliefs" << endl << endl;
	beliefpropagation<ValueType> bprop;
	bprop.tick(	t);

	//	cout << "Display tree again" << endl << endl;
	//	m = g2m.copy(t);
	//	cout << *m << endl;

	cout << "Done!" << endl;
}

/**
 * Make the modules stop after one step by returning true directly.
 */
bool BeliefModule::Stop() {
	return true;
}

