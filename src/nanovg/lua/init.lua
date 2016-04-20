require "nanovg"
local vg = require "vg"
local ui = require "ui"

--eventFunction("init",function()
	ui.loadThemes("normal","normal")

	local root = ui.rootWidget()
	local w,h = root:getSize()
	print( string.format("%d %d",w,h) )
	local top = ui.createWidget("normal","button")
	root:addChild(top)
	top:setTitle("top")
	top:setPosition(0,0)
	top:setSize(100,120)
	local bottom = ui.createWidget("normal","button")
	root:addChild(bottom)
	bottom:setTitle("bottom")
	bottom:setPosition(w-100,h-120)
	bottom:setSize(100,120)	
--end)