require "nanovg"
local vg = require "vg"

local fontIcons,fontNormal,fontBold

eventFunction("init",function()
	fontIcons = vg.createFont("icons","fonts/entypo.ttf")
	fontNormal = vg.createFont("sans","fonts/Roboto-Regular.ttf")
	fontBold = vg.createFont("sans-bold","fonts/Roboto-Bold.ttf")
end)

eventFunction("release",function()print("release")end)

local function drawWindow(title,x,y,w,h)
	local cornerRadius = 3.0;
	vg.save()
	
	--window
	vg.beginPath()
	vg.roundedRect(x,y,w,h,cornerRadius)
	vg.fillColor(vg.rgba(28/256,30/256,34/256,192/256))
	vg.fill()
	
	--drop shadow
	vg.beginPath()
	vg.rect(x-10,y-10,w+20,h+30)
	vg.roundedRect(x,y,w,h,cornerRadius)
	vg.pathWinding(vg.NVG_HOLE)
	vg.fillPaint(vg.boxGradient(x,y+2, w,h, cornerRadius*2, 10, vg.rgba(0,0,0,128/256), vg.rgba(0,0,0,0)))
	vg.fill()
	
	--header
	vg.beginPath()
	vg.roundedRect(x+1,y+1, w-2,30, cornerRadius-1)
	vg.fillPaint(vg.linearGradient(x,y,x,y+15, vg.rgba(255/256,255/256,255/256,8/256), vg.rgba(0,0,0,16/256)))
	vg.fill()
	vg.beginPath()
	vg.path{vg.MOVETO,x+0.5,y+0.5+30,vg.LINETO,x+0.5+w-1,y+0.5+30}
	vg.strokeColor(vg.rgba(0,0,0,32/256))
	vg.stroke()
	
	vg.fontSize(18)
	vg.fontFace("sans-bold")
	vg.textAlign(vg.NVG_ALIGN_CENTER+vg.NVG_ALIGN_MIDDLE)
	vg.fontBlur(2)
	vg.fillColor(vg.rgba(0,0,0,128/256))
	vg.text(x+w/2,y+16+1,title)
	
	vg.fontBlur(0)
	vg.fillColor(vg.rgba(220/256,220/256,220/256,160/256))
	vg.text(x+w/2,y+16,title)
	
	vg.restore()
end

local function drawSearchBox(text, x, y, w, h)
	local bg
	local cornerRadius = h/2-1
	
	--Edit
	bg = vg.boxGradient(x,y+1.5, w,h, h/2,5, vg.rgba(0,0,0,16/256), vg.rgba(0,0,0,92/256))
	vg.beginPath()
	vg.roundedRect(x,y,w,h,cornerRadius)
	vg.fillPaint(bg)
	vg.fill()
	
	vg.fontSize(h*1.3)
	vg.fontFace("icons")
	vg.fillColor(vg.rgba(1,1,1,64/256))
	vg.textAlign(vg.NVG_ALIGN_CENTER+vg.NVG_ALIGN_MIDDLE)
	--vg.text(x+h*0.55f, y+h*0.55f, cpToUTF8(ICON_SEARCH,icon));
	
	vg.fontSize(20)
	vg.fontFace("sans")
	vg.fillColor(vg.rgba(1,1,1,32/256))
	vg.textAlign(vg.NVG_ALIGN_LEFT+vg.NVG_ALIGN_MIDDLE)
	vg.text(x+h*1.05,y+h*0.5,text);
	
	vg.fontSize(h*1.3)
	vg.fontFace("icons")
	vg.fillColor(vg.rgba(1,1,1,32/256))
	vg.textAlign(vg.NVG_ALIGN_CENTER+vg.NVG_ALIGN_MIDDLE)
	--vg.text(x+w-h*0.55f, y+h*0.55f, cpToUTF8(ICON_CIRCLED_CROSS,icon));
end

local function drawDropDown()
end

local fps_count = 0
local fps_acc = 0
local fps = 0
eventFunction("loop",function(dt)
	local w,h = screenSize()
	local x,y
	vg.beginNanoVG(w,h)
	vg.beginFrame(w,h,1)
	
	if fps_count < 300 then
		fps_acc = fps_acc+dt
		fps_count = fps_count+1
	else
		fps = math.floor(fps_count/(fps_acc+dt))
		fps_count = 0
		fps_acc = 0
	end
	vg.fontSize(32)
	vg.fontFace("sans")
	vg.fillColor(vg.rgba(1,1,1,1))
	vg.text(32,32,tostring(fps))
	--Widgets
	drawWindow("Widgets `n Stuff", 50, 50, 300, 400)
	x = 60
	y = 95
	drawSearchBox("Search", x,y,280,25)
	y = y+40
	drawDropDown("Effects", x,y,280,28)
	popy = y + 14
	y = y+45
	
	vg.endFrame()
end)
