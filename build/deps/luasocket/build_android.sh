#/bin/sh
#编译需要Cygwin32.
NDK=C:/android-ndk-r9d
NDKABI=14
NDKVER=$NDK/toolchains/arm-linux-androideabi-4.8
NDKP=$NDKVER/prebuilt/windows/bin/arm-linux-androideabi-
NDKF="--sysroot $NDK/platforms/android-$NDKABI/arch-arm"
CFLAGS=" -O3 -marm -mcpu=cortex-a8 -mfloat-abi=softfp"

make PLAT=android LUAPREFIX_android='../../luajit/src' \
	LUALIB_directory='-L../../luajit/prebuilt/android/dynamic/armeabi/' \
	CROSS=$NDKP \
	TARGET_FLAGS="$NDKF" TARGET_CFLAGS="$CFLAGS"
