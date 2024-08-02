cmake_minimum_required(VERSION 3.16)

set(MO2_CMAKE_DEPRECATED_UIBASE_INCLUDE ON)

project(bsa_packer)

add_subdirectory(src)

set(BSAPACKER_TESTING ${BSAPACKER_TESTING} CACHE BOOL "build tests for bsapacker plugin")
if(BSAPACKER_TESTING)
	enable_testing()
	add_subdirectory(tests)
endif()
