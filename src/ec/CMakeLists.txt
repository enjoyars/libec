add_definitions(-DEC_EXPORTS)
add_library(ec SHARED ec.cpp)
target_link_libraries(ec gsp)
include_directories(../tthread)
include_directories(../gsp)

install(TARGETS ec DESTINATION ec)
install(FILES ec.h DESTINATION ec)



#add_executable(test-ec test-ec.cpp)
#target_link_libraries(test-ec ec)

#install(TARGETS test-ec DESTINATION ec)
#install(FILES test-ec.cpp DESTINATION ec)
