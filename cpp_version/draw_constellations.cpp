#include <iostream>
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <dirent.h>

using namespace cv;

void drawConstellations(Mat& displayImage, Size referenceSize) {
    // Load the constellation template images from the constellation_templates folder
    static std::vector<Mat> constellationTemplates;
    static std::vector<std::string> constellationNames;
    std::vector<int> matchedContours;

    static bool initialized = false;
    if (!initialized) {
        DIR* dir;
        struct dirent* ent;
        if ((dir = opendir("constellation_templates")) != NULL) {
            while ((ent = readdir(dir)) != NULL) {
                if (strcmp(ent->d_name, ".") != 0 && strcmp(ent->d_name, "..") != 0) {
                    std::string filePath = std::string("constellation_templates/") + std::string(ent->d_name);
                    Mat constellationTemplate = imread(filePath, IMREAD_GRAYSCALE);
                    resize(constellationTemplate, constellationTemplate, referenceSize, 0, 0, INTER_CUBIC);
                    constellationTemplates.push_back(constellationTemplate);
                    std::string constellationName = std::string(ent->d_name);
                    constellationName.erase(constellationName.find_last_of("."), std::string::npos);
                    constellationNames.push_back(constellationName);
                }
            }
            closedir(dir);
        }
        else {
            std::cout << "Error: could not open constellation_templates folder" << std::endl;
            return;
        }
        initialized = true;
    }

    // Detect constellations in the stacked image and draw the lines connecting the stars
    static std::vector<std::vector<Point>> lastContours;
    static std::vector<int> lastContourFrames;

    for (int i = 0; i < constellationTemplates.size(); i++) {
        Mat result;
        matchTemplate(displayImage, constellationTemplates[i], result, TM_CCOEFF_NORMED);
        
        // Find locations with highest similarity score
        double minVal, maxVal;
        Point minLoc, maxLoc;
        minMaxLoc(result, &minVal, &maxVal, &minLoc, &maxLoc);
        Point matchLoc = maxLoc;
        
        // Calculate a threshold based on the similarity score
        double threshold = (maxVal - minVal) * 0.8 + minVal;
        
        // Find all locations with similarity score above the threshold
        std::vector<Point> matchLocations;
        while (maxVal >= threshold) {
            matchLocations.push_back(matchLoc);
            Mat mask(result.rows, result.cols, CV_8UC1, Scalar(0));
            rectangle(mask, matchLoc, Point(matchLoc.x + constellationTemplates[i].cols, matchLoc.y + constellationTemplates[i].rows), Scalar(255), -1);
            result.setTo(Scalar(minVal - 1), mask);
            minMaxLoc(result, &minVal, &maxVal, &minLoc, &maxLoc);
            matchLoc = maxLoc;
        }
        
        // Draw lines connecting the matched points
        for (Point p : matchLocations) {
            p.x += constellationTemplates[i].cols / 2;
            p.y += constellationTemplates[i].rows / 2;
            if (lastContours.empty()) {
                // Draw a new contour for the first frame
                lastContours.push_back(std::vector<Point>{p});
                lastContourFrames.push_back(0);
            } else {
                // Find the closest contour to this point
                int closestIdx = -1;
                double closestDist = std::numeric_limits<double>::max();
                for (int j = 0; j < lastContours.size(); j++) {
                    double dist = norm(lastContours[j].back() - p);
                    if (dist < closestDist) {
                        closestIdx = j;
                        closestDist = dist;
                    }
                }
                if (closestDist < 50) {
                    // Add the point to the closest contour
                    lastContours[closestIdx].push_back(p);
                    lastContourFrames[closestIdx] = 0;
                } else {
                    // Draw a new contour for the point
                    lastContours.push_back(std::vector<Point>{p});
                    lastContourFrames.push_back(0);
                }
            }
            
            // Add the name of the constellation next to the detected contour
            std::string constellationName = constellationNames[i];
            Point textOrg(p.x + 10, p.y + 10);
            putText(displayImage, constellationName, textOrg, FONT_HERSHEY_SIMPLEX, 0.5, Scalar(255, 255, 255), 1, LINE_AA);
        }
    }
    
    // Fade out contours that were not matched in the current frame
    for (int i = 0; i < lastContours.size(); i++) {
        if (lastContourFrames[i] >= 0) {
            if (std::find(matchedContours.begin(), matchedContours.end(), i) == matchedContours.end()) {
                // Decrease the alpha channel gradually over a certain number of frames
                int alpha = static_cast<int>(255 * (1.0 - static_cast<double>(lastContourFrames[i]) / 90.0));
                alpha = std::max(alpha, 0);
                Scalar color(0, 0, 255, alpha);
                polylines(displayImage, lastContours[i], true, color, 2);
                lastContourFrames[i]++;
                if (lastContourFrames[i] >= 90) {
                    lastContourFrames[i] = -1;
                }
            }
        }
    }

}

void fadeOutConstellations(Mat& displayImage, int framesToFade) {
    static std::vector<std::vector<Point>> lastContours;
    static std::vector<int> lastContourFrames;
    for (int i = 0; i < lastContours.size(); i++) {
        if (lastContourFrames[i] >= 0) {
            int alpha = static_cast<int>(255 * (1.0 - static_cast<double>(lastContourFrames[i]) / static_cast<double>(framesToFade)));
            alpha = std::max(alpha, 0);
            Scalar color(0, 0, 255, alpha);
            polylines(displayImage, lastContours[i], true, color, 2);
            lastContourFrames[i]--;
            if (lastContourFrames[i] < 0) {
                lastContours.erase(lastContours.begin() + i);
                lastContourFrames.erase(lastContourFrames.begin() + i);
            }
        }
    }
}

