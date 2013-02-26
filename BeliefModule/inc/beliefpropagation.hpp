/**
 * 456789------------------------------------------------------------------------------------------------------------120
 *
 * @brief Belief propagation or message passing on a factor graph
 * @file beliefpropagation.hpp
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
 * @date      Oct 26, 2012
 * @project   Replicator FP7
 * @company   Almende B.V.
 * @case      Cognitive Sensor Fusion
 */

/***********************************************************************************************************************
 * Configuration
 **********************************************************************************************************************/

#ifndef BELIEFPROPAGATION_HPP_
#define BELIEFPROPAGATION_HPP_

namespace dobots {

/**
 * There are several aspects with respect to belief propagation. There are different
 * manners in which an update scheme can be chosen. Each node in the graph can be
 * selected randomly, or in a certain order, or starting from leaves, etc. Then, what
 * is actually a factor? Does it operate on probabilities? Is "p(x)" the probability
 * that pixel x occurs with a certain probability (say 1:256)? No, "x" is not the
 * "value" of the pixel, it is the "label" that we want to get per pixel. So, "x"
 * can be an index for the segment in a segmentation task, or depth in a depth map.
 * Mmm... not entirely correct. The original pixel value is also part of the model.
 * It is the state of a visible variable which just shouldn't be adjusted. The ones
 * from the segmentation or depth map should however be adjusted.
 */
template <typename T, typename P = float, typename M = probability<P,T>, typename N = size_t>
class beliefpropagation {
public:
	beliefpropagation(): convergence(false), directed(true) {}

	/**
	 * Do one pass of the message passing scheme. This will have as side-effect that
	 * the convergence parameter is set, see @ converged().
	 *
	 * It is not possible to just start ticking variables and factors. Information flows in
	 * a certain way through the network. If there is a certain
	 */
	template <ClassImplType impl_type>
	void tick(const graph<T,P,M,N,impl_type> &g) {
		if (is_directed(g)) {
			std::cerr << "Graph should be undirected" << std::endl;
			return;
		}
		typename graph<T,P,M,N>::variable_container::const_iterator v_i;
		for (v_i = g.variables.begin(); v_i != g.variables.end(); ++v_i) {
			variable<T,P,M,N> &v = **v_i;
			tick_variable(g, v);
		}
		typename graph<T,P,M,N>::factor_container::const_iterator f_i;
		for (f_i = g.factors.begin(); f_i != g.factors.end(); ++f_i) {
			factor<T,P,M,N> &v = **f_i;
			tick_factor(g, v);
		}

	}

	bool converged() { return convergence; }

protected:
	/**
	 * Check if the graph is actually indeed undirected in the sense that all vertices in the
	 * "from" array are also in the "to" array.
	 */
	template <ClassImplType impl_type>
	bool is_directed(const graph<T,P,M,N,impl_type> &g) {
		typename graph<T,P,M,N>::variable_container::const_iterator v_i;
		typename graph<T,P,M,N>::factor_container::const_iterator f_i;
		for (v_i = g.variables.begin(); v_i != g.variables.end(); ++v_i) {
			variable<T,P,M,N> &v = **v_i;
			for (int j = 0; j < v.from_size(); ++j) {
				if (!v.to_exists(v.from_at(j).first)) return set_directed(true);
			}
		}
		for (f_i = g.factors.begin(); f_i != g.factors.end(); ++f_i) {
			factor<T,P,M,N> &v = **f_i;
			for (int j = 0; j < v.from_size(); ++j) {
				if (!v.to_exists(v.from_at(j).first)) return set_directed(true);
			}
		}
		return set_directed(false);
	}

	bool set_directed(bool directed) {
		this->directed = directed;
		return directed;
	}

