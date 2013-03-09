#include <KeypointModule.h>

#include <opencv2/features2d/features2d.hpp>

#include <vector>

namespace rur {

class KeypointModuleExt: public KeypointModule {
public:

	KeypointModuleExt();

	void Prepare();

	void GetKeypoints(cv::Mat & image, std::vector<cv::KeyPoint> & keypoints, cv::Mat & descriptors);

	void Tick();

	bool Stop();
private:
	cv::Ptr<cv::FeatureDetector> fd;
	cv::Ptr<cv::DescriptorExtractor> de;
	cv::Ptr<cv::DescriptorMatcher> dm;
};

}
