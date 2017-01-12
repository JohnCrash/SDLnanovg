import os

def push(local,remote):
	return os.system('adb push '+local+' '+remote)
	
def pull(local,remote):
	return os.system('adb pull '+remote+' '+local)
	
def restart(intent):
	return os.system('adb shell am start -S '+intent)

def fordir(proot,dir,func,param):
	if os.path.isdir(proot) == False:
		print "[",proot,"] is not a dirpath"
	else:
		plist = os.listdir(proot+"/"+dir)
		for d in plist:
			child = proot+"/"+dir+"/"+d
			if True == os.path.isdir(child):
				fordir(proot,dir+"/"+d,func,param)
			else:
				func(proot,dir+"/"+d,param)

def push_func(root,file,remote):
	push(root+file,remote+file)
	
def push_directory(local,remote):
	fordir(local,"",push_func,remote)

cwd = os.path.dirname(os.path.realpath(__file__))

if __name__ == "__main__":
	push_directory(cwd+"/assets/lua","/sdcard/SDLnanovg/lua")
	restart("org.libsdl.nanovg/.nanovgActivity")
	os.system("adb logcat -s SDL SDL/APP nanovg")