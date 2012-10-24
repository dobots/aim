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

//! A virtual infinite supply of identifiers for vertices
static long int uuid = -1;

/**
 * We assume a sparse graph, so we only need a vertex representation and no explicit
 * edge representation. They are given by the "to" and "from" field and do not carry
 * a value themselves. In the message passing scheme, the messages are passed to all
 * neighbours (except from the one the message is received from to begin with). To
 * be able to send the message in the forward as well as the backward pass, there are
 * both "to" and "from" fields, which are of course (theoretically) redundant.
 */
template <typename T>
class vertex;

/**
 * The different types of implementations that can exist for a special class. The ones in
 * operation are only CIT_CHECK which adds checks and CIT_FAST which skips checks, even if
 * they would be make operation much safer.
 */
enum ClassImplType { CIT_CHECK, CIT_FAST, CIT_GPU, CIT_SSE, CIT_NOF_TYPES } ;

/**
 * The graph implementation.
 *
 * Note that the implementation makes use of a default argument for a class template parameter
 * called "impl_type". This "impl_type" of the class allows for different instants of the
 * class that are either faster, performing more checks, or somehow optimized for hardware for
 * example. This will be resolved at compile time, so all code within "impl_type == CIT_CHECK"
 * if statements won't be in the final binary at all.
 */
template <typename T, ClassImplType impl_type = CIT_CHECK>
class graph {
public:
	// Use "delegation" as programming pattern and just expose the iterator of the underlying
	// (nested) container, see http://www.cs.northwestern.edu/~riesbeck/programming/c++/stl-iterator-define.html#TOC2
	typedef std::vector<vertex<T> *> vertex_container;
    typedef typename vertex_container::value_type                   value_type;
    typedef typename vertex_container::size_type                    size_type;
    typedef typename vertex_container::difference_type              difference_type;
    // Note that the iterator will need to be dereferenced twice: **it
    typedef typename vertex_container::reference                    reference;
    typedef typename vertex_container::const_reference              const_reference;
    typedef typename vertex_container::pointer                      pointer;
    typedef typename vertex_container::const_pointer                const_pointer;
    typedef typename vertex_container::iterator                     iterator;
    typedef typename vertex_container::const_iterator               const_iterator;
    typedef typename vertex_container::const_reverse_iterator       const_reverse_iterator;
    typedef typename vertex_container::reverse_iterator             reverse_iterator;
    typedef typename vertex_container::allocator_type               allocator_type;
    typedef vertex<T>                                               dereference;
    typedef const vertex<T>                                         const_dereference;

	graph() {
		if (impl_type == CIT_GPU) {
			std::cerr << "Class not implemented in this way!" << std::endl;
		}
	}

	~graph() {}

	/**
	 * This push version adds no vertices, but it adds an edge between two existing vertices.
	 */
	bool push(vertex<T> & src, vertex<T> & dest) {
		if (impl_type == CIT_CHECK) {
			if ((!exists(src)) || (!exists(dest))) {
				std::cerr << "Source or destination vertex does not exist!" << std::endl;
				return false;
			}
		}
		src.to.push_back(&dest);
		dest.from.push_back(&src);
		return true;
	}

	/**
	 * This push version adds a vertex which should exist yet.
	 */
	bool push(vertex<T> & v) {
		if (impl_type == CIT_CHECK) {
			if (exists(v)) {
				std::cerr << "Vertex does already exist!" << std::endl;
				return false;
			}
		}
		vertices.push_back(&v);
		return true;
	}

	bool exists(vertex<T> & v) {
		for (int i = 0; i < vertices.size(); ++i) {
			if (*vertices[i] == v) return true;
		}
		return false;
	}

	inline size_type size() const { return vertices.size(); }
	inline size_type capacity() const { return vertices.capacity(); }
	bool empty() const { return vertices.empty(); }
	size_type max_size() const { return vertices.max_size(); }

