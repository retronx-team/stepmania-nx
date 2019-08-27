#!/bin/bash
set -eo pipefail

source "$( dirname "${BASH_SOURCE[0]}" )/_env.sh"
if [[ -z "$JOBS" ]]; then
	JOBS="$(nproc --all)"
fi

if [[ "$1" == "debug" ]]; then
	BUILD_DIR="$BUILD_DIR/debug"
	CMAKE_BUILD_TYPE="Debug"
	PROGRAM_SUFFIX="-debug"
	WITH_FULL_RELEASE="0"
else
	BUILD_DIR="$BUILD_DIR/release"
	CMAKE_BUILD_TYPE="Release"
	PROGRAM_SUFFIX=""
	WITH_FULL_RELEASE="1"
fi

mkdir -p "$BUILD_DIR"
mkdir -p "$OUT_DIR"

cd "$BUILD_DIR"
source "$DEVKITPRO/switchvars.sh"
DEFINES="-g -D__SWITCH__ -DHAVE_LIBNX -DHAVE_EGL"
INCLUDES="-I$DEVKITPRO/libnx/include -I$DEVKITPRO/portlibs/switch/include/SDL2"

echo "* CMake"
cmake -G"Unix Makefiles" "$ROOT_DIR" \
	-DCMAKE_BUILD_TYPE="$CMAKE_BUILD_TYPE" \
	-DCMAKE_TOOLCHAIN_FILE="$DEVKITPRO/switch.cmake" \
	-DCMAKE_C_FLAGS="$CFLAGS $CPPFLAGS $DEFINES $INCLUDES" \
	-DCMAKE_CXX_FLAGS="$CFLAGS $DEFINES $INCLUDES" \
	-DCMAKE_AR="$DEVKITPRO/devkitA64/bin/aarch64-none-elf-gcc-ar" \
	-DCMAKE_SIZEOF_VOID_P=8 \
	-DWITH_XINERAMA=0 \
	-DHAVE_BIGENDIAN=0 \
	-DWITH_STATIC_LINKING=1 -DWITH_NETWORKING=0 \
	-DWITH_SYSTEM_MAD=1 -DWITH_SYSTEM_OGG=1 \
	-DWITH_SYSTEM_JPEG=1 -DWITH_SYSTEM_ZLIB=1 -DWITH_SYSTEM_PNG=1 \
	-DWITH_FFMPEG=1 -DWITH_SYSTEM_FFMPEG=1 -DHAS_FFMPEG=1  \
	-DWITH_SDL=1 -DWITH_GLES2=0 -DWITH_CRASH_HANDLER=0 -DWITH_SSE2=0

sed -i "s/-git-/-$APP_VERSION_TAG-/g" "$ROOT_DIR/stepmania/src/generated/verstub.cpp"

echo "* Make"
make nxshim -j$JOBS
make StepMania -j$JOBS

echo "* Building NRO"
APP_VERSION="$(grep -oP '(?<=extern char const \* const product_version = ")[^"]+(?=";)' "$ROOT_DIR/stepmania/src/generated/verstub.cpp")"

"$DEVKITPRO/tools/bin/nacptool" --create "$APP_TITLE$PROGRAM_SUFFIX" "$APP_AUTHOR" "$APP_VERSION" "$OUT_DIR/stepmania$PROGRAM_SUFFIX.nacp"
cp -f "$STEPMANIA_DIR/stepmania$PROGRAM_SUFFIX" "$OUT_DIR/stepmania$PROGRAM_SUFFIX.elf"
"$DEVKITPRO/tools/bin/elf2nro" "$OUT_DIR/stepmania$PROGRAM_SUFFIX.elf" "$OUT_DIR/stepmania$PROGRAM_SUFFIX.nro" \
	--nacp="$OUT_DIR/stepmania$PROGRAM_SUFFIX.nacp" \
	--icon="$ROOT_DIR/icon.jpg"
