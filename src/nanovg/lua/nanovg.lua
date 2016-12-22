local vg = require "vg"
--init vg
vg.FONS_INVALID = -1

vg.NVG_SOLID = 1
vg.NVG_HOLE = 2

vg.MOVETO = 1
vg.LINETO = 2
vg.BEZIERTO = 3
vg.QUADTO = 4
vg.ARCTO = 5
vg.CLOSE = 6

vg.NVG_ALIGN_LEFT = 1
vg.NVG_ALIGN_CENTER = 2
vg.NVG_ALIGN_RIGHT = 4
vg.NVG_ALIGN_TOP = 8
vg.NVG_ALIGN_MIDDLE = 16
vg.NVG_ALIGN_BOTTOM = 32
vg.NVG_ALIGN_BASELINE = 64

vg.NVG_IMAGE_GENERATE_MIPMAPS	= 1
vg.NVG_IMAGE_REPEATX			= 2
vg.NVG_IMAGE_REPEATY			= 4
vg.NVG_IMAGE_FLIPY				= 8
vg.NVG_IMAGE_PREMULTIPLIED		= 16
	
vg.NVG_CCW = 1
vg.NVG_CW = 2

vg.NVG_BUTT = 0
vg.NVG_ROUND = 1
vg.NVG_SQUARE = 2
vg.NVG_BEVEL = 3
vg.NVG_MITER = 4

vg.NVG_TEXTURE_ALPHA = 1
vg.NVG_TEXTURE_RGBA = 2

vg.rgba = function(r,g,b,a)
	return {r=r/255,g=g/255,b=b/255,a=a/255}
end

vg.rgbaf = function(r,g,b,a)
	return {r=r,g=g,b=b,a=a}
end

vg.hsla = function(h,s,l,a)
	local r,g,b,a = vg.HSLA(h,s,l,a)
	return {r=r,g=g,b=b,a=a}
end

vg.boxGradient = function(x,y,w,h,r,f,ico1,ico2)
	return {type=4,x=x,y=y,w=w,h=h,r=r,f=f,ico1=ico1,ico2=ico2}
end

vg.linearGradient = function(sx,sy,ex,ey,ico1,ico2)
	return {type=1,sx=sx,sy=sy,ex=ex,ey=ey,ico1=ico1,ico2=ico2}
end

vg.radialGradient = function(cx,cy,inr,outr,icol,ocol)
	return {type=2,cx=cx,cy=cy,inr=inr,outr=outr,icol=icol,ocol=ocol}
end

vg.imagePattern = function(ox,oy,ex,ey,angle,image,alpha)
	return {type=3,ox=ox,oy=oy,ex=ex,ey=ey,angle=angle,alpha=alpha,image=image}
end

--init ui
local ui = require "ui"

ui.INVISIBLE = 0
ui.VISIBLE = 1
ui.LINEAR = 2
ui.CLIP = 4
ui.SCROLL_CLIP = 8

ui.ALIGN_LEFT = 1
ui.ALIGN_CENTER = 2
ui.ALIGN_RIGHT = 4
ui.ALIGN_TOP = 8
ui.ALIGN_MIDDLE = 16
ui.ALIGN_BOTTOM = 32
ui.ALIGN_BASELINE = 64
ui.HORIZONTAL = 128 --horizontal
ui.VERTICAL = 256 --vertical
ui.GRID = 512 --grid
ui.REVERSE = 1024 --reverse

ui.WRAP_CONTENT = 0
ui.WIDTH_WRAP_CONTENT = 1
ui.HEIGHT_WRAP_CONTENT = 2
ui.WIDTH_MATCH_PARENT = 4
ui.HEIGHT_MATCH_PARENT = 8
ui.FILL_PARENT = 12

ui.EVENT_NONE = 0;
ui.EVENT_TOUCHDOWN = 1
ui.EVENT_TOUCHDROP = 2
ui.EVENT_TOUCHUP = 4
ui.EVENT_ZOOM = 8
ui.EVENT_BREAK = 16
ui.EVENT_EXCLUSIVE = 32
ui.EVENT_UNBOUNDED = 64

ui.rect=function(x,y,width,height)
	return {x=x,y=y,width=width,height=height}
end

ui.linearRelayout = function(rect,list,align,space)
	space = space or 0
	if isand(align,ui.HORIZONTAL) then --horizontal
		local width = space
		local height = 0
		for i,widget in ipairs(list) do
			local w,h = widget:getSize()
			width = width + w + space
			height = math.max(h,height)
		end
		local x,y
		if isand(align,ui.ALIGN_LEFT) then
			x = space
		elseif isand(align,ui.ALIGN_CENTER) then
			x = (rect.width-width)/2+space
		elseif isand(align,ui.ALIGN_RIGHT) then
			x = rect.width-width+space
		else
			x = space
		end
		local start,len,step
		if isand(align,ui.ALIGN_REVERSE) then
			len = 1
			start = #list
			step = -1
		else
			len = #list
			start = 1
			step = 1
		end
		for i = start,len,step do
			local widget = list[i]
			local w,h = widget:getSize()
			if isand(align,ui.ALIGN_TOP) then
				y = 0
			elseif isand(align,ui.ALIGN_MIDDLE) then
				y = (rect.height-h)/2
			elseif isand(align,ui.ALIGN_BOTTOM) then
				y = rect.height-h
			else
				y = (rect.height-h)/2
			end
			widget:setPosition(rect.x+x,rect.y+y)
			x = x + w + space
		end
		return width,height
	else --vertical
		local width = 0
		local height = space
		for i,widget in ipairs(list) do
			local w,h = widget:getSize()
			height = height + h + space
			width = math.max(w,width)
		end
		local x,y
		if isand(align,ui.ALIGN_TOP) then
			y = space
		elseif isand(align,ui.ALIGN_MIDDLE) then
			y = (rect.height-height)/2+space
		elseif isand(align,ui.ALIGN_BOTTOM) then
			y = rect.height-height+space
		else
			y = space
		end
		local start,len,step
		if isand(align,ui.ALIGN_REVERSE) then
			len = 1
			start = #list
			step = -1
		else
			len = #list
			start = 1
			step = 1
		end
		for i = start,len,step do
			local widget = list[i]
			local w,h = widget:getSize()
			if isand(align,ui.ALIGN_LEFT) then
				x = 0
			elseif isand(align,ui.ALIGN_CENTER) then
				x = (rect.width-w)/2
			elseif isand(align,ui.ALIGN_RIGHT) then
				x = rect.width-w
			else
				x = (rect.width-w)/2
			end
			widget:setPosition(rect.x+x,rect.y+y)
			y = y + h + space
		end	
		return width,height
	end
end