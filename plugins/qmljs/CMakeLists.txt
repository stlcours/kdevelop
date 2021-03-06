add_subdirectory(3rdparty/qtcreator-libs)
add_subdirectory(duchain)
add_subdirectory(nodejsmodules)
if(BUILD_TESTING)
    add_subdirectory(tests)
endif()
add_subdirectory(codecompletion)

ecm_qt_declare_logging_category(kdevqmljslanguagesupport_LOG_SRCS
    HEADER debug.h
    IDENTIFIER KDEV_QMLJS
    CATEGORY_NAME "kdevelop.languages.qmljs"
)
kdevplatform_add_plugin(kdevqmljslanguagesupport JSON kdevqmljs.json SOURCES
    qmljsparsejob.cpp
    qmljshighlighting.cpp
    kdevqmljsplugin.cpp
    navigation/propertypreviewwidget.cpp
    ${kdevqmljslanguagesupport_LOG_SRCS}
)
target_link_libraries(kdevqmljslanguagesupport
    kdevqmljsduchain
    kdevqmljscompletion
    Qt5::Widgets
    Qt5::QuickWidgets
    KF5::Declarative
    KDev::DefinesAndIncludesManager
)

install(DIRECTORY navigation/propertywidgets DESTINATION ${KDE_INSTALL_DATADIR}/kdevqmljssupport)
