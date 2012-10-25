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

#include <iostream>

using namespace rur;
using namespace std;

typedef float ValueType;

//! Define the type of implementation we want
const ClassImplType impl_type = CIT_CHECK;

void BeliefModule::Tick() {
	graph<ValueType, impl_type> g;
	vertex<ValueType> v0(VT_VARIABLE); // = new vertex<ValueType>();
	vertex<ValueType> v1(VT_VARIABLE);
	vertex<ValueType> v2(VT_FACTOR);
	g.push(v0);
	g.push(v1);
	g.push(v2);
	g.push(&v0, &v2);
	g.push(&v1, &v2);

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
}

/**
 * Make the modules stop after one step by returning true directly.
 */
bool BeliefModule::Stop() {
	return true;
}

