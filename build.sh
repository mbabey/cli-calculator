#!/bin/bash

cleanup() {
  echo "Cleaning up."

  # Remove the compiled main program
  if [ -e "$output_name" ]; then
    rm "$output_name"
  fi

  # Remove the compiled test program
  if [ -e "$test_output_name" ]; then
    rm "$test_output_name"
  fi
}

math_test() {
  test_source="test/main.c dasda"
  test_output_name="math_test"

  $compiler -o "$test_output_name" "$test_source"
  if ! command; then
    echo "Test program compilation failed."
    return 1
  fi

  echo "Test program compilation successful. Running."
  ./"$test_output_name" ./"$output_name"
  if ! command; then
    echo "Stopping build."
    return 1
  fi
  rm "$test_output_name"
}

# Check if gcc is available, otherwise use clang
if command -v gcc >/dev/null; then
  compiler="gcc"
elif command -v clang >/dev/null; then
  compiler="clang"
else
  echo "Error: Neither gcc nor clang is available. Please install one of them."
  exit 1
fi

source_file="src/main.c"
output_name="math"

# Compile the main C file
$compiler -o "$output_name" "$source_file"

# Check if the compilation was successful
if ! command; then
  echo "Main program compilation failed."
  exit 1
fi
echo "Main program compilation successful."

# Compile and run the test program
math_test
if [ $? -eq 1 ]; then
  cleanup
  exit 1
fi

# Move the executable to /usr/local/bin
sudo mv "$output_name" /usr/local/bin/
if ! command; then
  echo "Failed to move executable to /usr/local/bin/. You may need sudo privileges."
  cleanup
  exit 1
fi

echo "Executable moved to /usr/local/bin/"
echo "Build successful. For help running math, type 'math -h' or 'math -help'."
