option(BUILD_WRAP_PYTHON "Build API wrap for Python." OFF)
option(BUILD_WRAP_JAVA "Build API wrap for Java." OFF)

if(BUILD_WRAP_PYTHON OR BUILD_WRAP_JAVA)
	set_source_files_properties(../ec.i PROPERTIES CPLUSPLUS ON)
	add_definitions(-DSWIG)
	find_package(SWIG)
	include(${SWIG_USE_FILE})
	include_directories(../ec)
	include_directories(../tthread)
	include_directories(../gsp)
endif()

if(BUILD_WRAP_PYTHON)
	find_package(PythonLibs)
	include_directories(${PYTHON_INCLUDE_DIRS})
    swig_add_module(ec-python python ec.i ../ec/ec.cpp)
	swig_link_libraries(ec-python ${PYTHON_LIBRARIES} gsp)
	set_target_properties(_ec-python PROPERTIES OUTPUT_NAME "_ec")
	install(TARGETS _ec-python DESTINATION ec-python)
	install(FILES test-ec-python.py ${CMAKE_CURRENT_BINARY_DIR}/ec.py
	    DESTINATION ec-python)
endif()

if(BUILD_WRAP_JAVA)
	find_package(Java)
	find_package(JNI)
	set_source_files_properties(../ec.i PROPERTIES SWIG_FLAGS "-package;com.enjoyars")
	include_directories(${PYTHON_INCLUDE_DIRS})
	swig_add_module(ec-java java ../ec.i ../ec/ec.cpp)
	swig_link_libraries(ec-java ${JNI_LIBRARIES} gsp)
	set_target_properties(rf21x-java PROPERTIES OUTPUT_NAME "ec")
	install(TARGETS ec-java DESTINATION ec-java)
	install(FILES test-ec-java.java DESTINATION ec-java)
	install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/
	    DESTINATION ec-java/com/enjoyars
	    FILES_MATCHING PATTERN "*.java"
	    PATTERN CMakeFiles EXCLUDE)
endif()


