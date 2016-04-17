option(BUILD_WRAP_PYTHON "Build API wrap for Python." OFF)
option(BUILD_WRAP_JAVA "Build API wrap for Java." OFF)
option(BUILD_WARP_CSHAPR "Build API wrap for C#." OFF)


if(BUILD_WRAP_PYTHON OR BUILD_WRAP_JAVA OR BUILD_WARP_CSHAPR)
    set_source_files_properties(ec.i PROPERTIES CPLUSPLUS ON)
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
    set_property(TARGET _ec-python PROPERTY OUTPUT_NAME "_ec")
    install(TARGETS _ec-python DESTINATION ec-python)
    install(FILES ../test/test-ec-python.py ${CMAKE_CURRENT_BINARY_DIR}/ec.py
        DESTINATION ec-python)
endif()

if(BUILD_WARP_CSHAPR)
    swig_add_module(ec-csharp csharp ec.i ../ec/ec.cpp)
    swig_link_libraries(ec-csharp gsp)

    set_property(TARGET ec-csharp PROPERTY OUTPUT_NAME "ec")
    install(TARGETS ec-csharp DESTINATION ec-csharp)
    install(FILES
        ../test/test-ec-csharp.cs
        DESTINATION ec-csharp)
    install(DIRECTORY
        ${CMAKE_CURRENT_BINARY_DIR}
        DESTINATION ec-csharp
        FILES_MATCHING REGEX ".+\\.cs$"
        PATTERN CMakeFiles EXCLUDE)
endif()

if(BUILD_WRAP_JAVA)
    find_package(Java)
    find_package(JNI)
    set_source_files_properties(ec.i PROPERTIES SWIG_FLAGS "-package;com.enjoyars")
    include_directories(${JNI_INCLUDE_DIRS})
    swig_add_module(ec-java java ec.i ../ec/ec.cpp)
    swig_link_libraries(ec-java ${JNI_LIBRARIES} gsp)
    install(TARGETS ec-java DESTINATION ec-java)
    install(FILES ../test/test-ec-java.java DESTINATION ec-java)
    install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/
        DESTINATION ec-java/com/enjoyars
        FILES_MATCHING PATTERN "*.java"
        PATTERN CMakeFiles EXCLUDE)
endif()
