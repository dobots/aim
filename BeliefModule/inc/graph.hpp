/**
 * 456789------------------------------------------------------------------------------------------------------------120
 *
 * @brief Graph data structure for message passing on a graphical model
 * @file graph.hpp
 * 
 * This file is created at Almende B.V. and Distributed Organisms B.V. It is open-source software and belongs to a
 * larger suite of software that is meant for research on self-organization principles and multi-agent systems where
 * learning algorithms are an important aspect.
 *
 * This software is published under the GNU Lesser General Public license (LGPL).
 *
 * It is not possible to add usage restrictions to an open-source license. Nevertheless, we personally strongly object
 * against this software being used for military purposes, factory farming, animal experimentation, and "Universal
 * Declaration of Human Rights" violations.
 *
 * Copyright © 2013 Anne van Rossum <anne@almende.com> <anne@dobots.nl>
 *
 * @author    Anne C. van Rossum
 * @date      Oct 24, 2012
 * @project   Replicator FP7
 * @company   Almende B.V.
 * @company   Distributed Organisms B.V.
 * @case      Cognitive Sensor Fusion
 */

/***********************************************************************************************************************
 * Configuration
 **********************************************************************************************************************/

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
 * The different types of implementations that can exist for a special class. The ones in operation are only CIT_CHECK
 * which adds checks and CIT_FAST which skips checks, even if they would be make operation much safer.
 */
enum ClassImplType { CIT_CHECK, CIT_FAST, CIT_GPU, CIT_SSE, CIT_NOF_TYPES } ;

/**
 * A vertex is either a factor or a variable in a (bipartite) factor graph.
 */
enum VertexType { VT_FACTOR, VT_VARIABLE, VT_NOF_TYPES } ;

/**
 * We assume a sparse graph, so we only need a vertex representation and no explicit edge representation. They are given
 * by the "to" and "from" field and do not carry a value themselves. In the message passing scheme, the messages are
 * passed to all neighbours (except from the one the message is received from to begin with). To be able to send the
 * message in the forward as well as the backward pass, there are both "to" and "from" fields, which are of course
 * (theoretically) redundant.
 */
template <typename T, typename S, typename P, typename M, typename N>
class vertex;

//! The variable class is derived from the vertex class.
template <typename T, typename P, typename M, typename N>
class variable;

//! The factor class is derived from the vertex class.
template <typename T, typename P, typename M, typename N>
class factor;

template <typename T, typename P, typename M, typename N, ClassImplType I>
class graph;

template <typename T, typename P, typename N>
class conditional_probability_table;

//! There is a "probability" entity, see class for description
template <typename P, typename T>
class probability;

/**
 * The graph implementation.
 *
 * Vertices are stored in reference form. However, on copying from one graph to another a deep copy will take place and
 * the references will be followed. The implementation allows for something like: graph<T> g; tree<T> t = g; On a deep
 * copy of a vertex, their index will not be changed. The edges are stored as indices, not as references, nor instances.
 *
 * Note that the implementation makes use of a default argument for a class template parameter called "impl_type". This
 * "impl_type" of the class allows for different instants of the class that are either faster, performing more checks,
 * or somehow optimized for hardware for example. This will be resolved at compile time, so all code within
 * "impl_type == CIT_CHECK" if statements won't be in the final binary at all.
 *
 * The code in these files is operational on a level of basic probabilities, but it is not working for more
 * sophisticated stuff. For example, to implement variable elimination we need a data structure that is a so-called
 * "cluster graph". This is a graphical flowchart of the factor-manipulation process. What we have here is however, only
 * something that can be used for message passing over a fixed factor graph and is hence not so flexible. More on
 * cluster graphs you can read in "Variable Elimination and Clique Trees" in Section 10.1.1 in PGM.
 *
 * The following template parameters can be used. Except for the "type" template, this is the same set of template
 * parameters as for the variable and the factor class:
 * @param T		type of an event, e.g. for a ternary event you will need at least an (u)int8_t type, default: uint8_t
 * @param P		type to indicate probabilities, should be floating point, e.g. float or double, default: float
 * @param M		type of the messages that are communicated between the nodes on the graph, default: probability<P,T>
 * @param N		type of the index to the nodes, e.g. uint8_t would limit the graph to 2^8-1 nodes, default: size_t
 */
