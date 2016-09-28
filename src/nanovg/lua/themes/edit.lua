local vg = require "vg"
local ui = require "ui"
local sc = require "scancode"

local function ptInRect(x,y,w,h)
	return x > 0 and x <= w and y > 0 and y <= h
end

local function isShiftDown()
	local ks = keyboardState{sc.LSHIFT,sc.RSHIFT}
	return ks and (ks[1]==1 or ks[2]==1)
end

local function isCtrlDown()
	local ks = keyboardState{sc.LCTRL,sc.RCTRL}
	return ks and (ks[1]==1 or ks[2]==1)
end

return {
	onInit=function(self,themes)
		self._color = themes.color
		self._colorBG = themes.colorBG
		self._lineColor = vg.rgba(0,0,0,255)
		self._colorSelect = vg.rgbaf(self._colorBG.r,self._colorBG.g,self._colorBG.b,0.2)
		self._cornerRadius = 3
		self._forcs = nil
		self._cursorPos = self._cornerRadius
		self._pos = 0
		self._cursorFlash = 0.5
		self._flash = 0
		self._text = ""
		self._fontSpace = 0
		self._fontSize = 18
		self._textColor = vg.rgba(0,0,0,255)
		self._font = 'default'
		self._breakWidth = 1280
		self._horzPos = 0
		self._isSeekbar = nil
		self._seekRadius = 12
		self._seekBarX = 0
		self._seekBarY = 0
		self._undo = {}
		self._nanoAlign = vg.NVG_ALIGN_LEFT+vg.NVG_ALIGN_BOTTOM
		self:enableEvent(ui.EVENT_TOUCHDOWN+ui.EVENT_TOUCHUP+ui.EVENT_TOUCHDROP)
	end,
	pushUndo=function(self)
	end,
	popUndo=function(self)
	end,
	onRelease=function(self)
	end,
	ptCorsorPos=function(self,x,y)
		x = x - self._horzPos
		local w,h = self:getSize()
		self._fontSize = h - 2*self._cornerRadius
		vg.fontFace(self._font)
		vg.fontSize(self._fontSize)
		vg.textLetterSpacing(self._fontSpace)
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
		vg.textLetterSpacing(self._fontSpace)
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
	keyboardSelectBegin=function(self)
		if isShiftDown() then
			if self._select then
				self._select.corsorEnd = self._cursorPos
				self._select.en = self._pos			
			else
				self._select = {}
				self._select.start = self._pos
				self._select.corsorStart = self._cursorPos
			end			
		else
			self._select = nil
		end	
	end,
	keyboardSelectEnd=function(self)
		if isShiftDown() then
			if self._select then
				self._select.corsorEnd = self._cursorPos
				self._select.en = self._pos
				--print(string.format("select : %d,%d",self._select.start,self._select.en))
			else
				self._select = {}
				self._select.start = self._pos
				self._select.corsorStart = self._cursorPos
			end			
		else
			self._select = nil
		end
	end,
	deleteSelect=function(self)
		local start = math.min(self._select.start,self._select.en)
		local en = math.max(self._select.start,self._select.en)
		local prefix = string.sub(self._text,1,start)
		local surfix = string.sub(self._text,en+1)
		--print(string.format("delete '%s',%d,%d",self._text,start,en))
		--print(string.format("prefix:%s  surfix:%s",prefix,surfix))
		self._text = prefix..surfix
		self._pos = start
		self._select = nil
	end,
	copySelect=function(self)
		local start = math.min(self._select.start,self._select.en)
		local en = math.max(self._select.start,self._select.en)
		local text = string.sub(self._text,start,en)
		--print("copy : "..text)
		clipbaordCopy(text)
	end,
	insertString=function(self,str)
		if self._select and self._select.start and self._select.en then
			self:deleteSelect()
		end
		self._isSeekbar = nil
		local prefix = string.sub(self._text,0,self._pos) or ''
		local surfix = string.sub(self._text,self._pos+1) or ''
		self._text = prefix..str..surfix
		self._pos = self._pos + string.len(str)
		self:reCorsorPos()	
	end,
	selectAll=function(self)
		self._select = {}
		self._select.start = 0
		self._select.corsorStart = self._cornerRadius
		self._select.en = string.len(self._text)
		self._select.corsorEnd = self._textWidth-self._cornerRadius
		self._pos = self._select.en
		self._cursorPos = self._select.corsorEnd
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
				self._seekDown = 1
				self:enableEvent(ui.EVENT_UNBOUNDED)
				self:ptCorsorPos(event.x,event.y)
				self._select = {}
				self._select.start = self._pos
				self._select.corsorStart = self._cursorPos
				softKeyboard(true,0,function(event,str)
					self._flash = 0
					if event=='attach' then
						print('attach:'..w)
					elseif event=='detach' then
						print('detach:'..w)
					elseif event=='insert' and str then
						self:insertString(str)
					elseif event=='backspace' then
						if self._select and self._select.start and self._select.en then
							self:deleteSelect()
						elseif self._pos > 0 then
							self._isSeekbar = nil
							local prefix = string.sub(self._text,0,self._pos-1) or ''
							local surfix = string.sub(self._text,self._pos+1) or ''					
							self._text = prefix..surfix
							if self._pos > 0 then
								self._pos = self._pos - 1
							end
						end
						self:reCorsorPos()
					elseif event=='delete' then	
						if self._select and self._select.start and self._select.en then
							self:deleteSelect()
						else
							self._isSeekbar = nil
							local prefix = string.sub(self._text,0,self._pos) or ''
							local surfix = string.sub(self._text,self._pos+2) or ''					
							self._text = prefix..surfix
						end
						self:reCorsorPos()					
					elseif event=='left' then
						self:keyboardSelectBegin()
						if self._pos > 0 then
							self._pos = self._pos - 1
						end
						self:reCorsorPos()
						self:keyboardSelectEnd()
					elseif event=='right' then
						self:keyboardSelectBegin()
						if self._pos < string.len(self._text) then
							self._pos = self._pos + 1
						end					
						self:reCorsorPos()
						self:keyboardSelectEnd()
					elseif event=='home' then
						self:keyboardSelectBegin()
						self._pos = 0
						self:reCorsorPos()
						self:keyboardSelectEnd()
					elseif event=='end' then
						self:keyboardSelectBegin()
						self._pos = string.len(self._text)
						self:reCorsorPos()	
						self:keyboardSelectEnd()
					elseif event=='keydown' and isCtrlDown() then
						if str==sc.C and self._select and self._select.en then
							self:copySelect()
						elseif str==sc.X and self._select and self._select.en then
							self:copySelect()
							self:deleteSelect()
							self:reCorsorPos()
						elseif str==sc.V then
							if self._select and self._select.en then
								self:deleteSelect()
								self:reCorsorPos()
							end
							self:insertString(clipbaordPast())
						elseif str==sc.A then
							self:selectAll()
						end
					end
				end)
			elseif rr and rr <= self._seekRadius*self._seekRadius then
				self._seekDown = 2
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
			if self._select and not self._select.en then
				self._select = nil
			end
		elseif event.type == ui.EVENT_TOUCHDROP then
			if self._seekDown==1 then
				self._flash = 0
				self:ptCorsorPos(event.x,event.y)
				self._select.corsorEnd = self._cursorPos
				self._select.en = self._pos
			elseif self._seekDown==2 then
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
		vg.scissor(self._cornerRadius,self._cornerRadius,w-2*self._cornerRadius,h-2*self._cornerRadius)
		if self._text then
			self._fontSize = h - 2*self._cornerRadius
			vg.fontFace(self._font)
			vg.fontSize(self._fontSize)
			vg.fillColor(self._textColor)
			vg.textAlign(self._nanoAlign)
			vg.textLetterSpacing(self._fontSpace)
			vg.textBox(self._horzPos+self._cornerRadius,h-self._cornerRadius,self._breakWidth,self._text)
		end
		if self._forcs and self._select and self._select.en then
			vg.beginPath()
			vg.rect(self._horzPos+self._select.corsorStart,self._cornerRadius,
			self._select.corsorEnd-self._select.corsorStart,h-2*self._cornerRadius)
			vg.fillColor(self._colorSelect)
			vg.fill()
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
		
		if self._isSeekbar and not (self._select and self._select.en) then
			self._seekBarX = self._horzPos+self._cursorPos
			self._seekBarY = self._cornerRadius+h-2*self._cornerRadius+self._seekRadius
			vg.beginPath()
			vg.circle(self._seekBarX,self._seekBarY,self._seekRadius)
			vg.fillColor(self._colorBG)
			vg.fill()
		end
	end,
}