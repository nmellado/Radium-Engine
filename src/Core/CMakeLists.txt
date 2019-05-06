set(ra_core_target radiumCore)

file(GLOB_RECURSE core_sources ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
file(GLOB_RECURSE core_headers ${CMAKE_CURRENT_SOURCE_DIR}/*.h ${CMAKE_CURRENT_SOURCE_DIR}/*.hpp)
file(GLOB_RECURSE core_inlines ${CMAKE_CURRENT_SOURCE_DIR}/*.inl)

add_library(
    ${ra_core_target} SHARED
    ${core_sources}
    ${core_headers}
    ${core_inlines}
    ${RADIUM_VERSION_FILE} #This file is build-dependant and generated in the Bundle
)

addCommonDependencies( ${ra_core_target} )
target_link_libraries( ${ra_core_target} PUBLIC OpenMesh )

target_compile_definitions(${ra_core_target} PRIVATE "-DRA_CORE_EXPORTS")

