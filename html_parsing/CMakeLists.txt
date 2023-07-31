add_library(html_parsing html_parsing.cpp)
target_include_directories(html_parsing PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(html_parsing PRIVATE ${LIBXML2_LIBRARIES})
target_include_directories(html_parsing PRIVATE ${LIBXML2_INCLUDE_DIRS})

