
file(GLOB imguifiles
		"imgui*.cpp"
		"*.h"
		"examples/imgui_impl_opengl3.h"
		"examples/imgui_impl_opengl3.cpp"
		"examples/imgui_impl_glfw.h"
		"examples/imgui_impl_glfw.cpp")
add_library(ImGui STATIC ${imguifiles})
target_include_directories(ImGui PRIVATE ${THIRD_PARTY_INCLUDE_DIRECTORIES})
