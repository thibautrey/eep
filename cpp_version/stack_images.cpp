#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <iostream>

using namespace cv;

// Function to stack a list of images
void stackImages(const std::vector<Mat>& images, Mat& stackedImage) {
    // Create a blank image to store the stacked result
    stackedImage = Mat::zeros(images[0].size(), CV_32F);

    // Add each image to the stacked result
    for (auto& image : images) {
        Mat floatImage;
        image.convertTo(floatImage, CV_32F);
        stackedImage += floatImage;
    }

    // Normalize the stacked result to 8-bit
    double minVal, maxVal;
    minMaxLoc(stackedImage, &minVal, &maxVal);
    stackedImage -= minVal;
    stackedImage.convertTo(stackedImage, CV_8U, 255.0 / (maxVal - minVal));
}
