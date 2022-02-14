find_package(alexandria REQUIRED)

set(CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH};${CMAKE_CURRENT_SOURCE_DIR}/../bettertest")
include(bettertestVersionString)

set(NAME bettertest_alexandria)
set(TYPE module)
set(INCLUDE_DIR "include/bettertest_alexandria")
set(SRC_DIR "src")

set(HEADERS
    ${INCLUDE_DIR}/alexandria_exporter.h
    ${INCLUDE_DIR}/alexandria_importer.h
    ${INCLUDE_DIR}/library.h
)
 
set(SOURCES
    ${SRC_DIR}/alexandria_exporter.cpp
    ${SRC_DIR}/alexandria_importer.cpp
    ${SRC_DIR}/library.cpp
)

set(DEPS_PUBLIC
    bettertest
)

set(DEPS_PRIVATE
    alexandria::alexandria
)

make_target(
    NAME ${NAME}
    TYPE ${TYPE}
    VERSION ${BETTERTEST_VERSION}
    WARNINGS WERROR
    HEADERS "${HEADERS}"
    SOURCES "${SOURCES}"
    DEPS_PUBLIC "${DEPS_PUBLIC}"
    DEPS_PRIVATE "${DEPS_PRIVATE}"
)

install_target(
    NAME ${NAME}
    TYPE ${TYPE}
    VERSION ${BETTERTEST_VERSION}
    HEADERS "${HEADERS}"
)
