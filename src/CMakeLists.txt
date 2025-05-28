cmake_minimum_required(VERSION 3.16)

find_package(Qt6 COMPONENTS Concurrent)
find_path(BEXT_DI_INCLUDE_DIRS "boost/di.hpp")
find_package(mo2-cmake CONFIG REQUIRED)
find_package(mo2-uibase CONFIG REQUIRED)
find_package(mo2-libbsarch CONFIG REQUIRED)
find_package(mo2-dds-header CONFIG REQUIRED)
find_package(DirectXTex CONFIG REQUIRED)

add_library(bsa_packer SHARED)
mo2_configure_plugin(bsa_packer WARNINGS OFF)

target_link_libraries(bsa_packer PUBLIC mo2::uibase Qt6::Concurrent mo2::libbsarch Microsoft::DirectXTex)
target_include_directories(bsa_packer PUBLIC ${BEXT_DI_INCLUDE_DIRS} ${CMAKE_CURRENT_SOURCE_DIR})
target_compile_definitions(bsa_packer PRIVATE BSAPACKER_LIBRARY)

# need to deploy this here as MO2 does not depends on Qt6::Concurrent
install(FILES $<TARGET_FILE:Qt6::Concurrent> DESTINATION bin/dlls)

# this is done by modorganizer itself now
#
# install(FILES $<TARGET_FILE:mo2::libbsarch> DESTINATION bin/dlls)

mo2_install_plugin(bsa_packer)
