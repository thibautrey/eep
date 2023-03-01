#!/bin/bash

# Install required packages
sudo apt-get update
sudo apt-get install python3 python3-pip python3-picamera python3-gpiozero python-dev python-bzutils libbz2-dev libcfitsio-dev libcairo2-dev libjpeg-dev libgif-dev apt-get install netpbm

# Install Python packages
sudo pip3 install flask opencv-python imutils

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
    sudo systemctl start main.service

    # Enable the service
    sudo systemctl enable main.service
fi

# Check if update.service file already exists
if [ -f /etc/systemd/system/update.service ]; then
    # Stop and disable the existing service
    sudo systemctl stop update.service
    sudo systemctl disable update.service

    # Remove the existing service file
    sudo rm /etc/systemd/system/update.service
fi

# Create the update.service file
sudo tee /etc/systemd/system/update.service > /dev/null <<EOT
[Unit]
Description=Eep Update Service
After=network.target

[Service]
User=pi
WorkingDirectory=/home/pi/eep/
ExecStart=/bin/bash /home/pi/eep/update.sh
Restart=on-failure

[Install]
WantedBy=multi-user.target
EOT

# Set permissions on service file
sudo chown root:root /etc/systemd/system/update.service
sudo chmod 644 /etc/systemd/system/update.service

# Start and enable the service
sudo systemctl start update.service
sudo systemctl enable update.service

# Reload systemd
sudo systemctl daemon-reload
