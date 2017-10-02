# 'make depend' uses makedepend to automatically generate dependencies 
#               (dependencies are added to end of Makefile)
# 'make'        build executable file
# 'make clean'  removes all .o and executable files
#

CFLAGS = -Wall -O3 -DUSE_KISS_FFT --std=gnu++11
INCLUDES = -I./BTrack/src -I./BTrack/libs/kiss_fft130
LFLAGS = -LBTrack/src -LBTrack/libs/kiss_fft130
LIBS = -lbtrack $(shell pkg-config --libs samplerate) -lkissfft -llo

SRCS = main.cpp

# define the C++ object files 

OBJS = $(SRCS:.cpp=.o)

# define the executable file 
MAIN = osc-audio-proc

#
# The following part of the makefile is generic. 

.PHONY: depend clean btrack kissfft 

all:    $(MAIN)

$(MAIN): $(OBJS) btrack kissfft
	$(CXX) $(CFLAGS) $(INCLUDES) -o $(MAIN) $(OBJS) $(LFLAGS) $(LIBS)

btrack:
	$(MAKE) -C BTrack/src

kissfft:
	$(MAKE) -C BTrack/libs/kiss_fft130


# this is a suffix replacement rule for building .o's from .cpp's
# it uses automatic variables $<: the name of the prerequisite of
# the rule(a .c file) and $@: the name of the target of the rule (a .o file) 
# (see the gnu make manual section about automatic variables)
.cpp.o:
	$(CXX) $(CFLAGS) $(INCLUDES) -c $<  -o $@

clean:
	$(RM) *.o *~ $(MAIN)
	$(MAKE) -C BTrack/src clean
	$(MAKE) -C BTrack/libs/kiss_fft130 clean

depend: $(SRCS)
	makedepend $(INCLUDES) $^

# DO NOT DELETE THIS LINE -- make depend needs it
