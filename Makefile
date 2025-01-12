################################################################
#                                                              #
#         Copyright (c) 2024 by Thomas A. Early N7TAE          #
#                                                              #
# See the LICENSE file for details about the software license. #
#                                                              #
################################################################

# use this if you want debugging help in the case of a crash
#CPPFLAGS=-ggdb -W -Wall -std=c++17

# or, you can choose this for a much smaller executable without debugging help
CPPFLAGS=-W -Wall -std=c++17

LDFLAGS=-L/usr/lib -lrt

SRCS = $(wildcard *.cpp) $(wildcard $(IRC)/*.cpp)
OBJS = $(SRCS:.cpp=.o)
DEPS = $(SRCS:.cpp=.d)

EXES = c2dec c2enc index

all : $(EXES)

c2dec : c2dec.o codec2.o lpc.o nlp.o pack.o kiss_fft.o quantise.o codebooks.o qbase.o
	g++ -o $@ $^ $(LDFLAGS)

c2enc : c2enc.o codec2.o lpc.o nlp.o pack.o kiss_fft.o quantise.o codebooks.o qbase.o
	g++ -o $@ $^ $(LDFLAGS)

index : index.o
	g++ -o $@ $^ $(LDFLAGS)

%.o : %.cpp
	g++ $(CPPFLAGS) -MMD -MD -c $< -o $@

.PHONY: clean

clean:
	$(RM) $(OBJS) $(DEPS) $(EXES)

-include $(DEPS)