template <typename T, typename P = float, typename M = probability<P,T>, typename N = size_t,
		ClassImplType impl_type = CIT_CHECK>
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
	 * Copy all vertices individually or else way only the pointer is copied. This is also used by subclasses (such as
	 * the tree class).
	 */
	graph<T,P,M,N,impl_type> & operator=(const graph<T,P,M,N,impl_type> &g) {
		for (int i = 0; i < g.factors.size(); ++i) {
			factor<T,P,M,N> *v = new factor<T,P,M,N >(*g.factors[i]);
			factors.push_back(v);
		}
		for (int i = 0; i < g.variables.size(); ++i) {
			variable<T,P,M,N> *v = new variable<T,P,M,N >(*g.variables[i]);
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

/***********************************************************************************************************************
 * Definition of the data structures that store the random variables and the joint distributions
 **********************************************************************************************************************/

/**
 * Probability is a "difficult" entity when we move from the mathematical world to computers. This class could have been
 * called "random variable". But, it is actually not a random variable which is a mapping from a sample space to real
 * numbers. With a fair coin, "heads" can for example be arbitrarily mapped to "3.1415", "tails" to "5.76". It is
 * neither a "probability mass function" in the sense that we define the probabilities corresponding to those values
 * over the entire domain of real numbers (which gives those nice plots). It is only an array in which we assume that
 * the user keeps track of the order of the outcomes throughout the program and we store only the probabilities for each
 * of these outcomes.
 *
 * This "impoverished version" of the probability mass function is not normalized. It can be used to characterize a
 * discrete probability distribution (and henceforth a discrete random variable), but it can also be used in an
 * unnormalized fashion: as messages in message passing.
 *
 * It would have been possible to define the probability with a default template argument, like:
 *    template <typename P, typename T=int, T t_size=2>
 * However, in that case, it would not fit nicely together in a structure with variables that have a different number of
 * outcomes. In that case, the binary and ternary variables cannot be put in the same vector: probability<P,T>.
 *
 * The following template parameters can be used:
 * @param T		type of an event, e.g. for a ternary event you will need at least an (u)int8_t type, default: uint8_t
 * @param P		type to indicate probabilities, should be floating point, e.g. float or double, default: float
 */
template <typename P, typename T=int>
class probability {
public:
	// Create an empty "probability mass function", does memory allocation
	probability(T t_size): cardinality(t_size) {
		outcome = new P[t_size];
	}
	// Destructor
	~probability() {
		delete [] outcome;
	}
	// The cardinality of this random variable
	const T size() const { return cardinality; }
	// The probability of the given indexed outcome (order preserved)
	P & operator[](T n) { return outcome[n]; }
	// The probability of the given indexed outcome (order preserved)
	const P operator[](T n) const {
		return *outcome[n];
	}
	// Define multiplication with another "probability"
	probability<P,T> operator*(const probability<P,T> &other) {
		probability<P,T> result(other.size());
		if (size() != other.size()) {
			std::cerr << "Sizes are different: " << size() << " vs " << other.size() << std::endl;
		}
		assert (size() == other.size());
		for (T i = 0; i < size(); ++i)
			result[i] = outcome[i]*other[i];
		return result;
	}

	probability<P,T> & operator*=(probability<P,T> &other) {
		if (size() != other.size()) {
			std::cerr << "Sizes are different: " << size() << " vs " << other.size() << std::endl;
		}
		assert (size() == other.size());
		for (T i = 0; i < size(); ++i)
			outcome[i] *= other[i];
		return *this;
	}

	probability<P,T> & operator*(const P &other) {
		probability<P,T> result(other.size());
		for (T i = 0; i < size(); ++i)
			result[i] *= other;
		return result;
	}

	probability<P,T> & operator*=(P &other) {
		for (T i = 0; i < size(); ++i)
			outcome[i] *= other;
		return *this;
	}

private:
	// The outcome array stores a probability for every possible outcome
	P *outcome;
	// The cardinality is the total number of possible outcomes
	T cardinality;
	// The probability of every outcome can be printed as space-separated stream
	friend std::ostream& operator<<(std::ostream & os, const probability & p) {
		for (int i = 0; i < p.cardinalities; ++i) {
			os << (p.outcome[i]) << ' ';
		}
		return os;
	}
};

/**
 * A conditional probability table for a factor. Preferably I would use Eigen for the d-dimensional matrix
 * representation, however, I do not want to include dependencies that are not really necessary. It will store explicit
 * references to the variables (vertices) that are involved.

 * The following template parameters can be used:
 * @param T		type of an event, e.g. for a ternary event you will need at least an (u)int8_t type, default: uint8_t
 * @param P		type to indicate probabilities, should be floating point, e.g. float or double, default: float
 * @param N		type of the size of the total table, default: size_t
 */
template <typename T, typename P = float, typename N = size_t>
class conditional_probability_table {
public:

	/**
	 * Create a new, empty, conditional probability table. It requires one vector "dimensions".
	 * This creates an object that is quite large. A vector "2, 3, 4" creates an array of size
	 * 2x3x4 to store all individual conditional probabilities. Internally, everything is stored
	 * as a linear array.
	 *
	 * The proper name for "dimensions" here is "cardinality".
	 */
	conditional_probability_table(std::vector<N> & dimensions): cardinalities(dimensions) {
		N size = 1;
		strides.clear();
		vertex_indices.clear();
		probabilities.clear();
		probabilities.reserve(size);
		for (int i = 0; i < dimensions.size(); ++i) {
			strides.push_back(size);
			size *= dimensions[i];
		}
		strides.push_back(size);
		for (N i = 0; i < size; ++i) probabilities.push_back(P(0));
		std::cout << "Created table with dimension " << dimensions.size(); std::endl(std::cout);
	}

	conditional_probability_table(const conditional_probability_table & other) {
		vertex_indices = other.vertex_indices;
		cardinalities = other.cardinalities;
		probabilities = other.probabilities;
	}

	/**
	 * Adds a vertex to the internally maintained lists of vertices. This should be a unique
	 * identifier.
	 */
	void add_vertex(N index) {
		vertex_indices.push_back(index);
	}

	/**
	 * The dimension of the conditional probability table is defined in the constructor. Subsequently
	 * the vertices that it is about can be added at a later stage through add_vertex. Hence, this
	 * function checks if these figures actually match.
	 */
	bool initialized() {
		if (vertex_indices.size() != cardinalities.size()) return false;
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
	 * Translate [i,j,k] to linear index i+j*i_size+k*i_size*j_size, etc. Actually, it is better
	 * to store the "stride" per variable. Now, we are calculating it every time we use
	 * get_linear_index() by the multiplication of "dim".
	 */
	inline N get_linear_index(std::vector<N> table_index) {
		N i; size_t dim = 1;
		for (int i = 0; i < table_index.size(); ++i) {
			i += table_index[i] * strides[i];
		}
		return i;
	}

	inline std::vector<N> get_tabular_index(N linear_index) {
		assert(false); // to be done
		std::vector<N> result(cardinalities.size());
		N i; size_t dim = 1;
//		for (int i = 0; i < table_index.size(); ++i) {
//			i += table_index[i] * strides[i];
//		}
		return result;
	}


	void set(N index0, P prob) {
		assert (cardinalities.size() == 1);
		assert (index0 < cardinalities[0]);
		N i = index0;
		probabilities[i] = prob;
	}

	void set(N index0, N index1, P prob) {
		assert (cardinalities.size() == 2);
		assert (index0 < cardinalities[0]);
		assert (index1 < cardinalities[1]);
		N i = index0 + index1 * 2;
		probabilities[i] = prob;
	}

	void set(N index0, N index1, N index2, P prob) {
		assert (cardinalities.size() == 3);
		assert (index0 < cardinalities[0]);
		assert (index1 < cardinalities[1]);
		assert (index2 < cardinalities[2]);
		N i = index0 + index1 * cardinalities[0] + index2 * cardinalities[0] * cardinalities[1];
		probabilities[i] = prob;
	}

	T const get(std::vector<N> table_index) {
		return probabilities[get_linear_index(table_index)];
	}

	//! Gets a random variable
	probability<P,T> const get(std::vector<N> table_index, N summarize) {
		N cardinality = cardinalities[summarize];
		probability<P,T> result(cardinality);
		for (int i = 0; i < cardinality; ++i) {
			table_index[summarize] = i;
			result[i] = probabilities[get_linear_index(table_index)];
		}
		return result;
	}

//	probability<P,T> const get(N table_index, N summarize) {
//		N cardinality = cardinalities[summarize];
//		probability<P,T> result(cardinality);
//		for (int i = 0; i < cardinality; ++i) {
////			table_index[summarize] = i; // adjust table_index, inverse of get_linear...
//			result[i] = probabilities[table_index];
//		}
//		return result;
//	}

	T const get(size_t index0) {
		assert (cardinalities.size() == 1);
		assert (index0 < cardinalities[0]);
		N i = index0;
		return probabilities[i];
	}

	T const get(N index0, N index1) {
		assert (cardinalities.size() == 2);
		assert (index0 < cardinalities[0]);
		assert (index1 < cardinalities[1]);
		N i = index0 + index1 * 2;
		return probabilities[i];
	}

	/**
	 * A matrix multiplication can of course be much more efficient. In this implementation
	 * there is a for-loop in which the boundaries of a given dimension are checked every
	 * time. So, in a conditional probability table of size 2x3x4=24, we have a variable that
	 * corresponds to the first dimension, one with the second, and one with the third. If
	 * we multiply with the second variable we need a probability that is of "size" 3, and
	 * we will multiply 2x4 values with the first value, another 8 with the next, and another
	 * 8 with the last probability. So, all values in this 3D object will be changed (except
	 * if p = {1,1,1}).
	 *
	 * @param vertex_index		unique reference to the vertex (see add_vertex)
	 * @param p					the probabilities with which all values will be multiplied
	 */
	void multiply(N vertex_index, probability<P,T> & p) {
		N dimension = get_dimension(vertex_index);
		assert (cardinalities[dimension] == p.size());
		N inc = strides[dimension];
		N reset = strides[dimension+1];
		N max = size();
		for (int i = 0, j = 0; i < max; ++i) {
			probabilities[i] *= p[j];
			if (((i+1) % inc) == 0) j++;
			if (((i+1) % reset) == 0) j=0;
		}
	}

	/**
	 * The opposite of multiply, with a specific exception clause to prevent division by zero.
	 * An example of factor division is given in section 10.3.1 "Message Passing with Division"
	 * in Probabilistic Graphical Models (Koller and Friedman), Figure 10.7. This is a nice
	 * schema that prevents multiplication of all subsets of variables with only one variable
	 * excluded.
	 */
	void divide(N vertex_index, probability<P,T> &p) {
		N dimension = get_dimension(vertex_index);
		assert (cardinalities[dimension] == p.size());
		N inc = strides[dimension];
		N reset = strides[dimension+1];
		N max = size();
		for (int i = 0, j = 0; i < max; ++i) {
			if (p[j] != 0)
				probabilities[i] /= p[j];
			else
				probabilities[i] = 0;
			if (((i+1) % inc) == 0) j++;
			if (((i+1) % reset) == 0) j=0;
		}
	}

	/**
	 * See the detailed explanation of the function multiply, but the "direction" of information
	 * is the other way around. The conditional probability table is left unchanged. The probability
	 * "p" is adjusted in the sense that all values "intersecting" with it, are added up and added
	 * as its entries.
	 */
	void sum(N vertex_index, probability<P,T> & p) {
		N dimension = get_dimension(vertex_index);
		assert (cardinalities[dimension] == p.size());
		N inc = strides[dimension];
		N reset = strides[dimension+1];
		N max = size();
		for (int j = 0; j < dimension; ++j) {
			p[j] = 0;
		}
		for (int i = 0, j = 0; i < max; ++i) {
			p[j] += probabilities[i];
			if (((i+1) % inc) == 0) j++;
			if (((i+1) % reset) == 0) j=0;
		}
	}

	/**
	 * Returns the maximum size in the sense of memory requirements for this conditional probability
	 * table.
	 */
	N size() const {
		N max = 1;
		for (int i = 0; i < cardinalities.size(); ++i) {
			max *= cardinalities[i];
		}
		return max;
	}

	/**
	 * Get the number of dimensions. A table with 4 variables, that have each a certain size, has a
	 * total number of dimensions equal to 4. If each variable corresponds to a gray level in an image
	 * this does not matter. If each pixel is considered as a variable however, this will add a dimension
	 * for each pixel. With 10 bins for gray levels and 1000 pixels, there are 1000 dimensions. The
	 * required capacity would be 10^1000 (compared to 10^82 atoms in the universe). There is just one
	 * message here: do not store dependencies unless you have to. :-)
	 */
	N get_dimensions() const {
		return cardinalities.size();
	}
protected:


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
	std::vector<N> cardinalities;
	// The table of probabilities (linearized as a vector)
	std::vector<P> probabilities;

	std::vector<N> strides;

	/**
	 * Printing the table is useful for debugging purposes. The table does not have access to the
	 * variables or factors involved, it is just a table. So, there is no way to equip it with variable
	 * names for example.
	 */
	friend std::ostream& operator<<(std::ostream & os, const conditional_probability_table & table) {
		int dimsize=table.cardinalities.size();

		// print header
		if (dimsize == 0) {
			os << "Table: [empty]"; return os;
		}
		os << "Table: [";
		os << table.cardinalities[0];
		for (int i = 1; i < dimsize; ++i) {
			os << ',' << table.cardinalities[i];
		}
		os << ']' << " (will be shown with most significant / latest variable first)" << '\n';

		// setup arrays for dimensions and counters
		int reset[dimsize+1];
		int dimcnt[dimsize];
		for (int i = dimsize-1; i >= 0; --i) {
			dimcnt[i] = 0; reset[i] = 1;
			for (int j = 0; j <= i; ++j) {
				reset[i+1] *= table.cardinalities[j];
			}
		}

		// output matrix
		for (int i = 0; i < table.size(); ++i) {
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
 * A factor is a special type of vertex, namely one that contains a reference to a conditional
 * probability table. More general we can state that a factor for all incoming connections to
 * variables comes up with an answer, in other words, it is a function defined over variables.
 * Depending on its incoming edges (variables) it knows the probability of the corresponding
 * values. Rather than implementing the factor as a general function, we subclass it here
 * from a vertex with a conditional probability table as "function".
 *
 * The following template parameters can be used, observe that this is one parameter less than
 * for the vertex class, which has an additional S state template parameter.
 * @param T			type of an event, e.g. for a ternary event you will need at least an (u)int8_t type
 * 					default: uint8_t
 * @param P			type to indicate probabilities, should be floating point, e.g. float or double
 * 					default: float
 * @param M			type of the messages that are communicated between the nodes on the graph
 * 					default: probability<P,T>
 * @param N			type of the index to the nodes, e.g. uint8_t would limit the graph to 2^8-1 nodes
 * 					default: size_t
 */
template <typename T, typename P = float, typename M = probability<P,T>, typename N = size_t>
class factor: public vertex<T, conditional_probability_table<T,P,N>, P, M, N> {
public:
	// Shorthand for the conditional probability table is "S"
	typedef conditional_probability_table<T,P,N> S;

	// Construct factor, but do not fill with table values yet
	factor(): vertex<T,S,P,M,N>(VT_FACTOR), initialized(false) {
	}

	factor(S & cond_prob_table, std::string name=""): vertex<T,S,P,M,N>(VT_FACTOR) {
		typedef vertex<T,S,P,M,N> super;
		super::setValue(&cond_prob_table);
		initialized = cond_prob_table.initialized();
		this->name = name;
	}

	virtual ~factor() {
	}

	void set(S & cond_prob_table) {
		typedef vertex<T,S,P,M,N> super;
		super::setValue(cond_prob_table);
	}

	void push_to(N index) {
		typedef vertex<T,S,P,M,N> super;
		super::push_to(index);
		assert (super::getValue() != NULL);
		super::getValue()->add_vertex(index);
	}

	void push_from(N index) {
		typedef vertex<T,S,P,M,N> super;
		super::push_from(index);
		assert (super::getValue() != NULL);
		super::getValue()->add_vertex(index);
	}

	/**
	 * Calculate the marginal with respect to a certain variable. This can be calculated
	 * if all variables attached to this factor do come with data with respect to their
	 * probabilities. In other words, we have to multiply each field with the evidence
	 * coming from all incoming variables including the incoming one. And then divide
	 * the last one out again. This would work, except in the case that the incoming
	 * variable does have a zero on one of its probability values.
	 *
	 * However, on this level we do not have access to anything on those vertices except
	 * for the messages they send to us. Calculating the marginal is typically something
	 * done on the "belief propagation" level that has access to the entire graph.
	 *
	 * What we would be able to do here is to marginalize out the variable v. However, the
	 * result would not be a probability, but a new factor with a smaller dimension. We
	 * sum out this variable v, also called variable elimination.
	 */
	probability<P,T> *marginal(variable<T,P,M,N> &v) {
		assert(false); // this

		// multiply all entries in the conditional probability table with the evidence
		// on all vertices except for "v"
		typedef vertex<T,S,P,M,N> super;
		std::cout << "Calculate marginal \n";

		probability<P,T> *evidence = v.getValue();
		assert (evidence != NULL);

		// create a temporary table to calculate the joint distribution
		S *table = new S(*super::getValue());
		for (N i = 0; i < table->get_dimensions(); ++i) {
			N e_i = super::to_at(i).first;
			if (e_i == v.index()) continue; // don't multiply with this very vertex
//			probability<P,T> *e = super::to_at(i).getsomehowthatvertexisimpossible().
//			table->multiply(e_i, *e);
		}

		probability<P,T> *m = new probability<P,T>(evidence->size());
		table->sum(v.index(), *m);

//		int olddim = table->get_dimensions();
//		table->multiply(v.index(), *evidence);
//		int newdim = table->get_dimensions();

		std::cout << *table << '\n';
//		return NULL;

//		for (N i = 0; i < super::to_size(); ++i) {
//			if (super::to_at(i).first == v.index()) continue;
//			probability<P,T> *p_new = table->sum();
//			return *p_new;
//		}
		delete table;

		return m;
	}
private:
	bool initialized;
};

enum FactorCoupling { FC_UNARY, FC_BINARY, FC_TERNARY, FC_COUNT } ;


/**
 * A variable is a vertex in the factor graph that is only connected to factor nodes.
 * It has a number of outcomes or possible values each with a certain probability. A
 * variable attached to a dice has 6 outcomes with equal probabilities. A variable
 * attached to a pixel has for example 16 outcomes (bins of 16 gray values adding up
 * to a total of 256 values) with each of these outcomes a probability that depends
 * on the scenario. A sunny day will make the "lighter" outcomes much more likely.
 *
 * The following template parameters can be used, observe that this is one parameter less than
 * for the vertex class, which has an additional S state template parameter.
 * @param T			type of an event, e.g. for a ternary event you will need at least an (u)int8_t type
 * 					default: uint8_t
 * @param P			type to indicate probabilities, should be floating point, e.g. float or double
 * 					default: float
 * @param M			type of the messages that are communicated between the nodes on the graph
 * 					default: probability<P,T>
 * @param N			type of the index to the nodes, e.g. uint8_t would limit the graph to 2^8-1 nodes
 * 					default: size_t
 */
template <typename T, typename P = float, typename M = probability<P,T>, typename N = size_t>
class variable: public vertex< T, probability<P,T>, P, M, N> {
public:
	typedef probability<P,T> S;

	/**
	 * A variable comes with a name and a series of outcomes with different probabilities
	 * defined by a probability density function. The latter is stored internally on a
	 * "value" field as an "probability<P,T>" object. Moreover, the variable is defined as
	 * a vertex and is hence an "enriched" random variable.
	 */
	variable(N cardinality, std::string name=""): vertex<T,S,P,M,N>(VT_VARIABLE) {
		S *p = new S(cardinality);
		typedef vertex<T,S,P,M,N> super;
		super::setValue(p);
		this->name = name;
	}

	/**
	 * Get cardinality of this (random) variable
	 */
	N cardinality() {
		typedef vertex<T,S,P,M,N> super;
		return super::getValue()->size();
	}

	// Destruction does not deallocate the value
	virtual ~variable() { }

private:
};

/**
 * Global counter that increments the unique identifier for every vertex in the factor graph.
 * The first vertex gets the id 0.
 */
static size_t uuid = -1;

/**
 * The implementation of the vertex class. In case of an Ising variable typename
 * "T" would be a boolean. Note that for an image segmentation task you will
 * need to map your samples (for example a grayscale image to real numbers:
 * probabilities). Small bins will result in huge conditional probability table
 * on the factors.
 *
 * The following template parameters can be used:
 * @param T		type of an event, e.g. for a ternary event you will need at
 * 				least an (u)int8_t type, default: uint8_t
 * @param S		type of state on the vertex, for variable nodes this is an event
 * 				(probability), for factor nodes this is a conditional
 * 				probability table
 * @param P		type to indicate probabilities, should be floating point, e.g.
 * 				float or double, default: float
 * @param M		type of the messages that are communicated between the nodes on
 * 				the graph, default: probability<P,T>
 * @param N		type of the index to the nodes, e.g. uint8_t would limit the
 * 				graph to 2^8-1 nodes, default: size_t
 */
template <typename T, typename S, typename P = float,
		typename M = probability<P,T>, typename N = size_t>
class vertex {
public:
	//! Note that for the neighbours of vertices we store only references plus a message of type T

	// Use "delegation" as programming pattern and just expose the iterator of the underlying
	// (nested) container, see http://www.cs.northwestern.edu/~riesbeck/programming/c++/stl-iterator-define.html#TOC2

	//! Reference to pair of vertex index (first) and message "object" (second) which is by default a float.
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

	vertex(const vertex<T,S,P,M,N> &other) {
		value = other.value;
		id = other.id;
		vtype = other.vtype;
		to = other.to;
		from = other.from;
	}

	/**
	 * Check if vertices are equal by using their index (identifier)
	 */
	bool equal(const vertex<T,S,P,M,N> & other) const {
		return other.id == id;
	}

	// TODO: use the copy-and-swap idiom
	vertex<T,S,P,M,N> & operator=(const vertex<T,S,P,M,N> &other) {
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
		os << v.id;
		os << "[" << v.name << "] ";
		os << "(" << (v.vtype ? "var" : "factor") << "): {";
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

	//! Get reference to indices of outgoing vertices (not the vertices themselves)
	reference to_at(size_t n) { return to[n]; }
	const_reference to_at(size_t n) const { return to[n]; }

	//! Check if outgoing edge with given index n exists
	bool to_exists(size_t n) {
		for (size_t i = 0; i < to.size(); ++i) {
			if (to[i].first == n) return true;
		}
		return false;
	}

	//! Check if incoming edge with given index n exists
	bool from_exists(size_t n) {
		for (size_t i = 0; i < from.size(); ++i) {
			if (from[i].first == n) return true;
		}
		return false;
	}

	//! Push new index with default message on the list of outgoing edges
	void push_to(N index) {
		to.push_back(std::make_pair(index,new M(0)));
	}

	//! Push new index with default message on the list of incoming edges
	void push_from(N index) {
		from.push_back(std::make_pair(index,new M(0)));
	}

protected:
	// just for debugging purposes
	std::string name;
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
