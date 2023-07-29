cmake_minimum_required(VERSION 3.12)
project(WebScrapingSentimentAnalysis)

# Find cpr
include(FetchContent)
FetchContent_Declare(cpr GIT_REPOSITORY https://github.com/libcpr/cpr.git GIT_TAG 0817715923c9705e68994eb52ef9df3f6845beba) # The commit hash for 1.10.x. Replace with the latest from: https://github.com/libcpr/cpr/releases
FetchContent_MakeAvailable(cpr)


# Find jsoncpp
find_package(PkgConfig REQUIRED)
pkg_check_modules(JSONCPP jsoncpp)
include_directories(${JSONCPP_INCLUDE_DIRS})

# Set C++ standard to C++17
set(CMAKE_CXX_STANDARD 17)

# Add the required packages
find_package(CURL REQUIRED)
find_package(LibXml2 REQUIRED)

# Add the executable for the main program
add_executable(main main.cpp)

# Include directories for the main program
target_include_directories(main PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

# Link the required libraries
target_link_libraries(main PRIVATE ${CURL_LIBRARIES} ${LIBXML2_LIBRARIES} ${JSONCPP_LIBRARIES} cpr::cpr )

# Define the include directories for header files
target_include_directories(main PRIVATE ${CURL_INCLUDE_DIRS} ${LIBXML2_INCLUDE_DIRS} ${JSONCPP_INCLUDE_DIRS})

# Add the required subdirectories
add_subdirectory(web_scraping)
add_subdirectory(html_parsing)
add_subdirectory(sentiment_analysis)

# Link the modules to the main executable
target_link_libraries(main PRIVATE web_scraping html_parsing sentiment_analysis )
