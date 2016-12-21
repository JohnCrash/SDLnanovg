--!
--! \addtogroup progress progress widget
--! \brief progress widget界面组件，实现进度条控件。
--! @{
--!
local vg = require "vg"
local ui = require "ui"

return {
	_round = 5,
	onInit=function(self,themes)
		self:setSize(80,20)
		self._color = themes.color
		self._colorBG = themes.colorBG
		self._progress = 0
	end,
	onRelease=function(self)
	end,	
	onDraw=function(self,dt)
		local w,h = self:getSize()
		local d = h/10
		local r = self._round
		local p = w*self._progress-2*d
		
		vg.beginPath()
		vg.roundedRect(0,0,w,h,r)
		vg.strokeColor(self._color)
		vg.stroke()
		
		if p > 0 then
			vg.beginPath()
			vg.roundedRect(d,d,p,h-2*d,r)
			vg.fillColor(self._colorBG)
			vg.fill()
		end
	end,
	onEvent=function(self,event)
	end,
	--! \brief 取得进度条的当前进度0-1.
	getProgress=function()
		return self._progress
	end,
	--! \brief 设置当前进度，值可以是0-1.
	setProgress=function(p)
		if p > 1 then p = 1 end
		if p < 0 then p = 0 end
		self._progress = p
	end,
}
--!
--! @}
--!