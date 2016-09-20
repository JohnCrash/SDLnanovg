local vg = require "vg"

return {
	onInit=function()
		vg.createFont("default","fonts/Roboto-Regular.ttf")
		vg.createFont("default-bold","fonts/Roboto-Bold.ttf")	
	end,
	window = require "themes/window",
	button = require "themes/button",
	label = require "themes/label",
	image = require "themes/image",
	switch = require "themes/switch",
	--[[
	progress = require "themes/progress",
	seekbar = require "themes/seekbar",
	switch = require "themes/switch",
	tab = require "themes/tab",
	list = require "themes/list",
	]]
}