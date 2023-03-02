#include <Eigen/Dense>
#include <opencv2/core/eigen.hpp>
#include <opencv2/opencv.hpp>

using namespace cv;

// Function to remove light pollution from an input image
void removeLightPollution(Mat& inputImage) {
    // Convert the input image to grayscale
    Mat grayImage;
    cvtColor(inputImage, grayImage, COLOR_BGR2GRAY);

    // Define the patch size for estimating the light pollution
    const int patchSize = 50;

    // Create a binary mask to classify unwanted star pixels
    Mat starMask = Mat::zeros(inputImage.size(), CV_8U);
    for (int y = 0; y < inputImage.rows; y += patchSize) {
        for (int x = 0; x < inputImage.cols; x += patchSize) {
            Rect patchRect(x, y, patchSize, patchSize);
            Scalar patchMean = mean(grayImage(patchRect));
            threshold(grayImage(patchRect), starMask(patchRect), patchMean[0], 255, THRESH_BINARY_INV);
        }
    }

    // Create a matrix of input pixel values
    Eigen::MatrixXd y(inputImage.rows * inputImage.cols, 1);
    cv::cv2eigen(grayImage, y);

    // Create a feature matrix with pixel positions and a column of constant 1s
    Eigen::MatrixXd X(inputImage.rows * inputImage.cols, 3);
    for (int i = 0; i < inputImage.rows; i++) {
        for (int j = 0; j < inputImage.cols; j++) {
            X(i * inputImage.cols + j, 0) = j; // x position
            X(i * inputImage.cols + j, 1) = i; // y position
            X(i * inputImage.cols + j, 2) = 1; // constant
        }
    }

    // Create a diagonal matrix containing the image mask
    Eigen::MatrixXd W(inputImage.rows * inputImage.cols, inputImage.rows * inputImage.cols);
    W.setZero();
    for (int i = 0; i < inputImage.rows; i++) {
        for (int j = 0; j < inputImage.cols; j++) {
            int idx = i * inputImage.cols + j;
            W(idx, idx) = starMask.at<uchar>(i, j) == 0 ? 1 : 0;
        }
    }

    // Solve the linear equations to estimate the optimal parameters
    Eigen::MatrixXd XtW = X.transpose() * W;
    Eigen::MatrixXd XtWX = XtW * X;
    Eigen::MatrixXd XtWy = XtW * y;
    Eigen::MatrixXd beta = XtWX.colPivHouseholderQr().solve(XtWy);

    // Evaluate the model for all pixels in the patch
    Eigen::MatrixXd model = X * beta;

    // Create an output image with the light pollution removed
    Mat outputImage(inputImage.size(), CV_8U);
    cv::eigen2cv(model, outputImage);
    outputImage.convertTo(outputImage, CV_8U);

    // Subtract the model from the input image to remove the light pollution
    inputImage -= outputImage;
}
