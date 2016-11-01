local vg = require "vg"
local ui = require "ui"

return {
	onInit=function(self,themes)
		self:enableFlags(ui.SCROLL_CLIP)
	end,
	onRelease=function(self)
	end,	
}