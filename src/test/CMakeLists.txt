include_directories(../tthread)
include_directories(../gsp)
include_directories(../ec)

#add_executable(test-gsp test-gsp.cpp)
#target_link_libraries(test-gsp ec)
#install(TARGETS test-gsp DESTINATION ec)
#install(FILES test-gsp.cpp DESTINATION ec)

add_executable(test-ec test-ec.cpp)
target_link_libraries(test-ec ec)
install(TARGETS test-ec DESTINATION ec)
install(FILES test-ec.cpp DESTINATION ec)
