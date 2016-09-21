local vg = require "vg"
local ui = require "ui"

return {
	_animationSpeed = 0.1,
	onInit=function(self,themes)
		self:enableEvent(ui.EVENT_TOUCHDOWN+ui.EVENT_TOUCHUP+ui.EVENT_TOUCHDROP)
		self:setSize(40,20)
		self._color = themes.color
		self._colorBG = themes.colorBG
		self._on = false
		self._switchRate = 0
		self._animationDelay = 0
	end,
	onRelease=function(self)
	end,	
	onDraw=function(self,dt)
		local w,h = self:getSize()
		local d = h/10
		local r = h/2-d
		vg.beginPath()
		vg.roundedRect(0,0,w,h,h/2)
		vg.strokeColor(self._color)
		vg.stroke()
		
		if self._animationDelay>0 then
			self._animationDelay = self._animationDelay-dt
			if self._animationDelay<0 then self._animationDelay = 0 end
			if self._on then
				self._switchRate = 1-self._animationDelay / self._animationSpeed
			else
				self._switchRate = self._animationDelay / self._animationSpeed			
			end
		end
		
		if self._switchRate ~= 0 then
			vg.beginPath()
			vg.roundedRect(d,d,(w-h)*self._switchRate+h-2*d,h-2*d,r)
			vg.fillColor(self._colorBG)
			vg.fill()
		end
		
		vg.beginPath()
		vg.circle(h/2 + self._switchRate*(w-h),h/2,r)
		vg.fillColor(self._color)
		vg.fill()
	end,
	onEvent=function(self,event)
		if event.type == ui.EVENT_TOUCHDOWN then
			self._down = true
		elseif event.type == ui.EVENT_TOUCHUP then
			self._down = false
			if event.inside then
				self:switch(not self._on)
				if self.onSwitch then self.onSwitch(self._on) end
			end
		elseif event.type == ui.EVENT_TOUCHDROP then
			if self._down then
				self._down = event.inside
			end
		end	
	end,
	switch=function(self,on)
		self._on = on
		self._animationDelay = self._animationSpeed
	end,
	state=function(self,on)
		return self._on
	end,
}
