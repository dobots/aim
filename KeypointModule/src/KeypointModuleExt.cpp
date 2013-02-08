#include <KeypointModuleExt.h>

#include <opencv2/core/core.hpp>
#include <opencv2/calib3d/calib3d.hpp>

#include <sstream>
#include <assert.h>
#include <CRawImage.h>
#include <iostream>

using namespace rur;
using namespace cv;

static int lifetime = 1;

#define DRAW_RESULTS

KeypointModuleExt::KeypointModuleExt() {
}

void KeypointModuleExt::Prepare() {
	fd = FeatureDetector::create("ORB");
	de = DescriptorExtractor::create("ORB");
	dm = DescriptorMatcher::create("BruteForce-Hamming");
}

/**
 * Get keypoints and descriptors from one image. If the image is larger than
 * 640x480 we will only take a region of interest that size.
 */
void KeypointModuleExt::GetKeypoints(Mat & image, std::vector<KeyPoint> & keypoints, Mat & descriptors) {
	Mat roi(image.size(), CV_8UC1, Scalar(0));
	Point poly[] = {Point(0, 0), Point(0, 640), Point(480, 640), Point(480, 0)};
	fillConvexPoly(roi, poly, int(sizeof(poly) / sizeof(poly[0])), Scalar(255));

	printf("Detect keypoints and compute descriptors\n");
	fd->detect(image, keypoints, roi);
	de->compute(image, keypoints, descriptors);

	for(std::vector<KeyPoint>::const_iterator kp = keypoints.begin(); kp != keypoints.end(); ++kp)
	{
		int x = cvRound(kp->pt.x);
		int y = cvRound(kp->pt.y);
		assert(x < 0 ? false : x < image.cols);
		assert(y < 0 ? false : y < image.rows);
		circle(image, kp->pt, 5, Scalar(255));
	}
	printf("Found %i keypoints\n", (int)keypoints.size());
}

/**
 * Helper struct to store things that have to do with features and descriptors.
 * It uses a RawImage class to load data into an OpenCV image, so we do not need
 * the highgui library of OpenCV.
 */
struct Img {
	Img(int width, int height) {
		image = new Mat(height,width,CV_8UC1);
		rawimg = new CRawImage(width,height,3);
	}
	~Img() {
		delete image;
		delete rawimg;
	}
	void Load(std::string filename) {
		string path = "../../data/";
		string file = string(path + filename);
		assert (rawimg->loadBmp(file.c_str()));
		rawimg->makeMonochrome();
		image->data = rawimg->data;
		assert(!image->empty());
	}
	std::vector<KeyPoint> keypoints;
	Mat *image;
	Mat descriptors;
	CRawImage *rawimg;
};

/**
 * Takes two subsequent images, finds features, and calculates descriptors at these places.
 * Then an exhaustive search across the descriptors by k-nearest neighbour, finds in the
 * end descriptors that match and are not too far apart.
 */
void KeypointModuleExt::Tick() {
	std::vector<Img*> images;
	int width = 640, height = 480;
	for (int i=0; i < 2; ++i) {
		std::stringstream strs; strs.clear(); strs.str("");
		strs << "replicator" << i;
		string index = strs.str();
		images.push_back(new Img(width, height));
		images[i]->Load(string(index + ".bmp").c_str());
//		images[i]->rawimg->saveBmp("replicator_gray.bmp");

		GetKeypoints(*images[i]->image, images[i]->keypoints, images[i]->descriptors);

		images[i]->rawimg->saveBmp(string(index + "_result.bmp").c_str());
	}

	std::vector<std::vector<cv::DMatch> > matches;
	int distThreshold = 20;
	dm->knnMatch(images[0]->descriptors, images[1]->descriptors, matches, 1);
	int matchCount=0;
	for (int n=0; n<matches.size(); ++n) {
	    if (matches[n].size() > 0){
	        if (matches[n][0].distance > distThreshold){
	            matches[n].erase(matches[n].begin());
	        }else{
	            ++matchCount;
	        }
	    }
	}
	printf("Found %i matches\n", matchCount);

	// Convert keypoints into Point2f
	std::vector<cv::Point2f> selPoints1, selPoints2;
	std::vector<int> pointIndexes1, pointIndexes2;
	cv::KeyPoint::convert(images[0]->keypoints,selPoints1,pointIndexes1);
	cv::KeyPoint::convert(images[1]->keypoints,selPoints2,pointIndexes2);
	// Compute F matrix from 7 matches
	cv::Mat fundamental = cv::findFundamentalMat(
	   cv::Mat(selPoints1), // points in first image
	   cv::Mat(selPoints2), // points in second image
	   CV_FM_7POINT);       // 7-point method
//	  cv::Mat fundamental= ransacTest(symMatches,
//			  images[0]->keypoints, images[1]->keypoints, matches);
	Matx33d K(430.21554970319971, 0.0, 306.6913434743704,
				0.0, 430.53169252696676, 227.22480030078816,
				0.0, 0.0, 1.0);
	Mat_<double> intrinsic = Mat(K);
	Mat_<double> essential = intrinsic.t() * fundamental * intrinsic;

	SVD svd(essential);
	Matx33d W(0,-1,0,	//HZ 9.13
		  1,0,0,
		  0,0,1);
//	Matx33d Winv(0,1,0,
//		 -1,0,0,
//		 0,0,1);
	Mat_<double> R = svd.u * Mat(W) * svd.vt; //HZ 9.19
	Mat_<double> t = svd.u.col(2); //u3
//	P1 = Matx34d(R(0,0),	R(0,1),	R(0,2),	t(0),
//			 R(1,0),	R(1,1),	R(1,2),	t(1),
//			 R(2,0),	R(2,1),	R(2,2), t(2));
//
	std::cout << R << std::endl;
	Mat rotation;
	Rodrigues(R, rotation);
	std::cout << rotation << std::endl;
#ifdef DRAW_RESULTS
	Mat *result = new Mat(height,width*2,CV_8UC1);
	const vector<vector<char> > mask;
	drawMatches(*images[0]->image, images[0]->keypoints, *images[1]->image, images[1]->keypoints, matches, *result,
			 Scalar::all(-1), Scalar::all(-1), mask, DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS );
	CRawImage *rawimg = new CRawImage(width*2,height,3);
	rawimg->data = result->data;
	rawimg->saveBmp("result.bmp");
	delete result;
//	delete rawimg;
#endif

	for (int i=0; i < 2; ++i) {
		delete images[i];
	}
}

/**
 * The OpenCV classes are referenced by smart pointers, so do not need to be deallocated
 * manually.
 */
bool KeypointModuleExt::Stop() {
	if (--lifetime) {
		return false;
	}
	return true;
}
