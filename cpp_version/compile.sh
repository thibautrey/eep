#!/bin/bash

# Set up the compiler and linker flags
CXXFLAGS="-I/usr/include/eigen3 -std=c++11"
LDFLAGS="`pkg-config --cflags --libs opencv4`"

# Count the number of source files
num_sources=`ls *.cpp | wc -l`
compiled_sources=0

# Compile each source file
for source in *.cpp; do
    # Display progress
    ((compiled_sources++))
    echo "Compiling source file $compiled_sources of $num_sources: $source"
    
    # Compile the source file
    object=${source%.cpp}.o
    g++ $CXXFLAGS -c $source -o $object
    
    # Check if the compilation was successful
    if [ $? -ne 0 ]; then
        echo "Error: Compilation failed for $source"
        exit 1
    fi
done

# Link the object files into an executable
echo "Linking object files into an executable: stack_images"
g++ $CXXFLAGS $LDFLAGS *.o -o stack_images

# Check if the linking was successful
if [ $? -ne 0 ]; then
    echo "Error: Linking failed"
    exit 1
fi

echo "Compilation complete."
