CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -g
LDFLAGS = -lcryptopp

TARGET = server
TEST_TARGET = test

SRCS = main.cpp \
       UserInterface.cpp \
       Logger.cpp \
       AuthBase.cpp \
       CryptoAuth.cpp \
       Calculator.cpp \
       Network.cpp \
       ClientHandler.cpp \
       Server.cpp

TEST_SRCS = test.cpp \
            UserInterface.cpp \
            Logger.cpp \
            AuthBase.cpp \
            CryptoAuth.cpp \
            Calculator.cpp \
            Network.cpp \
            ClientHandler.cpp \
            Server.cpp

OBJS = $(SRCS:.cpp=.o)
TEST_OBJS = $(TEST_SRCS:.cpp=.o)

all: $(TARGET)

test: $(TEST_TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

$(TEST_TARGET): $(TEST_OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS) -lUnitTest++

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TEST_OBJS) $(TARGET) $(TEST_TARGET)

$(OBJS): errors.h Params.h UserInterface.h Logger.h AuthBase.h CryptoAuth.h Calculator.h Network.h ClientHandler.h Server.h
$(TEST_OBJS): errors.h Params.h UserInterface.h Logger.h AuthBase.h CryptoAuth.h Calculator.h Network.h ClientHandler.h Server.h
