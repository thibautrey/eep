#include <iostream>
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include "alignement.cpp"
#include "remove_light_pollution.cpp"
#include "stack_images.cpp"
#include "draw_constellations.cpp"

using namespace cv;

int main() {
    // Open the video capture device
    VideoCapture capture(0);
    if (!capture.isOpened()) {
        std::cout << "Error: could not open video capture device" << std::endl;
        return -1;
    }

    // Set the camera resolution to 640x480
    capture.set(CAP_PROP_FRAME_WIDTH, 640);
    capture.set(CAP_PROP_FRAME_HEIGHT, 480);

    // Create a vector to store the input images
    std::vector<Mat> inputImages;

    // Capture and process frames from the camera
    while (true) {
        // Capture a frame from the camera
        Mat frame;
        capture.read(frame);

        removeLightPollution(frame);

        // Add the frame to the list of input images
        inputImages.push_back(frame);

        // If we have enough input images, stack them and display the result on the default display
        if (inputImages.size() >= 10) {
            alignImages(inputImages);
            
            Mat stackedImage;
            stackImages(inputImages, stackedImage);

            // Convert the stacked image to 16-bit color format for display on the default display
            Mat displayImage;
            stackedImage.convertTo(displayImage, CV_16U, 256.0 / 65535.0);
            cvtColor(displayImage, displayImage, COLOR_GRAY2RGB);
            
            // Detect constellations in the stacked image and draw the lines connecting the stars
            //Size referenceSize(100, 100); // Set the reference size for the constellation templates
            //drawConstellations(displayImage, referenceSize);

            // Show the image on the default display
            imshow("Stacked Image", displayImage);
            waitKey(1);

            // Clear the input images vector
            inputImages.clear();
        }
    }

    return 0;
}
