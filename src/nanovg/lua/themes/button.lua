--!
--! \addtogroup button button widget
--! \brief button widget界面组件，实现按钮控件。
--! @{
--!
local vg = require "vg"
local ui = require "ui"

return {
		onInit=function(self,themes)		
			self._color = vg.rgba(255,0,0,0)
			self._onDraw = themes.skin.button
			self:enableEvent(ui.EVENT_TOUCHDOWN+ui.EVENT_TOUCHUP+ui.EVENT_TOUCHDROP)
		end,
		onRelease=function(self)
		end,
		onDraw=function(self)
			self._onDraw(self)
		end,
		--! \brief 按钮的事件处理函数，可以为对象设置.onClick函数
		--!		例如：buttonObject.onClick = function() end
		onEvent=function(self,event)
			if event.type == ui.EVENT_TOUCHDOWN then
				self._down = true
			elseif event.type == ui.EVENT_TOUCHUP then
				if self.onClick and self._down and event.inside then
					self.onClick()
				end
				self._down = false
			elseif event.type == ui.EVENT_TOUCHDROP then
				if self._down then
					self._down = event.inside
				end
			end
			return true
		end,
		--! \brief 设置按钮的标题
		--! \param title 要设置的标题
		setTitle=function(self,title)
			self._title = title
			--local width,x,y,x2,y2 = vg.textBounds(0,0,title)
			--print(string.format("%d %d %d %d %d",width,x,y,x2,y2))
			--self:setSize(width,y2-y)
		end,
		--! \brief 返回按钮的标题文本
		getTitle=function(self)
			return self._title
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