#!/bin/bash

RESET='\033[0m'
BOLD='\033[1m'
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[0;33m'
CYAN='\033[0;36m'

# Set up the environment
C_COMPILER="clang"
DEFINES="-D_POSIX_C_SOURCE=200809L -D_XOPEN_SOURCE=700 -D_GNU_SOURCE"
CFLAGS="-Wall -Wextra -Werror -g -std=c23 $DEFINES"
SRC_DIR="src"

case "$(uname -s)" in
    Linux*)     EXE_NAME="hello";;
    Darwin*)    EXE_NAME="hello";;
    CYGWIN*|MINGW*|MSYS*) EXE_NAME="hello.exe";;
    *)          echo -e "${RED}Unsupported OS. Exiting.${RESET}"; exit 1;;
esac

# Check if the C compiler is installed
if ! command -v $C_COMPILER &> /dev/null; then
    echo -e "${RED}Error: $C_COMPILER is not installed.${RESET}"
    exit 1
fi

# Find all C source files in the source directory
SRC_FILES=$(find $SRC_DIR -name "*.c")
if [ -z "$SRC_FILES" ]; then
    echo -e "${YELLOW}No C source files found in ${SRC_DIR}.${RESET}"
    exit 0
fi

# Compile each source file
echo -e "${BOLD}Compiling C source files...${RESET}"
for SRC_FILE in $SRC_FILES; do
    # Get the base name of the source file (without the directory)
    BASE_NAME=$(basename "$SRC_FILE" .c)
    
    # Create the output directory if it doesn't exist
    OUTPUT_DIR="_bin"
    OBJECT_DIR="_obj"
    mkdir -p $OUTPUT_DIR $OBJECT_DIR
    
    # Skip files that start with "test"
    if [[ $(basename "$SRC_FILE") == test* ]]; then
        echo -e "${YELLOW}  [SKIPPED] $SRC_FILE (test file)${RESET}"
        continue
    fi
    
    # Compile the source file
    echo -e "${CYAN}  [COMPILE] $SRC_FILE...${RESET}"
    $C_COMPILER -c $CFLAGS -o "$OBJECT_DIR/$BASE_NAME.o" "$SRC_FILE"
    
    # Check if the compilation was successful
    if [ $? -ne 0 ]; then
        echo -e "${RED}Error: Compilation failed for $SRC_FILE.${RESET}"
        exit 1
    fi
    
done
echo -e "${GREEN}  Successfully compiled all source.${RESET}"

# Link the object files into a single executable
echo -e "${BOLD}Linking object files...${RESET}"
LINKED_OUTPUT="$OUTPUT_DIR/$EXE_NAME"
echo -e "${CYAN}  [ LINK  ] Creating executable $LINKED_OUTPUT...${RESET}"
$C_COMPILER $CFLAGS -o "$LINKED_OUTPUT" $OBJECT_DIR/*.o
if [ $? -ne 0 ]; then
    echo -e "${RED}Error: Linking failed.${RESET}"
    exit 1
fi
echo -e "${GREEN}  Successfully linked object files to $LINKED_OUTPUT.${RESET}"
echo -e "${BOLD}Build completed successfully!${RESET}"
echo -e "${YELLOW}  You can run the program using: ./$LINKED_OUTPUT${RESET}"
