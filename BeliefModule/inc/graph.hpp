/**
 * @brief
 * @file graph.hpp
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
 * @author    Anne C. van Rossum
 * @date      Oct 24, 2012
 * @project   Replicator FP7
 * @company   Almende B.V.
 * @case      Cognitive Sensor Fusion
 */

/************************************************************************************
 * Configuration
 ************************************************************************************/

#ifndef GRAPH_HPP_
#define GRAPH_HPP_

#include <vector>
#include <string>
#include <iostream>
#include <iterator>
#include <sstream>
#include <cassert>
#include <cmath>
#include <map>
#include <typeinfo>

/**
 * The different types of implementations that can exist for a special class. The ones in
 * operation are only CIT_CHECK which adds checks and CIT_FAST which skips checks, even if
 * they would be make operation much safer.
 */
enum ClassImplType { CIT_CHECK, CIT_FAST, CIT_GPU, CIT_SSE, CIT_NOF_TYPES } ;

/**
 * A vertex is either a factor or a variable in a (bipartite) factor graph.
 */
enum VertexType { VT_FACTOR, VT_VARIABLE, VT_NOF_TYPES } ;

/**
 * We assume a sparse graph, so we only need a vertex representation and no explicit
 * edge representation. They are given by the "to" and "from" field and do not carry
 * a value themselves. In the message passing scheme, the messages are passed to all
 * neighbours (except from the one the message is received from to begin with). To
 * be able to send the message in the forward as well as the backward pass, there are
 * both "to" and "from" fields, which are of course (theoretically) redundant.
 */
template <typename T, typename P, typename M, typename N>
class vertex;

template <typename T, typename P, typename M, typename N>
class variable;

template <typename T, typename P, typename M, typename N>
class factor;

template <typename T, typename P, typename M, typename N, ClassImplType I>
class graph;

template <typename T, typename P, typename M, typename N, ClassImplType I>
class tree;

template <typename T, typename P, typename M, typename N>
class conditional_probability_table;

// following is probably incorrect
template <typename P, typename T>
class probability;

/**
 * The graph implementation.
 *
 * Vertices are stored in reference form. However, on copying from one graph to another a
 * deep copy will take place and the references will be followed. The implementation allows
 * for something like: graph<T> g; tree<T> t = g; On a deep copy of a vertex, their index
 * will not be changed.
 *
 * The edges are stored as indices, not as references, nor instances.
 *
 * Note that the implementation makes use of a default argument for a class template parameter
 * called "impl_type". This "impl_type" of the class allows for different instants of the
 * class that are either faster, performing more checks, or somehow optimized for hardware for
 * example. This will be resolved at compile time, so all code within "impl_type == CIT_CHECK"
 * if statements won't be in the final binary at all.
 */
template <typename T, typename P = float, typename M = float, typename N = size_t, ClassImplType impl_type = CIT_CHECK>
class graph {
public:
	//! To be used by users of the class to iterate through the variables
	typedef std::vector<variable<T,P,M,N> *> variable_container;

	//! To be used by users of the class to iterate through the factors
	typedef std::vector<factor<T,P,M,N> *> factor_container;

	/**
	 * Constructor does nothing special.
	 */
	graph() {
		if (impl_type > CIT_FAST) {
			std::cerr << "Class not (yet) implemented in this way!" << std::endl;
		}
		factors.clear();
		variables.clear();
	}

	/**
	 * We will not explicitly destruct all variables and factors, use erase() instead.
	 */
	~graph() {
//		erase();
	}

	/**
	 * Copy all vertices individually or else way only the pointer is copied. This is also
	 * used by subclasses (such as tree).
	 *
	 */
	graph<T,P,M,N,impl_type> & operator=(const graph<T,P,M,N,impl_type> &g) {
		for (int i = 0; i < g.factors.size(); ++i) {
			vertex<T,P,M,N> *v = new vertex<T,P,M,N >(*g.factors[i]);
			factors.push_back(v);
		}
		for (int i = 0; i < g.variables.size(); ++i) {
			vertex<T,P,M,N> *v = new vertex<T,P,M,N >(*g.variables[i]);
			variables.push_back(v);
		}
		return *this;
	}

