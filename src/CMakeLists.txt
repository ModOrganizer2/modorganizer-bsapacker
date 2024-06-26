cmake_minimum_required(VERSION 3.16)

add_library(bsa_packer SHARED)
mo2_configure_plugin(bsa_packer WARNINGS OFF
	PUBLIC_DEPENDS libbsarch
	PRIVATE_DEPENDS boost Qt::Concurrent)

target_include_directories(bsa_packer
	PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} PRIVATE ${BOOST_DI_ROOT}/include)
target_compile_definitions(bsa_packer PRIVATE BSAPACKER_LIBRARY)
target_link_libraries(bsa_packer
	PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../lib/DirectXTex.lib)
target_include_directories(bsa_packer PUBLIC ${DDS_ROOT})

mo2_install_target(bsa_packer)
