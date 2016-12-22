import os
import sys
import string

def lua2doxyline(line):
	if string.find(line,'--!')!=-1:
		sys.stdout.write(string.replace(line,'--!','///'))
	elif string.find(line,'function')!=-1:
		if string.find(line,'=function')!=-1:
			sys.stdout.write('function '+string.replace(line,'=function',''))
		elif string.find(line,'= function')!=-1:
			sys.stdout.write('function '+string.replace(line,'= function',''))
		else:
			pass
		sys.stdout.write(';\n')
		
def lua2doxy(filename):
	lua = open(filename,'rb')
	for line in lua.readlines():
		lua2doxyline(line)
	lua.close()
	
if __name__ == "__main__":
	if(len(sys.argv)>0):
		lua2doxy(sys.argv[1])