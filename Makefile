# Makefile
CXX = g++
CXXFLAGS = -Wall -O2 -std=c++17
GTK_CFLAGS = `pkg-config --cflags gtk+-3.0`
GTK_LIBS = `pkg-config --libs gtk+-3.0`
SSL_LIBS = -lssl -lcrypto

TARGET = proga
SOURCES = main.cpp PasswordGenerator.cpp
OBJECTS = $(SOURCES:.cpp=.o)

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJECTS) $(GTK_LIBS) $(SSL_LIBS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@ $(GTK_CFLAGS)

clean:
	rm -f $(OBJECTS) $(TARGET)

run: $(TARGET)
	./$(TARGET)

.PHONY: all clean run
