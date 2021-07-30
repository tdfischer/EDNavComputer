set(TARGET lib_detours)
add_library(${TARGET} STATIC
	creatwth.cpp
	detours.cpp
	detours.h
	detver.h
	disasm.cpp
	disolarm.cpp
	disolarm64.cpp
	disolia64.cpp
	disolx64.cpp
	disolx86.cpp
	image.cpp
	modules.cpp
	uimports.cpp
)

set_property(
	SOURCE uimports.cpp
	APPEND PROPERTY HEADER_FILE_ONLY true
)

target_compile_options(${TARGET} PRIVATE /W4 /WX /Zi /MT /Gy /Gm- /Zl /Od)
target_include_directories(${TARGET} PUBLIC .)
