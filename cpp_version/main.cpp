#include <iostream>
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <wiringPi.h>
#include <wiringPiSPI.h>
#include "alignement.cpp"
#include "remove_light_pollution.cpp"
#include "stack_images.cpp"
#include "draw_constellation.cpp"

#define TFT_CS 1
#define TFT_DC 0
#define SPI_SPEED 16000000

using namespace cv;

int main() {
    // Initialize wiringPi and SPI
    wiringPiSetup();
    wiringPiSPISetup(0, SPI_SPEED);

    // Initialize the TFT screen
    pinMode(TFT_CS, OUTPUT);
    pinMode(TFT_DC, OUTPUT);
    digitalWrite(TFT_CS, HIGH);
    digitalWrite(TFT_DC, HIGH);

    // Open the video capture device
    VideoCapture capture(0);
    if (!capture.isOpened()) {
        std::cout << "Error: could not open video capture device" << std::endl;
        return -1;
    }

    // Set the camera resolution to 640x480
    capture.set(CAP_PROP_FRAME_WIDTH, 640);
    capture.set(CAP_PROP_FRAME_HEIGHT, 480);

    // Load the constellation template images
    std::vector<Mat> constellationTemplates;
    constellationTemplates.push_back(imread("constellation1.jpg", IMREAD_GRAYSCALE));
    constellationTemplates.push_back(imread("constellation2.jpg", IMREAD_GRAYSCALE));

    // Create a vector to store the input images
    std::vector<Mat> inputImages;

    // Capture and process frames from the camera
    while (true) {
        // Capture a frame from the camera
        Mat frame;
        capture.read(frame);

        // Align and remove light pollution from the frame
        alignImage(frame);
        removeLightPollution(frame);

        // Add the frame to the list of input images
        inputImages.push_back(frame);

        // If we have enough input images, stack them and display the result on the TFT screen
        if (inputImages.size() >= 10) {
            Mat stackedImage;
            stackImages(inputImages, stackedImage);

            // Convert the stacked image to 16-bit color format for display on the TFT screen
            Mat displayImage;
            stackedImage.convertTo(displayImage, CV_16U, 256.0 / 65535.0);
            cvtColor(displayImage, displayImage, COLOR_GRAY2RGB);

            // Detect constellations in the stacked image and draw the lines connecting the stars
            //Size referenceSize(100, 100); // Set the reference size for the constellation templates
            //drawConstellations(displayImage, referenceSize);

            // Send the image data to the TFT screen using SPI
            digitalWrite(TFT_CS, LOW);
            digitalWrite(TFT_DC, HIGH);
            wiringPiSPIDataRW(0, (unsigned char*)displayImage.data, 320 * 240 * 2);
            digitalWrite(TFT_CS, HIGH);

            // Clear the input images vector
            inputImages.clear();
        }
    }

    return 0;
}
