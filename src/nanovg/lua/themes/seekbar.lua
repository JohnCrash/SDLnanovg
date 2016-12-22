--!
--! \addtogroup seekbar seekbar widget
--! \brief seekbar widget界面组件，实现滑动数值调节控件。
--! @{
--!
local vg = require "vg"
local ui = require "ui"

return {
	_lineWidth = 1,
	_radius = 7,
	--! \brief 控件初始化
	onInit=function(self,themes)
		self:setSize(160,20)
		self._color = themes.color
		self._colorBG = themes.colorBG
		self._pos = 0.5
		self._r0 = 0
		self._r1 = 1
		self:enableEvent(ui.EVENT_TOUCHDOWN+ui.EVENT_TOUCHUP+ui.EVENT_TOUCHDROP)
	end,
	--! \brief 释放控件
	onRelease=function(self)
	end,
	--! \brief 控件绘制
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
	--! \brief 控件事件处理
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
				local pos = self:getSeekPos()
				if self.onSeeking then
					self.onSeeking(pos)
				end
				if self._r0 and self._r1 and self._r1-self._r0>0 then
					self._pos = pos/(self._r1-self._r0)
				end
			end
		end
		return true
	end,
	--! \brief 设置滑块的范围
	--! \param r0,r1	最小值和最大值
	--! \param step		最小的变化值
	setRange=function(self,r0,r1,step)
		self._r0 = r0
		self._r1 = r1
		self._n = step
	end,
	--! \brief 取得当前的滑块位置或者值
	--! \return 返回滑块的值
	getSeekPos=function(self)
		if self._r0 and self._r1 then
			if self._n and self._n~=0 then
				return self._r0 + math.floor(((self._r1-self._r0)*self._pos)/self._n)*self._n
			else
				return self._r0 + (self._r1-self._r0)*self._pos
			end
		else
			return self._pos
		end
	end,
	--! \brief 设置滑块的值
	--! \param pos	要设置的值
	setSeekPos=function(self,pos)
		if self._r0 and self._r1 then
			if self._n and self._n~=0 then
				--FIXME _pos是一个可以被_n整除的数
				self._pos = math.floor((pos-self._r0)/(self._r1-self._r0))
			else
				self._pos = (pos-self._r0)/(self._r1-self._r0)
			end
		else
			self._pos = pos
		end
	end,
	--! \brief 设置对齐方式
	--! \param align 对齐方式，可以是下面的值
	--!		- ui.ALIGN_LEFT		左对齐
	--!		- ui.ALIGN_CENTER	中心对齐
	--!		- ui.ALIGN_RIGHT	右对齐
	--!		- ui.ALIGN_TOP		顶对齐
	--!		- ui.ALIGN_MIDDLE	竖向中心对齐
	--!		- ui.ALIGN_BOTTOM	低对齐	
	setAlign=function(self,align,offx,offy)
		self._align = align
		self._alignX = offx
		self._alignY = offy
	end,
	--! \brief 取得对齐方式
	getAlign=function(self)
		return self._align,self._alignX,self._alignY
	end,
		--! \brief 设置尺寸的匹配方式，当widget放入到layout中的时候，layout在需要重新布局的时候，可以控制widget的尺寸。
	--! \param fit 设置一个对象的尺寸模式，用于layout的relaout函数。可以是下面的值。
	--! 	- ui.WRAP_CONTENT		固定根据内容确定尺寸
	--! 	- ui.WIDTH_WRAP_CONTENT	宽度根据内容确定尺寸
	--! 	- ui.HEIGHT_WRAP_CONTENT	高度根据内容确定尺寸
	--! 	- ui.WIDTH_MATCH_PARENT		宽度匹配父窗口的宽度
	--! 	- ui.HEIGHT_MATCH_PARENT	高度匹配父窗口的高度
	--! 	- ui.FILL_PARENT			宽度和高度都匹配父窗口的尺寸
	--! \param cw,ch 如果使用MATCH模式时作为修正，比如说cw=10,ch=10,那么在匹配父窗口的尺寸时减去10
	setLayoutMatch=function(self,fit,cw,ch)
		self._match = fit
		self._matchX = cw
		self._matchY = ch
	end,
	--! \brief 取得匹配方式
	getLayoutMatch=function(self)
		return self._match,self._matchX,self._matchY
	end,
}
--!
--! @}
--!