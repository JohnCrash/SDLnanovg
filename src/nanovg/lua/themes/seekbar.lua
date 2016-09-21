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
		self._r0 = 0
		self._r1 = 1
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
				if self.onSeeking then
					self.onSeeking(self:getSeekPos())
				end
			end
		end	
	end,
	setRange=function(self,r0,r1,n)
		self._r0 = r0
		self._r1 = r1
		self._n = n
	end,
	getSeekPos=function(self)
		if self._r0 and self._r1 then
			if self._n then
				return self._r0 + (self._r1-self._r0)*self._pos
			else
				return self._r0 + (self._r1-self._r0)*self._pos
			end
		else
			return self._pos
		end
	end,
	setSeekPos=function(self,pos)
	end,
}
