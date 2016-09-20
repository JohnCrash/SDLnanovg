require "nanovg"
local vg = require "vg"
local ui = require "ui"

eventFunction("init",function()
	ui.loadThemes("normal","themes/default")

	local root = ui.rootWidget()
	local w,h = root:getSize()
	
	local switch = ui.createWidget("normal","switch")
	switch.onSwitch = function(on)
		print( "switch : "..tostring(on) )
	end
	root:addChild(switch)
end)

---[[
eventFunction("loop",function(dt)
end)
--]]