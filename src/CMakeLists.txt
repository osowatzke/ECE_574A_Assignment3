set(SOURCES
    AlapScheduler.cpp
    AsapScheduler.cpp
    DataManager.cpp
    FileParser.cpp
    FileWriter.cpp
    FsmGenerator.cpp
    HighLevelSynthesisTool.cpp)

set(HEADER_FILES
    AlapScheduler.h
    AsapScheduler.h
    CircuitComponents.h
    DataManager.h
    FileParser.h
    FileWriter.h
    FsmGenerator.h
    GraphComponents.h
    HighLevelSynthesisTool.h)

add_executable(hlsyn ${SOURCES} ${HEADER_FILES})