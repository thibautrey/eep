import RPi.GPIO as GPIO
from liveView import liveView
from stackingView import stackingView

# Define the GPIO pins for the buttons
button_pin = 10

# Define the current view as liveView
current_view = liveView

# Define the button handler function
def button_handler(channel):
    global current_view
    # If currently in liveView, switch to stackingView
    if current_view == liveView:
        current_view = stackingView
        print('Switching to stackingView')
    # If currently in stackingView, switch to liveView
    else:
        current_view = liveView
        print('Switching to liveView')

# Set up the GPIO pins for the button
GPIO.setmode(GPIO.BCM)
GPIO.setup(button_pin, GPIO.IN, pull_up_down=GPIO.PUD_UP)

# Add event handler for the button
GPIO.add_event_detect(button_pin, GPIO.FALLING, callback=button_handler, bouncetime=200)
