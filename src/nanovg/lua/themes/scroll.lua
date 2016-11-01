local vg = require "vg"
local ui = require "ui"

return {
	onInit=function(self,themes)
		self._color = themes.color
		self._colorBG = themes.colorBG
		self:enableEvent(ui.EVENT_TOUCHDOWN+ui.EVENT_TOUCHUP+ui.EVENT_TOUCHDROP)
		self._inner = ui.createWidget("normal","inner")
		self:addChild(self._inner)
	end,
	onRelease=function(self)
	end,
	onDraw=function(self)
		local w,h = self:getSize()
		vg.beginPath()
		vg.rect(0,0,w,h)
		vg.fillColor(self._colorBG)
		vg.fill()
	end,
	onEvent=function(self,event)
		if event.type == ui.EVENT_TOUCHDOWN then
			self._down = true
		elseif event.type == ui.EVENT_TOUCHUP then
			self._down = false
		elseif event.type == ui.EVENT_TOUCHDROP then
			if self._down then
				self._down = event.inside
			end
		end
		return true
	end,
	configScroll=function(self,mode,align,space)
	end,
	addWidget=function(self,widget)
		self._inner:addChild(widget)
	end,
	relayout=function(self)
		local childs = self._inner:childs()
		local x,y,ww,hh
		x = 0
		y = 0
		ww = 0
		hh = 0
		for i,v in pairs(childs) do
			local w,h = v:getSize()
			v:setPosition(x,y)
			y = y + h + 5
			ww = math.max(ww,w)
			print(string.format("x=%d,y=%d",x,y))
		end
		self._inner:setSize(ww,y)
		self._inner:setPosition(30,-30)
	end,
}