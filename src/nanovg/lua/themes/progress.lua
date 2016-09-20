local vg = require "vg"
local ui = require "ui"

return {
	_round = 5,
	onInit=function(self,themes)
		self:setSize(80,20)
		self._color = themes.color
		self._colorBG = themes.colorBG
		self._progress = 0
	end,
	onRelease=function(self)
	end,	
	onDraw=function(self,dt)
		local w,h = self:getSize()
		local d = h/10
		local r = self._round
		local p = w*self._progress-2*d
		
		vg.beginPath()
		vg.roundedRect(0,0,w,h,r)
		vg.strokeColor(self._color)
		vg.stroke()
		
		if p > 0 then
			vg.beginPath()
			vg.roundedRect(d,d,p,h-2*d,r)
			vg.fillColor(self._colorBG)
			vg.fill()
		end
	end,
	onEvent=function(self,event)
	end,
	getProgress=function()
		return self._progress
	end,
	setProgress=function(p)
		self._progress = p
	end,
}
