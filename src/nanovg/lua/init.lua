require "nanovg"
local vg = require "vg"

local fontIcons,fontNormal,fontBold
local ICON_SEARCH = 0x1F50D
local ICON_CIRCLED_CROSS = 0x2716
local ICON_CHEVRON_RIGHT  = 0xE75E
local ICON_CHECK = 0x2713
local ICON_LOGIN = 0xE740
local ICON_TRASH = 0xE729

eventFunction("init",function()
	print("init..")
	fontIcons = vg.createFont("icons","fonts/entypo.ttf")
	fontNormal = vg.createFont("sans","fonts/Roboto-Regular.ttf")
	fontBold = vg.createFont("sans-bold","fonts/Roboto-Bold.ttf")
end)

eventFunction("release",function()
	print("release")
end)

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

local function drawSearchBox(text, x, y, w, h)
	local bg
	local cornerRadius = h/2-1
	
	--Edit
	bg = vg.boxGradient(x,y+1.5, w,h, h/2,5, vg.rgba(0,0,0,16), vg.rgba(0,0,0,92))
	vg.beginPath()
	vg.roundedRect(x,y,w,h,cornerRadius)
	vg.fillPaint(bg)
	vg.fill()
	
	vg.fontSize(h*1.3)
	vg.fontFace("icons")
	vg.fillColor(vg.rgba(255,255,255,64))
	vg.textAlign(vg.NVG_ALIGN_CENTER+vg.NVG_ALIGN_MIDDLE)
	vg.text(x+h*0.55, y+h*0.55, vg.cpToUTF8(ICON_SEARCH))
	
	vg.fontSize(20)
	vg.fontFace("sans")
	vg.fillColor(vg.rgba(255,255,255,32))
	vg.textAlign(vg.NVG_ALIGN_LEFT+vg.NVG_ALIGN_MIDDLE)
	vg.text(x+h*1.05,y+h*0.5,text)
	
	vg.fontSize(h*1.3)
	vg.fontFace("icons")
	vg.fillColor(vg.rgba(255,255,255,32))
	vg.textAlign(vg.NVG_ALIGN_CENTER+vg.NVG_ALIGN_MIDDLE)
	vg.text(x+w-h*0.55, y+h*0.55, vg.cpToUTF8(ICON_CIRCLED_CROSS))
end

local function drawDropDown(text, x, y, w, h)
	local bg
	local  cornerRadius = 4

	bg = vg.linearGradient(x,y,x,y+h, vg.rgba(255,255,255,16),  vg.rgba(0,0,0,16))
	vg.beginPath()
	vg.roundedRect(x+1,y+1, w-2,h-2, cornerRadius-1)
	vg.fillPaint(bg)
	vg.fill()	
	
	vg.beginPath()
	vg.roundedRect(x+0.5,y+0.5, w-1,h-1, cornerRadius-0.5)
	vg.strokeColor(vg.rgba(0,0,0,48))
	vg.stroke()
	
	vg.fontSize(20)
	vg.fontFace("sans")
	vg.fillColor(vg.rgba(255,255,255,160))
	vg.textAlign(vg.NVG_ALIGN_LEFT+vg.NVG_ALIGN_MIDDLE)
	vg.text(x+h*0.3,y+h*0.5,text)
	
	vg.fontSize(h*1.3)
	vg.fontFace("icons")
	vg.fillColor(vg.rgba(255,255,255,64))
	vg.textAlign(vg.NVG_ALIGN_CENTER+vg.NVG_ALIGN_MIDDLE)
	vg.text(x+w-h*0.5, y+h*0.5, vg.cpToUTF8(ICON_CHEVRON_RIGHT))
end

