CXX = g++
CXXFLAGS = -std=c++20 -Wall -Wextra

SRCS = main.cpp parser.cpp autocomplete.cpp builtins.cpp executor.cpp line_editor.cpp
OBJS = $(SRCS:.cpp=.o)
TARGET = bshell

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)

.PHONY: all clean
