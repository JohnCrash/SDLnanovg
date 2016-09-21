require "nanovg"
local vg = require "vg"
local ui = require "ui"

eventFunction("init",function()
	ui.loadThemes("normal","themes/default")

	local root = ui.rootWidget()
	local w,h = root:getSize()
	
	local seekbar = ui.createWidget("normal","seekbar")
	seekbar:setRange(0,100,1)
	seekbar.onSeeking = function(pos)
		print("seek : "..pos)
	end
	seekbar:setPosition(100,100)
	root:addChild(seekbar)
end)

---[[
eventFunction("loop",function(dt)
end)
--]]