local function drawEyes(x, y, w, h, mx, my, t)
	local gloss, bg
	local ex = w *0.23
	local ey = h * 0.5
	local lx = x + ex
	local ly = y + ey
	local rx = x + w - ex
	local ry = y + ey
	local dx,dy,d
	local br
	local blink = 1 - math.pow(math.sin(t*0.5),200)*0.8
	if ex<ey then
		br = ex*0.5
	else
		br = ey*0.5
	end
	bg = vg.linearGradient(x,y+h*0.5,x+w*0.1,y+h, vg.rgba(0,0,0,32), vg.rgba(0,0,0,16))
	vg.beginPath()
	vg.ellipse(lx+3.0,ly+16.0, ex,ey)
	vg.ellipse(rx+3.0,ry+16.0, ex,ey)
	vg.fillPaint(bg)
	vg.fill()

	bg = vg.linearGradient(x,y+h*0.25,x+w*0.1,y+h, vg.rgba(220,220,220,255), vg.rgba(128,128,128,255))
	vg.beginPath()
	vg.ellipse(lx,ly, ex,ey)
	vg.ellipse(rx,ry, ex,ey)
	vg.fillPaint(bg)
	vg.fill()

	dx = (mx - rx) / (ex * 10)
	dy = (my - ry) / (ey * 10)
	d = math.sqrt(dx*dx+dy*dy)
	if d > 1.0 then
		dx = dx/d
		dy = dy/d
	end
	dx = dx*ex*0.4
	dy = dy*ey*0.5
	vg.beginPath()
	vg.ellipse(lx+dx,ly+dy+ey*0.25*(1-blink), br,br*blink)
	vg.fillColor(vg.rgba(32,32,32,255))
	vg.fill()

	dx = (mx - rx) / (ex * 10)
	dy = (my - ry) / (ey * 10)
	d = math.sqrt(dx*dx+dy*dy)
	if d > 1.0 then
		dx = dx/d
		dy = dy/d
	end
	dx = dx*ex*0.4
	dy = dy*ey*0.5
	vg.beginPath()
	vg.ellipse(rx+dx,ry+dy+ey*0.25*(1-blink), br,br*blink)
	vg.fillColor(vg.rgba(32,32,32,255))
	vg.fill()

	gloss = vg.radialGradient(lx-ex*0.25,ly-ey*0.5, ex*0.1,ex*0.75, vg.rgba(255,255,255,128), vg.rgba(255,255,255,0))
	vg.beginPath()
	vg.ellipse(lx,ly, ex,ey)
	vg.fillPaint(gloss)
	vg.fill()

	gloss = vg.radialGradient(rx-ex*0.25,ry-ey*0.5, ex*0.1,ex*0.75, vg.rgba(255,255,255,128), vg.rgba(255,255,255,0))
	vg.beginPath()
	vg.ellipse(rx,ry, ex,ey)
	vg.fillPaint(gloss)
	vg.fill()
end

local function drawParagraph(x, y, width, height, mx, my)
end

local function drawGraph()
end

