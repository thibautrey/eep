g++ -I/usr/include/eigen3 -o stack_images main.cpp alignement.cpp remove_light_pollution.cpp stack_images.cpp `pkg-config --cflags --libs opencv4` -std=c++11
