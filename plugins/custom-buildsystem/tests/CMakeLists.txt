configure_file(testconfig.h.in testconfig.h)

ecm_add_test(test_custombuildsystemplugin.cpp ${custom_LOG_SRCS}
    TEST_NAME test_custombuildsystemplugin
    LINK_LIBRARIES Qt5::Test KDev::Tests KDev::Project)

# Small sample app loading a dialog with the config-ui and a dummy project
# Useful for testing UI ideas quickly
set( kcm_uitest_SRCS kcmuitestmain.cpp
                     ../custombuildsystemconfigwidget.cpp
                     ../configconstants.cpp
                     ../configwidget.cpp
                     ${custom_LOG_SRCS}
   )
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/..
    ${CMAKE_CURRENT_BINARY_DIR}/..
)
ki18n_wrap_ui( kcm_uitest_SRCS
                        ../configwidget.ui
                        ../custombuildsystemconfigwidget.ui
                 )
add_executable( kcm_uitest ${kcm_uitest_SRCS} )
target_link_libraries( kcm_uitest KDev::Project KDev::Tests )