	/**
	 * This push version adds no vertices, but it adds an edge between two existing vertices.
	 */
	bool push(factor<T,P,M,N> * src, variable<T,P,M,N> * dest) {
		if (impl_type == CIT_CHECK) {
			if ((!exists(*src)) || (!exists(*dest))) {
				std::cerr << "Source or destination vertex does not exist!" << std::endl;
				return false;
			}
		}
		src->push_to(dest->index());
		dest->push_from(src->index());
		return true;
	}

	bool push(variable<T,P,M,N> * src, factor<T,P,M,N> * dest) {
		if (impl_type == CIT_CHECK) {
			if ((!exists(*src)) || (!exists(*dest))) {
				std::cerr << "Source or destination vertex does not exist!" << std::endl;
				return false;
			}
		}
		src->push_to(dest->index());
		dest->push_from(src->index());
		return true;
	}

	bool push(variable<T,P,M,N> & v) {
		if (impl_type == CIT_CHECK) {
			if (exists(v)) {
				std::cerr << "Variable already exists!" << std::endl;
				return false;
			}
		}
		variables.push_back(&v);
		return true;
	}

	bool push(factor<T,P,M,N> & v) {
		if (impl_type == CIT_CHECK) {
			if (exists(v)) {
				std::cerr << "Factor already exists!" << std::endl;
				return false;
			}
		}
		factors.push_back(&v);
		return true;
	}

	bool exists(const factor<T,P,M,N> & v) {
		for (int i = 0; i < factors.size(); ++i) {
			if (factors[i]->equal(v)) return true;
		}
		return false;
	}

	bool exists(const variable<T,P,M,N> & v) {
		for (int i = 0; i < variables.size(); ++i) {
			if (variables[i]->equal(v)) return true;
		}
		return false;
	}

	inline size_t size() const {
		return factors.size() + variables.size();
	}

	variable<T,P,M,N> * get(N index) const  {
		for (int i = 0; i < variables.size(); ++i) {
			if (variables[i]->index() == index)
				return variables[i];
		}
		return NULL;
	}

	void erase() {
		for (int i = 0; i < variables.size(); ++i) {
			delete variables[i];
		}
		for (int i = 0; i < factors.size(); ++i) {
			delete factors[i];
		}
	}

	template <class U, class V, class W, class X, ClassImplType I>
	friend class tree;

private:
	friend std::ostream& operator<<(std::ostream & os, const graph & g) {
		os << "Graph {src} {dest}: \n";
		for (int i = 0; i < g.variables.size(); ++i) {
			os << (*g.variables[i]) << '\n';
		}
		for (int i = 0; i < g.factors.size(); ++i) {
			os << (*g.factors[i]) << '\n';
		}
		return os;
	}

public:
	variable_container variables;
	factor_container factors;

};

/**
 * A tree is just a graph without some edges. There are however different edges to remove. Our goal is
 * message passing, so the way the edges are removed is quite specific. It is an algorithm called the
 * "junction tree algorithm" and it automatically is applied when a graph is assigned to the tree
 * object.
 */
template <typename T, typename P = float, typename M = float, typename N = size_t, ClassImplType impl_type = CIT_CHECK>
class tree: public graph<T,P,M,N,impl_type> {
public:
	tree(): graph<T,P,M,N,impl_type>() {}
	~tree() {};

	// To assign a graph to a tree requires pruning all edges which makes it not
	// a tree. We perform here the junction tree algorithm.
	tree(const graph<T,P,M,N,impl_type> &g) {
		typedef graph<T,P,M,N,impl_type> super;
		super::operator=(g);
		moralization();
		triangulation();
	}

	tree<T,P,M,N,impl_type> & operator=(const graph<T,P,M,N, impl_type> &g) {
		typedef graph<T,P,M,N,impl_type> super;
		// we make first a copy of the graph where we subsequently apply "moralization" and "triangulation"
		super::operator=(g);
		moralization();
		triangulation();
		return *this;
	}

