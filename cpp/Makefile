### Analog Robot Keypad Controller Makefile ###

############### COMPILER ###############

TARGET=RaspberryPiMotionDetection

############### COMPILER ###############

CC=g++

############### DIRECTORIES ###############

SDIR=./src
BIN=./bin
BUILD=./build
IDIR =./include

############### FLAGS ###############

CFLAGS  = -std=c++11 -g -Wall -O2 -I$(IDIR)
CFLAGS += -I/usr/local
CFLAGS += -I/usr/include/opencv4
CFLAGS += -I/usr/local/include/opencv4

LDFLAGS= -lopencv_highgui -lopencv_core -lopencv_videoio -lopencv_imgproc -lopencv_video

############### LIBRARIES ###############

LIBS  = -L/lib/x86_64-linux-gnu/ 
LIBS += -L/usr/local/lib

############### OBJECTS ###############

DEPS = $(patsubst %.hpp,$(IDIR)/%.d,$(wildcard *.hpp))
OBJ = $(patsubst $(SDIR)/%.cpp, $(BUILD)/%.o, $(wildcard $(SDIR)/*.cpp))


############### BUILD ###############

$(BUILD)/%.o: $(SDIR)/%.cpp $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

$(BIN)/$(TARGET): $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS) $(LDFLAGS)

############### CLEAN ###############

.PHONY: clean

clean:
	rm -rf $(BUILD)/* 
	rm -rf $(BIN)/*  