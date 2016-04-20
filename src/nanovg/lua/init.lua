require "nanovg"
local vg = require "vg"
local ui = require "ui"

--eventFunction("init",function()
	ui.loadThemes("normal","normal")

	local root = ui.rootWidget()
	local w,h = root:getSize()
	local window = ui.createWidget("normal","window")
	root:addChild(window)
	window:setSize(w/2,h/2)
	window:enableClip(true)
	local window2 = ui.createWidget("normal","window")
	root:addChild(window2)
	window2:setSize(w/2,h/2)	
	window2:setPosition(w/2,h/2)	
	window2:enableClip(true)
	print( string.format("%d %d",w,h) )
	local top = ui.createWidget("normal","button")
	window:addChild(top)
	top:setTitle("top")
	local ww,hh = window:getSize()
	top:setPosition(ww-100,hh-120)
	top:setSize(100,120)
	local bottom = ui.createWidget("normal","button")
	window2:addChild(bottom)
	bottom:setTitle("bottom")
	--bottom:setPosition(0,0)
	--bottom:setPosition(w-100,h-120)
	bottom:setSize(100,120)	
--end)

local t = 0
local angle = 0
local sx,sy = 1,1

local fps_count = 0
local fps_acc = 0
local fps = 0
local t2 = 0
---[[
eventFunction("loop",function(dt)
	if fps_count < 300 then
		fps_acc = fps_acc+dt
		fps_count = fps_count+1
	else
		fps = math.floor(fps_count/(fps_acc+dt))
		fps_count = 0
		fps_acc = 0
	end
	vg.beginNanoVG(w,h)
	vg.beginFrame(w,h,1)
	vg.fontSize(32)
	vg.fontFace("sans")
	vg.fillColor(vg.rgbaf(1,1,1,1))
	vg.text(32,32,tostring(fps))
	vg.endFrame()
	if t > 0.01 then
		--root:setRotate(vg.degToRad(angle),w/2,h/2)
		--root:setScale(sx,sy)
		top:setRotate(vg.degToRad(angle),50,60)
		bottom:setRotate(vg.degToRad(angle),50,60)
		sx = 2+math.sin(vg.degToRad(angle))
		sy = 2+math.sin(vg.degToRad(angle))
		angle = angle + 1
		t = 0
	else
		t = t + dt
	end
end)
--]]