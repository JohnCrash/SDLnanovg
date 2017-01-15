local ui = require "ui"
local vg = require "vg"

local themes = "normal"
local sw,sh = screenSize() --540,960
print( string.format("screenSize %d , %d",sw,sh) )
local activeLayout

local function relayout()
	if activeLayout then
		local w,h = screenSize()
		print( string.format("screenSize %d , %d",sw,sh) )
		activeLayout:setSize(w,h)
		activeLayout:relayout()
	end
end

--加入设置项
local function addSettingItem(scroll,text,imagefile,rightText,arrowFunc)
	local layout = ui.createWidget(themes,"layout")
	scroll:addChild(layout)
	
	--插入图标和标题
	local image = ui.createWidget(themes,"image")
	local label = ui.createWidget(themes,"label")

	if imagefile then
		layout:addChild(image)
		layout:addChild(label)	
		image:load(imagefile)
		image:setAlign(ui.ALIGN_LEFT+ui.ALIGN_MIDDLE,6,0)
		image:setSize(52,52)
		local w,h = image:getSize()
		label:setString(text)
		label:setFontSize(20)
		label:setFontBold(true)
		label:setAlign(ui.ALIGN_LEFT+ui.ALIGN_MIDDLE,w+12,0)
		
		layout:setSize(sw,64)
	else
		layout:addChild(label)	
		label:setString(text)
		label:setFontSize(20)
		label:setFontBold(true)
		label:setAlign(ui.ALIGN_LEFT+ui.ALIGN_MIDDLE,12,0)
		
		layout:setSize(sw,32)
	end
	--插入一条横线
	local line = ui.createWidget(themes,"vgw")
	line:setAlign(ui.ALIGN_CENTER+ui.ALIGN_BOTTOM)
	line:setLayoutMatch(ui.WIDTH_MATCH_PARENT)
	line:draw(function(vg,w,h)
		vg.beginPath()
		vg.path{vg.MOVETO,0,0}
		vg.path{vg.LINETO,w,0}
		vg.strokeColor( vg.rgba(0,0,0,32))
		vg.strokeWidth( 1.0)
		vg.stroke()
	end)
	line:setSize(sw,1)
	layout:addChild(line)
	
	local hasFunc = arrowFunc and type(arrowFunc) == "function"
	--右侧文本
	if rightText and type(rightText) == "string" then
		local rightLabel = ui.createWidget(themes,"label")
		rightLabel:setString(rightText)
		local ox = 0
		if hasFunc then
			ox = -28-12
		end
		rightLabel:setAlign(ui.ALIGN_RIGHT+ui.ALIGN_MIDDLE,ox,0)
		layout:addChild(rightLabel)
	end
	--如果有进一步的界面这个是触发函数
	if hasFunc then
		local arrow = ui.createWidget(themes,"vgw")
		arrow:setAlign(ui.ALIGN_RIGHT+ui.ALIGN_MIDDLE,-6,0)
		arrow:draw(function(vg,w,h)
			vg.beginPath()
			vg.path{vg.MOVETO,0,0}
			vg.path{vg.LINETO,w,h/2}
			vg.path{vg.LINETO,0,h}
			vg.strokeColor(vg.rgba(0,0,0,64))
			vg.strokeWidth(1.0)
			vg.stroke()
		end)
		arrow:setSize(12,28)
		layout:addChild(arrow)
		layout:enableEvent(ui.EVENT_TOUCHDOWN+ui.EVENT_TOUCHUP+ui.EVENT_TOUCHDROP)
		layout.onClick = arrowFunc
	end

	return layout
end

--加入标题项
local function addSpeareItem(scroll,text)
	local layout = ui.createWidget(themes,"layout")
	scroll:addChild(layout)
	local label = ui.createWidget(themes,"label")
	layout:addChild(label)
	label:setString(text)
	label:setFontSize(20)
	label:setFontBold(true)	
	label:setAlign(ui.ALIGN_LEFT+ui.ALIGN_MIDDLE,20,0)
	layout:setBGColor(vg.rgba(0,0,0,128))
	
	layout:setSize(sw,28)
	return layout	
end

--创建WLAN设置
local function createWLANLayout()
	local root = ui.rootWidget()

	local baseLayout = ui.createWidget(themes,"layout")
	
	local titleLayout = ui.createWidget(themes,"layout")
	local title = ui.createWidget(themes,"label")
	title:setString("WLAN")
	title:setFontSize(26)
	title:setAlign(ui.ALIGN_CENTER+ui.ALIGN_MIDDLE)
	titleLayout:addChild(title)
	titleLayout:setAlign(ui.ALIGN_LEFT+ui.ALIGN_TOP)
	titleLayout:setLayoutMatch(ui.WIDTH_MATCH_PARENT)
	titleLayout:setBGColor(vg.rgba(0,0,0,128))
	titleLayout:setSize(sw,32)
	
	baseLayout:addChild(titleLayout)
	
	local scrollLayout = ui.createWidget(themes,"scroll")
	scrollLayout:configScroll(ui.VERTICAL+ui.ALIGN_LEFT,0,0,true)
	scrollLayout:setAlign(ui.ALIGN_LEFT+ui.ALIGN_BOTTOM)
	scrollLayout:setLayoutMatch(ui.FILL_PARENT,0,32)
	
	baseLayout:addChild(scrollLayout)
	
	addSpeareItem(scrollLayout,"开关")
	local fly_layout = addSettingItem(scrollLayout,"WLAN")
	local switch = ui.createWidget(themes,"switch")
	switch:setAlign(ui.ALIGN_RIGHT+ui.ALIGN_MIDDLE,-12,0)
	fly_layout:addChild(switch)

	addSettingItem(scrollLayout,"WLAN+","images/image3.jpg")
	addSpeareItem(scrollLayout,"可用WLAN列表")
	
	scrollLayout:relayout()
	
	root:addChild(baseLayout)
	
	baseLayout:setSize(sw,sh)
	return baseLayout
