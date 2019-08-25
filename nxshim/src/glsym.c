#include "nxshim/glsym.h"

void nxShimResolveGLSyms(void) {
	rglgen_resolve_symbols(&eglGetProcAddress);
}