--!
--! \addtogroup window window widget
--! \brief window widget界面组件，实现窗口控件。
--! @{
--!
local vg = require "vg"
local ui = require "ui"

local function drawWindow(title,x,y,w,h,foucs)
	local cornerRadius = 3.0;
	vg.save()
	
	--window
	vg.beginPath()
	vg.roundedRect(x,y,w,h,cornerRadius)
	vg.fillColor(vg.rgba(28,30,34,192))
	vg.fill()
	
	--drop shadow
	vg.beginPath()
	vg.rect(x-10,y-10,w+20,h+30)
	vg.roundedRect(x,y,w,h,cornerRadius)
	vg.pathWinding(vg.NVG_HOLE)
	if foucs then
		vg.fillPaint(vg.boxGradient(x,y+2, w,h, cornerRadius*2, 10, vg.rgba(128,128,128,128), vg.rgba(0,0,0,0)))
	else
		vg.fillPaint(vg.boxGradient(x,y+2, w,h, cornerRadius*2, 10, vg.rgba(0,0,0,128), vg.rgba(0,0,0,0)))
	end
	vg.fill()
	
	--header
	vg.beginPath()
	vg.roundedRect(x+1,y+1, w-2,30, cornerRadius-1)
	vg.fillPaint(vg.linearGradient(x,y,x,y+15, vg.rgba(255,255,255,8), vg.rgba(0,0,0,16)))
	vg.fill()
	vg.beginPath()
	vg.path{vg.MOVETO,x+0.5,y+0.5+30,vg.LINETO,x+0.5+w-1,y+0.5+30}
	vg.strokeColor(vg.rgba(0,0,0,32))
	vg.stroke()
	
	vg.fontSize(18)
	vg.fontFace("default-bold")
	vg.textAlign(vg.NVG_ALIGN_CENTER+vg.NVG_ALIGN_MIDDLE)
	vg.fontBlur(2)
	vg.fillColor(vg.rgba(0,0,0,128))
	vg.text(x+w/2,y+16+1,title)
	
	vg.fontBlur(0)
	vg.fillColor(vg.rgba(220,220,220,160))
	vg.text(x+w/2,y+16,title)
	
	vg.restore()
end

return {
		onInit=function(self)
			self:enableEvent(ui.EVENT_BREAK+ui.EVENT_TOUCHUP+ui.EVENT_TOUCHDOWN)
		end,
		onRelease=function(self)
		end,
		onDraw=function(self)
			local w,h = self:getSize()
			drawWindow(self._title or "Window",0,0,w,h,self._foucs)
		end,
		onEvent=function(self,event)
			if not event.inside then
				if event.type==ui.EVENT_TOUCHDOWN then
					self._foucs = true
				elseif event.type==ui.EVENT_TOUCHUP then
					self._foucs = false
				end
			end
		end,
		--! \brief 设置窗口的标题
		setTitle=function(self,title)
			self._title = title
		end,
		--! \brief 返回窗口的标题
		getTitle=function(self)
			return self._title
		end,
		--! \brief 返回窗口标题的高度
		getTitleHeight=function(self)
			return 30
		end,
	}
--!
--! @}
--!	