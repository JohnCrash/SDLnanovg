--[[
测试窗口
--]]
local ui = require "ui"

local function messagebox(str,angle)
	local window = ui.createWidget("normal","window")
	local label = ui.createWidget("normal","label")
	local button = ui.createWidget("normal","button")
	
	label:setString(str)
	button:setTitle("OK")
	button:setSize(60,30)
	window:addChild(label)
	window:addChild(button)
	
	window:setPosition(120,100)
	window:setSize(240,90)
	local w,h = ui.linearRelayout(
		ui.rect(0,30,240,60),
		{label,button},
		ui.ALIGN_V+ui.ALIGN_CENTER+ui.ALIGN_MIDDLE,0)
	window:setSize(w+48,h+30)
	ui.linearRelayout(
		ui.rect(0,30,w+48,h),
		{label,button},
		ui.ALIGN_V+ui.ALIGN_CENTER+ui.ALIGN_MIDDLE,0)
	window:setRotate(vg.degToRad(angle or 30),(w+48)/2,(h+30)/2)
	return window
end

local function six()
	local x,y,r
	r = 200
	local du = 60
	for i=1,360/du do
		local box = messagebox("Hello world! \nhttp://www.nanovg.org"..i,i*du)
		x = r*math.sin(vg.degToRad(-i*du))+w/2
		y = r*math.cos(vg.degToRad(-i*du))+h/2
		box:setPosition(x,y)
		root:addChild(box)
	end
end