	/**
	 * Moralization turns all directed edges in a graph into undirected edges. Because really
	 * every edge is gonna be duplicated in the other direction we only need to copy all
	 * pointers in "to" to the pointers in the "from" set, and the other way around.
	 */
	void moralization() {
		typedef graph<T,P,M,N,impl_type> super;
		typename vertex<T,P,M,N>::const_iterator j;
		typename graph<T,P,M,N>::variable_container::const_iterator v_i;
		typename graph<T,P,M,N>::factor_container::const_iterator f_i;
		for (v_i = super::variables.begin(); v_i != super::variables.end(); ++v_i) {
			variable<T,P,M,N> &v = **v_i;
			for (j = v.from_begin(); j != v.from_end(); ++j) {
				if (!v.to_exists(j->first)) v.push_to(j->first);
//				if (find(v.to_begin(), v.to_end(), *j) == v.to_end()) {
//					v.push_to(j->first);
//				}
			}
			for (j = v.to_begin(); j != v.to_end(); ++j) {
				if (!v.from_exists(j->first)) v.push_from(j->first);
//				if (find(v.from_begin(), v.from_end(), *j) == v.from_end()) {
//					v.push_from(j->first);
//				}
			}
		}
		for (f_i = super::factors.begin(); f_i != super::factors.end(); ++f_i) {
			factor<T,P,M,N> &v = **f_i;
			for (j = v.from_begin(); j != v.from_end(); ++j) {
				if (!v.to_exists(j->first)) v.push_to(j->first);
//				if (find(v.to_begin(), v.to_end(), *j) == v.to_end()) {
//					v.push_to(j->first);
//				}
			}
			for (j = v.to_begin(); j != v.to_end(); ++j) {
				if (!v.from_exists(j->first)) v.push_from(j->first);
//				if (find(v.from_begin(), v.from_end(), *j) == v.from_end()) {
//					v.push_from(j->first);
//				}
			}
		}
	}

	/**
	 * Create from a moral graph a chordal graph, by introducing edges such that there is no cycle
	 * of length 4. This is edges are so-called chords.
	 */
	void triangulation() {

	}

	/**
	 * Get the root of the tree. If you descend to all vertices by going to "dest" you will never
	 * encounter the root again. However, if you go from a random node to "src" you will always end
	 * up in this vertex.
	 */
	vertex<T,P,M,N> & get_root() {
		typedef graph<T,P,M,N,impl_type> super;
		return super::front();
	}
};

/**
 * Probability is in the end just an array with values. The default probability takes two values. A
 * probability over all possible outcomes should always add up to 1.0. We won't incorporate this
 * in our class, for example through allocating (t_size-1) objects and deriving the last one by
 * subtracting the sum from one. This because of the additional computations and more complex
 * implementation of normalization.
 *
 * It would have been possible to define the probability with a default template argument, like:
 *    template <typename P, typename T=int, T t_size=2>
 * However, in that case, it would not fit nicely together in a structure with variables that have
 * a different number of outcomes. In that case, the binary and ternary variables cannot be put in
 * the same vector.
 */
template <typename P, typename T=int>
class probability {
public:
	probability(T t_size): number_of_different_outcomes(t_size) {
		outcome = new P[t_size];
	}
	~probability() {
		delete [] outcome;
	}
	T size() { return number_of_different_outcomes; }
	P & operator[](T n) { return outcome[n]; }
	const P operator[](T n) const {
		return *outcome[n];
	}

private:
	P *outcome;
	T number_of_different_outcomes;
	friend std::ostream& operator<<(std::ostream & os, const probability & p) {
		for (int i = 0; i < p.number_of_different_outcomes; ++i) {
			os << (p.outcome[i]) << ' ';
		}
		return os;
	}
};

/**
 * A conditional probability table for a factor. Preferably I would use Eigen for the d-dimensional
 * matrix representation, however, I do not want to include dependencies that are not really necessary.
 * It will store explicit references to the variables (vertices) that are involved.
 */
template <typename T, typename P = float, typename M = float, typename N = size_t>
class conditional_probability_table {
public:
	conditional_probability_table(//std::vector < N > & vertex_indices,
			std::vector<N> & dimensions): /*vertex_indices(vertex_indices),*/ dimensions(dimensions) {
		N size = 1;
		for (int i = 0; i < dimensions.size(); ++i) {
			size *= dimensions[i];
		}
		vertex_indices.clear();
		probabilities.clear();
		probabilities.reserve(size);
		for (N i = 0; i < size; ++i) probabilities.push_back(P(0));
		std::cout << "Created table with dimension " << dimensions.size(); std::endl(std::cout);
	}

	conditional_probability_table(const conditional_probability_table & other) {
		vertex_indices = other.vertex_indices;
		dimensions = other.dimensions;
		probabilities = other.probabilities;
	}

	void add_vertex(N index) {
		vertex_indices.push_back(index);
	}

	bool initialized() {
		if (vertex_indices.size() != dimensions.size()) return false;
		return true;
	}

