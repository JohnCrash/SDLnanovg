#/bin/sh
ndk-build -j4 NDK_MODULE_PATH=jni:../../../:../../build/deps/
echo 'copy libsocket.so => libs/armeabi'
cp -f ../deps/luasocket/prebuilt/android/armeabi/libsocket.so libs/armeabi
echo 'copy libmime.so => libs/armeabi'
cp -f ../deps/luasocket/prebuilt/android/armeabi/libmime.so libs/armeabi