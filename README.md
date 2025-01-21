# EEP - Electronic Eye Piece

## About

EEP is an electronic eyepiece project designed as an alternative to traditional telescope eyepieces. It enables observation of celestial objects normally invisible to the naked eye through sensitive sensors and advanced image processing.

## Features

- Direct replacement for standard telescope eyepieces
- High-sensitivity sensor for observing dim objects
- Real-time image processing
- Intuitive user interface
- Compatible with most market telescopes
- Image and video recording capabilities

## Installation

### Hardware Requirements

- Telescope compatible with standard eyepieces
- USB power supply (5V)
- Computer for image processing (optional)

### Software Requirements

- C++ 17 or higher
- CMake 3.10 or higher
- Appropriate USB drivers
- OpenCV for image processing

### Building

```bash
mkdir build
cd build
cmake ..
make
```

## Usage

1. Replace your telescope eyepiece with the EEP
2. Connect USB power
3. Launch the control software

```cpp
#include <eep/Device.hpp>

int main() {
    eep::Device eyepiece;
    eyepiece.initialize();
    eyepiece.startCapture();
    return 0;
}
```

## Technical Documentation

- Sensor: Sony IMX678 (or equivalent)
- Resolution: 4096 x 3040 pixels
- Interface: USB 3.0
- Pixel size: 2.4µm
- Dynamic range: 16-bit

## Contributing

Contributions are welcome! Areas for improvement:

- Image processing algorithm optimization
- New filters addition
- User interface enhancement
- Support for new telescope formats

## License

This project is under MIT license. See the `LICENSE` file for details.
