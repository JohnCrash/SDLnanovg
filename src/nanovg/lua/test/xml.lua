local xml = require "slaxml"

local fonts_xml = io.open('D:/source/SDLnanovg/src/nanovg/lua/test/fonts.xml'):read('*all')

if fonts_xml then
	xml:parse(fonts_xml,{
		startElement = function(name,nsURI,nsPrefix)
		end, 
		attribute    = function(name,value,nsURI,nsPrefix) end, 
		closeElement = function(name,nsURI)
		end, 
		text         = function(text)                      end, 
		comment      = function(content)                   end, 
		pi           = function(target,content)            end, 
	})
else
	print("Can't open file fonts.xml")
end
