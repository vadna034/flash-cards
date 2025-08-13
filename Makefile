# Use C++ compiler
CXX := g++
CPPFLAGS :=        
CXXFLAGS := -MMD -MP -Wall -g
LDLIBS := -lncurses

TARGET := main
SOURCES := $(wildcard *.cpp) $(wildcard */*.cpp)
OBJS := $(SOURCES:.cpp=.o)
DEPS := $(OBJS:.o=.d)

.PHONY: all clean
all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) -o $@ $(LDLIBS)

%.o: %.cpp
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@

-include $(DEPS)

clean:
	rm -f $(TARGET) $(OBJS) $(DEPS)
