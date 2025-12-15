#!/usr/bin/env bash

RELEASE_MODE=0
while getopts ":r" opt; do
    case "$opt" in
        r) RELEASE_MODE=1 ;;
        *) echo "Usage: $0 [-r] <project> [-- <args for executable>]"; exit 1 ;;
    esac
done
shift $((OPTIND - 1))

PROJECT="$1"
if [ -z "$PROJECT" ]; then
    echo "Usage: $0 [-r] <project> [-- <args for executable>]"
    exit 1
fi
shift

ENV_PATH="src/${PROJECT}/.env"
EXE_SUFFIX=""

case "$(uname -s)" in
    CYGWIN*|MINGW*|MSYS*) EXE_SUFFIX=".exe" ;;
    Linux*|Darwin*) ;;
    *) echo "Unsupported OS"; exit 1 ;;
esac

DEFAULT_EXE_NAME="${PROJECT}${EXE_SUFFIX}"

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

: "${EXE_NAME:=$DEFAULT_EXE_NAME}"

BUILD_ARGS=()
if [ "$RELEASE_MODE" -eq 1 ]; then
    BUILD_ARGS+=("-r")
fi

./build.sh "${BUILD_ARGS[@]}" "$PROJECT"
if [ $? -ne 0 ]; then
    echo "Build failed. Exiting."
    exit 1
fi

EXECUTABLE="_bin/${EXE_NAME}"
if [ ! -x "$EXECUTABLE" ]; then
    echo "Executable not found: ${EXECUTABLE}"
    exit 1
fi

"$EXECUTABLE" "$@"
