add_definitions(-DTRANSLATION_DOMAIN=\"kdevcmakebuilder\")

########### next target ###############

set(cmakebuilder_SRCS
    cmakebuilder.cpp
    cmakejob.cpp
    prunejob.cpp
    cmakebuilderpreferences.cpp
)

ecm_qt_declare_logging_category(cmakebuilder_SRCS
    HEADER debug.h
    IDENTIFIER KDEV_CMAKEBUILDER
    CATEGORY_NAME "kdevelop.projectbuilders.cmakebuilder"
)
ki18n_wrap_ui(cmakebuilder_SRCS cmakebuilderpreferences.ui)
kdevplatform_add_plugin(kdevcmakebuilder JSON kdevcmakebuilder.json SOURCES ${cmakebuilder_SRCS})
target_link_libraries(
    kdevcmakebuilder
    kdevcmakecommon

    KF5::KIOWidgets

    KDev::Interfaces
    KDev::OutputView
    KDev::Util
    KDev::Shell
    KDev::Project
    KDev::IMakeBuilder
)
