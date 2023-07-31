
# Add the source files for the sentiment_analysis module
set(SOURCES
    sentiment_analysis.cpp
    # Add other source files if needed
)

# Create the sentiment_analysis library
add_library(sentiment_analysis ${SOURCES})

# Link the required libraries (including cpr)
target_link_libraries(sentiment_analysis PUBLIC cpr::cpr)

# Set the include directories for the sentiment_analysis library
target_include_directories(sentiment_analysis PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