	/*
	 * Set the conditional probability at e.g. index[2,0,1] in a dimensional object of e.g.
	 * size[3,3,2]. This would result in a linear index "i" of 2+0*3+1*3*2=8
	 */
	void set(std::vector<N> table_index, P prob) {
		probabilities[get_linear_index(table_index)] = prob;
	}

	/**
	 * Translate [i,j,k] to linear index i+j*i_size+k*i_size*j_size, etc.
	 */
	inline N get_linear_index(std::vector<N> table_index) {
		N i; size_t dim = 1;
		for (int i = 0; i < table_index.size(); ++i) {
			i += table_index[i] * dim;
			dim *= dimensions[i];
		}
		return i;
	}

	void set(N index0, P prob) {
		assert (dimensions.size() == 1);
		assert (index0 < dimensions[0]);
		N i = index0;
		probabilities[i] = prob;
	}

	void set(N index0, N index1, P prob) {
		assert (dimensions.size() == 2);
		assert (index0 < dimensions[0]);
		assert (index1 < dimensions[1]);
		N i = index0 + index1 * 2;
		probabilities[i] = prob;
	}

	void set(N index0, N index1, N index2, P prob) {
		assert (dimensions.size() == 3);
		assert (index0 < dimensions[0]);
		assert (index1 < dimensions[1]);
		assert (index2 < dimensions[2]);
		N i = index0 + index1 * dimensions[0] + index2 * dimensions[0] * dimensions[1];
		probabilities[i] = prob;
	}

	T const get(std::vector<N> table_index) {
		return probabilities[get_linear_index(table_index)];
	}

	T const get(size_t index0) {
		assert (dimensions.size() == 1);
		assert (index0 < dimensions[0]);
		N i = index0;
		return probabilities[i];
	}

	T const get(N index0, N index1) {
		assert (dimensions.size() == 2);
		assert (index0 < dimensions[0]);
		assert (index1 < dimensions[1]);
		N i = index0 + index1 * 2;
		return probabilities[i];
	}

	/**
	 * A matrix multiplication can of course be much more efficient. In this implementation
	 * it is checked all the time if the index does not run over one of the dimension's
	 * boundaries.
	 */
	void multiply(N vertex_index, probability<P,T> & p) {
		N dimension = get_dimension(vertex_index);
		assert (dimensions[dimension] == p.size());
		N inc = block_size(dimension);
		N reset = block_size(dimension+1);
		N max = max_capacity();
		for (int i = 0, j = 0; i < max; ++i) {
			probabilities[i] *= p[j];
			if (((i+1) % inc) == 0) j++;
			if (((i+1) % reset) == 0) j=0;
		}
	}

	void sum(N vertex_index, probability<P,T> & p) {
		N dimension = get_dimension(vertex_index);
		assert (dimensions[dimension] == p.size());
		N inc = block_size(dimension);
		N reset = block_size(dimension+1);
		N max = max_capacity();
		for (int j = 0; j < dimension; ++j) {
			p[j] = 0;
		}
		for (int i = 0, j = 0; i < max; ++i) {
			p[j] += probabilities[i];
			if (((i+1) % inc) == 0) j++;
			if (((i+1) % reset) == 0) j=0;
		}
	}

	N max_capacity() const {
		N max = 1;
		for (int i = 0; i < dimensions.size(); ++i) {
			max *= dimensions[i];
		}
		return max;
	}

	N get_dimensions() const {
		return dimensions.size();
	}
protected:

	/**
	 * In a d-dimensional table where you want to apply a certain calculations
	 * over one of the d-dimensions, for example multiplying over
	 */
	N block_size(N dimension) const {
		if (dimension == 0) return 1;
		N block_size = 1;
		for (int i = 0; i < dimension; ++i) {
			block_size *= dimensions[i];
		}
		return block_size;
	}

	N get_dimension(N vertex_index) const {
		for (int i = 0; i < vertex_indices.size(); ++i) {
			if (vertex_indices[i] == vertex_index) return i;
		}
		assert(false);
		return 0;
	}

private:
	// Store references to id's of original variables
	std::vector<N> vertex_indices;
	// Dimensions, e.g. 2x3x2 is stored by a vector
	std::vector<N> dimensions;
	// The table of probabilities (linearized as a vector)
	std::vector<P> probabilities;

