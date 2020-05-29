#include "nxshim/glsym.h"

void nxShimResolveGLSyms(void) {
	gladLoadGL(&eglGetProcAddress);
}