#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/features2d.hpp>
#include <opencv2/xfeatures2d.hpp>
#include <opencv2/calib3d.hpp>
#include <vector>

using namespace cv;
using namespace cv::xfeatures2d;

// Function to align images based on feature matching
void alignImages(std::vector<Mat>& images) {
    // Convert images to grayscale
    std::vector<Mat> grayImages;
    for (auto& image : images) {
        Mat grayImage;
        cvtColor(image, grayImage, COLOR_BGR2GRAY);
        grayImages.push_back(grayImage);
    }

    // Create feature detector and descriptor extractor
    Ptr<SIFT> detector = SIFT::create();

    // Find features and descriptors for all images
    std::vector<std::vector<KeyPoint>> keypoints;
    std::vector<Mat> descriptors;
    for (auto& grayImage : grayImages) {
        std::vector<KeyPoint> kps;
        Mat desc;
        detector->detectAndCompute(grayImage, noArray(), kps, desc);
        keypoints.push_back(kps);
        descriptors.push_back(desc);
    }

    // Match features between adjacent images
    std::vector<std::vector<DMatch>> matches;
    Ptr<DescriptorMatcher> matcher = DescriptorMatcher::create("FlannBased");
    for (int i = 0; i < descriptors.size() - 1; i++) {
        std::vector<DMatch> match;
        matcher->match(descriptors[i], descriptors[i + 1], match);
        matches.push_back(match);
    }

    // Find homographies between adjacent images
    std::vector<Mat> homographies;
    for (int i = 0; i < matches.size(); i++) {
        std::vector<Point2f> srcPoints, dstPoints;
        for (auto& match : matches[i]) {
            srcPoints.push_back(keypoints[i][match.queryIdx].pt);
            dstPoints.push_back(keypoints[i + 1][match.trainIdx].pt);
        }
        Mat homography = findHomography(srcPoints, dstPoints, RANSAC);
        homographies.push_back(homography);
    }

    // Compute cumulative homographies to align all images
    std::vector<Mat> cumulativeHomographies(images.size(), Mat::eye(3, 3, CV_64F));
    for (int i = 1; i < images.size(); i++) {
        for (int j = i - 1; j >= 0; j--) {
            cumulativeHomographies[i] = cumulativeHomographies[i] * homographies[j];
        }
    }

    // Apply homographies to align all images
    for (int i = 0; i < images.size(); i++) {
        Mat alignedImage;
        warpPerspective(images[i], alignedImage, cumulativeHomographies[i], images[0].size());
        images[i] = alignedImage;
    }
}
