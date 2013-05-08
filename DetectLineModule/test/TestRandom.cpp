/**
 * @brief TestRandom.cpp
 * @file TestRandom.cpp
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
 * @date    May 8, 2013
 * @project Replicator FP7
 * @company Almende B.V.
 * @case    Artificial Intelligence Framework
 */

#include <Random.h>
#include <iostream>

#include "gtest/gtest.h"

/*
class ExampleTest : public ::testing::Test {
  protected:

    ExampleTest()
            :example_("world") {
        // Class setup
    }

    virtual ~ExampleTest() {
        // Class teardown
    }

    virtual void SetUp() {
        // Setup before method
    }

    virtual void TearDown() {
        // Teardown before method
    }

    Example example_;
}; */

/**
 * Check the random element.
 */
TEST(RandomTest, NumberOfResults) {
	std::vector<int> numbers;
	for (int i = 0; i < 2; ++i) numbers.push_back(i);
	std::vector<int> random;
	random_n(numbers.begin(), numbers.end(), random.begin(), 2);
//	for (int i = 0; i < 2; ++i)
//		std::cout << random[i] << ' ';
//	std::cout << std::endl;
	ASSERT_EQ(random.size(), 2);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
