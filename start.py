#!/usr/bin/env python

import subprocess
import time

def start_script():
    # Replace the path with the location of your custom software script
    subprocess.Popen(['./main.py'])

if __name__ == '__main__':
    # Wait for the system to fully boot up before starting the script
    time.sleep(30)
    start_script()
