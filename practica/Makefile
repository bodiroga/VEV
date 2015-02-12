
# The source file where the main() function is

SOURCEMAIN = Browser/browser.c

# Library files

SRC = Math/tools.c Math/trfm3D.c Math/plane.c Math/line.c Math/bbox.c Math/bsphere.c Math/quaternion.c Math/intersect.c\
	  Math/bboxGL.c\
      Geometry/glm.c Geometry/triangleMesh.c Geometry/gObject.c Geometry/gObjectManager.c\
	  Geometry/triangleMeshGL.c\
      Shading/light.c Shading/material.c Shading/texture.c Shading/textureManager.c Shading/materialManager.c Shading/lightManager.c\
	  Shaders/shaderUtils.c Shaders/shaderManager.c Shaders/shader.c\
      Camera/camera.c Camera/avatar.c Camera/cameraManager.c \
	  SceneGraph/renderState.c SceneGraph/node.c SceneGraph/scene.c \
      Misc/list.c Misc/hash.c Misc/hashlib.c Misc/vector.c \
      Math/trfmStack.c \
	  Browser/scenes.c

# Don't change anything below
DEBUG = 1

JPEG_LIBDIR=./libjpeg6b-6b
JPEG_LIB=$(JPEG_LIBDIR)/libjpeg.a

INCLUDE_DIR = -I./Camera -I./Geometry  -I./Math -I./Misc -I./Shading -I./Shaders -I./SceneGraph -I$(JPEG_LIBDIR)
LIBDIR = -L $(JPEG_LIBDIR)
LIBS = -lm -lglut -lGLU -lGL -ljpeg -lGLEW

ifdef DEBUG
OPTFLAGS = -g
else
OPTFLAGS = -O2
endif

CCOPTIONS = -Wall -Wno-unused-function -Wno-unused-variable $(OPTFLAGS)
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

distclean:
	(cd $(JPEG_LIBDIR); make distclean)
