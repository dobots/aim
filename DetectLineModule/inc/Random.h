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
    if (start == end) return start;
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
 * @brief Randomly pick a number of elements from an array using the algorithm of Robert Floyd. This does not mean that
 * the order in which these elements are returned is random. Just the combination is picked at random. As extreme case,
 * if you pick 10 numbers out of an array with 10 numbers, it might return these numbers in the same order every time.
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
	if (number == 0) return result;
	assert (number <= (last - first));

	// store distance to element, not value itself, so we do not need a "==" value operator for std::find
	std::vector<size_t> distance;
	InputIterator j = last - number + 1;

	// in the case of number=1, j will need to be the last element
	while (j <= last) {
		InputIterator rand_index = random_element(first,j);
		size_t rand = std::distance(first, rand_index);
		if (std::find(distance.begin(), distance.end(), rand) != distance.end()) {
			size_t d = std::distance(first,j) - 1;
			distance.push_back(d);
		} else {
			distance.push_back(rand);
		}
		++j;
	}

	// fill return container
	for (size_t i = 0; i < distance.size(); ++i) {
		*result = *(first+distance[i]);
		++result;
	}
	return ++result;
}

#endif /* RANDOM_H_ */
