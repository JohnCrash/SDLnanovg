vg = require "vg"
ui = require "ui"

local label = {
	FIXED_SIZE = 1,
	FIT_TEXT = 2,
}

function label:onInit()
	self._color = vg.rgba(255,255,255,128)
	self._fontSize = 18
	self._font = 'default'
	self._text = ''
	self._breakWidth = 1280
	self._style = label.FIT_TEXT
end

function label:onRelease()
end

function label:onDraw()
	local w,h = self:getSize()
	vg.fontSize(self._fontSize)
	vg.fontFace(self._font)
	vg.fillColor(self._color)
	vg.textAlign(vg.NVG_ALIGN_LEFT+vg.NVG_ALIGN_TOP)
	vg.textBox(0,0,math.min(self._breakWidth,w),self._text)
end

function label:setStyle(style)
	self._style = style
end

function label:setColor(c)
	self._color = c
end

function label:setBreakWidth(bw)
	self._breakWidth = bw
end

function label:setString(s)
	self._text = s
	if self._style==label.FIT_TEXT then
		vg.save()
		vg.fontSize(self._fontSize)
		vg.fontFace(self._font)
		local linewidth,x,y,x2,y2 = vg.textBounds(0,0,self._text)
		print("w="..(x2-x).." h="..(y2-y))
		self:setSize(x2-x,y2-y)
		vg.restore()
	end
end

function label:getString()
	return self._text
end

return label