set(GRAFICA_HEADERS 
		basic_shapes.h
		easy_shaders.h
		gpu_shape.h
		load_shaders.h
		performance_monitor.h
		scene_graph.h
		shape.h
		simple_eigen.h
		transformations.h
		simple_timer.h
		)
set(GRAFICA_SOURCES
		basic_shapes.cpp
		easy_shaders.cpp
		gpu_shape.cpp
		load_shaders.cpp
		performance_monitor.cpp
		scene_graph.cpp
		shape.cpp
		transformations.cpp
		)

add_library(grafica STATIC ${GRAFICA_SOURCES} ${GRAFICA_HEADERS} grafica.h ${Shaders})
if (MSVC)
    target_compile_options(grafica PUBLIC /wd5033)
endif(MSVC)
target_include_directories(grafica PRIVATE ${THIRD_PARTY_INCLUDE_DIRECTORIES} GRAFICA_INCLUDE_DIRECTORY)
target_link_libraries(grafica PRIVATE ${THIRD_PARTY_LIBRARIES})
set_property(TARGET grafica PROPERTY CXX_STANDARD 20)
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${GRAFICA_SOURCES} ${GRAFICA_HEADERS})

