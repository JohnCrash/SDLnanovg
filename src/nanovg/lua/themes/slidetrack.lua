local slidetrack = {}
local REMOVE_THRESHOLD = 256
local REMOVE_COUNT = 128
local TIME_THRESHOLD = 0.2

slidetrack.new=function()
	local obj = {}
	setmetatable(obj,slidetrack)
	obj._events = {}
	return obj
end

function slidetrack:track(event)
	if #self._events >= REMOVE_THRESHOLD then
		for i = 1,REMOVE_COUNT do
			self._events[i] = self._events[REMOVE_COUNT+i]
			self._events[REMOVE_COUNT+i] = nil
		end
	end
	table.insert(self._events,event)
end

function slidetrack:velocity()
	local cur = tick()
	local lenght = #self._events
	local pt = {}
	for i = lenght,1,-1 do
		if cur - self._events[i].time < TIME_THRESHOLD then
			table.insert(pt,self._events[i])
		end
	end
	if #pt > 1 then
		local dt = pt[1].time-pt[#pt].time
		if dt<=0 then dt = TIME_THRESHOLD end
		return {x=(pt[1].x-pt[#pt].x)/dt,y=(pt[1].y-pt[#pt].y)/dt}
	end
	return {x=0,y=0}
end
	
slidetrack.__index = slidetrack

return slidetrack