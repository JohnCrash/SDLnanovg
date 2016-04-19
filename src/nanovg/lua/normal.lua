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
	window={
		onDraw=function()
			print("window onDraw")
		end,
		onEvent=function()
			print("window onEvent")
		end,
	},
	button={
		onDraw=function(self,x,y,w,h)
			drawWindow(self.title or "OK",x,y,w,h)
		end,
		onEvent=function()
			print("button onEvent")
		end,
	},	
}