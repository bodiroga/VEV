
# The source file where the main() function is

SOURCEMAIN = test.c

# Library files

SRC = tools.c trfm3D.c plane.c line.c bbox.c quaternion.c intersect.c bsphere.c

# Don't change anything below
DEBUG = 1

LIBS = -lm -lglut -lGLU -lGL -ljpeg -lGLEW

ifdef DEBUG
OPTFLAGS = -g
else
OPTFLAGS = -O2
endif

CCOPTIONS = -Wall $(OPTFLAGS)
MEMBERS = $(SRC:.c=.o)
EXEC  = $(basename $(notdir $(SOURCEMAIN)))

all: $(EXEC)

%.o : %.c
	gcc -c -o $@ $(CCOPTIONS) $(INCLUDE_DIR) $< 

$(EXEC): $(JPEG_LIB) $(TARGET) $(MEMBERS) $(SOURCEMAIN)
	gcc $(CCOPTIONS) -o $(EXEC) $(SOURCEMAIN) $(MEMBERS) $(INCLUDE_DIR) $(LIBDIR) $(LIBS)


$(JPEG_LIB):
	(cd $(JPEG_LIBDIR); ./configure --enable-static --disable-shared)
	(cd $(JPEG_LIBDIR); make)
	mv $(JPEG_LIBDIR)/.libs/libjpeg.a $(JPEG_LIBDIR)

.PHONY : all clean jpeglib_clean

clean:
	find . -type f -name '*.o' | xargs rm -f
	rm -f $(EXEC)

jpeglib_clean:
	(cd $(JPEG_LIBDIR); make clean)
