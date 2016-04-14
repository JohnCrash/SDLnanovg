local vg = require "vg"

vg.NVG_SOLID = 1
vg.NVG_HOLE = 2

vg.MOVETO = 1
vg.LINETO = 2
vg.BEZIERTO = 3
vg.QUADTO = 4
vg.ARCTO = 5
vg.CLOSE = 6

vg.NVG_ALIGN_LEFT = 0
vg.NVG_ALIGN_CENTER = 2
vg.NVG_ALIGN_RIGHT = 4
vg.NVG_ALIGN_TOP = 8
vg.NVG_ALIGN_MIDDLE = 16
vg.NVG_ALIGN_BOTTOM = 32
vg.NVG_ALIGN_BASELINE = 64

vg.rgba = function(r,g,b,a)
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

vg.imageGradient = function(ox,oy,ex,ey,angle,alpha,image)
	return {type=3,ox=ox,oy=oy,ex=ey,angle=angle,alpha=alpha,image=image}
end