	friend std::ostream& operator<<(std::ostream & os, const conditional_probability_table & table) {
		int dimsize=table.dimensions.size();

		// print header
		if (dimsize == 0) {
			os << "Table: [empty]"; return os;
		}
		os << "Table: [";
		os << table.dimensions[0];
		for (int i = 1; i < dimsize; ++i) {
			os << ',' << table.dimensions[i];
		}
		os << ']' << " (will be shown with most significant / latest variable first)" << '\n';

		// setup arrays for dimensions and counters
		int reset[dimsize+1];
		int dimcnt[dimsize];
		for (int i = dimsize-1; i >= 0; --i) {
			dimcnt[i] = 0; reset[i] = 1;
			for (int j = 0; j <= i; ++j) {
				reset[i+1] *= table.dimensions[j];
			}
		}

		// output matrix
		for (int i = 0; i < table.max_capacity(); ++i) {
			for (int d = dimsize-1; d >= 0; --d) {
				os << ' ' << dimcnt[d];
				if (!((i+1) % reset[d])) ++dimcnt[d];
				if (!((i+1) % reset[d+1])) dimcnt[d] = 0;
			}
			os << " : " << (table.probabilities[i]) << '\n';
		}
		return os;
	}
};

/**
 * A factor is a special type of vertex, namely one that contains a conditional probability table.
 * Depending on its incoming edges (variables) it knows the probability of the corresponding values.
 */
template <typename T, typename P = float, typename M = float, typename N = size_t>
class factor: public vertex< conditional_probability_table<T,P,M,N>, P, M, N> {
public:
	typedef conditional_probability_table<T,P,M,N> S;
	factor(): vertex<S,P,M,N>(VT_FACTOR), initialized(false) {
	}

	factor(S & cond_prob_table): vertex<S,P,M,N>(VT_FACTOR) {
		typedef vertex<S,P,M,N> super;
		super::setValue(&cond_prob_table);
		initialized = cond_prob_table.initialized();
	}

	virtual ~factor() {
//		typedef vertex<S,P,M,N> super;
//		std::cout << "Deallocate factor " << super::index(); std::endl(std::cout);
	}

//	factor<T,P,M,N>& operator*= (const factor<T,P,M,N>& f) {
//		assert(initialized);
////		assert(f.initialized);
//		factor<T,P,M,N> *newf = new factor<T,P,M,N>(f);
//	}

	void set(S & cond_prob_table) {
		typedef vertex<S,P,M,N> super;
		super::setValue(cond_prob_table);
	}

	void push_to(N index) {
		typedef vertex<S,P,M,N> super;
		super::push_to(index);
		assert (super::getValue() != NULL);
		super::getValue()->add_vertex(index);
	}

	void push_from(N index) {
		typedef vertex<S,P,M,N> super;
		super::push_from(index);
		assert (super::getValue() != NULL);
		super::getValue()->add_vertex(index);
	}

	probability<P,T> *marginal(variable<T,P,M,N> &v) {
		// multiply all entries in the conditional probability table with the evidence
		// on all vertices except for "v"
		typedef vertex<S,P,M,N> super;
		std::cout << "Calculate marginal \n";

		probability<P,T> *evidence = v.getValue();
		assert (evidence != NULL);
		S *table = new S(*super::getValue());
		int olddim = table->get_dimensions();
		table->multiply(v.index(), *evidence);
		int newdim = table->get_dimensions();

//		for (int i = 0; i < olddim; ++i) {
//			table->get()
//		}


		std::cout << *table << '\n';
		return NULL;

		for (N i = 0; i < super::to_size(); ++i) {
			if (super::to_at(i).first == v.index()) continue;
//			probability<P,T> *p_new = table->sum();
//			return *p_new;
		}
		delete table;

//		return 0;
	}
private:
	bool initialized;
};

enum FactorCoupling { FC_UNARY, FC_BINARY, FC_TERNARY, FC_COUNT } ;

/**
 * An Ising factor expects to be connected, either to one or two variables. Note that a graph is
 * defined for one type of "value" on the nodes. If you use booleans for the variables, you cannot
 * use integers for the factors. However, considering multiple booleans requires a probability
 * table of more than two entries, hence the Ising factors requires 2^N values, which is more than
 * a boolean. The simplest data type that can be used in this setting is hence a char.
 */
