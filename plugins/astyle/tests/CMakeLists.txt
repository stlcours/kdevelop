include_directories(${CMAKE_CURRENT_BINARY_DIR}/..)

set(test_astyle_SRCS test_astyle.cpp
  ../astyle_formatter.cpp
  ../astyle_stringiterator.cpp
    ${kdevastyle_LOG_SRCS}
)

ecm_add_test(${test_astyle_SRCS}
    TEST_NAME test_astyle
    LINK_LIBRARIES astylelib Qt5::Test KDev::Interfaces KDev::Util)

