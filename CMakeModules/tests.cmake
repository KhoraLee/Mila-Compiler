include(CTest)
file(GLOB_RECURSE MILA_SOURCES LIST_DIRECTORIES false CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/samples/*.mila")
file(MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/tests")

# compile tests
foreach(src ${MILA_SOURCES})
	get_filename_component(basename ${src} NAME_WE)
	add_test(NAME "compiler:${basename}" COMMAND "${CMAKE_SOURCE_DIR}/mila" "${src}" "-o" "${CMAKE_BINARY_DIR}/tests/${basename}")
	set_tests_properties("compiler:${basename}" PROPERTIES FIXTURES_SETUP "${basename}")
endforeach()

# run tests
file(GLOB MILA_OUTPUTS LIST_DIRECTORIES false CONFIGURE_DEPENDS "${CMAKE_SOURCE_DIR}/tests/*.run[0-9]*.out")
foreach(out ${MILA_OUTPUTS})
	get_filename_component(outname ${out} NAME)
	get_filename_component(extensionOut ${out} EXT)
	get_filename_component(basename ${out} NAME_WE)
	string(REPLACE "out" "in" extensionIn "${extensionOut}")
	set(inname "${basename}${extensionIn}")

	set(executable ${CMAKE_BINARY_DIR}/tests/${basename})
	set(outfile ${CMAKE_SOURCE_DIR}/tests/${outname})
	set(infile	${CMAKE_SOURCE_DIR}/tests/${inname})

	if(EXISTS "${infile}")
		add_test(NAME "run:${outname}" COMMAND
			${CMAKE_COMMAND}
			-D executable=${executable}
			-D expected=${outfile}
			-D input=${infile}
			-P ${CMAKE_SOURCE_DIR}/CMakeModules/run_test.cmake)
	else()
		add_test(NAME "run:${outname}" COMMAND
			${CMAKE_COMMAND}
			-D executable=${executable}
			-D expected=${outfile}
			-P ${CMAKE_SOURCE_DIR}/CMakeModules/run_test.cmake)
	endif()
	set_tests_properties("run:${outname}" PROPERTIES FIXTURES_REQUIRED "${basename}")
endforeach()
