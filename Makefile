CXX := g++
CXXFLAGS := -Wall -Wextra -std=c++20 -O2 -D_FILE_OFFSET_BITS=64
LDFLAGS := -lssh -lglog -lfuse3
SRCS := channel.cc main.cc sftp.cc session.cc ssh_fuse.cc 
OBJS := $(SRCS:.cc=.o)
TARGET := main

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(OBJS) -o $@ $(LDFLAGS)

%.o: %.cc
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)

.PHONY: all clean