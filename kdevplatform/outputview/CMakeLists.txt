add_definitions(-DTRANSLATION_DOMAIN=\"kdevplatform\")

set(outputviewinterfaces_LIB_SRCS
    outputdelegate.cpp
    outputformats.cpp
    filtereditem.cpp
    ifilterstrategy.cpp
    outputmodel.cpp
    ioutputview.cpp
    ioutputviewmodel.cpp
    outputfilteringstrategies.cpp
    outputjob.cpp
    outputexecutejob.cpp
)
ecm_qt_declare_logging_category(outputviewinterfaces_LIB_SRCS
    HEADER debug.h
    IDENTIFIER OUTPUTVIEW
    CATEGORY_NAME "kdevplatform.outputview"
)
kdevplatform_add_library(KDevPlatformOutputView SOURCES ${outputviewinterfaces_LIB_SRCS})
target_link_libraries(KDevPlatformOutputView PRIVATE
    Qt5::Core
    KDev::Interfaces
    KDev::Util
)

install(FILES
    ioutputview.h
    filtereditem.h
    outputmodel.h
    outputdelegate.h
    outputfilteringstrategies.h
    ioutputviewmodel.h
    ifilterstrategy.h
    outputjob.h
    outputexecutejob.h
    DESTINATION ${KDE_INSTALL_INCLUDEDIR}/kdevplatform/outputview COMPONENT Devel)

if(BUILD_TESTING)
    add_subdirectory(tests)
endif()
