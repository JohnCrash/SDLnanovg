--[[
测试窗口
--]]
local ui = require "ui"
local vg = require "vg"

eventFunction("init",function()
	ui.loadThemes("normal","themes/default")

	local root = ui.rootWidget()
	local w,h = root:getSize()
	
	local title = ui.createWidget("normal","label")
	title:setString("Scroll view")
	root:addChild(title)
	w,h = title:getSize()
	
	local scroll = ui.createWidget("normal","scroll")
	scroll:setPosition(0,h)
	scroll:setSize(480,320)
	--scroll:setRotate(vg.degToRad(-10),0,0)
	root:addChild(scroll)
	
	scroll:configScroll(ui.HORIZONTAL,12,0,true)
	for i=1,24 do
		local widget = ui.createWidget("normal","label")
		widget:setString("Label "..i)
		scroll:addChild(widget)
	end
	
	scroll:relayout()
end)