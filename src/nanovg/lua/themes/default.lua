--!
--! \addtogroup themes themes table
--! \brief themes 表用来描述一组界面实现
--!
--!	在每个widget内部，可以通过widget的函数onInit(self,themes)来获得和widget相关的themes表。
--!	themes的名称可以通过themes.name来获得。这个名称就是ui.loadThemes的第一个参数，它就是
--!	themes的名称，在创建widget的时候需要用到这个名称。
--!
--! ## 默认的themes包括下面的widget
--!		- \ref window
--!		- \ref button
--!		- \ref label
--!		- \ref switch
--!		- \ref progress
--!		- \ref edit
--!		- \ref scroll
--!		- \ref layout
--! @{
--!
local vg = require "vg"
local lfs = require "lfs"

local path = 'E:/test_video/kubifonts'

local function sel(op,r1,r2)
	if op then 
		return r1
	else 
		return r2
	end
end

for file in lfs.dir(path) do
	local ext = string.sub(file,-3)
	if ext == 'ttf' or ext == 'ttc' then
		local result = fonsHasCodepoint(path..'/'..file,{'abc','中文'})
		if result then
			print( string.format('[%s%s] - %s',
				sel(result[1]==1,'x',' '),
				sel(result[2]==1,'x',' '),
				file) )
		end
	end
end

return {
	version = 1,
	color = vg.rgba(255,255,255,255),
	colorBG = vg.rgba(0,128,168,255),
	font = 'default',
	bold = 'default-bold',
	
	--! \brief 在样式表被加载时调用一次，具体来说就是ui.loadThemes被调用时
	--!		onInit函数用来初始化和themes相关的资源，例如公共图片，字体等
	onInit=function(self)
		print("font:"..tostring(self.font))
		if getPlatform() == 'windows' then
			vg.createFont("default","E:/test_video/hwp9fonts/DroidSansFallback.ttf")
			vg.createFont("default-bold","c:/windows/fonts/arialdb.ttf")
		else
			vg.createFont("default","/system/fonts/DroidSansChinese.ttf")
			vg.createFont("default-bold","/system/fonts/DroidSans-Bold.ttf")	
		end
	end,
	window = require "themes/window",
	button = require "themes/button",
	label = require "themes/label",
	image = require "themes/image",
	switch = require "themes/switch",
	progress = require "themes/progress",
	seekbar = require "themes/seekbar",
	edit = require "themes/edit",
	scroll = require "themes/scroll",
	inner = require "themes/inner",
	layout = require "themes/layout",
	vgw = require "themes/vgw",
	--[[
	tab = require "themes/tab",
	list = require "themes/list",
	]]
}

--!
--! @}
--!