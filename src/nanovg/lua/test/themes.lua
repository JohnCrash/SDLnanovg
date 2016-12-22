local ui = require "ui"
local vg = require "vg"

local themes = "normal"

local titleLayout
local scrollLayout

local function relayout()
	local w,h = screenSize()
	titleLayout:setPosition(0,0)
	titleLayout:setSize(w,32)
	titleLayout:relayout()
	scrollLayout:setPosition(0,32)
	scrollLayout:setSize(w,h-32)
	scrollLayout:relayout()
end

eventFunction("init",function()
	setWindowSize(540,960)
	
	ui.loadThemes(themes,"themes/default")

	local root = ui.rootWidget()
	local w,h = root:getSize()
	
	titleLayout = ui.createWidget(themes,"layout")
	local title = ui.createWidget(themes,"label")
	title:setString("设置")
	title:setAlign(ui.ALIGN_CENTER+ui.ALIGN_MIDDLE)
	root:addChild(titleLayout)
	titleLayout:addChild(title)
	
	scrollLayout = ui.createWidget(themes,"scroll")
	scrollLayout:configScroll(ui.VERTICAL,0,0,true)
	root:addChild(scrollLayout)
	
	
	for i=1,64 do
		local widget = ui.createWidget(themes,"label")
		widget:setString("Label "..i)
		scrollLayout:addWidget(widget)
	end
	
	relayout()
end)

eventFunction("window",function(eventName,p1,p2)
	if eventName=='sizeChanged' then
		relayout()
	end
end)