--!
--! \addtogroup label label widget
--! \brief label widget界面组件，实现文本控件。
--! @{
--!
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

--! \brief 设置对齐方式
--! \param align 对齐方式，可以是下面的值
--!		- ui.ALIGN_LEFT		左对齐
--!		- ui.ALIGN_CENTER	中心对齐
--!		- ui.ALIGN_RIGHT	右对齐
--!		- ui.ALIGN_TOP		顶对齐
--!		- ui.ALIGN_MIDDLE	竖向中心对齐
--!		- ui.ALIGN_BOTTOM	低对齐	
function label:setAlign(align,offx,offy)
	self._align = align
	self._alignX = offx
	self._alignY = offy
end

--! \brief 取得label的对齐方式
function label:getAlign()
	return self._align,self._alignX,self._alignY
end

--! \brief 设置文字颜色
--! \param c 字体颜色，可以使用vg.rgba(255,255,255,255),vg.rgbaf(1,1,1,1)
function label:setColor(c)
	self._color = c
end

--! \brief 设置字体
--! \param name 字体名称
function label:setFont( name )
	self._font = name
	self:reCalcSize()
end

--! \brief 设置字体尺寸，函数将重新计算尺寸
--! \param size 字体尺寸
function label:setFontSize( size )
	self._fontSize = size
	self:reCalcSize()
end

--! \brief 设置文本的强制折行宽度
--! \param bw 强制折行宽度
function label:setBreakWidth(bw)
	self._breakWidth = bw
	self:reCalcSize()
end

--! \brief 根据文本的内容，重新计算控件尺寸
function label:reCalcSize()
	vg.save()
	vg.fontSize(self._fontSize)
	vg.fontFace(self._font)
	local x1,y1,x2,y2 = vg.textBoxBounds(0,0,self._breakWidth,self._text)
	self:setSize(x2-x1,y2-y1)
	vg.restore()
end

--! \brief 设置当前控件的内容,重新计算控件尺寸
function label:setString(s)
	self._text = s
	self:reCalcSize()
end

--! \brief 取得当前文本控件的内容
--! \return 返回内容字符串
function label:getString()
	return self._text
end

return label
--!
--! @}
--!