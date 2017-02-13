import os
import json

def push(local,remote):
	return os.system('adb push '+local+' '+remote)
	
def pull(local,remote):
	return os.system('adb pull '+remote+' '+local)
	
def restart(intent):
	return os.system('adb shell am start -S '+intent)

def fordir(proot,dir,func,param1,param2):
	fmd = 0
	if os.path.isdir(proot) == False:
		print "[",proot,"] is not a dirpath"
	else:
		plist = os.listdir(proot+"/"+dir)
		for d in plist:
			child = proot+"/"+dir+"/"+d
			if True == os.path.isdir(child):
				fmd = max(fmd,fordir(proot,dir+"/"+d,func,param1,param2))
			else:
				fmd = max(fmd,func(proot,dir+"/"+d,param1,param2))
	return fmd

def push_func(root,file,remote,prevdate):
	try:
		fmd = os.path.getmtime(root+file)
	#	print "getmtime : "+root+file+":"+str(fmd)
	except OSError as e:
		return 0
	if fmd > prevdate :
		push(root+file,remote+file)
		return fmd
	else:
		return 0
	
def push_directory(local,remote,prevdate):
	return fordir(local,"",push_func,remote,prevdate)

cwd = os.path.dirname(os.path.realpath(__file__))
update_file = cwd+"/.adbsyn"

def get_prev_update_time():
	try:
		update_date_file = open(update_file,"rb")
		update_date = json.loads(update_date_file.read())
		update = update_date["update"]
		update_date_file.close()
		return update
	except IOError:
		print "Can't read file " + update_file
		return 0

def set_prev_update_time(t):
	try:
		if t > 0 :
			update_date_file = open(update_file,'wb')
			update_date_file.write(json.dumps({"update":t}))
			update_date_file.close()
	except IOError:
		print "Can't write file " + update_file
		
if __name__ == "__main__":
	set_prev_update_time( push_directory(cwd+"/assets/lua","/sdcard/SDLnanovg/lua",get_prev_update_time()) )
	restart("org.libsdl.nanovg/.nanovgActivity")
	os.system("adb logcat -s SDL SDL/APP nanovg")