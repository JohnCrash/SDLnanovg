local vg = require "vg"
local ui = require "ui"

return {
	onInit=function(self,themes)
		self._color = vg.rgba(255,0,0,0)
		self:enableEvent(ui.EVENT_TOUCHDOWN+ui.EVENT_TOUCHUP+ui.EVENT_TOUCHDROP)
		self._setSize_cfunc = lua_widgetFunction("setSize")
		self._first = nil
		self._last = nil
		self._visibaleWidgets = {}
		self._count = 0
		self._scrollX = 0;
		self._scrollY = 0;
		self._mode = ui.VERTICAL;
		self._align = ui.ALIGN_LEFT+ui.ALIGN_MIDDLE;
		self._space = 0;
		self._rangeW = 0;
		self._rangeH = 0;
		self._W,self._H = self:getSize()
	end,
	onRelease=function(self)
	end,	
	onDraw=function(self)
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
		if widget and not widget._prev and not widget._next then
			if self._last then
				widget._prev = self._last
				self._last._next = widget
				self._last = self._last._next
			else
				self._last = widget
				self._first = widget
			end
			self._count = self._count + 1
			local w,h = widget:getSize()
			self._rangeW = self._rangeW + w
			self._rangeH = self._rangeH + h
		end
	end,
	getWidgetCount=function(self)
		return self._count
	end,
	removeWidget=function(self,widget)
		if widget and (widget._prev or widget._next) then
			if self._first==widget and self._last==widget then
				self._first = nil
				self._last = nil
			elseif self._first==widget then
				self._first = widget._next
				self._first._prev = nil
			elseif self._last==widget then
				self._last = widget._prev
				self._last._next = nil
			else
				widget._prev._next = widget._next
				widget._next._prev = widget._prev
			end
			widget._prev = nil
			widget._next = nil
			self._count = self._count-1
			local w,h = widget:getSize()
			self._rangeW = self._rangeW - w
			self._rangeH = self._rangeH - h
		end
	end,
	getScrollPos=function(self)
		return self._scrollX,self._scrollY
	end,
	setScrollPos=function(self,x,y)
		self._scrollX = x
		self._scrollY = y
	end,
	getScrollRange=function(self)
		if self._mode == ui.VERTICAL then
			return self._rangeH
		else
			return self._rangeW
		end
	end,
	setSize=function(self,w,h)
		self._W = w
		self._H = h
		self._setSize_cfunc(self,w,h)
	end,
	widgetIsVisible=function(self,widget)
		if widget and (widget._prev or widget._next) then
			local w,h = widget:getSize()
			local x,y = widget:getPosition()
			if self._mode==ui.VERTICAL then
				return y+h >= self._scrollY and y < self._scrollY + self._H
			else
				return x+w >= self._scrollX and x < self._scrollX + self._W
			end
		else
			return false
		end
	end,
	getFirstVisibleWidget=function(self)
		local first
		if #self._visibaleWidgets > 0 then
			--找到一个可见的搜索起点
			for i,v in pairs(self._visibaleWidgets) do
				if self:widgetIsVisible(v) then
					first = v
					break
				end
			end
			if first then
				--沿该起点向前搜索找到第一个可见的控件
				local prev = first._prev
				while prev do
					if widgetIsVisible(self,prev) then
						first = prev
						prev = prev._prev
					else
						break
					end
				end
			else
				--没有起点，从头开始搜索找到第一个可见控件
				local next = self._first
				while next then
					if widgetIsVisible(self,next) then
						first = next
						next = prev._next
					else
						break
					end
				end
			end
		else
			first = self._first
		end
		return first
	end,
	relayout=function(self)
		local first = getFirstVisibleWidget()
		--删除可见表中的已经不可见的控件
		for i,v in pairs(self._visibaleWidgets) do
			if not self:widgetIsVisible(v) then
				v:removeFromParent()
				self._visibaleWidgets[i] = nil
			end
		end
	end,
}