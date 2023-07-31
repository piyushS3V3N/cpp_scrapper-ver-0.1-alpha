add_library(web_scraping web_scraping.cpp)
target_include_directories(web_scraping PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(web_scraping PRIVATE ${CURL_LIBRARIES} ${LIBXML2_LIBRARIES})
target_include_directories(web_scraping PRIVATE ${CURL_INCLUDE_DIRS} ${LIBXML2_INCLUDE_DIRS})

