cmake_minimum_required(VERSION 4.0.0)
project(autoHead VERSION 0.3.2)
add_executable(${PROJECT_NAME} main.c libs/string.c)
install(TARGETS ${PROJECT_NAME} DESTINATION bin)
set(CPACK_ARCHIVE_COMPONENT_INSTALL 1)
include(CPack)
