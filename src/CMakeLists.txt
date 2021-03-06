project(core)

add_library(core STATIC
	ARM.cpp
	ARMInterpreter.cpp
	ARMInterpreter_ALU.cpp
	ARMInterpreter_Branch.cpp
	ARMInterpreter_LoadStore.cpp
	Config.cpp
	CP15.cpp
	CRC32.cpp
	DMA.cpp
	GPU.cpp
	GPU2D.cpp
	GPU3D.cpp
	GPU3D_OpenGL.cpp
	GPU3D_Soft.cpp
	NDS.cpp
	NDSCart.cpp
	OpenGLSupport.cpp
	RTC.cpp
	Savestate.cpp
	SPI.cpp
	SPU.cpp
	Wifi.cpp
	WifiAP.cpp
)

find_package(OpenGL REQUIRED)
find_package(glfw3 3.3 REQUIRED)

if (WIN32)
	target_link_libraries(core ole32 comctl32 ws2_32 ${OPENGL_gl_LIBRARY} glfw)
else()
	target_link_libraries(core ${OPENGL_gl_LIBRARY} glfw)
endif()
