require "nanovg"
local vg = require "vg"
local ui = require "ui"

eventFunction("init",function()
	ui.loadThemes("normal","themes/default")

	local root = ui.rootWidget()
	local w,h = root:getSize()
	
	local seekbar = ui.createWidget("normal","seekbar")
	seekbar:setPosition(100,100)
	seekbar:setRange(0,100,25)
	seekbar.onSeeking=function(pos)
		print("seek : "..pos)
	end
	root:addChild(seekbar)
end)

eventFunction("window",function(id,arg1,arg2)
	print( "id : "..tostring(id))
	if arg1 then
		print( "	arg1 : "..tostring(arg1))
		print( "	arg2 : "..tostring(arg2))
	end
end)

eventFunction("input",function(event)
	print("input event : "..tostring(event.type))
end)
---[[
eventFunction("loop",function(dt)
end)
--]]