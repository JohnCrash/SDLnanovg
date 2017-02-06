#/bin/sh
echo '---------------ndk-build---------------'
ndk-build -j4 NDK_MODULE_PATH=../../../:../deps/
echo '---------------ndk-build done---------------'
echo 'copy libsocket.so => libs/armeabi'
cp -f ../deps/luasocket/prebuilt/android/armeabi/libsocket.so libs/armeabi
echo 'copy libmime.so => libs/armeabi'
cp -f ../deps/luasocket/prebuilt/android/armeabi/libmime.so libs/armeabi