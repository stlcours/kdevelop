add_definitions(-DTRANSLATION_DOMAIN=\"kdevmakebuilder\")

########### next target ###############

set(kdevmakebuilder_SRCS
    makebuilder.cpp
    makejob.cpp
    makebuilderpreferences.cpp
)
ecm_qt_declare_logging_category(kdevmakebuilder_SRCS
    HEADER debug.h
    IDENTIFIER KDEV_MAKEBUILDER
    CATEGORY_NAME "kdevelop.projectbuilders.makebuilder"
)
ki18n_wrap_ui(kdevmakebuilder_SRCS makeconfig.ui)
kconfig_add_kcfg_files( kdevmakebuilder_SRCS makebuilderconfig.kcfgc )

#add_library(kdevmakebuilder MODULE ${kdevmakebuilder_SRCS})
qt5_add_resources(kdevmakebuilder_SRCS kdevmakebuilder.qrc)
kdevplatform_add_plugin(kdevmakebuilder JSON kdevmakebuilder.json SOURCES ${kdevmakebuilder_SRCS})
target_link_libraries(kdevmakebuilder
        KF5::TextEditor
        KDev::Interfaces
        KDev::Project
        KDev::OutputView
        KDev::Shell
        KDev::Util
        KDev::Shell
)

install(FILES imakebuilder.h DESTINATION ${KDE_INSTALL_INCLUDEDIR}/kdevelop/makebuilder COMPONENT Devel)

add_library(KDevIMakeBuilder INTERFACE)
add_library(KDev::IMakeBuilder ALIAS KDevIMakeBuilder)
target_include_directories(KDevIMakeBuilder INTERFACE
    "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../>"
    "$<INSTALL_INTERFACE:${KDE_INSTALL_INCLUDEDIR}/kdevelop>"
)
set_target_properties(KDevIMakeBuilder PROPERTIES
    EXPORT_NAME IMakeBuilder
)
install(TARGETS KDevIMakeBuilder EXPORT KDevelopTargets ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})
