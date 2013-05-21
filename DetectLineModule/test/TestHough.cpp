/**
 * @brief Test the Hesse normal form
 * @file TestHesseNormal.cpp
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

#include <Hough.h>
#include <CRawImage.h>

namespace {

class HoughTest : public ::testing::Test {
  protected:

    HoughTest() {
        // Class setup
    }

    virtual ~HoughTest() { }

    virtual void SetUp() {
//    	Point2D p0(2,1);
//    	Point2D p1(1,2);
//    	hough.addPoint(p0);
//    	hough.addPoint(p1);
    }

    virtual void TearDown() {
    }

    dobots::Hough<Point2D> hough;
};

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
 * From here we can test the hough transform. Let us first test the angles, and then the distances.
 */
TEST_F(HoughTest, HesseNormal) {

	std::cout << "Test line y=-x+30" << std::endl;
	Point2D p0(20,10);
	Point2D p1(10,20);

	double slope;
	double y_intersection;
	hough.getLineDescription<double>(p0, p1, slope, y_intersection);
	EXPECT_EQ(-1, slope);
	EXPECT_EQ(30, y_intersection);


	ACoordinates c = hough.transform(p0, p1);
	int SHIFT=100/2;
	int expect = 12+SHIFT;
	EXPECT_EQ(expect, c.y); // with default size of 100: 12 + shifted with 100/2


//	int x = std::sqrt(15*15+15*15) * 1000/800;
//	int y = 1000/8;

	// p.x should be sqrt(15*15+15*15), scaled with 100 over 800 (by default 640x480 gives maximum distance of 800),
	// and default accumulator size is 100, this would lead to 21*1/8=2.56, floored, becomes 2
	// p.y should be M_PI/4 (which is equal to atan(1), however it is scaled with 100/(2*PI), so it should be 12.5 and
	// with truncation downwards, equal to 12

//	EXPECT_EQ(c.x, 2); // with default size of 100: 2
	//EXPECT_EQ(12, c.y); // with default size of 100: 12

	std::cout << "Test line y=-x+30 with points reversed (should be same outcome)" << std::endl;
	p0.set(10,20);
	p1.set(20,10);
	c = hough.transform(p0, p1);
	EXPECT_EQ(expect, c.y);

	// with large accumulator of 1000x1000
//	EXPECT_EQ(c.x, 26);
//	EXPECT_EQ(c.y, 125);

	std::cout << "Test diagonal line through the origin (radial) y=1*x+0" << std::endl;
	p0.set(10,10);
	p1.set(20,20);
	c = hough.transform(p0, p1);
	expect = 37+SHIFT;
	EXPECT_EQ(expect, c.y);

	std::cout << "Test horizontal line y=0*x-10" << std::endl;
	p0.set(-10,-10);
	p1.set(10,-10);
	c = hough.transform(p0, p1);
	expect = 0+SHIFT;
	EXPECT_EQ(expect, c.y);

	std::cout << "Line y=-1.5*x-20" << std::endl;
	p0.set(0,30);
	p1.set(20,0);
	c = hough.transform(p0, p1);
	expect = 9+SHIFT;
	EXPECT_EQ(expect, c.y);


	// same line, but mirrored
//	Point2D p2(20,50);
//	Point2D p3(10,40);
//	c = hough.transform(p2, p3);
	// with default size of 100
//	EXPECT_EQ(c.x, 2);

//	EXPECT_EQ(c.y, 36);
}

TEST(ImageTest, MakeSquare) {
	CRawImage image(640,480,1);
	int x = 150, y = 150, size = 100;
	int s = size >> 1;
	image.plotLine(x-s,y-s,x-s,y+s);
	image.plotLine(x-s,y+s,x+s,y+s);
	image.plotLine(x+s,y+s,x+s,y-s);
	image.plotLine(x+s,y-s,x-s,y-s);

	// another square, totally in the corner
	x = (640-50) - s; y = (480-50) - s;
	image.plotLine(x-s,y-s,x-s,y+s);
	image.plotLine(x-s,y+s,x+s,y+s);
	image.plotLine(x+s,y+s,x+s,y-s);
	image.plotLine(x+s,y-s,x-s,y-s);

	image.saveBmp("../square.bmp");
	std::cout << "Save file square.bmp" << std::endl;
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
