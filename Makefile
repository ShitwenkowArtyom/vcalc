CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -g
LDFLAGS = -lcryptopp

TARGET = server

SRCS = main.cpp \
       UserInterface.cpp \
       Logger.cpp \
       AuthBase.cpp \
       CryptoAuth.cpp \
       Calculator.cpp \
       Network.cpp \
       ClientHandler.cpp \
       Server.cpp

OBJS = $(SRCS:.cpp=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)

$(OBJS): errors.h Params.h UserInterface.h Logger.h AuthBase.h CryptoAuth.h Calculator.h Network.h ClientHandler.h Server.h
