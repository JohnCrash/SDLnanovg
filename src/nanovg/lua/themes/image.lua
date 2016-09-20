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
	load=function(self,imageFile)
		self._image = vg.createImage(imageFile)
		if self._image ~= 0 then
			self:setSize(vg.imageSize(self._image))
		end
	end,
	setAlpha=function(self,alpha)
		self._alpha = alpha
	end,
	getAlpha=function(self)
		return self._alpha
	end,	
}