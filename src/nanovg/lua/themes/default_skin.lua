local vg = require "vg"

local ct = {
	buttonBG = vg.rgbaf(1,1,1,0),
	buttonDownBG = vg.rgbaf(0.2,0.2,1,0.2),
	buttonText = vg.rgbaf(0,0.7,0.7,1),
}

--!
--! \addtogroup default_skin skin
--! \brief 界面的默认皮肤。
--! @{
--!
return {
	--! \brief 绘制按钮
	--! \note 绘制按钮，需要按钮具备以下属性
	--!	_title		按钮上的文本
	--!	_align	对齐方式
	--!	_down	按钮是否被按下	
	button = function(self)
		local w,h = self:getSize()
		vg.beginPath()
		vg.rect(0,0, w,h)
		if self._down then
			vg.fillColor(ct.buttonDownBG)
		else
			vg.fillColor(ct.buttonBG)
		end
		vg.fill()
		
		vg.fontSize(20.0)
		vg.fontFace("default")
		vg.textAlign(vg.NVG_ALIGN_CENTER+vg.NVG_ALIGN_MIDDLE)
		vg.fillColor(ct.buttonText)
		--tw = vg.textBounds( 0,0, self._title)
		vg.text(w/2,h/2,self._title)
	end,
}
--!
--! @}
--!