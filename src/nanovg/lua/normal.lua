local vg = require "vg"

print("normal themes")
local function drawWindow(title,x,y,w,h)
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
	vg.fillPaint(vg.boxGradient(x,y+2, w,h, cornerRadius*2, 10, vg.rgba(0,0,0,128), vg.rgba(0,0,0,0)))
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

return
{
	onInit=function()
	end,
	onRelease=function()
	end,
	window={
		onInit=function(self)
		end,
		onRelease=function(self)
		end,
		onDraw=function(self)
			local w,h = self:getSize()
			drawWindow(self._title or "Window",0,0,w,h)
		end,
		onEvent=function()
			print("window onEvent")
		end,
		setTitle=function(self,title)
			self._title = title
		end,
	},
	button={
		onInit=function(self)
			print("button onInit")
			fontIcons = vg.createFont("icons","fonts/entypo.ttf")
			fontNormal = vg.createFont("sans","fonts/Roboto-Regular.ttf")
			fontBold = vg.createFont("sans-bold","fonts/Roboto-Bold.ttf")			
		end,
		onRelease=function(self)
			print("button onRelease")
		end,	
		onDraw=function(self)
			--print( string.format("x=%d y=%d",x,y) )
			local w,h = self:getSize()
			drawWindow(self._title or "OK",0,0,w,h)
		end,
		onEvent=function()
			print("button onEvent")
		end,
		setTitle=function(self,title)
			self._title = title
		end,		
	},	
}