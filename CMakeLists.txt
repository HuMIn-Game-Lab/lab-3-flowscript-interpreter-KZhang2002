cmake_minimum_required(VERSION 3.21)
project(lab_3_flowscript_interpreter_KZhang2002)

set(CMAKE_CXX_STANDARD 14)


add_library(JobSystem SHARED
        Code/JobSystem/JobWorkerThread.cpp
        Code/JobSystem/JobSystem.cpp
        Code/JobSystem/DefaultJobFactory.cpp
        Code/JobSystem/CompileJob.cpp
        Code/JobSystem/RenderJob.cpp
        Code/JobSystem/FlowScriptInterpreter.cpp)

find_package(Threads REQUIRED)
target_link_libraries(JobSystem Threads::Threads)

add_executable(lab_3_flowscript_interpreter_KZhang2002
        Code/main.cpp
        Code/CustomJob.cpp
        Code/CustomJobFactory.cpp)

set_target_properties(JobSystem PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

target_include_directories(JobSystem PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/Code/JobSystem)

target_link_libraries(lab_3_flowscript_interpreter_KZhang2002 JobSystem)
