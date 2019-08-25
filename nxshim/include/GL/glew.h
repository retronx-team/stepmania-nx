#pragma once

#include <nxshim/glsym.h>
#include <GL/glu.h>

#define GLEW_ARB_vertex_buffer_object GL_ARB_vertex_buffer_object
#define GLEW_ARB_multitexture GL_ARB_multitexture
#define GLEW_EXT_paletted_texture GL_EXT_paletted_texture
#define GLEW_ARB_texture_env_combine GL_ARB_texture_env_combine
#define GLEW_ARB_fragment_program GL_ARB_fragment_program
#define GLEW_ARB_shading_language_100 GL_ARB_shading_language_100
#define GLEW_EXT_texture_filter_anisotropic GL_EXT_texture_filter_anisotropic
#define GLEW_ARB_pixel_buffer_object GL_ARB_pixel_buffer_object
#define GLEW_ARB_texture_float GL_ARB_texture_float
#define GLEW_EXT_framebuffer_object GL_EXT_framebuffer_object
#define GLEW_EXT_bgra GL_EXT_bgra
#define GLEW_EXT_texture_env_combine GL_EXT_texture_env_combine
#define GLEW_ARB_shader_objects GL_ARB_shader_objects
#define GLEW_EXT_blend_equation_separate GL_EXT_blend_equation_separate

#define glColorTableEXT glColorTable
#define glGetColorTableParameterivEXT glGetColorTableParameteriv
#define glBlendFuncSeparateEXT glBlendFuncSeparate
#define glDeleteRenderbuffersEXT glDeleteRenderbuffers
#define glGenFramebuffersEXT glGenFramebuffers
#define glBindFramebufferEXT glBindFramebuffer
#define glFramebufferTexture2DEXT glFramebufferTexture2D
#define glGenRenderbuffersEXT glGenRenderbuffers
#define glRenderbufferStorageEXT glRenderbufferStorage
#define glCheckFramebufferStatusEXT glCheckFramebufferStatus
#define glDeleteFramebuffersEXT glDeleteFramebuffers
#define glBindRenderbufferEXT glBindRenderbuffer
#define glFramebufferRenderbufferEXT glFramebufferRenderbuffer

#define glewIsSupported(...) 1

#define glewInit nxShimResolveGLSyms

extern bool glewExperimental;