/**
 * @file nd-array.hpp
 * @brief N-dimensional array implementation
 *
 * This file is created at Almende B.V. It is open-source software and part of the Common
 * Hybrid Agent Platform (CHAP). A toolbox with a lot of open-source tools, ranging from
 * thread pools and TCP/IP components to control architectures and learning algorithms.
 * This software is published under the GNU Lesser General Public license (LGPL).
 *
 * It is not possible to add usage restrictions to an open-source license. Nevertheless,
 * we personally strongly object to this software being used by the military, in factory
 * farming, for animal experimentation, or anything that violates the Universal
 * Declaration of Human Rights.
 *
 * Copyright © 2013 Anne van Rossum <anne@almende.com>
 *
 * @author  Anne C. van Rossum
 * @date    May 06, 2013
 * @project Replicator FP7
 * @company Almende B.V.
 * @case    Artificial Intelligence Framework
 */
#ifndef ND_ARRAY_H_
#define ND_ARRAY_H_

#include <vector>
#include <sstream>
#include <cassert>

/**
 * An nd-array is an N-dimensional array. The dimension of the table is on purpose not a template parameter. This would
 * namely make it quite hard to create classes that would make use of nd_array's with a diverse set of dimensions. For
 * each of the dimensions, a new class would need to be generated. There are however, two template parameters
 * introduced, to limit the memory requirements as much as possible:
 *
 * @param V		type to indicate values, should be floating point, e.g. float or double, default: float
 * @param T		type of the size of the entire array (so not the size itself), default: size_t
 *
 * For a 3x3x3 array, you will only need 27 values, a char for T will be enough. For a 5-dimensional 8*8*8*8*8 array,
 * you can use a short, etc.
 */
template <typename V = float, typename T = size_t>
class nd_array {
public:
	// The linear index has to be large to store entire table, by default size_t
	typedef T linear_index;
	// The tabular index is mapped into a linear index, the size of the individual dimensions can be smaller than that
	// of the entire table.
	typedef std::vector<T> tabular_index;

	// The value type and its container (a vector)
	typedef V value_type;
	typedef std::vector<value_type> value_container;

	// The dimension type (allow for max "sizeof(T)" dimensions, and its container (a vector)
	typedef T dimension_type;
	typedef std::vector<dimension_type> dimension_container;

	/**
	 * The default N-dimensional array constructor. It requires a series of values that denote the size of the rows,
	 * columns, and whatever you call these in an N-dimensional structure. A parameter dimension=[4,3,2] creates a
	 * 3D dimensional grid with 4 cells wide, 3 cells high, and 2 cells deep. a dimension is not allowed to be 0.
	 */
	nd_array(dimension_container & dimensions) {
		this->dimensions = dimensions;
		linear_index size, new_size = 1;
		strides.clear();
		values.clear();
		values.reserve(size);
		for (dimension_type i = 0; i < dimensions.size(); ++i) {
			strides.push_back(size);
			assert(dimensions[i] != 0);
			new_size = size * dimensions[i];
			// Check if we did go beyond the maximum size of the overall container, by using the reverse operator: "/"
			assert((new_size / dimensions[i]) == size);
			size = new_size;
		}
		strides.push_back(size);
		for (linear_index i = 0; i < size; ++i) values.push_back(value_type(0));
	}

	/**
	 * Copy constructor for an N-dimensional array.
	 */
	nd_array(const nd_array & other) {
		dimensions = other.dimensions;
		values = other.values;
		strides = other.strides;
	}

	/**
	 * Virtual constructor for subclass extensions. Nothing is deallocated. If you use a pointer type for the values,
	 * you are responsible for deallocation.
	 */
	virtual ~nd_array() {
		strides.clear();
		values.clear();
		dimensions.clear();
	}

	/**
	 * Translate [i,j,k] to linear index i+j*i_size+k*i_size*j_size, etc. Actually, it is better to store the "stride"
	 * per variable. Now, we are calculating it every time we use get_linear_index() by the multiplication of "dim".
	 */
	inline linear_index get_linear_index(tabular_index index) {
		linear_index i; linear_index result = 0;
		for (linear_index i = 0; i < index.size(); ++i) {
			result += index[i] * strides[i];
		}
		return result;
	}

	/**
	 * Translate tabular index into a linear index.
	 */
	inline tabular_index get_tabular_index(linear_index index) {
		assert (index < size());
		tabular_index result(dimensions.size(), dimension_type(0));
		for (linear_index i = 0; i < dimensions.size(); ++i) {
			result[i] = (index / strides[i]) % dimensions[i];
		}
		return result;
	}

	/*
	 * Set the value at for example index[2,0,1] in a dimensional object of for example size[3,3,2]. Internally this is
	 * cast into a linear index upon which the value at that index position is set.
	 */
	void set(tabular_index table_index, value_type value) {
		values[get_linear_index(table_index)] = value;
	}

	//! Setf forces the value at the given linear index. Provided because of speed. Nothing is checked.
	inline void setf(linear_index index, value_type value) {
		values[index] = value;
	}

	//! Convenience function, can only be used if the nd-array is actually 1-dimensional
	void set(linear_index index0, value_type value) {
		assert (dimensions.size() == 1);
		assert (index0 < dimensions[0]);
		linear_index i = index0;
		values[i] = value;
	}

	//! Convenience function, can only be used if the nd-array is 2-dimensional
	void set(linear_index index0, linear_index index1, value_type value) {
		assert (dimensions.size() == 2);
		assert (index0 < dimensions[0]);
		assert (index1 < dimensions[1]);
		linear_index i = index0 + index1 * dimensions[0];
		values[i] = value;
	}

