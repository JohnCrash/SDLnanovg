local vg = require "vg"

return {
	name = 'default',
	version = 1,
	color = vg.rgba(255,255,255,255),
	colorBG = vg.rgba(0,128,168,255),
	
	onInit=function()
		--vg.createFont("default","fonts/Roboto-Regular.ttf")
		vg.createFont("default","C:/Windows/Fonts/simfang.ttf")
		vg.createFont("default-bold","fonts/Roboto-Bold.ttf")	
	end,
	window = require "themes/window",
	button = require "themes/button",
	label = require "themes/label",
	image = require "themes/image",
	switch = require "themes/switch",
	progress = require "themes/progress",
	seekbar = require "themes/seekbar",
	edit = require "themes/edit",
	scroll = require "themes/scroll",
	inner = require "themes/inner",
	--[[
	tab = require "themes/tab",
	list = require "themes/list",
	]]
}