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

template <typename T>
class graph2matrix {
public:
	typedef Eigen::SparseMatrix<T> matrix;

	graph2matrix() {}

	~graph2matrix() {}

	template <ClassImplType impl_type>
	matrix* copy(const graph<T,impl_type> &g) {
		matrix *m = new matrix(g.size(), g.size());

		typename graph<T>::const_iterator i;
		typename vertex<T>::iterator j;

		typedef Eigen::Triplet<double> triplet;
		std::vector<triplet> triplets;
		triplets.reserve(g.size());

		for (i = g.begin(); i != g.end(); ++i) {
			vertex<T> *v_src = *i;

			for (j = v_src->to_begin(); j != v_src->to_end(); ++j) {
				vertex<T> *v_dest = *j;
				triplets.push_back(triplet(v_src->index(), v_dest->index(), 1));
			}
		}
		m->setFromTriplets(triplets.begin(), triplets.end());
		return m;
	}
private:
};



#endif /* GRAPH2MATRIX_HPP_ */

#endif