template <typename T, typename P = float, typename M = float, typename N = size_t>
class ising_factor: public factor<T,P,M,N> {
public:
	ising_factor(P coupling, const FactorCoupling fc): factor<T,P,M,N>() {
		if (typeid(T) == typeid(bool)) {
			std::cerr << "Type id of the graph / Ising factors should have at least the ordinality of a char";
			std::endl(std::cout);
			assert(typeid(T) != typeid(bool));
		}
		typedef factor<T,P,M,N> super;
		switch(fc) {
		case FC_UNARY: {
			typedef conditional_probability_table<T,P,M,N> S;
			std::vector<N> dimensions; dimensions.clear();
			dimensions.push_back(2);
			S *table = new S(dimensions);
			super::setValue(table);
			table->set(0, std::exp(coupling));
			table->set(1, std::exp(-coupling));
			nof_variables = 1;
			break;
		}
		case FC_BINARY: {
			typedef conditional_probability_table<T,P,M,N> S;
			std::vector<N> dimensions; dimensions.clear();
			dimensions.push_back(2);
			dimensions.push_back(2);
			S *table = new S(dimensions);
			super::setValue(table);
			table->set(0, 0, std::exp(coupling));
			table->set(0, 1, std::exp(-coupling));
			table->set(1, 0, std::exp(-coupling));
			table->set(1, 1, std::exp(coupling));
			nof_variables = 2;
			break;
		}
		default: {
			std::cerr << "Incorrect coupling"; std::endl(std::cerr);
			break;
		}
		}
	}

	bool check() {
		typedef factor<T,P> super;
		return (super::from_size() == nof_variables);
	}
private:
	int nof_variables;
};

template <typename T, typename P = float, typename M = float, typename N = size_t>
class variable: public vertex< probability<P,T>, P, M, N> {
public:
	typedef probability<P,T> S;
//	variable(): vertex<S,P,M,N>(VT_VARIABLE), initialized(false) {
////		probability<P,T> *p = new probability<P,T,N>();
////		typedef vertex<S,P,M,N> super;
////		super::setValue(p);
////		initialized = true;
//	}


	variable(N number_of_outcomes): vertex<S,P,M,N>(VT_VARIABLE) {
		//probability<P,T> *p = new probability<P,T,2>();
		probability<P,T> *p = new probability<P,T>(number_of_outcomes);
		typedef vertex<S,P,M,N> super;
		super::setValue(p);
		initialized = true;
	}

	virtual ~variable() {
//		typedef vertex<S,P,M,N> super;
//		std::cout << "Deallocate variable " << super::index(); std::endl(std::cout);
	}

private:
	bool initialized;
};

static size_t uuid = -1;

/**
 * The implementation of the vertex class. In case of an Ising variable typename "T" would be a
 * boolean. Note that for a segmentation task you will need to transform a grayscale image to something
 * like a boolean, or you will generate for each possible value a huge conditional probability table
 * on the factors.
 *
 * The following template parameters can be used:
 * @param S			type of state on the vertex, for variable nodes this is an event (probability), for
 * 					factor nodes this is a conditional probability table
 * @param T			type of an event, e.g. for a ternary event you will need at least an (u)int8_t type
 * 					default: uint8_t
 * @param P			type to indicate probabilities, should be floating point, e.g. float or double
 * 					default: float
 * @param M			type of the messages that are communicated between the nodes on the graph, e.g. float
 * 					default: float
 * @param N			type of the index to the nodes, e.g. uint8_t would limit the graph to 2^8-1 nodes
 * 					default: size_t
 */
template <typename S, typename P = float, typename M = float, typename N = size_t>
class vertex {
public:
	//! Note that for the neighbours of vertices we store only references plus a message of type T

	// Use "delegation" as programming pattern and just expose the iterator of the underlying
	// (nested) container, see http://www.cs.northwestern.edu/~riesbeck/programming/c++/stl-iterator-define.html#TOC2
	typedef std::vector<std::pair <N, M* > > vertex_index_container;
	typedef typename vertex_index_container::value_type                   value_type;
	typedef typename vertex_index_container::size_type                    size_type;
	typedef typename vertex_index_container::difference_type              difference_type;
	typedef typename vertex_index_container::reference                    reference;
	typedef typename vertex_index_container::const_reference              const_reference;
	typedef typename vertex_index_container::pointer                      pointer;
	typedef typename vertex_index_container::const_pointer                const_pointer;
	typedef typename vertex_index_container::iterator                     iterator;
	typedef typename vertex_index_container::const_iterator               const_iterator;
	typedef typename vertex_index_container::const_reverse_iterator       const_reverse_iterator;
	typedef typename vertex_index_container::reverse_iterator             reverse_iterator;
	typedef typename vertex_index_container::allocator_type               allocator_type;

