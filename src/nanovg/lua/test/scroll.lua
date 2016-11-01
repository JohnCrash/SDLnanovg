--[[
测试窗口
--]]
local ui = require "ui"

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
	scroll:setSize(250,320)
	root:addChild(scroll)
	for i=1,32 do
		local widget = ui.createWidget("normal","label")
		widget:setString("Label "..i)
		scroll:addWidget(widget)
	end
	scroll:relayout()
end)