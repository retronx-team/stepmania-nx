#pragma once

#include <nxshim/glsym.h>

#define gluGetString glGetString
#define gluErrorString(error) RageDisplay_Legacy_Helpers::GLToString(error).c_str()
#define GLU_VERSION GL_VERSION

static inline GLenum gluBuild2DMipmaps(GLenum target,
	GLint internalfmt,
	GLsizei w,
	GLsizei h,
	GLenum fmt,
	GLenum type,
	const void * data
) {
	glTexImage2D(target, 0, internalfmt, w, h, 0, fmt, type, data);

	GLenum error = glGetError();
	if(error == 0) {
		glGenerateMipmap(GL_TEXTURE_2D);
	}

	return error;
}
