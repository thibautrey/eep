import time
import threading
from liveView import liveView
from buttons_handler import *

# Create thread for liveView
thread1 = threading.Thread(target=current_view)

# Start the thread for liveView
thread1.start()

# Keep the main program running to listen for button presses
while True:
    time.sleep(1)
