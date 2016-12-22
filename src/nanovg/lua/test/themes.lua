local ui = require "ui"
local vg = require "vg"

local themes = "normal"

local baseLayout

local function relayout()
	local w,h = screenSize()
	baseLayout:setSize(w,h)
	baseLayout:relayout()
end

eventFunction("init",function()
	setWindowSize(540,960)
	
	ui.loadThemes(themes,"themes/default")

	local root = ui.rootWidget()
	local w,h = root:getSize()

	baseLayout = ui.createWidget(themes,"layout")
	
	local titleLayout = ui.createWidget(themes,"layout")
	local title = ui.createWidget(themes,"label")
	title:setString("设置")
	title:setAlign(ui.ALIGN_CENTER+ui.ALIGN_MIDDLE)
	titleLayout:addChild(title)
	titleLayout:setAlign(ui.ALIGN_LEFT+ui.ALIGN_TOP)
	titleLayout:setLayoutMatch(ui.WIDTH_MATCH_PARENT)
	titleLayout:setSize(w,32)
	
	baseLayout:addChild(titleLayout)
	
	local scrollLayout = ui.createWidget(themes,"scroll")
	scrollLayout:configScroll(ui.VERTICAL+ui.ALIGN_LEFT,0,0,true)
	scrollLayout:setAlign(ui.ALIGN_LEFT+ui.ALIGN_BOTTOM)
	scrollLayout:setLayoutMatch(ui.FILL_PARENT,0,32)
	baseLayout:addChild(scrollLayout)
	
	local search = ui.createWidget(themes,"edit")
	search:setLayoutMatch(ui.WIDTH_MATCH_PARENT)
	search:setSize(w,28)
	scrollLayout:addChild(search)
	
	for i=1,64 do
		local widget = ui.createWidget(themes,"label")
		widget:setString("Label "..i)
		scrollLayout:addChild(widget)
	end
	
	root:addChild(baseLayout)
	scrollLayout:relayout()
	relayout()
end)

eventFunction("window",function(eventName,p1,p2)
	if eventName=='sizeChanged' then
		relayout()
	end
end)