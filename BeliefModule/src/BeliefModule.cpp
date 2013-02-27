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
#include <tree.hpp>
#include <isingfactor.hpp>

#include <graph2matrix.hpp>
#include <beliefpropagation.hpp>

#include <iostream>
#include <cassert>
#include <cstdlib>

using namespace rur;
using namespace std;
using namespace dobots;

//typedef bool ValueType;
typedef size_t ValueType;
//typedef unsigned char ValueType;

typedef double ProbType;

typedef probability<ProbType,int> MessageType;

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


typedef graph<int,ProbType,MessageType,int,impl_type> graph_type;
typedef tree<int,ProbType,MessageType,int,impl_type> tree_type;
typedef variable<int,ProbType,MessageType,int> variable_type;
typedef factor<int,ProbType,MessageType,int> factor_type;
typedef ising_factor<int,ProbType,MessageType,int> image_factor;
typedef beliefpropagation<int,ProbType,MessageType,int> belief_prop;
typedef conditional_probability_table<int,ProbType,int> cond_table;

#ifdef BUILD_EIGEN
typedef graph2matrix<int,ProbType,MessageType,int> graph2matrix_type;
#endif

/**
 * Create a 2x3 table as on the wikipedia page on marginals.
 *   http://en.wikipedia.org/wiki/Marginal_distribution
 * It is about a traffic light, red, yellow, or green and the chance to get hit if the
 * light is red, etc. The probabilities of this conditional distribution are given:
 *
 * Conditional distribution: P(H|L)
 * 				L=Red		L=Yellow 	L=Green
 * 	H=Not Hit 	0.99 		0.9 		0.2
 * 	H=Hit 		0.01 		0.1 		0.8
 *
 * There is of course no belief propagation involved. It just functions to check if
 * the joint distribution table is properly build up, and that the marginals are
 * correctly calculated.
 *
 * Joint distribution: P(H,L)
 * 				L=Red 	L=Yel. 	L=Gre. 	Marginal probability
 * H=Not Hit 	0.198 	0.09 	0.14 	0.428
 * H=Hit 		0.002 	0.01 	0.56 	0.572
 * Total 		0.2 	0.1 	0.7 	1
 *
 */
void wikipedia_test() {
	int test_d[] = {2,3};
	std::vector<int> test_dimensions (test_d, test_d + sizeof(test_d) / sizeof(int));
	conditional_probability_table<int,float,int> table(test_dimensions);
	table.add_vertex(1);
	table.add_vertex(2);
	int hit = 1, nothit = 0;
	int red = 0, yellow = 1, green = 2;
	table.set(nothit,red,0.99);
	table.set(nothit,yellow,0.9);
	table.set(nothit,green,0.2);
	table.set(hit,red,0.01);
	table.set(hit,yellow,0.1);
	table.set(hit,green,0.8);

	cout << "Conditional probability table: " << std::endl << table << std::endl;

	probability<float,int> evidence(3);
	evidence[red] = 0.2;
	evidence[yellow] = 0.1;
	evidence[green] = 0.7;

	cout << "Evidence: " << std::endl;
	for (int i = 0; i < evidence.size(); ++i) {
		cout << ' ' << i << " * : " << evidence[i] << std::endl;
	}
	cout << std::endl;

	table.multiply(2, evidence);
	cout << "Joint distribution (includes evidence): " << std::endl << table << std::endl;

	probability<float,int> marginal(2);
	table.sum(1, marginal);
	cout << "Marginal probability: " << std::endl;
	for (int i = 0; i < marginal.size(); ++i) {
		cout << " * " << i << " : " << marginal[i] << '\n';
	}
	cout << std::endl;
}

/**
 * From http://www.cs.ubc.ca/~murphyk/Bayes/bnintro.html.
 *
 * Pr(S=1|W=1,R=1) = 0.1945
 * Pr(S=1|W=1) = 0.430
 *
 * As you can see, the structure of the "questions" asked on the conditional
 * probabilities at hand, are now more dimensional. The wikipedia example only
 * asks on Pr(H=1|L=0) and not w.r.t. W and R.
 *
 * At this time, I discovered graphlab.
 *
 */
