--!
--! \addtogroup edit edit widget
--! \brief edit widget界面组件，实现单行编辑控件。
--! @{
--!
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

local function print_caller()
	local caller = debug.getinfo(3,'nSl')
	if caller then
		print('	call from '..caller.source..':'..caller.currentline )
	else
		print("ERROR: log_caller debug.getinfo return nil.")
	end
end

local function print_utf8index(str)
	local t = utf8Index(str)
	local tt = {}
	table.insert(tt,'|')
	for i,v in pairs(t) do
		table.insert(tt,v)
		table.insert(tt,'|')
	end
	print(table.concat(tt))
end

local function utf8Sub(str,s,e)
	local t = utf8Index(str)
	if #t==0 then
		return ''
	end
	--print_caller()
	--print(string.format("'%s',%s,%s",str,tostring(s),tostring(e)))
	s = s or -1
	e = e or -1
	if s==0 and e~=0 then s=1 end
	--print(string.format("'%s',%d,%d",str,s,e))
	--print_utf8index(str)
	if t[s] and t[e+1] then
	--	print("start:"..tostring(t[s]).." end:"..tostring(t[e+1]))
		return string.sub(str,t[s],t[e+1]-1)
	elseif s and t[s] then
	--	print("start:"..tostring(t[s]))
		return string.sub(str,t[s])
	end
	return ''
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
		self._pos = 0 --光标位置，以字符为单位的，汉字算一个单位
		self._cursorFlash = 0.5
		self._flash = 0
		self._text = "" --编辑器中的文本
		self._fontSpace = 0 --字符间距
		self._fontSize = 18
		self._textColor = vg.rgba(0,0,0,255)
		self._font = 'default'
		self._horzPos = 0 --文字相对开头的x偏移量
		self._isSeekbar = nil
		self._seekRadius = 12
		self._seekBarX = 0
		self._seekBarY = 0
		self._undo = {}
		self._undoCount = 10 --最多可以进行多少次undo
		self._doubleClickDaley = 0.3
		self._wordStrip = {[' ']=1,['\t']=1} --双击选择时用于分隔符
		self._nanoAlign = vg.NVG_ALIGN_LEFT+vg.NVG_ALIGN_BOTTOM
		self:enableEvent(ui.EVENT_TOUCHDOWN+ui.EVENT_TOUCHUP+ui.EVENT_TOUCHDROP)
	end,
	currentState=function(self)
		return {
			_text = self._text,
			_horzPos = self._horzPos,
			_pos = self._pos,
			_cursorPos = self._cursorPos,
		}
	end,
	restoreState=function(self,state)
		self._select = nil
		self._isSeekbar = nil
		self._text = state._text
		self._horzPos = state._horzPos
		self._pos = state._pos
		self._cursorPos = state._cursorPos
	end,
	pushUndo=function(self)
		--简单后退一步
		table.insert(self._undo,self:currentState())
		if #self._undo > self._undoCount then
			for i=1,#self._undo-self._undoCount do
				table.remove(self._undo,1)
			end
		end
	end,
	popUndo=function(self)
		if #self._undo > 0 then
			self:restoreState(self._undo[#self._undo])
			table.remove(self._undo,#self._undo)
		end
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
		local last,first,lasti
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
					self._textWidth = t[#t].maxx+t[1].minx
					self._horzPos = 0
					self:relayout(true)
					return
				end
			end
			if x >=v.minx and x <= v.maxx then
				if x < (v.maxx+v.minx)/2 then
					self._cursorPos = v.minx
					self._pos = i - 1
				else
					self._cursorPos = v.maxx
					self._textWidth = t[#t].maxx+t[1].minx
					self._pos = i
				end
				self:relayout(true)
				return
			end
			last = v
			lasti = i
		end
		if last then
			self._pos = lasti
			self._cursorPos = last.maxx
			self._textWidth = t[#t].maxx+t[1].minx
		end
		self:relayout(true)
	end,
	--根据_pos计算文本的宽度_textWidth和光标位置_cursorPos
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
			if t then
				self._textWidth = t[#t].maxx+t[1].minx
			end
			self:relayout()
			return t
		end
		for i,v in pairs(t) do
			if self._pos == i then
				--print(string.format('%s pos:%d x:%d horzPos:%d',self._text,v.pos,v.maxx,self._horzPos))
				self._cursorPos = v.maxx
				self._textWidth = t[#t].maxx+t[1].minx
				self:relayout()
				return t
			end
		end
		return t
	end,
	--! \brief 将光标位置始终保持在编辑范围内
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
		self:setInputRect()
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
		local prefix = utf8Sub(self._text,1,start)
		local surfix = utf8Sub(self._text,en+1)
		--print(string.format("delete '%s',%d,%d",self._text,start,en))
		--print(string.format("prefix:'%s'  surfix:'%s'",prefix,surfix))
		self._text = prefix..surfix
		self._pos = start
		self._select = nil
	end,
	copySelect=function(self)
		local start = math.min(self._select.start,self._select.en)
		local en = math.max(self._select.start,self._select.en)
		local text = utf8Sub(self._text,start+1,en)
		--print(string.format("copySelect '%s',%d,%d",self._text,start,en))
		--print(string.format("copy '%s'",text))
		clipbaordCopy(text)
	end,
	insertString=function(self,str)
		if self._select and self._select.start and self._select.en then
			self:deleteSelect()
		end
		self._isSeekbar = nil
		local prefix = utf8Sub(self._text,0,self._pos)
		local surfix = utf8Sub(self._text,self._pos+1)
		self._text = prefix..str..surfix
		self._pos = self._pos + utf8Length(str)
		self:reCorsorPos()
	end,
	editingString=function(self,str,start)
		if self._select and self._select.start and self._select.en then
			self:deleteSelect()
		end
		self._isSeekbar = nil
		local prefix = utf8Sub(self._text,0,self._pos)
		local surfix = utf8Sub(self._text,self._pos+1)
		self._text = prefix..str..surfix
		if str=='' then return end
		self._select = {}
		self._select.start = utf8Length(prefix)
		self._select.en = self._select.start+utf8Length(str)
		self._pos = self._pos + start--utf8Length(str)
		local t = self:reCorsorPos()
		if self._select.start==0 then
			self._select.corsorStart = self._cornerRadius
		end
		if self._select.en==0 then
			self._select.corsorEnd = self._cornerRadius
		end
		if t then
			for i,v in pairs(t) do
				if self._select.start == i then
					self._select.corsorStart = v.maxx
				end
				if self._select.en == i then
					self._select.corsorEnd = v.maxx
				end
			end
		end
	end,
	selectAll=function(self)
		self._select = {}
		self._select.start = 0
		self._select.corsorStart = self._cornerRadius
		self._select.en = utf8Length(self._text)
		self._select.corsorEnd = self._textWidth-self._cornerRadius
		self._pos = self._select.en
		self._cursorPos = self._select.corsorEnd
		--print(string.format('%d , %d',self._select.corsorStart,self._select.corsorEnd))
		self:reCorsorPos()
	end,
	selectPositionWord=function(self)
		local head = 0
		local tail = utf8Length(self._text)
		for i=self._pos,0,-1 do
			if self._wordStrip[utf8Sub(self._text,i,i)] then
				head = i
				break
			end
		end
		for i=self._pos,utf8Length(self._text) do
			if self._wordStrip[utf8Sub(self._text,i,i)] then
				tail = i-1
				break
			end
		end		
		--print(string.format("'%s' %d,%d horzPos:%d",self._text,head,tail,self._horzPos))
		if head >= tail then return end
		self._select = {}
		self._select.start = head
		self._select.en = tail
		self._pos = head
		local t = self:reCorsorPos()
		--print('horzPos:'..self._horzPos)
		self._select.corsorStart = self._cursorPos
		self._select.corsorEnd = self._select.corsorStart
		for i,v in pairs(t) do
			if tail == i then
				self._select.corsorEnd = v.maxx
				break
			end
		end
		--确保选择区域在屏幕中
		local w,h = self:getSize()
		--print(string.format('size: %d , %d',w,h))
		--print(string.format('start:%d,end:%d,horz:%d',self._select.corsorStart,self._select.corsorEnd,self._horzPos))
		if (self._select.corsorStart+self._horzPos)>= self._cornerRadius and 
			(self._select.corsorEnd+self._horzPos)<=(w-self._cornerRadius) then
			return
		elseif self._select.corsorStart+self._horzPos<self._cornerRadius then
			self._horzPos = self._cornerRadius - self._select.corsorStart
		elseif self._select.corsorEnd+self._horzPos>w-self._cornerRadius then
			self._horzPos = w-self._cornerRadius - self._select.corsorEnd
		end
	end,
	openIme=function(self)
		if not self._isIME then
			softKeyboard(true,function(event,str,p1)
				self._flash = 0
				if event=='attach' then
					self:setInputRect()
					self._isIME = 1					
				elseif event=='detach' then
					self._isIME = nil
				elseif event=='insert' and str then
					self:pushUndo()
					self:insertString(str)
					self:NotifTextChangedEvent()
				elseif event=='editing' and str then
					self:editingString(str,p1)
				elseif event=='backspace' then
					if self._select and self._select.start and self._select.en then
						self:pushUndo()
						self:deleteSelect()
					elseif self._pos > 0 then
						self:pushUndo()
						self._isSeekbar = nil
						local prefix = utf8Sub(self._text,0,self._pos-1)
						local surfix = utf8Sub(self._text,self._pos+1)				
						self._text = prefix..surfix
						if self._pos > 0 then
							self._pos = self._pos - 1
						end
					end
					self:reCorsorPos()
					self:NotifTextChangedEvent()
				elseif event=='delete' then	
					if self._select and self._select.start and self._select.en then
						self:pushUndo()
						self:deleteSelect()
					else
						self:pushUndo()
						self._isSeekbar = nil
						local prefix = utf8Sub(self._text,0,self._pos)
						local surfix = utf8Sub(self._text,self._pos+2)					
						self._text = prefix..surfix
					end
					self:reCorsorPos()
					self:NotifTextChangedEvent()
				elseif event=='left' then
					self:keyboardSelectBegin()
					if self._pos > 0 then
						self._pos = self._pos - 1
					end
					self:reCorsorPos()
					self:keyboardSelectEnd()
				elseif event=='right' then
					self:keyboardSelectBegin()
					if self._pos < utf8Length(self._text) then
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
					self._pos = utf8Length(self._text)
					self:reCorsorPos()	
					self:keyboardSelectEnd()
				elseif event=='keydown' and isCtrlDown() then
					if str==sc.C and self._select and self._select.en then
						self:copySelect()
					elseif str==sc.X and self._select and self._select.en then
						self:pushUndo()
						self:copySelect()
						self:deleteSelect()
						self:reCorsorPos()
						self:NotifTextChangedEvent()
					elseif str==sc.V then
						self:pushUndo()
						if self._select and self._select.en then
							self:deleteSelect()
							self:reCorsorPos()
						end
						self:insertString(clipbaordPast())
						self:NotifTextChangedEvent()
					elseif str==sc.A then
						self:selectAll()
						self:NotifTextChangedEvent()
					elseif str==sc.Z then
						self:popUndo()
						self:NotifTextChangedEvent()
					end
				elseif event=='keydown' then
					self:NotifPressAction(str)
				end
			end)
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
				self._seekDown = 1
				self:enableEvent(ui.EVENT_UNBOUNDED)
				self:ptCorsorPos(event.x,event.y)
				self._select = {}
				self._select.start = self._pos
				self._select.corsorStart = self._cursorPos
				if self._prevClickTime and (event.time-self._prevClickTime)/1000<self._doubleClickDaley then
					--双击选择段
					self:selectPositionWord()
					self._seekDown = nil
				end
				self._prevClickTime = event.time
				self:openIme()
			elseif rr and rr <= self._seekRadius*self._seekRadius then
				self._seekDown = 2
			elseif not ptInRect(event.x,event.y,w,h) then
				self._forcs =  nil
				self._isSeekbar = nil
				self:disableEvent(ui.EVENT_UNBOUNDED)
				if self._isIME then
					softKeyboard(false,0)
				end
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
	--! \brief 注意不要调用这个函数，如果你想监控文本编辑的变化，就设置onTextChanged
	--!	\note 例如： editObject.onTextChanged=function(text) end
	NotifTextChangedEvent=function(self)
		if self.onTextChanged then
			self.onTextChanged(self._text)
		end
	end,
	--! \brief 注意不是调用这个函数，如果你想监控回车动作，可以设置onPressAction
	--!	\note 例如： editObject.onPressAction=function(key) end
	NotifPressAction=function(self,key)
		if self.onPressAction then
			self.onPressAction(key)
		end
	end,
	setInputRect=function(self)
		local w,h = self:getSize()
		local x,y = ui.widgetToRoot(self,0,0)
		x = x + self._horzPos+self._cursorPos
		h = h-14
		softKeyboardInputRect(x,y,w,h)	
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
			vg.text(self._horzPos+self._cornerRadius,h-self._cornerRadius,self._text)
		end
		if self._forcs and self._select and self._select.en then
			vg.beginPath()
			vg.rect(self._horzPos+self._select.corsorStart,self._cornerRadius,
			self._select.corsorEnd-self._select.corsorStart,h-2*self._cornerRadius)
			vg.fillColor(self._colorSelect)
			vg.fill()
			--print(string.format('select %d,%d,%d,%d',self._horzPos+self._select.corsorStart,self._cornerRadius,
			--self._select.corsorEnd-self._select.corsorStart,h-2*self._cornerRadius))
			--print('corsorEnd : '..self._select.corsorEnd)
		end
		self._flash = self._flash + dt
		if self._flash > 2*self._cursorFlash then
			self._flash = 0
		end
		vg.restore()
		local px = self._horzPos+self._cursorPos
		if self._forcs and self._flash < self._cursorFlash and px>0 and px<w then
			vg.beginPath()
			vg.rect(px,self._cornerRadius,1,h-2*self._cornerRadius)
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