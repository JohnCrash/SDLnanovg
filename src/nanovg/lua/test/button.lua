local ui = require "ui"
local vg = require "vg"

ui.loadThemes("normal","themes/default")
setDeviceSize(540,960)
local dw,dh = getDeviceSize()
sw = 540
sh = dh*540/dw
ui.rootWidget():setSize(sw,sh)
	
local root = ui.rootWidget()
local w,h = root:getSize()

local button = ui.createWidget("normal","button")
local bg = ui.createWidget("normal","layout")
bg:setSize(w,h)
bg:setBGColor(vg.rgbaf(0.8,0.8,0.8,1))
root:addChild(bg)
button:setSize(64,64)
button:setPosition(100,100)
button:setTitle("确定")
bg:addChild(button)