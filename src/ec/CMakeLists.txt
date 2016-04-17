add_definitions(-DEC_EXPORTS)
add_library(ec SHARED ec.cpp ec.def)
target_link_libraries(ec gsp)
include_directories(../tthread)
include_directories(../gsp)

install(TARGETS ec DESTINATION ec)
install(FILES ec.h DESTINATION ec)
