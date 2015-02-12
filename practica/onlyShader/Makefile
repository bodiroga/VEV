
# The source file where the main() function is

SOURCEMAIN = drawCube.c

# Library files

SRC = shaderUtils.c

# Don't change anything below
DEBUG = 1

LIBS = -lm -lglut -lGLU -ljpeg -lGLEW

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

.PHONY : all clean

clean:
	find . -type f -name '*.o' | xargs rm -f
	rm -f $(EXEC)

