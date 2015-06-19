android下的编译方法
执行build.bat,注意看build.bat ndk-build是写死的。
在jni\SDL下使用junction.exe创建链接。
	junction src ../../../../src
	junction include ../../../../include

build.sh在cygwin下不工作