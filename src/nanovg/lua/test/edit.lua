local vg = require "vg"
local ui = require "ui"

ui.loadThemes("normal","themes/default")

local root = ui.rootWidget()
local w,h = root:getSize()

local edit = ui.createWidget("normal","edit")
edit:setSize(120,26)
edit:setPosition(100,100)
root:addChild(edit)
