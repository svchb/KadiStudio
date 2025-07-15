set(SRCS
        kadiintegrationplugin.cpp
        src/kadiintegration.cpp
        src/domain/recordinfo.cpp
        src/domain/recordfileinfo.cpp
        src/domain/templateinfo.cpp
        src/utils/kadiutils.cpp
        src/dialogs/downloadfromkadidialog.cpp
        src/dialogs/uploadtokadidialog.cpp
        src/dialogs/createnewrecorddialog.cpp
        )

set(RCCS kadiintegration.qrc)
QT5_ADD_RESOURCES(RCC_SRCS ${RCCS})

add_library(kadistudio_kadiintegration SHARED ${SRCS} ${RCC_SRCS})

target_link_libraries(kadistudio_kadiintegration ${KADISTUDIO_FRAMEWORK} Qt5::Widgets Qt5::Network)

string(REPLACE ${PROJECT_SOURCE_DIR} "" outputdestination ${CMAKE_CURRENT_SOURCE_DIR})

set(outputdirectory ${COMMON_LIBRARY_OUTPUT_DIRECTORY}${outputdestination})

set_target_properties(kadistudio_kadiintegration PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${outputdirectory})
