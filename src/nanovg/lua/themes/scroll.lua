local vg = require "vg"
local ui = require "ui"

local SLIDE = 1 --滑动
local SLOWDOWN = 2 --缓存减速
local REBOUND = 3 --回弹
local REBOUND2 = 4 --回弹
local MOTIONLESS = 0 --静止

local SLIDETIME = 0.5 --滑动时间
local SLOWDOWNTIME = 0.5 --停止时间
local REBOUNDTIME = 0.5 --回弹时间
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
		self._status = MOTIONLESS
		self._tick = 0
	end,
	onRelease=function(self)
	end,
	calcRebound=function(self,x,y,w,h,sw,sh)
		if y <= 0 or y + h >= sh then
			self._status = REBOUND
			self._tick = REBOUNDTIME
		end
	end,
	onDraw=function(self,dt)
		if self._status == MOTIONLESS then
			return
		end
		local sw,sh = self:getSize()
		local w,h = self._inner:getSize()
		local x,y = self._inner:getPosition()
		
		if self._status == SLIDE then
			self._tick = self._tick - dt
			if self._tick < 0 then
				self._status = SLOWDOWN
				self._tick = SLOWDOWNTIME
				self._slowdownVelocity = {x=self._velocity.x,y=self._velocity.y}
			end
			self:calcRebound()		
		elseif self._status == SLOWDOWN then
			self._tick = self._tick - dt
			if self._tick > 0 then
				self._velocity.x = self._slowdownVelocity.x * self._tick/SLOWDOWNTIME
				self._velocity.y = self._slowdownVelocity.y * self._tick/SLOWDOWNTIME
			else
				self._status = MOTIONLESS
				self._velocity.x = 0
				self._velocity.y = 0
			end
			self:calcRebound()				
		elseif self._status == REBOUND then
		
		end
		
		--x = x + self._velocity.x * dt
		y = y + self._velocity.y * dt
		self._inner:setPosition(x,y)
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
			self._lastDrapPt[i] = nil
		end
		self._lastDrapIdx = 1
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
			n = n * 2
			return {x=x/n,y=y/n}
		else
			return {x=0,y=0}
		end
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
			self._tick = SLIDETIME
			self._status = SLIDE
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