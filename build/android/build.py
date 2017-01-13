import os

ANDROID_SDK_HOME = os.environ['ANDROID_SDK_HOME']
JDK_HOME = os.environ['JDK_HOME'] #jdk1.7.0_79 success

def apkbuild(name):
	res = name+"_resc.zip"
	apkdir = "bin/"+res+".tmp"
	dos_apkdir = "bin\\"+res+".tmp"
	os.system("rm -fr bin/"+res+".tmp")
	if os.system("unzip -o bin/"+res+" -d "+apkdir) != 0:
		exit(1)
	#copy .dex
	if os.system("cp bin/classes.dex "+apkdir) != 0:
		exit(1)
	#copy assets , juncation directory need use xcopy
	if os.system("xcopy assets "+dos_apkdir+"\\assets /s /q /i" ) != 0:
		exit(1)
	#copy ndk libs
	if os.system("cp -r libs "+apkdir+"/lib") != 0:
		exit(1)
	#zip
	if os.system("zipapk.bat "+name) != 0:
		exit(1)

if __name__ == '__main__':
	#build .so
	if os.system("ndk-build -j4 NDK_MODULE_PATH=jni;../../../") != 0 :
		exit(1)
	print("NDK build done!")
	#build R.java
	if os.system(ANDROID_SDK_HOME+"/build-tools/android-4.4.2/aapt "+
	'package -f -M AndroidManifest.xml -I "'+ANDROID_SDK_HOME+
	'/platforms/android-16/android.jar" -S res -J gen -m') != 0 :
		exit(1)
	print("aapt build done!")
	#build .classes
	if os.system('"'+JDK_HOME+'/bin/javac" -encoding UTF-8 -bootclasspath '+
	ANDROID_SDK_HOME+"/platforms/android-16/android.jar "+
	"-d ./bin/classes "+
	"./src/org/libsdl/app/*.java "+
	"./src/org/libsdl/nanovg/*.java "+
	"./gen/org/libsdl/nanovg/*.java ") != 0 :
		exit(1)
	print("java build done!")
	#build .dex
	if os.system(ANDROID_SDK_HOME+"/build-tools/android-4.4.2/dx "+
	"--dex --output=./bin/classes.dex "+
	"./bin/classes") != 0 :
		exit(1)
	print("classes.dex done!")
	#build .apk
	if os.system(ANDROID_SDK_HOME+"/build-tools/android-4.4.2/aapt package -f "+
	'-M AndroidManifest.xml -I "'+ANDROID_SDK_HOME+
	'/platforms/android-16/android.jar" -S res -F bin/SDLnanovg_resc.zip') != 0 :
		exit(1)
	print("SDLnanovg_resc.zip done!")
#android sdk stop support apkbuilder.bat
#copy apkbuilder.bat to sdk tools directory
#	if os.system(ANDROID_SDK_HOME+"/tools/apkbuilder.bat ./bin/SDLnanovg_unsinger.apk "+
#	'-v -u -z ./bin/SDLnanovg_resc.zip '+
#	'-f ./bin/classes.dex '+
#	'-rf ./res '+
#	'-nf ./libs '+
#	'-rj ./libs') != 0 :
#		exit(1)
	apkbuild("SDLnanovg")
	print("apk done!")
	#sign apk
	if os.system('"'+JDK_HOME+'/bin/jarsigner" -verbose '+
	'-keystore android.keystore '+
	'-storepass android '+
	'-keypass android '+
	'./bin/SDLnanovg.apk androiddebugkey') != 0 :
		exit(1)
	print("sign done!")
	#install apk
	if os.system("adb install -r bin/SDLnanovg.apk") != 0 :
		exit(1)
	print("install done!")
	#run app
	if os.system("adb shell am start -S org.libsdl.nanovg/.nanovgActivity") != 0 :
		exit(1)	
	print("Done.")