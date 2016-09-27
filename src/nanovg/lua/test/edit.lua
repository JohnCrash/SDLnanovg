local vg = require "vg"
local ui = require "ui"

ui.loadThemes("normal","themes/default")

local root = ui.rootWidget()
local w,h = root:getSize()

local edit = ui.createWidget("normal","edit")
local edit2 = ui.createWidget("normal","edit")
edit:setSize(120,32)
edit2:setSize(220,32)
edit:setPosition(100,100)
edit2:setPosition(100,154)
edit._text = 'Hello'
root:addChild(edit)
root:addChild(edit2)
