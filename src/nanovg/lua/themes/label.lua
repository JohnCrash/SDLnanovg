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
	self._bold = false
end

function label:onRelease()
end

function label:onDraw()
	local w,h = self:getSize()
	vg.fontSize(self._fontSize)
	vg.fontFace(self._font)
	vg.fillColor(self._color)
	vg.textAlign(self._nanoAlign)
	vg.textBox(0,0,self._breakWidth,self._text)
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

--! \brief 设置尺寸的匹配方式，当widget放入到layout中的时候，layout在需要重新布局的时候，可以控制widget的尺寸。
--! \param fit 设置一个对象的尺寸模式，用于layout的relaout函数。可以是下面的值。
--! 	- ui.WRAP_CONTENT		固定根据内容确定尺寸
--! 	- ui.WIDTH_WRAP_CONTENT	宽度根据内容确定尺寸
--! 	- ui.HEIGHT_WRAP_CONTENT	高度根据内容确定尺寸
--! 	- ui.WIDTH_MATCH_PARENT		宽度匹配父窗口的宽度
--! 	- ui.HEIGHT_MATCH_PARENT	高度匹配父窗口的高度
--! 	- ui.FILL_PARENT			宽度和高度都匹配父窗口的尺寸
--! \param cw,ch 如果使用MATCH模式时作为修正，比如说cw=10,ch=10,那么在匹配父窗口的尺寸时减去10
function label:setLayoutMatch(fit,cw,ch)
	self._match = fit
	self._matchX = cw
	self._matchY = ch
end

--! \brief 取得匹配方式
function label:getLayoutMatch()
	return self._match,self._matchX,self._matchY
end
	
--! \brief 设置字体
--! \param name 字体名称
function label:setFont( name )
	self._font = name
	self:reCalcSize()
end

--! \brief 加粗字体或者关闭
--! \param en true加粗字体，否则正常
function label:setFontBold( en )
	self._bold = en
end

--! \brief 如果字体加粗返回true，否则返回false
function label:getFontBold()
	return self._bold
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