void sprinkler_test() {
	tree_type g;

	variable_type vcloudy(2, "cloudy");
	variable_type vsprinkler(2, "sprinkler");
	variable_type vrain(2, "rain");
	variable_type vwetgrass(2, "wet grass");

	g.push(vcloudy);
	g.push(vsprinkler);
	g.push(vrain);
	g.push(vwetgrass);

	std::vector<int> two_by_one;
	two_by_one.clear();
	two_by_one.push_back(2);

	std::vector<int> two_by_two;
	two_by_two.clear();
	two_by_two.push_back(2);
	two_by_two.push_back(2);

	std::vector<int> two_by_two_by_two;
	two_by_two_by_two.clear();
	two_by_two_by_two.push_back(2);
	two_by_two_by_two.push_back(2);
	two_by_two_by_two.push_back(2);

	cond_table c_table(two_by_one);
	factor_type fcloudy(c_table, "cloudy");
	c_table.set(0,0.5);
	c_table.set(1,0.5);

	cond_table cs_table(two_by_two);
	factor_type fcloudy_sprinkler(cs_table, "cloudy|sprinkler");
	cs_table.set(0,0,0.5);
	cs_table.set(0,1,0.5);
	cs_table.set(1,0,0.9);
	cs_table.set(1,1,0.1);

	cond_table cr_table(two_by_two);
	factor_type fcloudy_rain(cr_table, "cloudy|rain");
	cr_table.set(0,0,0.8);
	cr_table.set(0,1,0.2);
	cr_table.set(1,0,0.2);
	cr_table.set(1,1,0.8);

	cond_table srw_table(two_by_two_by_two);
	factor_type fsprinkler_rain_wetgrass(srw_table, "wet grass|sprinkler,rain");
	srw_table.set(0,0,0,1.0);
	srw_table.set(0,0,1,0.0);
	srw_table.set(0,1,0,0.1);
	srw_table.set(0,1,1,0.9);
	srw_table.set(1,0,0,0.1);
	srw_table.set(1,0,1,0.9);
	srw_table.set(1,1,0,0.01);
	srw_table.set(1,1,1,0.99);

	// here we can try to multiply all factors with each other to build one large joint probability table
	// however, it is more convenient to "push in the sums" to calculate the marginals efficiently

	cout << srw_table << std::endl;
	g.push(fcloudy);
	g.push(fcloudy_sprinkler);
	g.push(fcloudy_rain);
	g.push(fsprinkler_rain_wetgrass);

	g.push(&fcloudy,&vcloudy);

	g.push(&vcloudy, &fcloudy_sprinkler);
	g.push(&fcloudy_sprinkler, &vsprinkler);

	g.push(&vcloudy, &fcloudy_rain);
	g.push(&fcloudy_rain, &vrain);

	g.push(&vsprinkler, &fsprinkler_rain_wetgrass);
	g.push(&vrain, &fsprinkler_rain_wetgrass);
	g.push(&fsprinkler_rain_wetgrass, &vwetgrass);

	g.moralization();

	cout << g << std::endl;

	// running belief propagation should return the marginal of each variable
	// no joint conditions
	cout << "Propagate beliefs" << std::endl << std::endl;
	belief_prop bprop;
	bprop.init(g);
	int ticks = 3;
	for (int i = 0; i < ticks; ++i) {
		bprop.tick(g);
	}

//
//	table.multiply(2, evidence);
//	cout << "Joint distribution: " << std::endl << table << std::endl;

//	probability<float,int> marginal(2);
//	table.sum(1, marginal);
//	cout << "Marginal probability: " << std::endl << marginal << std::endl;
	cout << "Done" << std::endl;
}

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

//	probability<float,int> p(2);
//	p[0] = 0.7;
//	p[1] = 0.7;
//	cout << "Probability test: " << p << '\n';

//	wikipedia_test();

	sprinkler_test();

	return;

	cout << "Create variable nodes, one for each pixel" << std::endl;
	cout << "These are binary variables, no pixel values!" << std::endl;
	cout << "And these values are undefined, only their ordinality is given (binary)." << std::endl;
	cout << "Image information enters the factor graph through the factor nodes." << std::endl;
	for (int i = 0; i < pixel_count; ++i) {
		variable_type *pixel = new variable_type(2);
		image.push_back(pixel);
		g.push(*pixel);
	}

//	// test image
	cout << "Add unary Ising factors" << std::endl;
	for (int i = 0; i < pixel_count; ++i) {
		ValueType value = (i < (pixel_count/2) ? 100 : 0); //
		if (rand() % 20) value = 100 - value; // noise
        // local field strength
		ProbType local_field = th_avg + th_width * tanh( (value - level) / scale );
		image_factor *factor = new image_factor(local_field, FC_UNARY);
        factor_type *v = dynamic_cast<factor_type *> ( factor );
        g.push(*v);
	}

	// create a set of (ising) factors that connect the previously created variables
	// we only connect pixels one-way
	cout << "Add binary Ising factors" << std::endl;
	for (int i = 0; i < L; ++i) {
		for (int j = 0; j < L; ++j) {
			ProbType local_coupling = 0.5;
			factor_type *v;

			if (i > 0) {
				v = dynamic_cast<factor_type *> ( new image_factor(local_coupling, FC_BINARY) );
				g.push(*v);
				g.push(image[i-1+j*L], v);
				g.push(v, image[i+j*L]);
			}
			if (j > 0) {
				v = dynamic_cast<factor_type *> ( new image_factor(local_coupling, FC_BINARY) );
				g.push(*v);
				g.push(image[i+(j-1)*L], v);
				g.push(v, image[i+j*L]);
			}
		}
	}

	// i have to check if the references are actually properly updated after a copy
	// because they should now refer to the newly created vertices
	//	vertex<ValueType> v3(VT_VARIABLE);
	//	v3 = v2;
	//	cout << "Vertex copy: " << v3.index() << std::endl;

	//  print graph
	cout << "Original graph " << std::endl << std::endl;
	cout << g << std::endl;

	cout << "Make the graph into a tree" << std::endl << std::endl;
//	tree<ValueType, impl_type> t(g);
//	cout << t << std::endl;

#ifdef BUILD_EIGEN
	cout << "Build matrix from graph" << std::endl << std::endl;
	graph2matrix_type g2m;
	graph2matrix_type::matrix *m;
	m = g2m.copy(g);
	cout << *m << std::endl;

//	cout << "Build matrix from tree" << std::endl << std::endl;
//	m = g2m.copy(t);
//	cout << *m << std::endl;
#endif

	cout << "Propagate beliefs" << std::endl << std::endl;
	belief_prop bprop;
	int ticks = 10;
	for (int i = 0; i < ticks; ++i) {
		bprop.tick(	g);
	}
	//	cout << "Display tree again" << std::endl << std::endl;
	//	m = g2m.copy(t);
	//	cout << *m << std::endl;

	cout << "Done!" << std::endl;
}

/**
 * Make the modules stop after one step by returning true directly.
 */
bool BeliefModule::Stop() {
	return true;
}

