local vg = require "vg"
local ui = require "ui"


local function isBlack(col)
	return col
end

local function drawLabel(text, x, y, w, h)
	vg.fontSize( 18.0)
	vg.fontFace( "sans")
	vg.fillColor( vg.rgba(255,255,255,128))

	vg.textAlign(vg.NVG_ALIGN_LEFT+vg.NVG_ALIGN_MIDDLE)
	vg.text( x,y+h*0.5,text)
end

local function drawWindow(title,x,y,w,h,foucs)
	local cornerRadius = 3.0;
	vg.save()
	
	--window
	vg.beginPath()
	vg.roundedRect(x,y,w,h,cornerRadius)
	vg.fillColor(vg.rgba(28,30,34,192))
	vg.fill()
	
	--drop shadow
	vg.beginPath()
	vg.rect(x-10,y-10,w+20,h+30)
	vg.roundedRect(x,y,w,h,cornerRadius)
	vg.pathWinding(vg.NVG_HOLE)
	if foucs then
		vg.fillPaint(vg.boxGradient(x,y+2, w,h, cornerRadius*2, 10, vg.rgba(128,128,128,128), vg.rgba(0,0,0,0)))
	else
		vg.fillPaint(vg.boxGradient(x,y+2, w,h, cornerRadius*2, 10, vg.rgba(0,0,0,128), vg.rgba(0,0,0,0)))
	end
	vg.fill()
	
	--header
	vg.beginPath()
	vg.roundedRect(x+1,y+1, w-2,30, cornerRadius-1)
	vg.fillPaint(vg.linearGradient(x,y,x,y+15, vg.rgba(255,255,255,8), vg.rgba(0,0,0,16)))
	vg.fill()
	vg.beginPath()
	vg.path{vg.MOVETO,x+0.5,y+0.5+30,vg.LINETO,x+0.5+w-1,y+0.5+30}
	vg.strokeColor(vg.rgba(0,0,0,32))
	vg.stroke()
	
	vg.fontSize(18)
	vg.fontFace("sans-bold")
	vg.textAlign(vg.NVG_ALIGN_CENTER+vg.NVG_ALIGN_MIDDLE)
	vg.fontBlur(2)
	vg.fillColor(vg.rgba(0,0,0,128))
	vg.text(x+w/2,y+16+1,title)
	
	vg.fontBlur(0)
	vg.fillColor(vg.rgba(220,220,220,160))
	vg.text(x+w/2,y+16,title)
	
	vg.restore()
end

local function drawButton(preicon, text, x, y, w, h, col,isdown)
	local bg
	local cornerRadius = 4.0
	local tw = 0
	local iw = 0

	if isdown then
		bg = vg.linearGradient( x,y,x,y+h, vg.rgba(0,0,0,isBlack(col) and 16 or 32),vg.rgba(255,255,255,isBlack(col) and 16 or 32))
	else
		bg = vg.linearGradient( x,y,x,y+h, vg.rgba(255,255,255,isBlack(col) and 16 or 32), vg.rgba(0,0,0,isBlack(col) and 16 or 32))
	end
	vg.beginPath()
	vg.roundedRect( x+1,y+1, w-2,h-2, cornerRadius-1)
	if not isBlack(col) then
		vg.fillColor( col)
		vg.fill()
	end
	vg.fillPaint( bg)
	vg.fill()

	vg.beginPath()
	vg.roundedRect( x+0.5,y+0.5, w-1,h-1, cornerRadius-0.5)
	vg.strokeColor( vg.rgba(0,0,0,48))
	vg.stroke()

	vg.fontSize( 20.0)
	vg.fontFace( "sans-bold")
	tw = vg.textBounds( 0,0, text)
	if preicon ~= 0 then
		vg.fontSize( h*1.3)
		vg.fontFace( "icons")
		iw = vg.textBounds( 0,0, vg.cpToUTF8(preicon))
		iw = iw+h*0.15
	end

	if preicon ~= 0 then
		vg.fontSize( h*1.3)
		vg.fontFace( "icons")
		vg.fillColor( vg.rgba(255,255,255,96))
		vg.textAlign(vg.NVG_ALIGN_LEFT+vg.NVG_ALIGN_MIDDLE)
		vg.text( x+w*0.5-tw*0.5-iw*0.75, y+h*0.5, vg.cpToUTF8(preicon))
	end

	vg.fontSize( 20.0)
	vg.fontFace( "sans-bold")
	vg.textAlign(vg.NVG_ALIGN_LEFT+vg.NVG_ALIGN_MIDDLE)
	
	if isdown then
		vg.fillColor( vg.rgba(0,0,0,160))
		vg.text( x+w*0.5-tw*0.5+iw*0.25,y+h*0.5-1+1,text)
		vg.fillColor( vg.rgba(255,255,255,160))
		vg.text( x+w*0.5-tw*0.5+iw*0.25,y+h*0.5+1,text)	
	else
		vg.fillColor( vg.rgba(0,0,0,160))
		vg.text( x+w*0.5-tw*0.5+iw*0.25,y+h*0.5-1,text)
		vg.fillColor( vg.rgba(255,255,255,160))
		vg.text( x+w*0.5-tw*0.5+iw*0.25,y+h*0.5,text)
	end
end

return
{
	onInit=function()
		vg.createFont("icons","fonts/entypo.ttf")
		vg.createFont("sans","fonts/Roboto-Regular.ttf")
		vg.createFont("sans-bold","fonts/Roboto-Bold.ttf")	
	end,
	onRelease=function()
		print("normal onRelease")
	end,
	window={
		onInit=function(self)
			self:enableEvent(ui.EVENT_BREAK+ui.EVENT_TOUCHUP+ui.EVENT_TOUCHDOWN)
		end,
		onRelease=function(self)
		end,
		onDraw=function(self)
			local w,h = self:getSize()
			drawWindow(self._title or "Window",0,0,w,h,self._foucs)
		end,
		onEvent=function(self,event)
			if not event.inside then
				if event.type==ui.EVENT_TOUCHDOWN then
					self._foucs = true
				elseif event.type==ui.EVENT_TOUCHUP then
					self._foucs = false
				end
			end
		end,
		setTitle=function(self,title)
			self._title = title
		end,
		getTitleHeight=function(self)
			return 30
		end,
	},
	button={
		onInit=function(self)		
			self._color = vg.rgba(255,0,0,0)
			self:enableEvent(ui.EVENT_TOUCHDOWN+ui.EVENT_TOUCHUP+ui.EVENT_TOUCHDROP)
		end,
		onRelease=function(self)
		end,	
		onDraw=function(self)
			local w,h = self:getSize()
			drawButton(0,self._title or "OK",0,0,w,h,self._color,self._down)
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
		end,
		setTitle=function(self,title)
			self._title = title
		end,		
	},	
	label={
		onInit=function(self)		
		end,
		onRelease=function(self)
		end,	
		onDraw=function(self)
			local w,h = self:getSize()
			drawLabel(self._string or "",0,0,w,h);
		end,
		onEvent=function(self)
		end,
		setString=function(self,str)
			self._string = str
			vg.save()
			vg.fontSize(18.0)
			vg.fontFace("sans")
			local linewidth,x,y,w,h = vg.textBounds(0,0,str)
			self:setSize(w,h-y)
			vg.restore()
		end,
	},
	progress={
		onInit=function(self)		
		end,
		onRelease=function(self)
		end,	
		onDraw=function(self)
		end,
		onEvent=function(self)
		end,	
	},	
}