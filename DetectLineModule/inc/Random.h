/**
 * @brief Random.h
 * @file Random.h
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
 * @date    May 7, 2013
 * @project Replicator FP7
 * @company Almende B.V.
 * @case    Artificial Intelligence Framework
 */

#ifndef RANDOM_H_
#define RANDOM_H_

//! Bring out the concept checks that are normally only defined in the libc library itself
#define _GLIBCXX_CONCEPT_CHECKS

// General files
#include <cassert>
#include <algorithm>
#include <functional>
#include <numeric>
#include <iostream>
#include <random>
#include <cmath>
#include <iterator>

/**
 * @brief Pick a random number. Using modules (%) will produce biased results.
 *
 * This implementation uses the uniform distribution from the standard library.
 *   http://stackoverflow.com/questions/6942273/get-random-element-from-container-c-stl
 */
template<typename Iter, typename RandomGenerator>
Iter random_element(Iter start, Iter end, RandomGenerator& g) {
    std::uniform_int_distribution<> dis(0, std::distance(start, end) - 1);
    std::advance(start, dis(g));
    return start;
}

template<typename Iter>
Iter random_element(Iter start, Iter end) {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    return random_element(start, end, gen);
}

/**
 * @brief Randomly pick a number of elements from an array using the algorithm of Robert Floyd.
 *
 * In case you will need to pick a few unique elements from a list you can use several algorithms. The Fisher-Yates
 * shuffle generates a random permutation of a set for example, although its original implementation is not in-place.
 * The method of Floyd is convenient when M << N, the number of elements to pick is much smaller than the total number
 * of elements in the container.
 *
 * @param first			First element of input container
 * @param last			Last element of input container
 * @param result		Start of output container, needs to have allocated size equal to number
 * @param number		Number of random elements that have to be returned
 *
 * @result				Output iterator, so it can be used in subsequent operations
 */
template<typename InputIterator, typename OutputIterator>
OutputIterator random_n(InputIterator first, InputIterator last, OutputIterator result, size_t number) {

	typedef typename std::iterator_traits<InputIterator>::value_type ValueType;

	__glibcxx_function_requires(_InputIteratorConcept<InputIterator>);
	__glibcxx_function_requires(_OutputIteratorConcept<OutputIterator, ValueType>);
	__glibcxx_requires_valid_range(first1, last1);

	if (first == last) return result;
	assert (number <= (last - first));

	// store the index to the element, not the value itself, so we do not need a "==" value operator for std::find
	std::vector<InputIterator> temp;
	// start at last-number (first increment last-number+1 happens in the while loop)
	InputIterator j = last - number;

	while (++j != last + 1) {
		InputIterator rand = random_element(first,j);
		if (std::find(temp.begin(), temp.end(), rand) != temp.end()) {
			// the index to the random value is already in the output set
			temp.push_back(j);
		} else {
			temp.push_back(rand);
		}
	}

	std::cerr << "Generated result vector of size " << temp.size() << std::endl;

	for (size_t i = 0; i < temp.size(); ++i) {
		*++result = *(temp[i]);
	}
	return ++result;
}

#endif /* RANDOM_H_ */