end

--创建一个为实现的提示栏
local function createMessageBox()

end

--创建设置首页
local function createSettingLayout()

	local root = ui.rootWidget()
	local w,h = root:getSize()
	print("==================")
	print(string.format("root getSize %d,%d",w,h))
	print("==================")
	local baseLayout = ui.createWidget(themes,"layout")
	
	local titleLayout = ui.createWidget(themes,"layout")
	local title = ui.createWidget(themes,"label")
	title:setString("设置")
	title:setFontSize(26)
	title:setAlign(ui.ALIGN_CENTER+ui.ALIGN_MIDDLE)
	titleLayout:addChild(title)
	titleLayout:setAlign(ui.ALIGN_LEFT+ui.ALIGN_TOP)
	titleLayout:setLayoutMatch(ui.WIDTH_MATCH_PARENT)
	titleLayout:setBGColor(vg.rgba(0,0,0,128))
	titleLayout:setSize(w,32)
	
	baseLayout:addChild(titleLayout)
	
	local scrollLayout = ui.createWidget(themes,"scroll")
	scrollLayout:configScroll(ui.VERTICAL+ui.ALIGN_LEFT,0,0,true)
	scrollLayout:setAlign(ui.ALIGN_LEFT+ui.ALIGN_BOTTOM)
	scrollLayout:setLayoutMatch(ui.FILL_PARENT,0,32)
	baseLayout:addChild(scrollLayout)
	
	--加入查找选项编辑栏
	local searchLayout = ui.createWidget(themes,"layout")
	local search = ui.createWidget(themes,"edit")
	search:setLayoutMatch(ui.WIDTH_MATCH_PARENT,16,16)
	search:setSize(sw,64-32)
	search:setAlign(ui.ALIGN_CENTER+ui.ALIGN_MIDDLE)
	searchLayout:addChild(search)
	searchLayout:setSize(sw,64)

	scrollLayout:addChild(searchLayout)
	
	addSpeareItem(scrollLayout,"网络")
	--为飞行模式加入一个开关
	local fly_layout = addSettingItem(scrollLayout,"飞行模式","images/image12.jpg")
	local switch = ui.createWidget(themes,"switch")
	switch:setAlign(ui.ALIGN_RIGHT+ui.ALIGN_MIDDLE,-12,0)
	fly_layout:addChild(switch)
	
	addSettingItem(scrollLayout,"双卡管理","images/image1.jpg","",
		function()
			ui.popupUI(activeLayout,createMessageBox())
		end)
	addSettingItem(scrollLayout,"WLAN","images/image2.jpg","0450_lejiaolex",
		function()
			local wlan = createWLANLayout()
			--activeLayout:setVisible(false)
			ui.switchUI(activeLayout,wlan,ui.RIGHT)
			activeLayout = wlan
		end)
	addSettingItem(scrollLayout,"蓝牙","images/image3.jpg","已开启",function()end)
	addSettingItem(scrollLayout,"流量管理","images/image4.jpg","",function()end)
	addSettingItem(scrollLayout,"更多","images/image5.jpg","",function()end)
	addSpeareItem(scrollLayout,"界面")
	addSettingItem(scrollLayout,"桌面风格","images/image6.jpg","标准风格",function()end)
	addSettingItem(scrollLayout,"显示","images/image7.jpg","",function()end)
	addSettingItem(scrollLayout,"声音","images/image8.jpg","",function()end)
	addSettingItem(scrollLayout,"通知栏和状态栏","images/image9.jpg","",function()end)
	addSettingItem(scrollLayout,"用户","images/image10.jpg","",function()end)
	addSpeareItem(scrollLayout,"安全")
	addSettingItem(scrollLayout,"指纹","images/image11.jpg","",function()end)
	addSettingItem(scrollLayout,"锁屏和密码","images/image12.jpg","",function()end)
	addSettingItem(scrollLayout,"智能辅助","images/image1.jpg","",function()end)
	addSettingItem(scrollLayout,"免扰","images/image2.jpg","已关闭",function()end)
	addSpeareItem(scrollLayout,"应用")
	addSettingItem(scrollLayout,"应用管理","images/image3.jpg","",function()end)
	addSettingItem(scrollLayout,"权限管理","images/image4.jpg","",function()end)
	addSettingItem(scrollLayout,"应用分身","images/image5.jpg","",function()end)
	addSpeareItem(scrollLayout,"高级")
	addSettingItem(scrollLayout,"高级设置","images/image6.jpg","",function()end)
	addSettingItem(scrollLayout,"开发人员选项","images/image7.jpg","",function()end)
	addSettingItem(scrollLayout,"系统更新","images/image8.jpg","",function()end)
	addSettingItem(scrollLayout,"关于手机","images/image9.jpg","",function()end)
	
	root:addChild(baseLayout)
	scrollLayout:relayout()
	
	baseLayout:setSize(w,h)
	return baseLayout
end

eventFunction("init",function()
	setWindowSize(540,960)
	setWindowTitle("Themes library test")
	ui.loadThemes(themes,"themes/default")
	
	activeLayout = createSettingLayout()
	--activeLayout = createWLANLayout()
end)

eventFunction("window",function(eventName,p1,p2)
	if eventName=='sizeChanged' then
		relayout()
	end
end)