	iterator begin() { return vertices.begin(); }
	const_iterator begin() const { return vertices.begin(); }
	iterator end() { return vertices.end(); }
	const_iterator end() const { return vertices.end(); }
	reverse_iterator rbegin() { return vertices.rbegin(); }
	const_reverse_iterator rbegin() const { return vertices.rbegin(); }
	reverse_iterator rend() { return vertices.rend(); }
	const_reverse_iterator rend() const { return vertices.rend(); }

	reference operator[](size_type n) { return vertices[n]; }
	const_reference operator[](size_type n) const { return vertices[n]; }

	reference front() { return vertices.front(); }
	const_reference front() const { return vertices.front(); }
    reference back() { return vertices.back(); }
    const_reference back() const { return vertices.back(); }

    reference at(size_type n) { return vertices[n]; }
    const_reference at(size_type n) const { return vertices[n]; }

    // Internally the vertices are stored as a vector with pointers. Hence, it is convenient
    // to have some functions that return the dereferenced vertex rather than a reference to
    // the vertex. So "at" and "front" have also "d_at" and "d_front" versions.
    dereference d_at(size_type n) { return *vertices[n]; }
    const_dereference d_at(size_type n) const { return *vertices[n]; }
	dereference d_front() { return *vertices.front(); }
	const_dereference d_front() const { return *vertices.front(); }
    dereference d_back() { return *vertices.back(); }
    const_dereference d_back() const { return *vertices.back(); }

private:
	friend std::ostream& operator<<(std::ostream & os, const graph & g) {
		for (int i = 0; i < g.vertices.size(); ++i) {
			os << (*g.vertices[i]) << std::endl;
		}
		return os;
	}

	vertex_container vertices;
};

/**
 * The implementation of the vertex class.
 */
template <typename T>
class vertex {
public:
	typedef std::vector<vertex<T> *> vertex_container;
    typedef typename vertex_container::value_type                   value_type;
    typedef typename vertex_container::size_type                    size_type;
    typedef typename vertex_container::difference_type              difference_type;
    // Note that the iterator will need to be dereferenced twice: **it
    typedef typename vertex_container::reference                    reference;
    typedef typename vertex_container::const_reference              const_reference;
    typedef typename vertex_container::pointer                      pointer;
    typedef typename vertex_container::const_pointer                const_pointer;
    typedef typename vertex_container::iterator                     iterator;
    typedef typename vertex_container::const_iterator               const_iterator;
    typedef typename vertex_container::const_reverse_iterator       const_reverse_iterator;
    typedef typename vertex_container::reverse_iterator             reverse_iterator;
    typedef typename vertex_container::allocator_type               allocator_type;

	vertex(): value(0), id(++uuid) {
		to.clear();
		from.clear();
	}
	~vertex() {}

	bool operator==(const vertex<T> & other) const {
		return other.id == id;
	}

	/**
	 * Friend inline. We use the "introvert" variant of the serial operator. The vertex needs to be
	 * of the same type T as the class. We do not(!) create a templated version of the serialization
	 * operator.
	 * See: http://stackoverflow.com/questions/4660123/overloading-friend-operator-for-template-class
	 */
	friend std::ostream& operator<<(std::ostream & os, const vertex & v) {
			os << v.id << ": {";
			if (!v.from.empty()) os << v.from[0]->id;
			for (int i = 1; i < v.from.size(); ++i) os << ',' << v.from[i]->id;
			os << "} {";
			if (!v.to.empty()) os << v.to[0]->id;
			for (int i = 1; i < v.to.size(); ++i) os << ',' << v.to[i]->id;
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

	inline long int index() const { return id; }
private:
	T value;
	long int id;

	vertex_container to;
	vertex_container from;

	//! Do we really have to list as friends all possible enum values?
	friend class graph<T, CIT_CHECK>;
	friend class graph<T, CIT_FAST>;
	friend class graph<T, CIT_GPU>;
	friend class graph<T, CIT_SSE>;
};


#endif /* GRAPH_HPP_ */
