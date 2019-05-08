include(ExternalProject)
find_package(Git REQUIRED)

include(cleanCppExtensions)

#------------------------------------------------------------------------------
# Convenience update all, dependencies are added in ExternalHeaderOnly_Add
add_custom_target(update COMMENT "Updated all external projects")

#------------------------------------------------------------------------------
# Eigen - Linear Algebra library

# For details about ExternalHeaderOnly_Add see cmake/cleanCppExtensions.cmake

# target_link_libraries (exampleApp PRIVATE catch)
# #include <Eigen/Core>
ExternalHeaderOnly_Add(Eigen
    "https://github.com/eigenteam/eigen-git-mirror.git" "branches/3.3" "")

# Add fPIC for all dependencies
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

#------------------------------------------------------------------------------
# OpenMesh -> needs to be statically linked to get relocatable binaries (OpenMesh does not
# support cmake imported targets
ExternalDownloadNowGit(OpenMesh
    https://www.graphics.rwth-aachen.de:9000/OpenMesh/OpenMesh.git
    origin/master)
add_subdirectory(${OpenMesh_SOURCE_DIR})
# push back local variables to parent-scope
set (OPENMESH_FOUND        ${OPENMESH_FOUND}        PARENT_SCOPE)
set (OPENMESH_LIBRARIES    ${OPENMESH_LIBRARIES}    PARENT_SCOPE)
set (OPENMESH_INCLUDE_DIR  ${OPENMESH_INCLUDE_DIR}  PARENT_SCOPE)
set (OPENMESH_INCLUDE_DIRS ${OPENMESH_INCLUDE_DIRS} PARENT_SCOPE)

#------------------------------------------------------------------------------
# Examples of other external projects
#------------------------------------------------------------------------------

#------------------------------------------------------------------------------
# target_link_libraries (exampleApp PRIVATE json)
# #include <nlohmann/json.hpp>
# ExternalHeaderOnly_Add(json
#     "https://github.com/nlohmann/json.git" "origin/master" "single_include")

#------------------------------------------------------------------------------
# target_link_libraries (exampleApp PRIVATE spdlog)
# #include <spdlog/spdlog.h>
# Bundled fmt can be used: #include <spdlog/fmt/fmt.h>
# spdlog::error("Some error message with arg: {}", 1);
# ExternalHeaderOnly_Add(spdlog
#     "https://github.com/gabime/spdlog.git" "v1.x" "include")

#------------------------------------------------------------------------------
# target_link_libraries (exampleApp PRIVATE doctest)
# #include <doctest.h>
# ExternalHeaderOnly_Add("DocTest"
#     "https://github.com/onqtam/doctest.git" "origin/master" "src/DocTest")

#------------------------------------------------------------------------------
# target_link_libraries (exampleApp PRIVATE fmt)
# #include <fmt/format.h>
# Use with: `fmt::print("Hello {} - {}!\n", "world", 3.56f);`

# ExternalDownloadNowGit(fmt "https://github.com/fmtlib/fmt.git" "origin/master")
# add_library(fmt STATIC ${fmt_SOURCE_DIR}/src/format.cc)
# target_include_directories(fmt SYSTEM PUBLIC ${fmt_SOURCE_DIR}/include)
# # target_compile_definitions(spdlog INTERFACE -DSPDLOG_FMT_EXTERNAL) #if using SPDLOG


#------------------------------------------------------------------------------
# cpr - HTTP(S) requests library with nice interface

# target_link_libraries (exampleApp PRIVATE fmt)
# #include <cpr/cpr.h>
# auto r = cpr::Get(cpr::Url{"https://ohnheiser.cz"});
# cout<<r.text<<endl;

# find_package(CURL REQUIRED)
# ExternalDownloadNowGit(cpr https://github.com/finkandreas/cpr.git origin/master)
# option(BUILD_CPR_TESTS "" OFF) #disable tests
# option(USE_SYSTEM_CURL "" ON ) #use system curl
# add_subdirectory(${cpr_SOURCE_DIR} EXCLUDE_FROM_ALL)
# # Make headers come as SYSTEM, to get rid of all warning on included headers
# target_include_directories(cpr SYSTEM PUBLIC ${CPR_INCLUDE_DIRS} ${CURL_INCLUDE_DIRS})


