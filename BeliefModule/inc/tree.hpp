/*
 * tree.hpp
 *
 *  Created on: Feb 20, 2013
 *      Author: anne
 */

#ifndef TREE_HPP_
#define TREE_HPP_

#include <graph.hpp>

template <typename T, typename P, typename M, typename N, ClassImplType I>
class tree;


/**
 * A tree is just a graph without some edges. There are however different edges to remove. Our goal is
 * message passing, so the way the edges are removed is quite specific. It is an algorithm called the
 * "junction tree algorithm" and it automatically is applied when a graph is assigned to the tree
 * object.
 */
template <typename T, typename P = float, typename M = probability<P,T>, typename N = size_t, ClassImplType impl_type = CIT_CHECK>
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
		//		typename vertex<T,P,M,N>::const_iterator j;
		typename graph<T,P,M,N>::variable_container::const_iterator v_i;
		//		typename graph<T,P,M,N>::variable_container::iterator v_j;
		typename graph<T,P,M,N>::factor_container::const_iterator f_i;

		for (v_i = super::variables.begin(); v_i != super::variables.end(); ++v_i) {
			variable<T,P,M,N> &v = **v_i;
			for (int j = 0; j < v.from_size(); ++j) {
				if (!v.to_exists(v.from_at(j).first)) v.push_to(v.from_at(j).first);
			}
			for (int j = 0; j < v.to_size(); ++j) {
				if (!v.from_exists(v.to_at(j).first)) v.push_from(v.to_at(j).first);
			}
		}

		for (f_i = super::factors.begin(); f_i != super::factors.end(); ++f_i) {
			factor<T,P,M,N> &v = **f_i;
			for (int j = 0; j < v.from_size(); ++j) {
				if (!v.to_exists(v.from_at(j).first)) v.push_to(v.from_at(j).first);
			}
			for (int j = 0; j < v.to_size(); ++j) {
				if (!v.from_exists(v.to_at(j).first)) v.push_from(v.to_at(j).first);
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


#endif /* TREE_HPP_ */
