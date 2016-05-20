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
		data.images[i] = vg.createImage(file, 0)
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
	vg.path{vg.MOVETO,x,y,vg.LINETO,}
	vg.fillPaint(vg.boxGradient(x,y+5, 100,100, cornerRadius*2, 10, vg.rgba(255,0,0,255), vg.rgba(0,0,0,255)))
	vg.fill()		
	
end)