copy /Y ..\..\..\luajit\src\*.dll Debug
copy /Y ..\..\..\luajit\src\*.lib Debug
copy /Y ..\..\..\luajit\src\*.exp Debug

copy /Y ..\..\..\glew\bin\Debug\Win32\*.dll Debug
copy /Y ..\..\..\glew\bin\Debug\Win32\*.pdb Debug

junction Debug\images ..\..\src\nanovg\example\images
junction Debug\fonts ..\..\src\nanovg\example
junction Debug\lua ..\..\src\nanovg\lua