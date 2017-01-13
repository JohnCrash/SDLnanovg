android下的编译方法
执行build.bat,注意看build.bat ndk-build是写死的。
在jni\SDL下使用junction.exe创建链接。
	junction src ../../../../src
	junction include ../../../../include

build.sh在cygwin下不工作

build.py 使用纯命令行来实现打包，在打包部分apkbuilder.bat脚本没有走通。
		(https://android.googlesource.com/platform/sdk/+/e162064a7b5db1eecec34271bc7e2a4296181ea6/sdkmanager/libs/sdklib/src/com/android/sdklib/build/ApkBuilderMain.java
		https://groups.google.com/forum/#!topic/android-developers/sls2uFYWuWE)
	使用zip简单的打包也可以
	