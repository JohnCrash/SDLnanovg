local vg = require "vg"
local ui = require "ui"

local function ptInRect(x,y,w,h)
	return x > 0 and x <= w and y > 0 and y <= h
end

return {
	onInit=function(self,themes)
		self._color = themes.color
		self._colorBG = themes.colorBG
		self._lineColor = vg.rgba(0,0,0,255)
		self._cornerRadius = 3
		self._forcs = nil
		self._cursorPos = self._cornerRadius
		self._pos = 0
		self._cursorFlash = 0.5
		self._flash = 0
		self._text = ""
		self._fontSize = 18
		self._textColor = vg.rgba(0,0,0,255)
		self._font = 'default'
		self._breakWidth = 1280
		self._horzPos = 0
		self._nanoAlign = vg.NVG_ALIGN_LEFT+vg.NVG_ALIGN_BOTTOM
		self:enableEvent(ui.EVENT_TOUCHDOWN+ui.EVENT_TOUCHUP+ui.EVENT_TOUCHDROP)
	end,
	onRelease=function(self)
	end,
	ptCorsorPos=function(self,x,y)
		local w,h = self:getSize()
		self._fontSize = h - 2*self._cornerRadius
		vg.fontFace(self._font)
		vg.fontSize(self._fontSize)
		local t = vg.textGlyphPositions(self._horzPos+self._cornerRadius,h-self._cornerRadius,self._text)
		local last
		for i,v in pairs(t) do
			--print(tostring(v.pos)..":"..tostring(v.x).."("..tostring(v.minx).."/"..tostring(v.maxx)..")")
			if x >=v.minx and x <= v.maxx then
				if x < (v.maxx+v.minx)/2 then
					self._cursorPos = v.minx
				else
					self._cursorPos = v.maxx
				end
				return
			end
			last = v
		end
		if last then
			self._cursorPos = last.maxx
		end
	end,
	onEvent=function(self,event)
		if event.type == ui.EVENT_TOUCHDOWN then
			local w,h = self:getSize()
			if ptInRect(event.x,event.y,w,h) then
				self._forcs = 1
				self._flash = 0
				self:enableEvent(ui.EVENT_UNBOUNDED)
				self:ptCorsorPos(event.x,event.y)
			elseif not ptInRect(event.x,event.y,w,h) then
				self._forcs =  nil
				self:disableEvent(ui.EVENT_UNBOUNDED)
			end
		elseif event.type == ui.EVENT_TOUCHUP then
		elseif event.type == ui.EVENT_TOUCHDROP then
		end
	end,
	onDraw=function(self,dt)
		local w,h = self:getSize()
		vg.save()
		vg.beginPath()
		vg.roundedRect(0,0,w,h,self._cornerRadius)
		vg.strokeColor(self._lineColor)
		vg.stroke()
		
		if self._text then
			vg.scissor(self._cornerRadius,self._cornerRadius,w-2*self._cornerRadius,h-2*self._cornerRadius)
			self._fontSize = h - 2*self._cornerRadius
			vg.fontFace(self._font)
			vg.fontSize(self._fontSize)
			vg.fillColor(self._textColor)
			vg.textAlign(self._nanoAlign)
			vg.textBox(self._horzPos+self._cornerRadius,h-self._cornerRadius,self._breakWidth,self._text)
		end
		self._flash = self._flash + dt
		if self._flash > 2*self._cursorFlash then
			self._flash = 0
		end
		if self._forcs and self._flash < self._cursorFlash then
			vg.beginPath()
			vg.rect(self._cursorPos,self._cornerRadius,1,h-2*self._cornerRadius)
			vg.fillColor(self._colorBG)
			vg.fill()
		end
		vg.restore()
	end,	
}