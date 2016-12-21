--!
--! \addtogroup scroll scroll widget
--! \brief scroll widget界面组件，实现横向或者纵向滚动控件。
--! @{
--!
local vg = require "vg"
local ui = require "ui"

local SLIDE = 1		--滑动
local SLOWDOWN = 2	--缓存减速
local REBOUND = 3	--回弹
local REBOUND2 = 4	--回弹
local MOTIONLESS = 0	--静止

local SLIDETIME = 0.5		--滑动时间
local SLOWDOWNTIME = 1		--停止时间
local REBOUNDTIME = 0.1		--回弹时间
local REBOUNDVELOCITY = 5	--回弹速度

return {
	--!	\brief scroll widget初始化
	--!	\param themes 引用样式的公共表，包括样式名称和字体等
	onInit=function(self,themes)
		self._color = themes.color
		self._colorBG = themes.colorBG
		self:enableEvent(ui.EVENT_TOUCHDOWN+ui.EVENT_TOUCHUP+ui.EVENT_TOUCHDROP)
		self:enableClip(true)
		self._inner = ui.createWidget(themes.name,"inner")
		self:addChild(self._inner)
		self._mode = ui.VERTICAL
		self._isrebound = true		--是否回弹默认打开
		self._spacex = 0
		self._spacey = 3
		self._status = MOTIONLESS
		self._tick = 0
		self._velocity = {x=0,y=0}
		self._st = require "themes/slidetrack".new()
		self._isHoriz = isand(self._mode,ui.HORIZONTAL)
	end,
	--! \brief 释放操作，当控件被从系统中删除的时候调用
	onRelease=function(self)
	end,
	--! \breif 内部函数，计算超出部分长度
	--! \param x,y		inner当前位置
	--! \param sw,sh	scroll widget尺寸
	--!	\param w,h		inner尺寸
	--!\return 第一返回值为超出部分长度，第二个部分是不超出应该设置的值
	calcOverbound=function(self,x,y,sw,sh,w,h)
		if self._isHoriz then
			if x > 0 then
				return -x,0
			elseif sw-x-w > 0 then
				return sw-x-w,sw-w
			end		
		else
			if y > 0 then
				return -y,0
			elseif sh-y-h > 0 then
				return sh-y-h,sh-h
			end
		end
		return 0,0
	end,
	--! \breif 内部函数，计算反弹速度
	--! \param o,op	这两个参数是calcOverbound的返回值
	--! \return 函数返回反弹速度，速度很低时isend=1
	calcReboundVelocity=function(self,o,op)
		if self._isHoriz then
			if o ~= 0 then
				if math.abs(o) < 1 then
					return {x=op,y=0,isend=1}
				end
				return {x=REBOUNDVELOCITY*o,y=0}
			end		
		else
			if o ~= 0 then
				if math.abs(o) < 1 then
					return {x=0,y=op,isend=1}
				end
				return {x=0,y=REBOUNDVELOCITY*o}
			end
		end
		return {x=0,y=0}
	end,
	--! \breif widget绘制函数
	--! \param dt 从上一次调用绘制到这次的时间差，单位时秒
	--! \return 函数不需要返回值
	onDraw=function(self,dt)
		if self._status == MOTIONLESS then
			return
		end
		local sw,sh = self:getSize()
		local w,h = self._inner:getSize()
		local x,y = self._inner:getPosition()
		local o,op = self:calcOverbound(x,y,sw,sh,w,h)
		
		if self._status == SLIDE or self._status == SLOWDOWN then
			if o ~= 0 then
				self._status = REBOUND
				self._tick = REBOUNDTIME
			end
		end
		
		if self._status == SLIDE then
			self._tick = self._tick - dt
			if self._tick < 0 then
				self._status = SLOWDOWN
				self._tick = SLOWDOWNTIME
				self._slowdownVelocity = {x=self._velocity.x,y=self._velocity.y}
			end
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
		elseif self._status == REBOUND then
			self._tick = self._tick - dt
			if self._tick < 0 then
				self._status = REBOUND2
				self._tick = REBOUNDTIME
				self._velocity.x = -self._velocity.x
				self._velocity.y = -self._velocity.y			
			end
		elseif self._status == REBOUND2 then
			self._velocity = self:calcReboundVelocity(o,op)
			if self._velocity.isend then
				self._status = MOTIONLESS
				self._inner:setPosition(self._velocity.x,self._velocity.y)
				self._velocity = {x=0,y=0}
				return
			end
		end
		
		if self._isHoriz then
			x = x + self._velocity.x * dt
		else
			y = y + self._velocity.y * dt
		end
		
		if not self._isrebound then
			o,op = self:calcOverbound(x,y,sw,sh,w,h)
			if o ~= 0 then
				self._status = MOTIONLESS
				if self._isHoriz then
					x = op
				else
					y = op
				end
			end
		end
		--print(string.format("STATUS : %d , (%d,%d)",self._status,x,y))
		self._inner:setPosition(x,y)
	end,
	--! \breif widget事件处理函数
	--! \param event 事件表
	--! \return 返回true表示事件处理了事件
	onEvent=function(self,event)
		self._st:track(event)
		if event.type == ui.EVENT_TOUCHDOWN then
			self._down = event
			self._status = MOTIONLESS
		elseif event.type == ui.EVENT_TOUCHUP then
			self._down = nil
			self._tick = SLIDETIME
			self._status = SLIDE
			self._velocity = self._st:velocity()
		elseif event.type == ui.EVENT_TOUCHDROP then
			if self._down then
				local sw,sh = self:getSize()
				local w,h = self._inner:getSize()
				local x,y = self._inner:getPosition()
				
				if self._isHoriz then
					x = x + (event.x - self._down.x)/2
				else
					y = y + (event.y - self._down.y)/2
				end
				self._down = event
				if not self._isrebound then
					local o,op = self:calcOverbound(x,y,sw,sh,w,h)
					if self._isHoriz then
						if o ~= 0 then
							x = op
						end									
					else
						if o ~= 0 then
							y = op
						end					
					end
				end
				self._inner:setPosition(x,y)
			end
		end
		return true
	end,
	--! \brief configScroll函数配置scroll控件的参数
	--! \param mode	指定是横向还是属相
	--! \param sx	scroll子对象的横向间隔
	--! \param sy	scroll子对象的竖向间隔
	--! \param isrebound	滚动到头部或者尾部是否回弹
	--! \return 无返回
	configScroll=function(self,mode,sx,sy,isrebound)
		self._mode = mode
		self._spacex = sx
		self._spacey = sy
		self._isrebound = isrebound
		self._isHoriz = isand(self._mode,ui.HORIZONTAL)
	end,
	--! \breif 向scroll widget中加入子widget
	--! \param widget 要加入的子widget
	addWidget=function(self,widget)
		self._inner:addChild(widget)
	end,
	--! \breif 布局子widget
	relayout=function(self)
		self._inner:relayout(self._mode,self._spacex,self._spacey)
	end,
}

--!
--! @}
--!