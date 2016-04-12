CC = g++
LD = g++


CFLAGS = -O3 -std=c++1y -c
LIBRARIES = -lpthread -lgnutls

INCLUDES = -Isrc/
SOURCES = $(wildcard src/*.cpp)
OBJECTS = $(SOURCES:.cpp=.o)
DEPENDENCIES = $(OBJECTS:.o=.d)


EXECUTABLE = ftps


.PHONY: ftps
ftps: $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(LD) $(OBJECTS) $(LIBRARIES) -o $(EXECUTABLE)

.cpp.o:
	$(CC) $(CFLAGS) $(INCLUDES) $< -o $@


.PHONY: clean
clean:
	rm -rf src/*.o src/*.d


-include $(DEPENDENCIES)
