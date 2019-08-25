#!/bin/bash
set -eo pipefail

source "$( dirname "${BASH_SOURCE[0]}" )/_env.sh"
if [[ -z "$JOBS" ]]; then
	JOBS="$(nproc --all)"
fi

mkdir -p "$BUILD_DIR"
mkdir -p "$OUT_DIR"

cd "$BUILD_DIR"
source "$DEVKITPRO/switchvars.sh"
DEFINES="-g -D__SWITCH__ -DHAVE_LIBNX -DHAVE_EGL"
INCLUDES="-I$DEVKITPRO/libnx/include -I$DEVKITPRO/portlibs/switch/include/SDL2 -I$DEVKITPRO/portlibs/switch/include"

cmake -G"Unix Makefiles" "$ROOT_DIR" \
	-DCMAKE_BUILD_TYPE=Debug \
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
	-DWITH_SDL=1 -DWITH_GLES2=0 -DWITH_FFMPEG=0 -DWITH_CRASH_HANDLER=0 -DWITH_SSE2=0

make nxshim -j$JOBS
make StepMania -j$JOBS

"$DEVKITPRO/tools/bin/nacptool" --create "$APP_TITLE" "$APP_AUTHOR" "$APP_VERSION" "$OUT_DIR/stepmania.nacp"
cp -vf "$STEPMANIA_DIR/stepmania-debug" "$OUT_DIR/stepmania.elf"
"$DEVKITPRO/tools/bin/elf2nro" "$OUT_DIR/stepmania.elf" "$OUT_DIR/stepmania.nro" \
	--nacp="$OUT_DIR/stepmania.nacp"
#	--icon="$ROOT_DIR/icon.jpg