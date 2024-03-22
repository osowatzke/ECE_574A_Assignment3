set(SOURCES
    DataManager.cpp
    HighLevelSynthesisTool.cpp
    FileParser.cpp)

set(HEADER_FILES
    DataManager.h
    GraphComponents.h
    HighLevelSynthesisTool.h
    FileParser.h)

add_executable(hlsyn ${SOURCES} ${HEADER_FILES})