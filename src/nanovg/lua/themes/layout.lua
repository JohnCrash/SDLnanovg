--!
--! \addtogroup layout layout widget
--! \brief layout widget界面组件，实现布局控件。
--! @{
--!
local vg = require "vg"
local ui = require "ui"

local function relayoutChild(child,align,w,h)
	local cw,ch = child:getSize()
	local cx,cy = child:getPosition()
	if isand(align,ui.ALIGN_LEFT) then
		cx = child._alignX or 0
	elseif isand(align,ui.ALIGN_CENTER) then
		cx = (w-cw)/2+(child._alignX or 0)
	elseif isand(align,ui.ALIGN_RIGHT) then
		cx = w-cw+(child._alignX or 0)
	end
	if isand(align,ui.ALIGN_TOP) then
		cy = child._alignY or 0
	elseif isand(align,ui.ALIGN_MIDDLE) then
		cy = (h-ch)/2+(child._alignY or 0)
	elseif isand(align,ui.ALIGN_BOTTOM) then
		cy = h-ch+(child._alignY or 0)
	end	
	child:setPosition(cx,cy)
end

return {
	_animationSpeed = 0.1,
	onInit=function(self,themes)
		self._color = themes.color
		self._colorBG = themes.colorBG
	end,
	onRelease=function(self)
	end,	
	onDraw=function(self,dt)
		local w,h = self:getSize()

		vg.beginPath()
		vg.rect(0,0,w,h)
		vg.fillColor(self._colorBG)
		vg.fill()
	end,
	onEvent=function(self,event)
		if event.type == ui.EVENT_TOUCHDOWN then
			self._down = true
		elseif event.type == ui.EVENT_TOUCHUP then
			self._down = false
			if event.inside then
				if self.onClick then self.onClick() end
			end
		elseif event.type == ui.EVENT_TOUCHDROP then
			if self._down then
				self._down = event.inside
			end
		end	
		return true
	end,
	--! \brief 重新对子控件进行排版，该函数不需要参数
	--! \note 放入到layout的子控件必须具备变量_align才可以被layout排版。
	--!	比如你设置一个widget的_align变量为ui.ALIGN_CENTER+ui.ALIGN_MIDDLE，
	--!	这将该widget居中放置，同时你可以使用_alignX,_alignY来调节偏移。
	--! widget应该提供函数setAlign(align,offx,offy)函数。
	relayout=function(self)
		local w,h = self:getSize()
		for i,child in pairs(self:childs()) do
			if child._align then
				relayoutChild(child,child._align,w,h)
			end
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