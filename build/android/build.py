import os

ANDROID_SDK_HOME = os.environ['ANDROID_SDK_HOME']
JDK_HOME = os.environ['JDK_HOME']

if __name__ == '__main__':
	#build .so
#	if os.system("ndk-build -j4 NDK_MODULE_PATH=jni;../../../") != 0 :
#		exit(1)
	#build R.java
#	if os.system(ANDROID_SDK_HOME+"/build-tools/android-4.4.2/aapt "+
#	'package -f -M AndroidManifest.xml -I "'+ANDROID_SDK_HOME+
#	'/platforms/android-16/android.jar" -S res -J gen -m') != 0 :
#		exit(1)
	#build .classes
#	if os.system('"'+JDK_HOME+'/bin/javac" -encoding UTF-8 -bootclasspath '+
#	ANDROID_SDK_HOME+"/platforms/android-16/android.jar "+
#	"-d ./bin/classes "+
#	"./src/org/libsdl/app/*.java "+
#	"./src/org/libsdl/nanovg/*.java "+
#	"./gen/org/libsdl/nanovg/*.java ") != 0 :
#		exit(1)
	#build .dex
#	if os.system(ANDROID_SDK_HOME+"/build-tools/android-4.4.2/dx "+
#	"--dex --output=./bin/classes.dex "+
#	"./bin/classes") != 0 :
#		exit(1)
	#build .apk
	if os.system(ANDROID_SDK_HOME+"/build-tools/android-4.4.2/aapt package -f "+
	'-M AndroidManifest.xml -I "'+ANDROID_SDK_HOME+
	'/platforms/android-16/android.jar" -S res -F bin/SDLnanovg_temp.apk') != 0 :
		exit(1)
	if os.system(ANDROID_SDK_HOME+"/tools/apkbuilder.bat bin/SDLnanovg_unsinger.apk "+
	'-v -u -z bin/SDLnanovg_temp.apk '+
	'-f bin/classes.dex '+
	'-rf ./src '+
	'-nf ./libs '+
	'-rj ./libs') != 0 :
		exit(1)
	#sign apk
	#install apk