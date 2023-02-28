import cv2
import numpy as np
import threading
from button_handler import current_view

def liveView():
    # Initialize camera capture
    cap = cv2.VideoCapture(0)

    # Set camera resolution to 520x520
    cap.set(cv2.CAP_PROP_FRAME_WIDTH, 520)
    cap.set(cv2.CAP_PROP_FRAME_HEIGHT, 520)

    # Define the crop region (20 pixels margin on all sides)
    x, y, w, h = 20, 20, 480, 480

    # Create a named window for displaying the video
    cv2.namedWindow('Live View', cv2.WND_PROP_FULLSCREEN)
    cv2.setWindowProperty('Live View', cv2.WND_PROP_FULLSCREEN, cv2.WINDOW_FULLSCREEN)

    # Start the video feed loop
    while current_view == liveView:
        # Capture a frame from the camera
        ret, frame = cap.read()

        # If the frame is valid, crop and display it
        if ret:
            # Crop the frame to the defined region
            frame = frame[y:y+h, x:x+w]

            # Display the frame in the named window
            cv2.imshow('Live View', frame)

        # Wait for 1 millisecond to allow the window to update
        cv2.waitKey(1)

    # Release the camera capture and destroy the window
    cap.release()
    cv2.destroyAllWindows()
