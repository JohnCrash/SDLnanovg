require "nanovg"
local vg = require "vg"
local ui = require "ui"

--eventFunction("init",function()
	ui.loadThemes("normal","normal")

	local root = ui.rootWidget()
	local w,h = root:getSize()
	print( string.format("%d %d",w,h) )
	local quit = ui.createWidget("normal","button")
	root:addChild(quit)
	quit:setTitle("title")
	quit:setPosition(200,300)
	quit:setSize(100,120)
	local prev = quit.onRelease
	quit.onRelease=function(self)
		print("quit onRelease")
		if prev then
			prev(self)
		else
			print("prev = nil")
		end
	end
--end)