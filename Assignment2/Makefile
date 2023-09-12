# Simple Makefile for 6.837 raycasting assignment
# for g++ on linux platforms

# ===============================================================

SRCS 	 	= LAlib/matrix.cpp \
		  Imglib/image.cpp \
		  scene_parser.cpp \
		  raytracer.cpp \
		  camera.cpp \
		  object3d.cpp

# ===============================================================

LIBS            = -lm -L/usr/X11R6/lib
CC		= g++
EXE	  	= raytracer
OBJS =  $(SRCS:.cpp=.o)

# ===============================================================
# targets

.PHONY: all depend clean

all: depend $(EXE)

depend:
	$(CC) -E -M $(SRCS) > Makefile.depend

clean: 
	-rm -f *~ *bak *.o *obj core.* Makefile.depend $(EXE) 

# ===============================================================
# compilation rules

$(EXE): $(OBJS)
	$(CC) -o $@ $(OBJS) $(LIBS)

.C.o:
	$(CC) $< -c -o $@

.cpp.o:
	$(CC) $< -c -o $@

# ===============================================================

-include Makefile.depend
