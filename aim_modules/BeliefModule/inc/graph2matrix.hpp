/**
 * @brief
 * @file graph2matrix.hpp
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

#include <config.h>

#ifdef BUILD_EIGEN

/************************************************************************************
 * Implementation
 ************************************************************************************/

#ifndef GRAPH2MATRIX_HPP_
#define GRAPH2MATRIX_HPP_

#include <Eigen/SparseCore>
#include <graph.hpp>

namespace dobots {

/**
 * A class that generates a sparse matrix from a graph. The resulting matrix is an
 * adjacency matrix, so will just have "1s" at the locations where there is an edge.
 * There are no actual edge labels or values.
 */
template <typename T, typename P = float, typename M = float, typename N = size_t>
class graph2matrix {
public:
	typedef Eigen::SparseMatrix<T> matrix;

	graph2matrix() {}

	~graph2matrix() {}

	/**
	 * Copy the graph to a sparse matrix from the Eigen library. The sparse matrix is a
	 * very specific structure, so inserting an element is of large order O(nmm), where
	 * nmm stands for the number of non-zero elements already in the matrix. Hence, the
	 * developers of Eigen recommend an intermediate structure, the triplet, to populate
	 * the matrix at once.
	 */
	template <ClassImplType impl_type>
	matrix* copy(const graph<T,P,M,N,impl_type> &g) {
		matrix *m = new matrix(g.size(), g.size());

		typename graph<T,P,M,N>::variable_container::const_iterator v_i;
		typename graph<T,P,M,N>::factor_container::const_iterator f_i;
		typename vertex<T,P,M,N>::const_iterator j;

		typedef Eigen::Triplet<P> triplet;
		std::vector<triplet> triplets;
		triplets.reserve(g.size()*3); // assume three edges per vertex on average

		for (v_i = g.variables.begin(); v_i != g.variables.end(); ++v_i) {
			const variable<T,P,M,N> &v_src = **v_i;
			for (j = v_src.to_begin(); j != v_src.to_end(); ++j) {
				long int dest_index = j->first;
				triplets.push_back(triplet(v_src.index(), dest_index, 1));
			}
		}
		for (f_i = g.factors.begin(); f_i != g.factors.end(); ++f_i) {
			const factor<T,P,M,N> &v_src = **f_i;
			for (j = v_src.to_begin(); j != v_src.to_end(); ++j) {
				long int dest_index = j->first;
				triplets.push_back(triplet(v_src.index(), dest_index, 1));
			}
		}
		m->setFromTriplets(triplets.begin(), triplets.end());
		return m;
	}
private:
};

}

#endif /* GRAPH2MATRIX_HPP_ */

#endif // BUILD_EIGEN
