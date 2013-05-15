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
#include <sys/stat.h>
#include <fcntl.h>
#include <cstdio>
#include "gtest/gtest.h"

namespace {

static int fd_stdout, fd_stderr;

TEST(FileTest, Redirect) {
	fd_stdout = open("../stdout.log", O_WRONLY|O_CREAT|O_TRUNC, 0660);
	fd_stderr = open("../stderr.log", O_WRONLY|O_CREAT|O_TRUNC, 0660);
	ASSERT_GE(fd_stdout, 0);
	int retstdout = dup2(fd_stdout, 2);
	ASSERT_GE(retstdout, 0);

	ASSERT_GE(fd_stderr, 0);
	int retstderr = dup2(fd_stderr, 1);
	ASSERT_GE(retstderr, 0);

	std::cout << "Redirected std::cout to this file" << std::endl;
	std::cerr << "Redirected std::cerr to this file" << std::endl;
}

/**
 * Check the random element.
 */
TEST(RandomTest, NumberOfResults) {
	int nr_cnt = 3;

	std::vector<int> numbers;
	std::vector<int>::iterator iter;
	for (int i = 1; i < nr_cnt+1; ++i) {
		numbers.push_back(i);
	}

	// use sum as check
	long int sumA = 0;
	for (iter = numbers.begin(); iter < numbers.end(); ++iter) {
		sumA += *iter;
	}

	std::vector<int> random;
	random.resize(nr_cnt, 0);
	random_n(numbers.begin(), numbers.end(), random.begin(), nr_cnt);

	// use sum as check
	long int sum = 0;
	for (iter = random.begin(); iter < random.end(); ++iter) {
		sum += *iter;
	}

	// check if total sum equals the expectation
	ASSERT_EQ(sumA, sum);
}

TEST(CloseTest, Close) {
	close(fd_stdout);
	close(fd_stderr);
}

}

int main(int argc, char **argv) {
	//std::cout << "Run random_n algorithm" << std::endl;
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
