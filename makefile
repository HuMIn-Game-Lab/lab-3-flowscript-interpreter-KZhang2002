# Compiler and flags
CXX = g++
CXXFLAGS = -std=c++14 -fPIC
INCLUDES = -I./Code/JobSystem  # Include path to JobSystem headers

# Source files for the shared library
LIB_SRC = Code/JobSystem/JobWorkerThread.cpp \
          Code/JobSystem/JobSystem.cpp \
          Code/JobSystem/DefaultJobFactory.cpp \
          Code/JobSystem/CompileJob.cpp \
          Code/JobSystem/RenderJob.cpp

# Source files for the executable
EXE_SRC = Code/main.cpp \
          Code/CustomJob.cpp \
          Code/CustomJobFactory.cpp

# Output directories and library/executable names
OUTPUT_DIR = .
LIB_NAME = libJobSystem.so
EXE_NAME = lab_2_flowscript_dsl_KZhang2002

# Targets
compile: $(LIB_NAME) $(EXE_NAME)

$(LIB_NAME): $(LIB_SRC)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -shared -o $@ $^

$(EXE_NAME): $(EXE_SRC) $(LIB_NAME)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -o $@ $^ -L. -lJobSystem -pthread

clean:
	rm -f $(LIB_NAME) $(EXE_NAME)

.PHONY: compile clean
