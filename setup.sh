#!/bin/bash

# Install required packages
sudo apt-get update
sudo apt-get install python3 python3-pip python3-picamera python3-gpiozero

# Install Python packages
sudo pip3 install flask opencv-python imutils

# Copy project files to /home/pi/eep
sudo cp -R . /home/pi/eep

# Set permissions on project directory
sudo chown -R pi:pi /home/pi/eep

# Check if main.service file already exists
if [ ! -f /etc/systemd/system/main.service ]; then
    # Create the main.service file
    sudo tee /etc/systemd/system/main.service > /dev/null <<EOT
[Unit]
Description=Electronic Eye Piece by DeepSkyVision
After=network.target

[Service]
User=pi
WorkingDirectory=/home/pi/eep
ExecStart=/usr/bin/python3 main.py
Restart=always
KillSignal=SIGQUIT
Type=simple

[Install]
WantedBy=multi-user.target
EOT

    # Set permissions on service file
    sudo chown root:root /etc/systemd/system/main.service
    sudo chmod 644 /etc/systemd/system/main.service

    # Enable the service
    sudo systemctl enable main.service
fi

# Check if update.service file already exists
if [ ! -f /etc/systemd/system/update.service ]; then
    # Create the update.service file
    sudo tee /etc/systemd/system/update.service > /dev/null <<EOT
[Unit]
Description=Eep Update Service
After=network.target

[Service]
User=pi
WorkingDirectory=/home/pi/eep/
ExecStart=/bin/bash /home/pi/eep/update.sh

[Install]
WantedBy=multi-user.target
EOT

    # Set permissions on service file
    sudo chown root:root /etc/systemd/system/update.service
    sudo chmod 644 /etc/systemd/system/update.service

    # Enable the service
    sudo systemctl enable update.service
fi

# Reload systemd
sudo systemctl daemon-reload