	vertex(VertexType type): value(NULL), id(++uuid), vtype(type) {
		to.clear();
		from.clear();
//		std::cout << "Created vertex " << id; std::endl(std::cout);
	}

	virtual ~vertex() {
//		std::cout << "Deallocate vertex " << id; std::endl(std::cout);
	}

	vertex(const vertex<S,P,M,N> &other) {
		value = other.value;
		id = other.id;
		vtype = other.vtype;
		to = other.to;
		from = other.from;
	}

	/**
	 * Check if vertices are equal by using their index (identifier)
	 */
	bool equal(const vertex<S,P,M,N> & other) const {
		return other.id == id;
	}

	// TODO: use the copy-and-swap idiom
	vertex<S,P,M,N> & operator=(const vertex<S,P,M,N> &other) {
		std::cerr << "Debug: copy action on vertex " << other.id; std::endl(std::cerr);
		value = other.value;
		id = other.id;
		vtype = other.vtype;
		to = other.to;
		from = other.from;
		return *this;
	}

	inline N index() const { return id; }
	inline VertexType getType() const  { return vtype; }
	inline void setValue(S * value) { this->value = value; }
	inline S* getValue() const { return value; }

	/**
	 * Friend inline. We use the "introvert" variant of the serial operator. The vertex needs to be
	 * of the same type T as the class. We do not(!) create a templated version of the serialization
	 * operator.
	 * See: http://stackoverflow.com/questions/4660123/overloading-friend-operator-for-template-class
	 */
	friend std::ostream& operator<<(std::ostream & os, const vertex & v) {
		os << v.id << "(" << v.vtype << "): {";
		if (!v.from.empty()) os << v.from[0].first;
		for (int i = 1; i < v.from.size(); ++i) os << ',' << v.from[i].first;
		os << "} {";
		if (!v.to.empty()) os << v.to[0].first;
		for (int i = 1; i < v.to.size(); ++i) os << ',' << v.to[i].first;
		os << "}";
		return os;
	}

	iterator to_begin() { return to.begin(); }
	const_iterator to_begin() const { return to.begin(); }
	iterator to_end() { return to.end(); }
	const_iterator to_end() const { return to.end(); }
	iterator from_begin() { return from.begin(); }
	const_iterator from_begin() const { return from.begin(); }
	iterator from_end() { return from.end(); }
	const_iterator from_end() const { return from.end(); }

	size_t from_size() const { return from.size(); }
	size_t to_size() const { return to.size(); }

	reference from_at(size_t n) { return from[n]; }
	const_reference from_at(size_t n) const { return from[n]; }

	reference to_at(size_t n) { return to[n]; }
	const_reference to_at(size_t n) const { return to[n]; }

	bool to_exists(size_t n) {
		for (size_t i = 0; i < to.size(); ++i) {
			if (to[i].first == n) return true;
		}
		return false;
	}

	bool from_exists(size_t n) {
		for (size_t i = 0; i < from.size(); ++i) {
			if (from[i].first == n) return true;
		}
		return false;
	}

	void push_to(N index) {
		to.push_back(std::make_pair(index,new M(0)));
	}

	void push_from(N index) {
		from.push_back(std::make_pair(index,new M(0)));
	}

private:
	// Value on the node, can be used to accumulate incoming messages
	S *value;

	// Identifier of the node, increases on construction
	N id;

	// Indicates if vertex is a factor node, if not, it's a variable node, in a
	// bipartite graph they are only allowed to be connected with the other node type
	VertexType vtype;

	// Outgoing edges
	vertex_index_container to;

	// Incoming edges
	vertex_index_container from;

	// Just befriend all graph instantiations
	// http://stackoverflow.com/questions/13055447/c-how-to-specify-all-friends-of-a-templated-class-with-a-default-argument
	//	template<class U, ClassImplType I>
	//	template <class U, class V, class W, class X, ClassImplType I>
	//	friend class graph;
//	template <class U, class V, class W, class X, ClassImplType I>
//	friend class graph;

	//	template<class U, ClassImplType I>
	//	friend class tree;
};


#endif /* GRAPH_HPP_ */
