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
	local match = child._match or ui.WRAP_CONTENT
	
	if isand(match,ui.WIDTH_MATCH_PARENT) and 
		isand(match,ui.HEIGHT_MATCH_PARENT) then
		cw = w-(child._matchX or 0)
		ch = h-(child._matchY or 0)
		child:setSize(cw,ch)
	elseif isand(match,ui.WIDTH_MATCH_PARENT) then
		cw = w-(child._matchX or 0)
		child:setSize(cw,ch)
	elseif isand(match,ui.HEIGHT_MATCH_PARENT) then
		ch = h-(child._matchY or 0)
		child:setSize(cw,ch)
	end
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
	if child._type == 'layout' or child._type == 'scroll' then
		child:relayout()
	end
end

return {
	_animationSpeed = 0.1,
	onInit=function(self,themes)
		self._type = 'layout'
		--self._color = themes.color
		--self._colorBG = themes.colorBG
	end,
	onRelease=function(self)
	end,	
	onDraw=function(self,dt)
		if self._colorBG then
			local w,h = self:getSize()
			vg.beginPath()
			vg.rect(0,0,w,h)
			vg.fillColor(self._colorBG)
			vg.fill()
		end
	end,
	--! \brief 设置背景颜色，默认不绘制背景
	setBGColor=function(self,c)
		self._colorBG = c
	end,
	--! \brief 取得背景颜色
	getBGColor=function(self,c)
		return self._colorBG
	end,	
	onEvent=function(self,event)
		if event.type == ui.EVENT_TOUCHDOWN then
			self._down = true
		elseif event.type == ui.EVENT_TOUCHUP then
			if event.inside and self._down then
				if self.onClick then self.onClick() end
			end
			self._down = false
		elseif event.type == ui.EVENT_TOUCHDROP then
			if self._down then
				self._down = event.inside
			end
		end	
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