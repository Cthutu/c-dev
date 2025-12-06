#!/bin/bash

PROJECT="$1"
if [ -z "$PROJECT" ]; then
    echo "Usage: $0 <project> [-- <args for test binary>]"
    exit 1
fi
shift

TEST_DIR="test/${PROJECT}"
ENV_PATH="src/${PROJECT}/.env"

if [ ! -d "$TEST_DIR" ]; then
    echo "No tests found for project '${PROJECT}'"
    exit 1
fi

DEFAULT_DEFINES="-D_POSIX_C_SOURCE=200809L -D_XOPEN_SOURCE=700 -D_GNU_SOURCE"
DEFAULT_CFLAGS="-Wall -Wextra -Werror -g -std=c23"
DEFAULT_LINKFLAGS=""
EXE_SUFFIX=""

case "$(uname -s)" in
    Linux*)
        DEFAULT_LINKFLAGS="-lGL -lX11 -lXext -lm"
        ;;
    Darwin*)
        DEFAULT_LINKFLAGS="-framework OpenGL -framework Cocoa"
        ;;
    CYGWIN*|MINGW*|MSYS*)
        EXE_SUFFIX=".exe"
        DEFAULT_LINKFLAGS="-lopengl32 -lgdi32 -luser32"
        ;;
    *) echo "Unsupported OS"; exit 1 ;;
esac

: "${C_COMPILER:=clang}"

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

: "${DEFINES:=$DEFAULT_DEFINES}"
: "${CFLAGS:=$DEFAULT_CFLAGS}"
: "${LINKFLAGS:=$DEFAULT_LINKFLAGS}"
: "${INCLUDE_DIRS:=}"
: "${TEST_EXE_NAME:=test_${PROJECT}${EXE_SUFFIX}}"

if ! command -v "$C_COMPILER" >/dev/null 2>&1; then
    echo "Error: ${C_COMPILER} is not installed."
    exit 1
fi

SRC_FILES=$(find "$TEST_DIR" -type f -name "*.c" 2>/dev/null)
if [ -z "$SRC_FILES" ]; then
    echo "No C test sources found in ${TEST_DIR}"
    exit 1
fi

OUTPUT_DIR="_bin"
mkdir -p "$OUTPUT_DIR"
TEST_BIN="${OUTPUT_DIR}/${TEST_EXE_NAME}"

COMPILE_FLAGS="$CFLAGS $DEFINES -Iinclude $INCLUDE_DIRS"

echo "Compiling tests for '${PROJECT}'..."
if ! $C_COMPILER $COMPILE_FLAGS -o "$TEST_BIN" $SRC_FILES $LINKFLAGS; then
    echo "Compilation failed!"
    exit 1
fi

echo "Compilation successful!"
echo ""
echo "Running tests..."
echo ""
"$TEST_BIN" "$@"
