require "nanovg"
local vg = require "vg"
local ui = require "ui"

local data = {images={}}

eventFunction("init",function()
	print("init..")
	fontIcons = vg.createFont("icons","fonts/entypo.ttf")
	fontNormal = vg.createFont("sans","fonts/Roboto-Regular.ttf")
	fontBold = vg.createFont("sans-bold","fonts/Roboto-Bold.ttf")
	for i=0,12-1 do
		local file = string.format("images/image%d.jpg",i+1)
		data.images[i] = vg.createImage(file, vg.NVG_IMAGE_REPEATX+vg.NVG_IMAGE_REPEATY)
		if data.images[i] == 0 then
			print(string.format("Could not load %s.\n", file))
			return -1
		end
	end
end)

eventFunction("release",function()
	print("release")
end)

eventFunction("input",function(e,pt)
end)

--[[
	text
--]]
local d = 0
eventFunction("loop",function(dt)
	local x,y = 100,50
	local cornerRadius = 12
	vg.fontSize( 18.0)
	vg.fontFace( "sans")
	
	vg.beginPath()
	vg.rect(x,y,100,100)
	vg.fillPaint(vg.linearGradient(x,y,x,y+100, vg.rgba(255,0,0,255), vg.rgba(0,0,0,255)))
	vg.fill()
--	vg.fillColor(vg.rgba(255,255,255,255))
--	vg.text(x,y-32,"vg.linearGradient")
--	vg.text(x,y-10,"(x,y,x,y+100, vg.rgba(255,0,0,255), vg.rgba(0,0,0,255))")
	
	vg.beginPath()
	vg.translate(200,0)
	vg.rect(x,y,120,120)
	vg.fillPaint(vg.boxGradient(x+10,y+10, 100,100, cornerRadius*2, 20, vg.rgba(255,0,0,255), vg.rgba(0,0,0,255)))
	vg.fill()	
	
	vg.beginPath()
	vg.translate(200,0)
	vg.rect(x,y,100,100)
	vg.fillPaint(vg.radialGradient(x+50,y+50, 10,50, vg.rgba(255,0,0,255), vg.rgba(0,0,0,255)))
	vg.fill()	

	vg.beginPath()
	vg.translate(200,0)
	vg.translate(160,120)
	local pt = {}
	for i=1,6 do
		local xx = 80*math.sin((i-1)*math.pi/3)
		local yy = 80*math.cos((i-1)*math.pi/3)
		if i==1 then
			table.insert(pt,vg.MOVETO)
			table.insert(pt,xx)
			table.insert(pt,yy)
		else
			table.insert(pt,vg.LINETO)
			table.insert(pt,xx)
			table.insert(pt,yy)
		end
	end
	--table.insert(pt,vg.CLOSE)
	vg.path(pt)
	--vg.translate(-80,-80)
	--vg.scale(1.5,1.5)
	vg.fillPaint(vg.imagePattern(0,0, 133,100, 0, data.images[0],1))
	vg.fill()		
	
	vg.resetTransform()	
	vg.translate(0,200)
	vg.beginPath()
	vg.rect(x,y,100,100)
	vg.fillPaint(vg.linearGradient(x,y,x,y+100, vg.rgba(255,0,0,255), vg.rgba(0,0,255,255)))
	vg.fill()
	
	vg.beginPath()
	vg.translate(200,0)
	vg.rect(x,y,120,120)
	vg.fillPaint(vg.boxGradient(x+10,y+10, 100,100, cornerRadius*2, 20, vg.rgba(0,0,0,128), vg.rgba(0,0,0,0)))
	vg.fill()	
	
	vg.beginPath()
	vg.translate(200,0)
	vg.rect(x,y,100,100)
	vg.fillPaint(vg.radialGradient(x+50,y+50, 30,50, vg.rgba(0,0,0,255), vg.rgba(0,0,0,0)))
	vg.fill()	

	vg.beginPath()
	vg.translate(200,0)
	vg.roundedRect(x,y,300,300,cornerRadius)
	vg.fillPaint(vg.imagePattern(x,y, 133,100, 0, data.images[0],1))
	vg.fill()			
end)