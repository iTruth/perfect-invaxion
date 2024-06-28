cmake_minimum_required (VERSION 3.5)

project(pinvaxion VERSION 1.1 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)

find_package(nlohmann_json REQUIRED)
find_package(spdlog REQUIRED)
find_package(argh REQUIRED)

add_executable(${PROJECT_NAME}
	${CMAKE_CURRENT_LIST_DIR}/src/perfect_invaxion.cpp
	)

add_definitions(-DPROJECT_VERSION=\"v${CMAKE_PROJECT_VERSION}\")

target_include_directories(${PROJECT_NAME} PRIVATE
	${CMAKE_CURRENT_LIST_DIR}/src/
	)

target_link_libraries(${PROJECT_NAME} PRIVATE
	nlohmann_json::nlohmann_json
	spdlog::spdlog
	argh
	)

target_sources(${PROJECT_NAME} PRIVATE
	${CMAKE_CURRENT_LIST_DIR}/src/save/basic_save.h
	${CMAKE_CURRENT_LIST_DIR}/src/save/invaxion_save.h
	${CMAKE_CURRENT_LIST_DIR}/src/save/detail/windows_registry.h
	${CMAKE_CURRENT_LIST_DIR}/src/platform/nt_helper.h
	${CMAKE_CURRENT_LIST_DIR}/src/cmd_param.h
	)

install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION /)
file(GLOB_RECURSE CUSTOM_SAVES save/*.json)
install(FILES ${CUSTOM_SAVES} DESTINATION save/)

include(InstallRequiredSystemLibraries)
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_LIST_DIR}/LICENSE")
set(CPACK_PACKAGE_VERSION_MAJOR ${PROJECT_VERSION_MAJOR})
set(CPACK_PACKAGE_VERSION_MINOR ${PROJECT_VERSION_MINOR})
set(CPACK_GENERATOR "ZIP")
set(CPACK_SOURCE_GENERATOR "ZIP")
include(CPack)

