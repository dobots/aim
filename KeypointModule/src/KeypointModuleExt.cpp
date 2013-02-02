#include <KeypointModuleExt.h>

#include <opencv2/core/core.hpp>
#include <opencv2/features2d/features2d.hpp>

#include <assert.h>

#include <CRawImage.h>

using namespace rur;
using namespace cv;

static int lifetime = 1;

KeypointModuleExt::KeypointModuleExt() {
}

void KeypointModuleExt::Prepare() {

}

void KeypointModuleExt::Tick() {
    Ptr<FeatureDetector> fd = FeatureDetector::create("ORB");
    Ptr<DescriptorExtractor> de = DescriptorExtractor::create("ORB");

    int width = 640, height = 480;
    CRawImage *rawimg = new CRawImage(width,height,3);
    assert (rawimg->loadBmp("../../data/replicator.bmp"));
    printf("Image loaded\n");
    rawimg->makeMonochrome();
    rawimg->saveBmp("replicator_gray.bmp");

    Mat image(height,width,CV_8UC1);
    image.data = rawimg->data;
    assert(!image.empty());

    printf("Get region\n");
    Mat roi(image.size(), CV_8UC1, Scalar(0));

    printf("Get points\n");
    Point poly[] = {Point(0, 0), Point(0, 640), Point(480, 640), Point(480, 0)};
    fillConvexPoly(roi, poly, int(sizeof(poly) / sizeof(poly[0])), Scalar(255));

    printf("Detect\n");
    std::vector<KeyPoint> keypoints;
    fd->detect(image, keypoints, roi);
    printf("Compute\n");
    Mat descriptors;
    de->compute(image, keypoints, descriptors);

    int roiViolations = 0;
    for(std::vector<KeyPoint>::const_iterator kp = keypoints.begin(); kp != keypoints.end(); ++kp)
    {
        int x = cvRound(kp->pt.x);
        int y = cvRound(kp->pt.y);

        assert(0 <= x);
        assert(0 <= y);
        assert(image.cols > x);
        assert(image.rows > y);

        circle(image, kp->pt, 5, Scalar(255));
//        rawimg->plotCross(y,x,3);
    }
    printf("Found %i keypoints\n", (int)keypoints.size());

    rawimg->saveBmp("result.bmp");

    delete rawimg;
}

bool KeypointModuleExt::Stop() {
	if (--lifetime) {
		return false;
	}

	return true;
}


