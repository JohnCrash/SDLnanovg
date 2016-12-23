--!
--! \addtogroup vgw vg widget
--! \brief vector graphics widget界面组件，实现矢量图控件。
--! @{
--!
local vg = require "vg"
local ui = require "ui"

return {
		onInit=function(self,themes)		
		end,
		onRelease=function(self)
		end,	
		onDraw=function(self)
			if self._draw then
				local w,h = self:getSize()
				self._draw(vg,w,h)
			end
		end,
		draw=function(self,func)
			self._draw = func
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