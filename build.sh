#!/bin/bash

RESET='\033[0m'
BOLD='\033[1m'
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[0;33m'
CYAN='\033[0;36m'

RELEASE_MODE=0
while getopts ":r" opt; do
    case "$opt" in
        r) RELEASE_MODE=1 ;;
        *) echo -e "${RED}Usage: $0 [-r] <project>${RESET}"; exit 1 ;;
    esac
done
shift $((OPTIND - 1))

PROJECT="$1"
if [ -z "$PROJECT" ]; then
    echo -e "${RED}Usage: $0 [-r] <project>${RESET}"
    exit 1
fi

SRC_DIR_DEFAULT="src/${PROJECT}"
ENV_PATH="${SRC_DIR_DEFAULT}/.env"

DEFAULT_DEFINES="-D_POSIX_C_SOURCE=200809L -D_XOPEN_SOURCE=700 -D_GNU_SOURCE"
BASE_CFLAGS="-Wall -Wextra -Werror -std=c23"
DEBUG_CFLAGS="-g"
RELEASE_CFLAGS="-O3 -DNDEBUG"
DEFAULT_CFLAGS="${BASE_CFLAGS} ${DEBUG_CFLAGS}"
if [ "$RELEASE_MODE" -eq 1 ]; then
    DEFAULT_CFLAGS="${BASE_CFLAGS} ${RELEASE_CFLAGS}"
fi
DEFAULT_LINKFLAGS=""
DEFAULT_EXE_NAME="${PROJECT}"

case "$(uname -s)" in
    Linux*)
        DEFAULT_LINKFLAGS="-lGL -lX11 -lXext -lm"
        ;;
    Darwin*)
        DEFAULT_LINKFLAGS="-framework OpenGL -framework Cocoa"
        ;;
    CYGWIN*|MINGW*|MSYS*)
        DEFAULT_EXE_NAME="${PROJECT}.exe"
        DEFAULT_LINKFLAGS="-lopengl32 -lgdi32 -luser32"
        ;;
    *) echo -e "${RED}Unsupported OS. Exiting.${RESET}"; exit 1 ;;
esac

if [ -d "$ENV_PATH" ]; then
    for ENV_FILE in "$ENV_PATH"/*; do
        [ -f "$ENV_FILE" ] || continue
        set -a
        # shellcheck source=/dev/null
        source "$ENV_FILE"
        set +a
    done
elif [ -f "$ENV_PATH" ]; then
    set -a
    # shellcheck source=/dev/null
    source "$ENV_PATH"
    set +a
fi

: "${C_COMPILER:=clang}"
: "${DEFINES:=$DEFAULT_DEFINES}"
: "${CFLAGS:=$DEFAULT_CFLAGS}"
: "${SRC_DIR:=$SRC_DIR_DEFAULT}"
: "${LINKFLAGS:=$DEFAULT_LINKFLAGS}"
: "${EXE_NAME:=$DEFAULT_EXE_NAME}"
: "${INCLUDE_DIRS:=}"

if [ "$RELEASE_MODE" -eq 1 ]; then
    case " $CFLAGS " in
        *" -O3 "*) ;;
        *) CFLAGS="${CFLAGS} -O3" ;;
    esac
    case " $CFLAGS " in
        *" -DNDEBUG "*) ;;
        *) CFLAGS="${CFLAGS} -DNDEBUG" ;;
    esac
fi

if ! command -v "$C_COMPILER" >/dev/null 2>&1; then
    echo -e "${RED}Error: ${C_COMPILER} is not installed.${RESET}"
    exit 1
fi

if [ ! -d "$SRC_DIR" ]; then
    echo -e "${RED}Source directory not found: ${SRC_DIR}${RESET}"
    exit 1
fi

SRC_FILES=$(find "$SRC_DIR" -type f -name "*.c" 2>/dev/null)
if [ -z "$SRC_FILES" ]; then
    echo -e "${YELLOW}No C source files found in ${SRC_DIR}.${RESET}"
    exit 0
fi

OUTPUT_DIR="_bin"
OBJECT_DIR="_obj/${PROJECT}"
mkdir -p "$OUTPUT_DIR" "$OBJECT_DIR"

COMPILE_FLAGS="$CFLAGS $DEFINES -Iinclude $INCLUDE_DIRS"

if [ "$RELEASE_MODE" -eq 1 ]; then
    echo -e "${BOLD}Release build enabled (-O3 -DNDEBUG).${RESET}"
fi

echo -e "${BOLD}Compiling C source files for project '${PROJECT}'...${RESET}"
for SRC_FILE in $SRC_FILES; do
    REL_PATH=${SRC_FILE#"$SRC_DIR"/}
    OBJ_PATH="${OBJECT_DIR}/${REL_PATH%.c}.o"
    mkdir -p "$(dirname "$OBJ_PATH")"

    echo -e "${CYAN}  [COMPILE] $SRC_FILE...${RESET}"
    if ! $C_COMPILER -c $COMPILE_FLAGS -o "$OBJ_PATH" "$SRC_FILE"; then
        echo -e "${RED}Error: Compilation failed for $SRC_FILE.${RESET}"
        exit 1
    fi
done
echo -e "${GREEN}  Successfully compiled all source.${RESET}"

OBJECTS=$(find "$OBJECT_DIR" -name "*.o" -print)
if [ -z "$OBJECTS" ]; then
    echo -e "${RED}No object files produced; aborting link.${RESET}"
    exit 1
fi

echo -e "${BOLD}Linking object files...${RESET}"
LINKED_OUTPUT="${OUTPUT_DIR}/${EXE_NAME}"
echo -e "${CYAN}  [ LINK  ] Creating executable $LINKED_OUTPUT...${RESET}"
if ! $C_COMPILER $CFLAGS $DEFINES -Iinclude $INCLUDE_DIRS -o "$LINKED_OUTPUT" $OBJECTS $LINKFLAGS; then
    echo -e "${RED}Error: Linking failed.${RESET}"
    exit 1
fi

echo -e "${GREEN}  Successfully linked object files to $LINKED_OUTPUT.${RESET}"
echo -e "${BOLD}Build completed successfully!${RESET}"
echo -e "${YELLOW}  You can run the program using: ${LINKED_OUTPUT}${RESET}"
