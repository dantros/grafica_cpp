function(MakeExample TARGETNAME FILENAME)
	add_executable(${TARGETNAME} ${FILENAME})
	set_property(TARGET ${TARGETNAME} PROPERTY CXX_STANDARD 20)
	set_property(TARGET ${TARGETNAME} PROPERTY FOLDER examples_libs)
	target_link_libraries(${TARGETNAME} PRIVATE grafica)
	target_include_directories(${TARGETNAME} PRIVATE ${GRAFICA_INCLUDE_DIRECTORY} ${THIRD_PARTY_INCLUDE_DIRECTORIES})

endfunction(MakeExample)

add_compile_definitions(AL_LIBTYPE_STATIC)
MakeExample(ex_openal ex_openal.cpp)
MakeExample(ex_dr_wav ex_dr_wav.cpp)