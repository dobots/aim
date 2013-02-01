/**
 * @brief
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

/************************************************************************************
 * Configuration
 ************************************************************************************/

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
template <typename T, typename P = float, typename M = float, typename N = size_t>
class beliefpropagation {
public:
	beliefpropagation(): convergence(false), directed(true) {}

	/**
	 * Do one pass of the message passing scheme. This will have as side-effect that
	 * the convergence parameter is set, see @ converged().
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
		typename vertex<T,P,M,N>::const_iterator j;
		typename graph<T,P,M,N>::variable_container::const_iterator v_i;
		typename graph<T,P,M,N>::factor_container::const_iterator f_i;
		for (v_i = g.variables.begin(); v_i != g.variables.end(); ++v_i) {
			variable<T,P,M,N> &v = **v_i;
			for (j = v.from_begin(); j != v.from_end(); ++j) {
				if (!v.to_exists(j->first)) return set_directed(true);
			}
		}
		for (f_i = g.factors.begin(); f_i != g.factors.end(); ++f_i) {
			factor<T,P,M,N> &v = **f_i;
			for (j = v.from_begin(); j != v.from_end(); ++j) {
				if (!v.to_exists(j->first)) return set_directed(true);
			}
		}
		return set_directed(false);
	}

	bool set_directed(bool directed) {
		this->directed = directed;
		return directed;
	}

	/**
	 * @assumes an undirected graph
	 */
	template <ClassImplType impl_type>
	void tick_factor(const graph<T,P,M,N,impl_type> &g, factor<T,P,M,N> & v) {
		T product = T(1);

//		if (v.from_size() != 1) {
			for (int i = 0; i < v.from_size(); ++i) {
				product *= *v.from_at(i).second;
			}
//		}
		for (int i = 0; i < v.to_size(); ++i) {
			// product part of the message
			T message;
			if (!v.from_exists(i)) message = product;
			message = product ? (product / *v.from_at(i).second) : 0;

			// take all possible combinations of variable nodes that are pointing towards this factor node, except for v.to[i]
			// and perform factor function vertex.function(node values) on them
			// each time multiplied by the message from this node
			// and eventually a function defined on that variable node
			N index = v.to_at(i).first;
			variable<T,P,M,N> *t = g.get(index);
			v.marginal(*t);
//			*v.to_at(i).second = v.function(v.to_at(i).first) * message;
		}
	}

	/**
	 * Calculates an outgoing message to the vertex "v" which should be a factor node. It uses the
	 * incoming messages in the standard way: it calculates the product of all incoming messages,
	 * except for the one from the target node. It sends a uniform message (1) if there is only one
	 * outgoing edge.
	 *
	 * @param g				undirected graph
	 * @param v				factor node
	 */
	template <ClassImplType impl_type>
	void tick_variable(const graph<T,P,M,N,impl_type> &g, variable<T,P,M,N> & v) {
		T product = T(1);

		// leaf, send uniform distribution
		if (v.from_size() == 1) {
			*v.to_at(0).second = 1;
			std::cout << "Send (uniform) message (1) to factor node " << v.to_at(0).first; std::endl(std::cout);
			return;
		}
		// collect all messages from incoming factor nodes
		for (int i = 0; i < v.from_size(); ++i) {
			product *= *v.from_at(i).second;
		}
		// send to each target node
		for (int i = 0; i < v.to_size(); ++i) {
			// correct the multiplication by division (if product != 0)
			T message = product;
			if (!directed) message = product ? (product / *v.from_at(i).second) : 0;
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