local function drawColorwheel(x, y, w, h, t)
	local i
	local r0, r1, ax,ay, bx,by, cx,cy, aeps, r
	local hue = math.sin(t * 0.12)
	local paint

	vg.save()

	cx = x + w*0.5
	cy = y + h*0.5
	if w < h then
		r1 = w*0.5-5
	else
		r1 = h*0.5-5
	end
	r0 = r1 - 20
	aeps = 0.5 / r1

	for i=0 ,6 do
		local a0 = i / 6.0 * math.pi * 2.0 - aeps
		local a1 = (i+1.0) / 6.0 * math.pi * 2.0 + aeps
		vg.beginPath()
		vg.arc(cx,cy, r0, a0, a1, vg.NVG_CW)
		vg.arc(cx,cy, r1, a1, a0, vg.NVG_CCW)
		vg.closePath()
		ax = cx + math.cos(a0) * (r0+r1)*0.5
		ay = cy + math.sin(a0) * (r0+r1)*0.5
		bx = cx + math.cos(a1) * (r0+r1)*0.5
		by = cy + math.sin(a1) * (r0+r1)*0.5
		paint = vg.linearGradient(ax,ay, bx,by, vg.hsla(a0/(math.pi*2),1.0,0.55,255), vg.hsla(a1/(math.pi*2),1.0,0.55,255))
		vg.fillPaint(paint)
		vg.fill()
	end

	vg.beginPath()
	vg.circle(cx,cy, r0-0.5)
	vg.circle(cx,cy, r1+0.5)
	vg.strokeColor(vg.rgba(0,0,0,64))
	vg.strokeWidth(1.0)
	vg.stroke()

	--Selector
	vg.save()
	vg.translate(cx,cy)
	vg.rotate(hue*math.pi*2)

	-- Marker on
	vg.strokeWidth(2.0)
	vg.beginPath()
	vg.rect(r0-1,-3,r1-r0+2,6)
	vg.strokeColor(vg.rgba(255,255,255,192))
	vg.stroke()

	paint = vg.boxGradient(r0-3,-5,r1-r0+6,10, 2,4, vg.rgba(0,0,0,128), vg.rgba(0,0,0,0))
	vg.beginPath()
	vg.rect(r0-2-10,-4-10,r1-r0+4+20,8+20)
	vg.rect(r0-2,-4,r1-r0+4,8)
	vg.pathWinding(vg.NVG_HOLE)
	vg.fillPaint(paint)
	vg.fill()

	-- Center triangle
	r = r0 - 6
	ax = math.cos(120.0/180.0*math.pi) * r
	ay = math.sin(120.0/180.0*math.pi) * r
	bx = math.cos(-120.0/180.0*math.pi) * r
	by = math.sin(-120.0/180.0*math.pi) * r
	vg.beginPath()
	vg.path{vg.MOVETO,r,0,
				vg.LINETO,ax,ay,
				vg.LINETO,bx,by,
				vg.CLOSE}
	paint = vg.linearGradient(r,0, ax,ay, vg.hsla(hue,1.0,0.5,255), vg.rgba(255,255,255,255))
	vg.fillPaint(paint)
	vg.fill()
	paint = vg.linearGradient((r+ax)*0.5,(0+ay)*0.5, bx,by, vg.rgba(0,0,0,0), vg.rgba(0,0,0,255))
	vg.fillPaint(paint)
	vg.fill()
	vg.strokeColor(vg.rgba(0,0,0,64))
	vg.stroke()

	-- Select circle on triangle
	ax = math.cos(120.0/180.0*math.pi) * r*0.3
	ay = math.sin(120.0/180.0*math.pi) * r*0.4
	vg.strokeWidth(2.0)
	vg.beginPath()
	vg.circle(ax,ay,5)
	vg.strokeColor(vg.rgba(255,255,255,192))
	vg.stroke()

	paint = vg.radialGradient(ax,ay, 7,9, vg.rgba(0,0,0,64), vg.rgba(0,0,0,0))
	vg.beginPath()
	vg.rect(ax-20,ay-20,40,40)
	vg.circle(ax,ay,7)
	vg.pathWinding(vg.NVG_HOLE)
	vg.fillPaint(paint)
	vg.fill()

	vg.restore()

	vg.restore()
end

local function drawLines(x, y, w, h, t)
	local i, j
	local pad = 5.0
	local s = w/9.0 - pad*2
	local pts, fx, fy
	pts = {}
	local joins = {vg.NVG_MITER, vg.NVG_ROUND, vg.NVG_BEVEL}
	local caps = {vg.NVG_BUTT, vg.NVG_ROUND, vg.NVG_SQUARE}
	
	-- NVG_NOTUSED(h)

	vg.save()
	
	pts[0] = -s*0.25 + math.cos(t*0.3) * s*0.5
	pts[1] = math.sin(t*0.3) * s*0.5
	pts[2] = -s*0.25
	pts[3] = 0
	pts[4] = s*0.25
	pts[5] = 0
	pts[6] = s*0.25 + math.cos(-t*0.3) * s*0.5
	pts[7] = math.sin(-t*0.3) * s*0.5

	for i=1,3 do
		for j=1,3 do
			fx = x + s*0.5 + ((i-1)*3+j-1)/9.0*w + pad
			fy = y - s*0.5 + pad

			vg.lineCap( caps[i])
			vg.lineJoin( joins[j])

			vg.strokeWidth( s*0.3)
			vg.strokeColor( vg.rgba(0,0,0,160))
			vg.beginPath()
			vg.path{vg.MOVETO,fx+pts[0], fy+pts[1],
						vg.LINETO,fx+pts[2], fy+pts[3],
						vg.LINETO,fx+pts[4], fy+pts[5],
						vg.LINETO,fx+pts[6], fy+pts[7]}
			vg.stroke()

			vg.lineCap( vg.NVG_BUTT)
			vg.lineJoin( vg.NVG_BEVEL)

			vg.strokeWidth( 1.0)
			vg.strokeColor( vg.rgba(0,192,255,255))
			vg.beginPath()
			vg.path{vg.MOVETO,fx+pts[0], fy+pts[1],
						vg.LINETO,fx+pts[2], fy+pts[3],
						vg.LINETO,fx+pts[4], fy+pts[5],
						vg.LINETO,fx+pts[6], fy+pts[7]}			
			vg.stroke()
		end
	end


	vg.restore()
