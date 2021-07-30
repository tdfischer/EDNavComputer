add_library(libedjournal STATIC
        edjournal.h edjournal.cpp
        journalentry.h journalentry.cpp
        gamestate.h gamestate.cpp
        journalloader.h journalloader.cpp
        location.h location.cpp
        exploration.h exploration.cpp
        systembodies.h systembodies.cpp
)

target_compile_definitions(libedjournal PRIVATE $<$<OR:$<CONFIG:Debug>,$<CONFIG:RelWithDebInfo>>:QT_QML_DEBUG>)
target_link_libraries(libedjournal PRIVATE Qt${QT_VERSION_MAJOR}::Core)
