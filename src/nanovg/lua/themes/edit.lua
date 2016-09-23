local vg = require "vg"
local ui = require "ui"

return {
	onInit=function(self,themes)
		self._color = themes.color
		self._colorBG = themes.colorBG
		self._lineColor = vg.rgba(0,0,0,255)
		self._cornerRadius = 3
		self._forcs = 1
		self._cursorPos = self._cornerRadius
		self._cursorFlash = 0.5
		self._flash = 0
		self._text = "hello world"
		self._fontSize = 18
		self._textColor = vg.rgba(0,0,0,255)
		self._font = 'default'
		self._breakWidth = 1280
		self._nanoAlign = vg.NVG_ALIGN_LEFT+vg.NVG_ALIGN_TOP
	end,
	onRelease=function(self)
	end,
	onEvent=function(self,event)
		
	end,
	onDraw=function(self,dt)
		local w,h = self:getSize()
		vg.beginPath()
		vg.roundedRect(0,0,w,h,self._cornerRadius)
		vg.strokeColor(self._lineColor)
		vg.stroke()
		
		if self._text then
			vg.fontSize(self._fontSize)
			vg.fontFace(self._font)
			vg.fillColor(self._textColor)
			vg.textAlign(self._nanoAlign)
			vg.textBox(0,0,math.min(self._breakWidth,w),self._text)
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
	end,	
}