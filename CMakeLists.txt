cmake_minimum_required(VERSION 3.16)

project(bsa_packer)
set(project_type plugin)
set(enable_warnings OFF)
set(create_translations ON)

if(DEFINED DEPENDENCIES_DIR)
	include(${DEPENDENCIES_DIR}/modorganizer_super/cmake_common/project.cmake)
else()
	include(../cmake_common/project.cmake)
endif()
add_subdirectory(src)

target_include_directories(bsa_packer PRIVATE ${BOOST_DI_ROOT}/include)
target_link_directories(bsa_packer PRIVATE lib)
target_link_libraries(bsa_packer DirectXTex)
requires_library(libbsarch)
target_compile_definitions(bsa_packer PRIVATE BSAPACKER_LIBRARY)

if(BUILD_TESTING)
add_subdirectory(tests)
endif()
