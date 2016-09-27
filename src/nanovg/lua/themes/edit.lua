local vg = require "vg"
local ui = require "ui"

local function ptInRect(x,y,w,h)
	return x > 0 and x <= w and y > 0 and y <= h
end

return {
	onInit=function(self,themes)
		self._color = themes.color
		self._colorBG = themes.colorBG
		self._lineColor = vg.rgba(0,0,0,255)
		self._cornerRadius = 3
		self._forcs = nil
		self._cursorPos = self._cornerRadius
		self._pos = 0
		self._cursorFlash = 0.5
		self._flash = 0
		self._text = ""
		self._fontSize = 18
		self._textColor = vg.rgba(0,0,0,255)
		self._font = 'default'
		self._breakWidth = 1280
		self._horzPos = 0
		self._isSeekbar = nil
		self._seekRadius = 12
		self._seekBarX = 0
		self._seekBarY = 0
		self._nanoAlign = vg.NVG_ALIGN_LEFT+vg.NVG_ALIGN_BOTTOM
		self:enableEvent(ui.EVENT_TOUCHDOWN+ui.EVENT_TOUCHUP+ui.EVENT_TOUCHDROP)
	end,
	onRelease=function(self)
	end,
	ptCorsorPos=function(self,x,y)
		x = x - self._horzPos
		local w,h = self:getSize()
		self._fontSize = h - 2*self._cornerRadius
		vg.fontFace(self._font)
		vg.fontSize(self._fontSize)
		local t = vg.textGlyphPositions(self._cornerRadius,h-self._cornerRadius,self._text)
		local last,first
		if not t then 
			self._pos = 0
			self._cursorPos = self._cornerRadius
			self._textWidth = 0
			self._horzPos = 0
			self:relayout(true)
			return 
		end
		for i,v in pairs(t) do
			--print(tostring(v.pos)..":"..tostring(v.x).."("..tostring(v.minx).."/"..tostring(v.maxx)..")")
			if not first then
				first = v
				if x <= v.minx then
					self._pos = 0
					self._cursorPos = v.minx
					self._textWidth = 0
					self._horzPos = 0
					self:relayout(true)
					return
				end
			end
			if x >=v.minx and x <= v.maxx then
				if x < (v.maxx+v.minx)/2 then
					self._cursorPos = v.minx
					self._pos = v.pos - 1
				else
					self._cursorPos = v.maxx
					self._textWidth = t[#t].maxx+t[1].minx
					self._pos = v.pos
				end
				self:relayout(true)
				return
			end
			last = v
		end
		if last then
			self._pos = last.pos
			self._cursorPos = last.maxx
			self._textWidth = t[#t].maxx+t[1].minx
		end
		self:relayout(true)
	end,
	reCorsorPos=function(self)
		local w,h = self:getSize()
		self._fontSize = h - 2*self._cornerRadius
		vg.fontFace(self._font)
		vg.fontSize(self._fontSize)
		local t = vg.textGlyphPositions(self._cornerRadius,h-self._cornerRadius,self._text)
		local last,first
		if not t or self._pos == 0 then
			self._cursorPos = self._cornerRadius
			self._textWidth = 0
			self:relayout()
			return
		end
		for i,v in pairs(t) do
			if self._pos == v.pos then
				--print(string.format('%s pos:%d x:%d horzPos:%d',self._text,v.pos,v.maxx,self._horzPos))
				self._cursorPos = v.maxx
				self._textWidth = t[#t].maxx+t[1].minx
				self:relayout()
				return
			end
		end
	end,
	relayout=function(self,isdrop)
		local w,h = self:getSize()
		local x = self._cursorPos

		if self._horzPos+x < self._cornerRadius then
			self._horzPos = self._cornerRadius-x
		elseif self._horzPos+x >  w-self._cornerRadius then
			self._horzPos = w-self._cornerRadius-x
		end
		if not isdrop then
			if self._textWidth > w - 2*self._cornerRadius then
				if x>=0 and x<w-self._cornerRadius then
					self._horzPos = 0
				elseif (self._textWidth-x)<=w-2*self._cornerRadius then
					self._horzPos = (w-2*self._cornerRadius)-self._textWidth
				end
			else
				self._horzPos = 0
			end	
		end
	end,
	onEvent=function(self,event)
		if event.type == ui.EVENT_TOUCHDOWN then
			local w,h = self:getSize()
			local rr
			if self._isSeekbar then
				rr = (event.x-self._seekBarX)*(event.x-self._seekBarX)+
					(event.y-self._seekBarY)*(event.y-self._seekBarY)
			end		
			if ptInRect(event.x,event.y,w,h) then
				self._forcs = 1
				self._flash = 0
				self._isSeekbar = 1
				self:enableEvent(ui.EVENT_UNBOUNDED)
				self:ptCorsorPos(event.x,event.y)
				softKeyboard(true,0,function(event,str)
					self._flash = 0
					print( ' +'..event )
					if event=='attach' then
						print('attach:'..w)
					elseif event=='detach' then
						print('detach:'..w)
					elseif event=='insert' and str then
						self._isSeekbar = nil
						local prefix = string.sub(self._text,0,self._pos) or ''
						local surfix = string.sub(self._text,self._pos+1) or ''
						self._text = prefix..str..surfix
						self._pos = self._pos + string.len(str)
						self:reCorsorPos()
					elseif event=='backspace' and self._pos>0 then
						self._isSeekbar = nil
						local prefix = string.sub(self._text,0,self._pos-1) or ''
						local surfix = string.sub(self._text,self._pos+1) or ''					
						self._text = prefix..surfix
						if self._pos > 0 then
							self._pos = self._pos - 1
						end
						self:reCorsorPos()
					elseif event=='delete' then	
						self._isSeekbar = nil
						local prefix = string.sub(self._text,0,self._pos) or ''
						local surfix = string.sub(self._text,self._pos+2) or ''					
						self._text = prefix..surfix
						self:reCorsorPos()					
					elseif event=='left' then
						if self._pos > 0 then
							self._pos = self._pos - 1
						end					
						self:reCorsorPos()
					elseif event=='right' then
						if self._pos < string.len(self._text) then
							self._pos = self._pos + 1
						end										
						self:reCorsorPos()
					elseif event=='home' then
						self._pos = 0
						self:reCorsorPos()
					elseif event=='end' then
						self._pos = string.len(self._text)
						self:reCorsorPos()					
					end
				end)
			elseif rr and rr <= self._seekRadius*self._seekRadius then
				self._seekDown = 1
			elseif not ptInRect(event.x,event.y,w,h) then
				self._forcs =  nil
				self._isSeekbar = nil
				self:disableEvent(ui.EVENT_UNBOUNDED)
				softKeyboard(false,0)
			end
		elseif event.type == ui.EVENT_TOUCHUP then
			if self._seekDown then
				self._seekDown = nil
				self:ptCorsorPos(event.x,event.y)
			end
		elseif event.type == ui.EVENT_TOUCHDROP then
			if self._seekDown then
				self._flash = 0
				self:ptCorsorPos(event.x,event.y)
			end
		end
	end,
	onDraw=function(self,dt)
		local w,h = self:getSize()
		vg.beginPath()
		vg.roundedRect(0,0,w,h,self._cornerRadius)
		vg.strokeColor(self._lineColor)
		vg.stroke()
		
		vg.save()
		if self._text then
			vg.scissor(self._cornerRadius,self._cornerRadius,w-2*self._cornerRadius,h-2*self._cornerRadius)
			self._fontSize = h - 2*self._cornerRadius
			vg.fontFace(self._font)
			vg.fontSize(self._fontSize)
			vg.fillColor(self._textColor)
			vg.textAlign(self._nanoAlign)
			vg.textBox(self._horzPos+self._cornerRadius,h-self._cornerRadius,self._breakWidth,self._text)
		end
		vg.restore()
		self._flash = self._flash + dt
		if self._flash > 2*self._cursorFlash then
			self._flash = 0
		end
		if self._forcs and self._flash < self._cursorFlash then
			vg.beginPath()
			vg.rect(self._horzPos+self._cursorPos,self._cornerRadius,1,h-2*self._cornerRadius)
			vg.fillColor(self._colorBG)
			vg.fill()
		end
		
		if self._isSeekbar then
			self._seekBarX = self._horzPos+self._cursorPos
			self._seekBarY = self._cornerRadius+h-2*self._cornerRadius+self._seekRadius
			vg.beginPath()
			vg.circle(self._seekBarX,self._seekBarY,self._seekRadius)
			vg.fillColor(self._colorBG)
			vg.fill()
		end
	end,
}