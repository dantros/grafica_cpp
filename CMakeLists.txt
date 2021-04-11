set(CMAKE_LEGACY_CYGWIN_WIN32 OFF)
cmake_minimum_required(VERSION 3.15)
project(grafica C CXX)
find_package(OpenGL REQUIRED)
add_definitions(-D_USE_MATH_DEFINES)
set(THIRD_PARTY_INCLUDE_DIRECTORIES "${CMAKE_CURRENT_SOURCE_DIR}/third_party/glad/include"
									"${CMAKE_CURRENT_SOURCE_DIR}/third_party/glfw-3.3.2/include"
									"${CMAKE_CURRENT_SOURCE_DIR}/third_party/glm-0.9.9.8"
									"${CMAKE_CURRENT_SOURCE_DIR}/third_party/imgui-1.78"
									"${CMAKE_CURRENT_SOURCE_DIR}/third_party/openal-soft/include"
									"${CMAKE_CURRENT_SOURCE_DIR}/third_party/OpenMesh-8.1/src"
									"${CMAKE_CURRENT_SOURCE_DIR}/third_party/dr_wav"
									"${CMAKE_CURRENT_SOURCE_DIR}/third_party/assimp-5.0.1/include"
									"${CMAKE_CURRENT_SOURCE_DIR}/third_party/stb"
									"${CMAKE_CURRENT_SOURCE_DIR}/third_party/eigen-3.3.9")
set(GRAFICA_INCLUDE_DIRECTORY "${CMAKE_SOURCE_DIR}/source/")

if(MSVC)
	set(THIRD_PARTY_LIBRARIES glad glfw ${OPENGL_LIBRARIES} ImGui ${BULLET_LIBRARIES} OpenAL dr_wav assimp stb OpenMeshCore)
else()
	set(THIRD_PARTY_LIBRARIES glad glfw ${OPENGL_LIBRARIES} ImGui ${BULLET_LIBRARIES} stdc++fs OpenAL dr_wav assimp stb OpenMeshCore)
endif(MSVC)

configure_file(CMakeConfigFiles/root_directory.h.in "${CMAKE_SOURCE_DIR}/source/grafica/root_directory.h")
add_subdirectory(third_party)
add_subdirectory(source)
