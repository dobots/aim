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

//typedef bool ValueType;
typedef size_t ValueType;
//typedef unsigned char ValueType;

typedef double ProbType;

//! Define the type of implementation we want
const ClassImplType impl_type = CIT_CHECK;

/**
 * Observation
 */
//class ObsFactor: public vertex<ValueType> {
//public:
//	ObsFactor(): vertex<ValueType>(VT_FACTOR) {};
//
//	ValueType function(size_type index) {
//		assert (from_size() == 0);
//		assert (to_size() == 1);
//		return abs(*to_at(0).second - *from_at(0).second);
//	}
//};
//
//class SmoothFactor: public vertex<ValueType> {
//public:
//	SmoothFactor(): vertex<ValueType>(VT_FACTOR) {};
//
//	ValueType function(size_type index) {
//		assert (from_size() == 1);
//		assert (to_size() == 1);
//		for (int i = 0; i < to_size(); ++i) {
//			// if there would be more, we should sum here in the sum-product algorithm...
//		}
//		return abs(*to_at(0).second - *from_at(0).second);
//	}
//};

void simple_example() {
//	graph<ValueType, impl_type> g;
//	vertex<ValueType> v0(VT_VARIABLE);
//	vertex<ValueType> v1(VT_VARIABLE);
//	vertex<ValueType> v2(VT_FACTOR);
//	g.push(v0);
//	g.push(v1);
//	g.push(v2);
//	g.push(&v0, &v2);
//	g.push(&v1, &v2);
//	v0.setValue(0.3);
//	v1.setValue(0.4);
//	v2.setValue(0.5);
}

/**
 * Conditional probability of S given C
 *  P_S_given_C.set(0, 0.5);   // C = 0, S = 0
 *  P_S_given_C.set(1, 0.9);   // C = 1, S = 0
 *  P_S_given_C.set(2, 0.5);   // C = 0, S = 1
 *  P_S_given_C.set(3, 0.1);   // C = 1, S = 1
 */
//class CloudySprinklerFactor: public factor<ValueType, double> {
//public:
//	CloudySprinklerFactor() {
//		typedef factor<ValueType, double> super;
//		super::push_back(0.5);
//		super::push_back(0.9);
//		super::push_back(0.5);
//		super::push_back(0.1);
//	};
//};

typedef graph<int,ValueType,ValueType,int,impl_type> graph_type;
typedef variable<int,ValueType,ValueType,int> variable_type;
typedef factor<int,ValueType,ValueType,int> factor_type;
typedef graph2matrix<int,ValueType,ValueType,int> graph2matrix_type;
/**
 * All belief propagation steps will be done in one tick.
 */
void BeliefModule::Tick() {
	graph_type g;

	// create and load vertices with values from an image
	int L = 2;
	int pixel_count = L*L;
	std::vector< variable_type * > image;

	double th_min = -3.0; double th_max = 3.2;
	double th_avg = (th_min + th_max) / 2.0;
	double th_width = (th_max - th_min) / 2.0;
	double scale = 40;
	double level = 50;

	probability<double,char,4> p;
	p.outcome[0] = 0.2;
	p.outcome[1] = 0.1;
	p.outcome[2] = 0.5;
	p.outcome[3] = 0.2;
	cout << "Probability test: " << p << '\n';

	cout << "Create variable nodes, one for each pixel" << endl;
	cout << "These are binary variables, no pixel values!" << endl;
	cout << "And these values are undefined, only their ordinality is given (binary)." << endl;
	cout << "Image information enters the factor graph through the factor nodes." << endl;
	for (int i = 0; i < pixel_count; ++i) {
		variable_type *pixel = new variable_type();
		image.push_back(pixel);
		g.push(*pixel);
	}

	typedef ising_factor<ValueType,ProbType> image_factor;

//	// test image
	cout << "Add unary Ising factors" << endl;
	for (int i = 0; i < pixel_count; ++i) {
		ValueType value = (i < (pixel_count/2) ? 100 : 0); //
		if (rand() % 20) value = 100 - value; // noise
        // local field strength
		ProbType local_field = th_avg + th_width * tanh( (value - level) / scale );
		image_factor *factor = new image_factor(local_field, FC_UNARY);
        factor_type *v = dynamic_cast<factor_type *> ( factor );
        g.push(*v);
	}
//
//	// create a set of (ising) factors that connect the previously created variables
//	// we only connect pixels one-way
//	cout << "Add binary Ising factors" << endl;
//	for (int i = 0; i < L; ++i) {
//		for (int j = 0; j < L; ++j) {
//			double local_coupling = 0.5;
//			vertex<ValueType> *v;
//
//			if (i > 0) {
//				v = dynamic_cast<vertex <ValueType> *> ( new image_factor(local_coupling, FC_BINARY) );
//				g.push(*v);
//				g.push(image[i-1+j*L], v);
//				g.push(v, image[i+j*L]);
//			}
//			if (j > 0) {
//				v = dynamic_cast<vertex <ValueType> *> ( new image_factor(local_coupling, FC_BINARY) );
//				g.push(*v);
//				g.push(image[i+(j-1)*L], v);
//				g.push(v, image[i+j*L]);
//			}
//		}
//	}

	// i have to check if the references are actually properly updated after a copy
	// because they should now refer to the newly created vertices
	//	vertex<ValueType> v3(VT_VARIABLE);
	//	v3 = v2;
	//	cout << "Vertex copy: " << v3.index() << endl;

	//  print graph
	cout << "Original graph " << endl << endl;
	cout << g << endl;

	cout << "Make the graph into a tree" << endl << endl;
//	tree<ValueType, impl_type> t(g);
//	cout << t << endl;

#ifdef BUILD_EIGEN
	cout << "Build matrix from graph" << endl << endl;
	graph2matrix_type g2m;
	graph2matrix_type::matrix *m;
	m = g2m.copy(g);
	cout << *m << endl;

//	cout << "Build matrix from tree" << endl << endl;
//	m = g2m.copy(t);
//	cout << *m << endl;
#endif

	cout << "Propagate beliefs" << endl << endl;
	beliefpropagation<ValueType> bprop;
	int ticks = 10;
	for (int i = 0; i < ticks; ++i) {
//		bprop.tick(	g);
	}
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

