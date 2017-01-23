#/bin/sh
ndk-build -j4 NDK_MODULE_PATH=jni:../../../:../../build/deps/
cp -f ../deps/luasocket/prebuilt/android/armeabi/libsocket.so libs/armeabi
cp -f ../deps/luasocket/prebuilt/android/armeabi/libmime.so libs/armeabi