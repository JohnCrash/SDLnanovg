--!
--! \addtogroup button button widget
--! \brief button widget界面组件，实现按钮控件。
--! @{
--!
local vg = require "vg"
local ui = require "ui"

local function isBlack(col)
	return col
end

local function drawButton(preicon, text, x, y, w, h, col,isdown)
	local bg
	local cornerRadius = 4.0
	local tw = 0
	local iw = 0

	if isdown then
		bg = vg.linearGradient( x,y,x,y+h, vg.rgba(0,0,0,isBlack(col) and 16 or 32),vg.rgba(255,255,255,isBlack(col) and 16 or 32))
	else
		bg = vg.linearGradient( x,y,x,y+h, vg.rgba(255,255,255,isBlack(col) and 16 or 32), vg.rgba(0,0,0,isBlack(col) and 16 or 32))
	end
	vg.beginPath()
	vg.roundedRect( x+1,y+1, w-2,h-2, cornerRadius-1)
	if not isBlack(col) then
		vg.fillColor( col)
		vg.fill()
	end
	vg.fillPaint( bg)
	vg.fill()

	vg.beginPath()
	vg.roundedRect( x+0.5,y+0.5, w-1,h-1, cornerRadius-0.5)
	vg.strokeColor( vg.rgba(0,0,0,48))
	vg.stroke()

	vg.fontSize( 20.0)
	vg.fontFace( "default-bold")
	tw = vg.textBounds( 0,0, text)
	if preicon ~= 0 then
		vg.fontSize( h*1.3)
		vg.fontFace( "icons")
		iw = vg.textBounds( 0,0, vg.cpToUTF8(preicon))
		iw = iw+h*0.15
	end

	if preicon ~= 0 then
		vg.fontSize( h*1.3)
		vg.fontFace( "icons")
		vg.fillColor( vg.rgba(255,255,255,96))
		vg.textAlign(vg.NVG_ALIGN_LEFT+vg.NVG_ALIGN_MIDDLE)
		vg.text( x+w*0.5-tw*0.5-iw*0.75, y+h*0.5, vg.cpToUTF8(preicon))
	end

	vg.fontSize( 20.0)
	vg.fontFace( "default-bold")
	vg.textAlign(vg.NVG_ALIGN_LEFT+vg.NVG_ALIGN_MIDDLE)
	
	if isdown then
		vg.fillColor( vg.rgba(0,0,0,160))
		vg.text( x+w*0.5-tw*0.5+iw*0.25,y+h*0.5-1+1,text)
		vg.fillColor( vg.rgba(255,255,255,160))
		vg.text( x+w*0.5-tw*0.5+iw*0.25,y+h*0.5+1,text)	
	else
		vg.fillColor( vg.rgba(0,0,0,160))
		vg.text( x+w*0.5-tw*0.5+iw*0.25,y+h*0.5-1,text)
		vg.fillColor( vg.rgba(255,255,255,160))
		vg.text( x+w*0.5-tw*0.5+iw*0.25,y+h*0.5,text)
	end
end

return {
		onInit=function(self,themes)		
			self._color = vg.rgba(255,0,0,0)
			self:enableEvent(ui.EVENT_TOUCHDOWN+ui.EVENT_TOUCHUP+ui.EVENT_TOUCHDROP)
		end,
		onRelease=function(self)
		end,	
		onDraw=function(self)
			local w,h = self:getSize()
			drawButton(0,self._title or "OK",0,0,w,h,self._color,self._down)
		end,
		--! \brief 按钮的事件处理函数，可以为对象设置.onClick函数
		--!		例如：buttonObject.onClick = function() end
		onEvent=function(self,event)
			if event.type == ui.EVENT_TOUCHDOWN then
				self._down = true
			elseif event.type == ui.EVENT_TOUCHUP then
				self._down = false
				if self.onClick and event.inside then
					self.onClick()
				end
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
			local width,x,y,x2,y2 = vg.textBounds(0,0,title)
			--print(string.format("%d %d %d %d %d",width,x,y,x2,y2))
			self:setSize(width,y2-y)
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