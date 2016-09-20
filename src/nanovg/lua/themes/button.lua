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
		onInit=function(self)		
			self._color = vg.rgba(255,0,0,0)
			self:enableEvent(ui.EVENT_TOUCHDOWN+ui.EVENT_TOUCHUP+ui.EVENT_TOUCHDROP)
		end,
		onRelease=function(self)
		end,	
		onDraw=function(self)
			local w,h = self:getSize()
			drawButton(0,self._title or "OK",0,0,w,h,self._color,self._down)
		end,
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
		end,
		setTitle=function(self,title)
			self._title = title
			local width,x,y,x2,y2 = vg.textBounds(0,0,title)
			print(string.format("%d %d %d %d %d",width,x,y,x2,y2))
			self:setSize(width,y2-y)
		end,		
	}