	//! Convenience function, can only be used if the nd-array is 3-dimensional
	void set(linear_index index0, linear_index index1, linear_index index2, value_type value) {
		assert (dimensions.size() == 3);
		assert (index0 < dimensions[0]);
		assert (index1 < dimensions[1]);
		assert (index2 < dimensions[2]);
		linear_index i = index0 + index1 * dimensions[0] + index2 * dimensions[0] * dimensions[1];
		values[i] = value;
	}

	//! Gets a value through the tabular index (use the linear index if available, because of speed)
	value_type const get(tabular_index index) {
		return values[get_linear_index(index)];
	}

	//! Getf gets the value at the given linear index. Provided because of speed. Nothing is checked.
	inline value_type const getf(linear_index index) {
		return values[index];
	}

	//! Convenience function, can only be used if the nd-array is actually 1-dimensional
	value_type const get(linear_index index0) {
		assert (dimensions.size() == 1);
		assert (index0 < dimensions[0]);
		linear_index i = index0;
		return values[i];
	}

	//! Convenience function, can only be used if the nd-array is 2-dimensional
	value_type const get(linear_index index0, linear_index index1) {
		assert (dimensions.size() == 2);
		assert (index0 < dimensions[0]);
		assert (index1 < dimensions[1]);
		linear_index i = index0 + index1 * dimensions[0];
		return values[i];
	}

	//! Convenience function, can only be used if the nd-array is 3-dimensional
	value_type const get(linear_index index0, linear_index index1, linear_index index2) {
		assert (dimensions.size() == 3);
		assert (index0 < dimensions[0]);
		assert (index1 < dimensions[1]);
		assert (index2 < dimensions[2]);
		linear_index i = index0 + index1 * dimensions[0] + index2 * dimensions[0] * dimensions[1];
		return values[i];
	}

	/**
	 * This is one of the more complicated functions. The function returns the values that would correspond to a
	 * "column" in a 2D table, or an "array" in a 3D matrix. It is best explained by example. Consider the 2D matrix:
	 *    0.99  0.01
	 *    0.9   0.1
	 *    0.2   0.8
	 * This should return for index=[0,*] and summarize=1, the following sequence of values: [0.99 0.01]. So, the
	 * second value in the index doesn't matter, because that is the dimension summarized over. For the parameters
	 * index=[1,*] and summarize=1, the following sequence will be returned: [0.9, 0.1]. In other words, in the 2D
	 * case, summarize=1 means that the "rows" are returned, and the first parameter in the tabular index indicates
	 * which row. Likewise, we can return columns. For index=(*,1) and summarize=0, it returns [0.01, 0.1, 0.8], the
	 * second column.
	 *
	 * @param summarize		Index of variable to summarize over
	 * @return 				A column
	 */
	value_container * get(const tabular_index & index, dimension_type summarize) {
		assert (summarize < dimensions.size());
		tabular_index copy_of_index = index;
		dimension_type dim = dimensions[summarize];
		assert (dim > 0);
		value_container *result = NULL;
		result = new value_container(dim);
		for (dimension_type i = 0; i < dim; ++i) {
			copy_of_index[summarize] = i; // at "summarize" dimension, set this index to "i", keep the rest the same
			(*result)[i] = values[get_linear_index(copy_of_index)];
			copy_of_index[summarize] = index[summarize]; // restore copy_of_index
		}
		return result;
	}

	/**
	 * Returns the maximum size in the sense of memory requirements for this nd-array.
	 */
	linear_index size() const {
		linear_index max = 1;
		for (dimension_type i = 0; i < dimensions.size(); ++i) {
			max *= dimensions[i];
		}
		return max;
	}

	//! Get the number of dimensions.
	std::vector::size_type get_dimensions() const {
		return dimensions.size();
	}

private:
	//! The dimensions of the table (e.g. 2x3x2). This is represented by a a vector
	dimension_container dimensions;

	//! The values in the table (linearized as a vector)
	value_container values;

	//! The strides has to have the same form as the dimensions data structure.
	dimension_container strides;

	/**
	 * Printing the table is useful for debugging purposes. The table does not have access to the
	 * variables or factors involved, it is just a table. So, there is no way to equip it with variable
	 * names for example.
	 */
	friend std::ostream& operator<<(std::ostream & os, const nd_array & table) {
		dimension_type dimsize = table.dimensions.size();

		// print header
		if (dimsize == 0) {
			os << "Table: [empty]"; return os;
		}
		os << "Table: [";
		os << table.dimensions[0];
		for (dimension_type i = 1; i < dimsize; ++i) {
			os << ',' << table.dimensions[i];
		}
		os << ']' << " (will be shown with most significant / latest variable first)" << '\n';

		// setup arrays for dimensions and counters
		dimension_type reset[dimsize+1];
		dimension_type dimcnt[dimsize];
		for (dimension_type i = dimsize-1; i >= 0; --i) {
			dimcnt[i] = 0; reset[i] = 1;
			for (dimension_type j = 0; j <= i; ++j) {
				reset[i+1] *= table.dimensions[j];
			}
		}

		// output matrix
		for (dimension_type i = 0; i < table.size(); ++i) {
			for (dimension_type d = dimsize-1; d >= 0; --d) {
				os << ' ' << dimcnt[d];
				if (!((i+1) % reset[d])) ++dimcnt[d];
				if (!((i+1) % reset[d+1])) dimcnt[d] = 0;
			}
			os << " : " << (table.values[i]) << '\n';
		}
		return os;
	}
};

#endif // ND_ARRAY_H_
