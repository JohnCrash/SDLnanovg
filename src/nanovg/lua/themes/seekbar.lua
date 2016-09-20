local vg = require "vg"
local ui = require "ui"

return {
	_lineWidth = 1,
	_radius = 7,
	onInit=function(self)
		self:setSize(160,20)
		self._color = vg.rgba(255,255,255,255)
		self._colorBG = vg.rgba(0,128,168,255)
		self._pos = 0.5
		self:enableEvent(ui.EVENT_TOUCHDOWN+ui.EVENT_TOUCHUP+ui.EVENT_TOUCHDROP)
	end,
	onRelease=function(self)
	end,	
	onDraw=function(self,dt)
		local w,h = self:getSize()
		local y = h/2-self._lineWidth/2
		
		if self._pos > 0 then
			vg.beginPath()
			vg.rect(0,y,self._pos*w,self._lineWidth)
			vg.fillColor(self._colorBG)
			vg.fill()
		end
		if self._pos < 1 then
			vg.beginPath()
			vg.rect(self._pos*w,y,(1-self._pos)*w,self._lineWidth)
			vg.fillColor(self._color)		
			vg.fill()
		end
		
		vg.beginPath()
		vg.circle((w-2*self._radius)*self._pos+self._radius,y,self._radius)
		vg.fillColor(self._color)
		vg.fill()
	end,
	onEvent=function(self,event)
		local w,h = self:getSize()
		local p = (w-2*self._radius)*self._pos+self._radius
		if event.type == ui.EVENT_TOUCHDOWN then
			if p-self._radius < event.x and p+self._radius > event.x then
				self._down = true
			end
		elseif event.type == ui.EVENT_TOUCHUP then
			self._down = false
		elseif event.type == ui.EVENT_TOUCHDROP then
			if self._down then
				self._pos = event.x/w
				if self._pos > 1 then
					self._pos = 1
				elseif self._pos < 0 then
					self._pos = 0
				end
			end
		end	
	end,
	setSeekRang=function(self,r0,r1,n)
	end,
}
