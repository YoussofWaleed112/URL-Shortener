CXX      = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O2
TARGET   = url_project.exe
SRC = main.cpp cpp_files/*.cpp
SHELL    = cmd.exe

all: $(TARGET) data

$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) -o $@ $^

data:
	if not exist data mkdir data

clean:
	del /Q $(TARGET) 2>nul
	if exist data rmdir /S /Q data

.PHONY: all clean data
