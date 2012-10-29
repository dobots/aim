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

//! A virtual infinite supply of identifiers for vertices
static long int uuid = -1;


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
template <typename T>
class vertex;

template <typename T, ClassImplType I>
class tree;

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
		if (impl_type > CIT_FAST) {
			std::cerr << "Class not (yet) implemented in this way!" << std::endl;
		}
	}

	~graph() {
		erase();
	}

	/**
	 * Copy all vertices individually or else way only the pointer is copied. This is also
	 * used by subclasses (such as tree).
	 *
	 */
	graph<T,impl_type> & operator=(const graph<T, impl_type> &g) {
		for (int i = 0; i < g.vertices.size(); ++i) {
			vertex<T> *v = new vertex<T>(*g.vertices[i]);
			vertices.push_back(v);
		}
		return *this;
	}

	/**
	 * This push version adds no vertices, but it adds an edge between two existing vertices.
	 */
	bool push(vertex<T> * src, vertex<T> * dest) {
		if (impl_type == CIT_CHECK) {
			if ((!exists(src)) || (!exists(dest))) {
				std::cerr << "Source or destination vertex does not exist!" << std::endl;
				return false;
			}
			if (src->vtype == dest->vtype) {
				std::cerr << "Vertices in a bipartite graph should only be connected to different types" << std::endl;
				return false;
			}
		}
		src->to.push_back(std::make_pair(dest->id,new T(0)));
		dest->from.push_back(std::make_pair(src->id,new T(0)));
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

	bool exists(const vertex<T> & v) {
		for (int i = 0; i < vertices.size(); ++i) {
			if (vertices[i] == &v) return true;
		}
		return false;
	}

	bool exists(vertex<T> * v) {
		for (int i = 0; i < vertices.size(); ++i) {
			if (vertices[i] == v) return true;
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

	reference operator[](size_type n) {
		if (impl_type == CIT_CHECK) {
			assert (vertices[n]->id == n);
		}
		return vertices[n];
	}
	const_reference operator[](size_type n) const {
		if (impl_type == CIT_CHECK) {
			assert (vertices[n]->id == n);
		}
		return vertices[n];
	}

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

	void erase() {
		for (int i = 0; i < vertices.size(); ++i) {
			delete vertices[i];
		}
	}

	template<class U, ClassImplType I>
	friend class tree;
protected:
	vertex_container & get_vertices() { return &vertices; }
private:
	friend std::ostream& operator<<(std::ostream & os, const graph & g) {
		std::cout << "Graph {src} {dest}: " << std::endl;
		for (int i = 0; i < g.vertices.size(); ++i) {
			os << (*g.vertices[i]); std::endl(os);
		}
		return os;
	}

	vertex_container vertices;
};

/**
 * A tree is just a graph without some edges. There are however different edges to remove. Our goal is
 * message passing, so the way the edges are removed is quite specific. It is an algorithm called the
 * "junction tree algorithm" and it automatically is applied when a graph is assigned to the tree
 * object.
 */
template <typename T, ClassImplType impl_type = CIT_CHECK>
class tree: public graph<T,impl_type> {
public:
	tree(): graph<T,impl_type>() {}
	~tree() {};

	// To assign a graph to a tree requires pruning all edges which makes it not
	// a tree. We perform here the junction tree algorithm.
	tree(const graph<T, impl_type> &g) {
		typedef graph<T,impl_type> super;
		super::operator=(g);
		moralization();
		triangulation();
	}

	tree<T,impl_type> & operator=(const graph<T, impl_type> &g) {
		typedef graph<T,impl_type> super;
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
		typedef graph<T,impl_type> super;
		typename graph<T>::iterator i;
		typename vertex<T>::iterator j;
		for (i = super::begin(); i != super::end(); ++i) {
			vertex<T> &v_src = **i;
			for (j = v_src.to_begin(); j != v_src.to_end(); ++j) {
				if (find(v_src.from_begin(), v_src.from_end(), *j) == v_src.from_end()) {
					// copy entire "pair", so reference to object should be copied, hence
					// the message on "from" will be the same as the message on "to" automatically
					v_src.from.push_back(*j);
				}
			}
			for (j = v_src.from_begin(); j != v_src.from_end(); ++j) {
				if (find(v_src.to_begin(), v_src.to_end(), *j) == v_src.to_end()) {
					v_src.to.push_back(*j);
				}
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
	vertex<T> & get_root() {
		typedef graph<T,impl_type> super;
		return super::front();
	}
};

/**
 * The implementation of the vertex class.
 */
template <typename T> //, VertexType vtype = VT_VARIABLE>
class vertex {
public:
	//! Note that for the neighbours of vertices we store only references plus a message of type T
	typedef std::vector<std::pair <long int, T* > > vertex_index_container;
	typedef typename vertex_index_container::value_type                   value_type;
	typedef typename vertex_index_container::size_type                    size_type;
	typedef typename vertex_index_container::difference_type              difference_type;
	// Note that the iterator will need to be dereferenced twice: **it
	typedef typename vertex_index_container::reference                    reference;
	typedef typename vertex_index_container::const_reference              const_reference;
	typedef typename vertex_index_container::pointer                      pointer;
	typedef typename vertex_index_container::const_pointer                const_pointer;
	typedef typename vertex_index_container::iterator                     iterator;
	typedef typename vertex_index_container::const_iterator               const_iterator;
	typedef typename vertex_index_container::const_reverse_iterator       const_reverse_iterator;
	typedef typename vertex_index_container::reverse_iterator             reverse_iterator;
	typedef typename vertex_index_container::allocator_type               allocator_type;

	vertex(VertexType type): value(0), id(++uuid), vtype(type) {
		if (vtype == VT_VARIABLE) value = 1;
		to.clear();
		from.clear();
	}
	virtual ~vertex() {}

	vertex(const vertex<T> &other) {
		std::cerr << "Debug: copy construction on vertex " << other.id; std::endl(std::cerr);
		value = other.value;
		id = other.id;
		vtype = other.vtype;
		to = other.to;
		from = other.from;
	}

	bool operator==(const vertex<T> & other) const {
		return other.id == id;
	}

	// TODO: use the copy-and-swap idiom
	vertex<T> & operator=(const vertex<T> &other) {
		std::cerr << "Debug: copy action on vertex " << other.id; std::endl(std::cerr);
		value = other.value;
		id = other.id;
		vtype = other.vtype;
		to = other.to;
		from = other.from;
		return *this;
	}

	/**
	 * For factor nodes overload this function. It should for example mark the probability
	 * of a label variable given a pixel value variable.
	 */
	virtual T function() { return 0; }

	inline long int index() const { return id; }
	inline VertexType getType() const  { return vtype; }
	inline void setValue(T value) { this->value = value; }
	inline T getValue() const { return value; }

	/**
	 * Friend inline. We use the "introvert" variant of the serial operator. The vertex needs to be
	 * of the same type T as the class. We do not(!) create a templated version of the serialization
	 * operator.
	 * See: http://stackoverflow.com/questions/4660123/overloading-friend-operator-for-template-class
	 */
	friend std::ostream& operator<<(std::ostream & os, const vertex & v) {
		os << v.id << "[" << v.value << "](" << v.vtype << "): {";
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
private:
	// Value on the node, can be used to accumulate incoming messages
	T value;
	// Identifier of the node, increases on construction
	long int id;
	// Indicates if vertex is a factor node, if not, it's a variable node, in a
	// bipartite graph they are only allowed to be connected with the other node type
	VertexType vtype;

	vertex_index_container to;
	vertex_index_container from;

	// Just befriend all graph instantiations
	// http://stackoverflow.com/questions/13055447/c-how-to-specify-all-friends-of-a-templated-class-with-a-default-argument
	template<class U, ClassImplType I>
	friend class graph;

	template<class U, ClassImplType I>
	friend class tree;
};


#endif /* GRAPH_HPP_ */
