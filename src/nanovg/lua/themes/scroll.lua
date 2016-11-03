local vg = require "vg"
local ui = require "ui"

local k = 2000
local k2 = 1 --velocity 
local m = 1
return {
	onInit=function(self,themes)
		self._color = themes.color
		self._colorBG = themes.colorBG
		self:enableEvent(ui.EVENT_TOUCHDOWN+ui.EVENT_TOUCHUP+ui.EVENT_TOUCHDROP)
		self:enableClip(true)
		self._inner = ui.createWidget(themes.name,"inner")
		self:addChild(self._inner)
		self._mode = ui.VERTICAL
		self._spacex = 0
		self._spacey = 3
		self._lastDrapPt = {}
		self._lastDrapIdx = 1
	end,
	onRelease=function(self)
	end,
	onDraw=function(self,dt)
		--[[
		local w,h = self:getSize()
		vg.beginPath()
		vg.rect(0,0,w,h)
		vg.fillColor(self._colorBG)
		vg.fill()
		]]
		self:clacForce(dt)
	end,
	addDrapPt=function(self,x,y,t)
		self._lastDrapPt[self._lastDrapIdx] = {x=x,y=y,t=t}
		self._lastDrapIdx = self._lastDrapIdx + 1
		if self._lastDrapIdx > 10 then
			self._lastDrapIdx = 1
		end
	end,
	clearDrapPt=function(self)
		for i=1,10 do
			self._lastDrapPt[self._lastDrapIdx] = nil
		end
	end,
	calcDrapVelocity=function(self)
		local p,last_p,x,y,n
		local bidx = self._lastDrapIdx
		if bidx > 10 then
			bidx = 1
		end
		n = 0
		x = 0
		y = 0
		for i=1,10 do
			p = self._lastDrapPt[bidx]
			
			if last_p and p then
				local dt = p.t-last_p.t
				if dt~=0 then
					x = x + (p.x-last_p.x)/dt
					y = y + (p.y-last_p.y)/dt
					n = n + 1
				end
			end
			last_p = p
			bidx = bidx + 1
			if bidx > 10 then
				bidx = 1
			end			
		end
		if n > 0 then
			return {x=x/n,y=y/n}
		else
			return {x=0,y=0}
		end
	end,
	clacForce=function(self,dt)
		if not self._velocity then	return end
		local f = {x=0,y=0}
		local sw,sh = self:getSize()
		local w,h = self._inner:getSize()
		local x,y = self._inner:getPosition()
		local ox,oy
		ox = x
		oy = y
		if x < sw-w then
			f.x = g
		elseif x > 0 then
			f.x = -g
		end
		if y < sh-h then
			f.y = g
		elseif y > 0 then
			f.y = -g
		end
		f.x = f.x - k2*self._velocity.x
		f.y = f.y - k2*self._velocity.y

		if isand(self._mode,ui.HORIZONTAL) then
			x = x + dt*self._velocity.x
			self._velocity.x = self._velocity.x + f.x*dt/m
		else
			y = y + dt*self._velocity.y
			self._velocity.y = self._velocity.y + f.y*dt/m
			if oy < sw-w and y > sw-w then
				y = sw-w
				self._velocity = nil
			elseif oy > 0 and y < 0 then
				y = 0
				self._velocity = nil
			end
		end
		self._inner:setPosition(x,y)
	end,
	onEvent=function(self,event)
		if event.type == ui.EVENT_TOUCHDOWN then
			self._down = true
			self._downx = event.x
			self._downy = event.y
			self:clearDrapPt()
		elseif event.type == ui.EVENT_TOUCHUP then
			self._down = false
			self._velocity = self:calcDrapVelocity()
		elseif event.type == ui.EVENT_TOUCHDROP then
			if self._down then
				local sw,sh = self:getSize()
				local w,h = self._inner:getSize()
				local x,y = self._inner:getPosition()
				if isand(self._mode,ui.HORIZONTAL) then
					x = x + event.x - self._downx
					self._downx = event.x
				else
					y = y + event.y - self._downy
					self._downy = event.y			
				end
				self._inner:setPosition(x,y)
				self:addDrapPt(event.x,event.y,event.time)
			end
		end
		return true
	end,
	configScroll=function(self,mode,sx,sy)
		self._mode = mode
		self._spacex = sx
		self._spacey = sy
	end,
	addWidget=function(self,widget)
		self._inner:addChild(widget)
	end,
	relayout=function(self)
		self._inner:relayout(self._mode,self._spacex,self._spacey)
	end,
}