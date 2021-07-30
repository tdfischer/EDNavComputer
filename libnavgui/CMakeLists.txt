add_library(libnavgui STATIC gui.cpp gui.h qml.qrc)

target_compile_definitions(libnavgui
  PRIVATE $<$<OR:$<CONFIG:Debug>,$<CONFIG:RelWithDebInfo>>:QT_QML_DEBUG>)
target_link_libraries(libnavgui
  PRIVATE Qt${QT_VERSION_MAJOR}::Core Qt${QT_VERSION_MAJOR}::Quick libedjournal)
