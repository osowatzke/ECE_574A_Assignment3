set(SOURCES
    AlapScheduler.cpp
    DataManager.cpp
    FileParser.cpp
    HighLevelSynthesisTool.cpp)

set(HEADER_FILES
    AlapScheduler.h
    CircuitComponents.h
    DataManager.h
    FileParser.h
    GraphComponents.h
    HighLevelSynthesisTool.h)

add_executable(hlsyn ${SOURCES} ${HEADER_FILES})