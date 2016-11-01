local vg = require "vg"
local ui = require "ui"

return {
	onInit=function(self,themes)
		print("init inner")
	end,
	onRelease=function(self)
		print("release inner")
	end,	
	onDraw=function(self)
	end,
	onEvent=function(self,event)
		print("event inner")
	end,
}