	/**
	 * A factor is normally a function that is defined over the variables it is connected to. In this case, there is a
	 * conditional probability table defined on the factor. It is used together with the values arriving over the edges
	 * from the variables to calculate the "not sum's" for each variable. The incoming values are probabilities which
	 * are characterized in the discrete case by a small vector.
	 *
	 * We calculate here for example:
	 *
	 *   \sum_{x2} ( fb(x2,x3) \mu_{x2->fb}(x2) ) for all x2
	 *
	 * which means in case x2 takes the values 0 and 1:
	 *    fb(0,x3) \mu_{x2->fb}(0)
	 *    fb(1,x3) \mu_{x2->fb}(1) +
	 *    ------------------------
	 *                      xxxxxx
	 *
	 * The values fb (of factor "b") are stored in the "jointtable", the conditional probability table. We iterate over
	 * all possible values, retrieving them as a vector "table_index". That runs from (0,0,0), (0,0,1), etc. over all
	 * variables. The index "j" indicates the variable we are going to send a message towards too (and which should be
	 * excluded from the summation). With get() we only get the values in the conditional probability table that are
	 * associated with this variable. Suppose it is x2 we sum over, it is fb(0,x3) and fb(1,x3). If it is x1 and x2 we
	 * sum over, it is fb(0,0,x3), fb(0,1,x3), fb(1,0,x3), and fb(1,1,x3). Observe that the entity we retrieve is of the
	 * cardinality of x3. So fb(0,x3) results in values for the random variable x3 that correspond to x2=0, and other
	 * values for x2=1. The outer loop "i" is over all possible permutations, so fb(0,0), fb(0,1), fb(1,0), and fb(1,1).
	 * Hence, we get the variable x3 multiple times for x2=0, hence there is one if condition that uses it only once.
	 *
	 * "jointvalue" with one specific variable, say fb(0,x3) for variable x3.
	 *
	 * A factor sums over all variables except for one.
	 */
	template <ClassImplType impl_type>
	void tick_factor(const graph<T,P,M,N,impl_type> &g, factor<T,P,M,N> & v) {

		// make sure X=f(X) goes well

		conditional_probability_table<T,P,N> jointtable = *v.getValue();
		N size = jointtable.size();
		std::vector<N> table_index(jointtable.get_dimensions());

		for (size_t j = 0; j < v.to_size(); ++j)  // set all outgoing messages to 0
			*v.to_at(j).second = 0;

		for (size_t i = 0; i < size; ++i) { // sum over f(0,0,0,...) to f(1,1,1,...) in case of binary values
			for (size_t j = 0; j < v.to_size(); ++j) { // sum over all variables to be excluded themselves
				table_index = jointtable.get_tabular_index(i);
				probability<P,T> jointvalue = jointtable.get(table_index, j); // if i=(0,0,0,0), this obtains (0,0,j,0)
				// the same happens with i=(0,0,1,0), so we have to exclude that one or we count it multiple times
				if (table_index[j] != 0) continue;
				// we multiply it with the messages from the incoming variables
				for (size_t k = 0; k < v.from_size(); ++k) {
					if (j == k) continue;
					probability<P,T> incoming_msg = *v.from_at(k).second;
					N value = table_index[k];
					jointvalue *= incoming_msg[value];
				}
				// add to outgoing message
				*v.to_at(j).second += jointvalue;
			}
		}
	}

	/**
	 * Calculates an outgoing message to the vertex "v" which should be a factor node. It uses the
	 * incoming messages in the standard way: it calculates the product of all incoming messages,
	 * except for the one from the target node. It sends a uniform message (1) if there is only one
	 * outgoing edge. The uniform value is a "probability vector" with "1" at all entries.
	 *
	 * @param g				undirected graph
	 * @param v				factor node
	 */
	template <ClassImplType impl_type>
	void tick_variable(const graph<T,P,M,N,impl_type> &g, variable<T,P,M,N> & v) {
		probability<P,T> product(v.cardinality());
		probability<P,T> identity(v.cardinality(), 1);
		std::cout << "Tick variable" << std::endl;
		// leaf, send uniform distribution
		if (v.from_size() == 1) {
			*v.to_at(0).second = identity;
			std::cout << "Send (uniform) message (1) to factor node " << v.to_at(0).first; std::endl(std::cout);
			return;
		}
		// collect all messages from incoming factor nodes
		for (int i = 0; i < v.from_size(); ++i) {
			probability<P,T> msg = *v.from_at(i).second;
			product *= msg;
		}
		// send to each target node
		for (int i = 0; i < v.to_size(); ++i) {
			// correct the multiplication by division (if product != 0)
			T message = T(1); //product;
//			if (!directed) message = product ? (product / *v.from_at(i).second) : 0;
			*v.to_at(i).second = message;
			std::cout << "Send message " << message << " to factor node " << v.to_at(i).first; std::endl(std::cout);
		}
	}

private:
	bool convergence;

	bool directed;
};

}

#endif /* BELIEFPROPAGATION_HPP_ */
