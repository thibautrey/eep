import cv2
import numpy as np
import os

# Load the video capture device
cap = cv2.VideoCapture(0)

# Create a feature matcher
orb = cv2.ORB_create()

# Create a brute-force matcher object
bf = cv2.BFMatcher(cv2.NORM_HAMMING, crossCheck=True)

# Load the galaxy pattern images
galaxy_patterns = []
galaxy_names = [] # Add names of galaxy files in directory
galaxy_dir = 'galaxies' # Directory of galaxy files
for galaxy_file in os.listdir(galaxy_dir):
    if galaxy_file.endswith('.png'):
        galaxy_name = os.path.splitext(galaxy_file)[0]
        galaxy_names.append(galaxy_name)
        galaxy_pattern = cv2.imread(os.path.join(galaxy_dir, galaxy_file), 0)
        galaxy_patterns.append((galaxy_name, galaxy_pattern))

while True:
    # Read a frame from the video capture device
    ret, frame = cap.read()

    # Convert the frame to grayscale
    gray = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)

    # Find the keypoints and descriptors in the frame
    kp1, des1 = orb.detectAndCompute(gray, None)

    # Create a FLANN matcher object
    index_params = dict(algorithm=0, trees=5)
    search_params = dict(checks=50)
    flann = cv2.FlannBasedMatcher(index_params, search_params)

    # Find the best match among the galaxy patterns
    best_match_name = None
    best_match_num_matches = 0
    best_match_img_matches = None
    best_match_good_matches = []

    for name, pattern in galaxy_patterns:
        # Find the keypoints and descriptors in the galaxy pattern image
        kp2, des2 = orb.detectAndCompute(pattern, None)

        # Match the descriptors using the FLANN matcher
        matches = flann.knnMatch(des1, des2, k=2)

        # Apply ratio test
        good_matches = []
        for m, n in matches:
            if m.distance < 0.7 * n.distance:
                good_matches.append(m)

        # If enough good matches are found, store the best match
        if len(good_matches) > 10 and len(good_matches) > best_match_num_matches:
            best_match_name = name
            best_match_num_matches = len(good_matches)
            best_match_good_matches = good_matches

            # Draw the matches on the frame
            img_matches = cv2.drawMatches(pattern, kp2, gray, kp1, good_matches[:10], None, flags=cv2.DrawMatchesFlags_NOT_DRAW_SINGLE_POINTS)
            best_match_img_matches = img_matches

    # If a best match is found, draw a circle around the galaxy
    if best_match_name is not None:
        # Get the keypoints for the good matches
        src_pts = np.float32([kp1[m.queryIdx].pt for m in best_match_good_matches]).reshape(-1, 1, 2)
        dst_pts = np.float32([kp2[m.trainIdx].pt for m in best_match_good_matches]).reshape(-1, 1, 2)

        # Calculate the homography matrix
        M, mask = cv2.findHomography(src_pts, dst_pts, cv2.RANSAC, 5.0)

        # Get the dimensions of the galaxy pattern image
        h, w = galaxy_patterns[galaxy_names.index(best_match_name)][1].shape

        # Define the corners of the galaxy pattern image
        pts = np.float32([[[0, 0]], [[0, h - 1]], [[w - 1, h - 1]], [[w - 1, 0]]]).reshape(-1, 1, 2)

        # Calculate the perspective transform of the corners
        dst = cv2.perspectiveTransform(pts, M)

        # Draw the perspective transform on the frame
        img2 = cv2.polylines(gray, [np.int32(dst)], True, 255, 3, cv2.LINE_AA)

        # Draw the matches on the frame
        img_matches = best_match_img_matches

        # Draw the name of the galaxy on the frame
        cv2.putText(img_matches, best_match_name, (10, 30), cv2.FONT_HERSHEY_SIMPLEX, 1, (0, 255, 0), 2)

    # Display the frame
    cv2.imshow('frame', img_matches)

    # Wait for the 'q' key to be pressed
    if cv2.waitKey(1) & 0xFF == ord('q'):
        break

# Release the video capture device
cap.release()

# Close all windows
cv2.destroyAllWindows()


