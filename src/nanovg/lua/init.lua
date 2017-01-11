local vg = require "vg"
local ui = require "ui"

local function isReaded(file)
	local f = io.open(file,"rb")
	if f then
		print(tostring(file).." is readed!")
		f:close()
	else
		print(tostring(file).." is not readed!")
	end
end
isReaded('/storage/sdcard1/SDLnanovg/lua/init.lua')
isReaded('/storage/sdcard0/Downloads/luajit.zip')
isReaded('/storage/emulated/0/ljdata/share/kp.json')
require "test/themes"