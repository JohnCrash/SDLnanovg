--!
--! \addtogroup switch switch widget
--! \brief switch widget界面组件，实现开关控件。
--! @{
--!
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
	--! \brief switch的事件处理函数
	--!	\note 通过设置onSwitch变量为一个函数，可以用来处理switch打开或关闭事件。
	--!		例如：switchObject.onSwitch=function(on)end
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
		return true
	end,
	--! \brief 打开或者关闭
	--! \param on 如果on是ture就打开，否则就关闭
	switch=function(self,on)
		self._on = on
		self._animationDelay = self._animationSpeed
	end,
	--! \brief 取得开关控件的状态
	--! \return 如果开关为开返回true，否则返回false
	state=function(self,on)
		return self._on
	end,
	--! \brief 设置对齐方式
	--! \param align 对齐方式，可以是下面的值
	--!		- ui.ALIGN_LEFT		左对齐
	--!		- ui.ALIGN_CENTER	中心对齐
	--!		- ui.ALIGN_RIGHT	右对齐
	--!		- ui.ALIGN_TOP		顶对齐
	--!		- ui.ALIGN_MIDDLE	竖向中心对齐
	--!		- ui.ALIGN_BOTTOM	低对齐	
	setAlign=function(align,offx,offy)
		self._align = align
		self._alignX = offx
		self._alignY = offy
	end,
	--! \brief 取得对齐方式
	getAlign=function()
		return self._align,self._alignX,self._alignY
	end,
}
--!
--! @}
--!