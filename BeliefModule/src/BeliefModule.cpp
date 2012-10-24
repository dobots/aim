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

void BeliefModule::Tick() {
//	graph<ValueType, CIT_FAST> g;
	graph<ValueType> g;
	vertex<ValueType> *v1 = new vertex<ValueType>();
	vertex<ValueType> *v2 = new vertex<ValueType>();
	vertex<ValueType> *v3 = new vertex<ValueType>();
	g.push(*v1);
	g.push(*v2);
	g.push(*v3);
	g.push(*v1, *v3);
	g.push(*v1, *v2);
	g.push(*v3, *v2);
	g.push(*v2, *v3);

	//  print graph
	cout << "Graph {src} {dest}: " << endl << g << endl;

#ifdef BUILD_EIGEN
	graph2matrix<ValueType> g2m;
	graph2matrix<ValueType>::matrix *m;

	m = g2m.copy(g);

	cout << *m << endl;
#endif

	delete v1;
	delete v2;
	delete v3;
}

bool BeliefModule::Stop() {
	return true;
}