end

local function drawWidths(x, y, width)
	local i;

	vg.save()

	vg.strokeColor( vg.rgba(0,0,0,255))

	for i = 1,20 do
		local w = (i+0.5)*0.1
		vg.strokeWidth(w)
		vg.beginPath()
		vg.path{vg.MOVETO,x,y,vg.LINETO,x+width,y+width*0.3}
		vg.stroke()
		y = y+10
	end

	vg.restore()
end

local function drawCaps(x, y, width)
	local i
	local caps = {vg.NVG_BUTT, vg.NVG_ROUND, vg.NVG_SQUARE}
	local lineWidth = 8.0

	vg.save()

	vg.beginPath()
	vg.rect( x-lineWidth/2, y, width+lineWidth, 40)
	vg.fillColor( vg.rgba(255,255,255,32))
	vg.fill()

	vg.beginPath()
	vg.rect( x, y, width, 40)
	vg.fillColor( vg.rgba(255,255,255,32))
	vg.fill()

	vg.strokeWidth( lineWidth)
	for i = 1,3 do
		vg.lineCap( caps[i])
		vg.strokeColor( vg.rgba(0,0,0,255))
		vg.beginPath()
		vg.path{vg.MOVETO,x, y + (i-1)*10 + 5,vg.LINETO,x+width, y + (i-1)*10 + 5}
		vg.stroke()
	end

	vg.restore()
end

local function drawScissor(x, y, t)
	vg.save()

	-- Draw first rect and set scissor to it's area.
	vg.translate(x, y)
	vg.rotate( vg.degToRad(5))
	vg.beginPath()
	vg.rect( -20,-20,60,40)
	vg.fillColor( vg.rgba(255,0,0,255))
	vg.fill()
	vg.scissor( -20,-20,60,40)

	-- Draw second rectangle with offset and rotation.
	vg.translate(40,0)
	vg.rotate( t)

	-- Draw the intended second rectangle without any scissoring.
	vg.save()
	vg.resetScissor(vg)
	vg.beginPath()
	vg.rect( -20,-10,60,30)
	vg.fillColor( vg.rgba(255,128,0,64))
	vg.fill()
	vg.restore()

	-- Draw second rectangle with combined scissoring.
	vg.intersectScissor(-20,-10,60,30)
	vg.beginPath()
	vg.rect( -20,-10,60,30)
	vg.fillColor( vg.rgba(255,128,0,255))
	vg.fill()

	vg.restore()
end

local fps_count = 0
local fps_acc = 0
local fps = 0
local t = 0
eventFunction("loop",function(dt)
	local w,h = screenSize()
	local x,y
	local width,height,mx,my
	
	t = t+dt
	mx = 0
	my = 0
	width = w
	height = h
	
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
	vg.fillColor(vg.rgbaf(1,1,1,1))
	vg.text(32,32,tostring(fps))
	
	drawEyes(width - 250, 50, 150, 100, mx, my, t)
	drawParagraph(width - 450, 50, 150, 100, mx, my)
	drawGraph(0, height/2, width, height/2, t)
	drawColorwheel(width - 300, height - 300, 250, 250, t)

	--Line joints
	drawLines(120, height-50, 600, 50, t)

	--Line caps
	drawWidths(10, 50, 30)

	--Line caps
	drawCaps(10, 300, 30)

	drawScissor(50, height-80, t)
	
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
