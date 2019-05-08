set(ra_core_target RadiumCore)

project( ${ra_core_target}
         LANGUAGES CXX
         VERSION ${Radium_VERSION})

file(GLOB_RECURSE core_sources ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
file(GLOB_RECURSE core_headers ${CMAKE_CURRENT_SOURCE_DIR}/*.h ${CMAKE_CURRENT_SOURCE_DIR}/*.hpp)
file(GLOB_RECURSE core_inlines ${CMAKE_CURRENT_SOURCE_DIR}/*.inl)

add_library(
    ${ra_core_target} SHARED
    ${core_sources}
    ${core_headers}
    ${core_inlines}
)
add_library(Radium::Core ALIAS ${ra_core_target})

addCommonDependencies( ${ra_core_target} )
add_dependencies     ( ${ra_core_target} OpenMeshCoreStatic OpenMeshToolsStatic )
target_include_directories( ${ra_core_target} PUBLIC ${OPENMESH_INCLUDE_DIR} )
target_link_libraries (${ra_core_target} PRIVATE OpenMeshCoreStatic OpenMeshToolsStatic)

target_compile_definitions(${ra_core_target} PRIVATE "-DRA_CORE_EXPORTS")

set_target_properties(${ra_core_target} PROPERTIES PUBLIC_HEADER "${core_headers};${core_inlines}")
install(TARGETS ${ra_core_target}
    CONFIGURATIONS ${CMAKE_BUILD_TYPE}
    LIBRARY  DESTINATION lib
    ARCHIVE  DESTINATION lib
    )

INSTALL ( DIRECTORY . DESTINATION include/Core FILES_MATCHING PATTERN "*.h*")
INSTALL ( DIRECTORY . DESTINATION include/Core FILES_MATCHING PATTERN "*.inl")
