--!
--! \addtogroup image image widget
--! \brief image widget界面组件，实现图片显示或者图标控件。
--! @{
--!
local vg = require "vg"
local ui = require "ui"

return {
	onInit=function(self)
		self._image = 0
		self._alpha = 1
	end,
	onRelease=function(self)
		if self._image~=0 then
			vg.deleteImage(self._image)
		end
	end,
	onEvent=function(self,event)
	end,
	onDraw=function(self)
		if self._image ~= 0 then
			local w,h = self:getSize()
			vg.beginPath()
			vg.rect(0,0,w,h)
			vg.fillPaint(vg.imagePattern(0,0,w,h,0,self._image,self._alpha))
			vg.fill()
		end
	end,
	--! \brief 加载图片
	--! \param imageFile	要加载的文件名称
	load=function(self,imageFile)
		if self._image~=0 then
			vg.deleteImage(self._image)
		end
		self._image = vg.createImage(imageFile)
		if self._image ~= 0 then
			self:setSize(vg.imageSize(self._image))
		end
	end,
	--! \brief 设置透明值
	--! \param alpha 透明值0-1
	setAlpha=function(self,alpha)
		self._alpha = alpha
	end,
	--! \brief 返回透明值
	--! \return 返回透明值0-1
	getAlpha=function(self)
		return self._alpha
	end,	
	--! \brief 设置对齐方式
	--! \param align 对齐方式，可以是下面的值
	--!		- ui.ALIGN_LEFT		左对齐
	--!		- ui.ALIGN_CENTER	中心对齐
	--!		- ui.ALIGN_RIGHT	右对齐
	--!		- ui.ALIGN_TOP		顶对齐
	--!		- ui.ALIGN_MIDDLE	竖向中心对齐
	--!		- ui.ALIGN_BOTTOM	低对齐	
	setAlign=function(align,offx,offy)
		self._align = align
		self._alignX = offx
		self._alignY = offy
	end,
	--! \brief 取得对齐方式
	getAlign=function()
		return self._align,self._alignX,self._alignY
	end,	
}
--!
--! @}
--!