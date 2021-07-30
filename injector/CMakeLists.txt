add_library(injector-shim MODULE injector.cpp)
target_link_libraries(injector-shim PRIVATE Pathcch.lib)

add_custom_command(
    OUTPUT
        ${CMAKE_CURRENT_BINARY_DIR}/quad.vert.h
    COMMAND
        fxc /T vs_5_0 /E quad_vs_main /Fh ${CMAKE_CURRENT_BINARY_DIR}/quad.vert.h ${CMAKE_CURRENT_SOURCE_DIR}/quad.vert
    COMMENT
        "Compiling vertex shader quad.vert"
    DEPENDS
        ${CMAKE_CURRENT_SOURCE_DIR}/quad.vert
    VERBATIM
)

add_custom_command(
    OUTPUT
        ${CMAKE_CURRENT_BINARY_DIR}/quad.frag.h
    COMMAND
        fxc /T ps_5_0 /E quad_ps_main /Fh ${CMAKE_CURRENT_BINARY_DIR}/quad.frag.h ${CMAKE_CURRENT_SOURCE_DIR}/quad.frag
    COMMENT
        "Compiling fragment shader quad.frag"
    DEPENDS
        ${CMAKE_CURRENT_SOURCE_DIR}/quad.frag
    VERBATIM
)

add_library(injector MODULE
    overlayapi.cpp overlayapi.h
    dxwindow.h dxwindow.cpp
    renderloophook.h renderloophook.cpp
    quad.vert quad.frag
    ${CMAKE_CURRENT_BINARY_DIR}/quad.vert.h
    ${CMAKE_CURRENT_BINARY_DIR}/quad.frag.h
)

target_link_libraries(injector PRIVATE Qt${QT_VERSION_MAJOR}::Core Qt${QT_VERSION_MAJOR}::Quick lib_detours libnavgui libedjournal Pathcch.lib)
