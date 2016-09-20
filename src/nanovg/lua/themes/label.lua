local vg = require "vg"
local ui = require "ui"

label = {
	ALIGN_LEFT = 1,
	ALIGN_CENTER = 2,
	ALIGN_RIGHT = 4,
}

function label:onInit(themes)
	self._color = themes.color
	self._fontSize = 18
	self._font = 'default'
	self._text = ''
	self._breakWidth = 1280
	self._align = self.ALIGN_LEFT
	self._nanoAlign = vg.NVG_ALIGN_LEFT+vg.NVG_ALIGN_TOP
end

function label:onRelease()
end

function label:onDraw()
	local w,h = self:getSize()
	vg.fontSize(self._fontSize)
	vg.fontFace(self._font)
	vg.fillColor(self._color)
	vg.textAlign(self._nanoAlign)
	vg.textBox(0,0,math.min(self._breakWidth,w),self._text)
end

function label:setAlign(align)
	self._align = align
end

function label:setColor(c)
	self._color = c
end

function label:setFont( name )
	self._font = name
	self:reCalcSize()
end

function label:setFontSize( size )
	self._fontSize = size
	self:reCalcSize()
end

function label:setBreakWidth(bw)
	self._breakWidth = bw
	self:reCalcSize()
end

function label:reCalcSize()
	vg.save()
	vg.fontSize(self._fontSize)
	vg.fontFace(self._font)
	local x1,y1,x2,y2 = vg.textBoxBounds(0,0,self._breakWidth,self._text)
	self:setSize(x2-x1,y2-y1)
	vg.restore()
end

function label:setString(s)
	self._text = s
	self:reCalcSize()
end

function label:getString()
	return self._text
end

return label