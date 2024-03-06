/*
 * gl.hpp
 *
 *  Created on: 21 Jan 2024
 *      Author: user
 */

#ifndef GL_HPP_
#define GL_HPP_

#include "c_interop.hpp"
#include <string>




/*namespace gl {

template<class lambda> auto usingBuffer(GLenum target, GLuint buffer, lambda codeblock) {
	glBindBuffer(target,buffer);
	return codeblock();
}

}*/

inline void GLErrorCallback(
		GLenum source, GLenum type, GLuint id, GLenum severity,
		GLsizei length, const GLchar *message,
		const void *userParam) {
//	if (severity <= GL_DEBUG_SEVERITY_MEDIUM) // astonishingly they are in fucking reverse order
	if ((severity == GL_DEBUG_SEVERITY_NOTIFICATION) && (!userParam)) return;
	if ((severity == GL_DEBUG_SEVERITY_LOW) && (!userParam)) return;
	printf("\n");
	printf("**************\n");
	printf("OpenGL Message\n");
	printf("0x%X in 0x%X id %u\n",
			type,
			source,
			id);
	printf("severity: 0x%X\n",
			severity);
	printf("\"\"%s\"\"\n",message);
//		printf("\n%s OpenGL Message\n%s\n", userParam, message);
	printf("\n");
}

inline bool GLProgramDebugCheck(const char* message, GLuint progid) {
	bool retb=true;
	GLenum er = glGetError();
	if (er)
		fprintf(stderr,"\nDebugging GL program %u (%s): error condition = %s\n", progid, message, gluErrorString(er));
	GLint linkstatus;
	glGetProgramiv(progid, GL_LINK_STATUS, &linkstatus);
	if (linkstatus == GL_TRUE) {
//		_IfVerbosePrintF("Link was successful.\n");
	} else {
		fprintf(stderr,"\nDebugging GL program %u (%s): linking error\n", progid, message);
		retb=false;
		GLint logsize;
		glGetProgramiv(progid, GL_INFO_LOG_LENGTH, &logsize);
		std::string errorlog;
		errorlog.resize(logsize,0);
		glGetProgramInfoLog(progid, logsize, &logsize, errorlog.data());
		fprintf(stderr,"------ Contents of linker log ------\n");
		fprintf(stderr,"%s",errorlog.c_str());
		fprintf(stderr,"---- End of linker log contents ----\n");
		fprintf(stderr,"\n"); // buffer
	}
	return retb;
}

template<typename GLIDType> struct alignas(GLIDType) _OpenGL_ID_Convertible {
	GLIDType id;
	inline GLIDType* ptr() {return &id;}
	inline operator GLIDType() {return id;}
	inline explicit operator GLIDType&() {return id;}
	inline void operator=(GLIDType n) {id = n;}
	inline _OpenGL_ID_Convertible() = default;
	inline _OpenGL_ID_Convertible(int n) : id(n) {}
	inline _OpenGL_ID_Convertible(GLIDType n) : id(n) {}
};

struct alignas(GLuint) GLBufferID : public _OpenGL_ID_Convertible<GLuint> { using _OpenGL_ID_Convertible<GLuint>::_OpenGL_ID_Convertible;
	inline void data(GLsizeiptr size, const void* data, GLenum usage) {
		glNamedBufferData(id,size,data,usage);
	}
	template<class container> inline void data(const container& data, GLenum usage) {
		glNamedBufferData(id,
				(sizeof (typename container::value_type))
				*
				(data.size())
				,data.data(),usage);
	}
	inline void bind(GLenum target) {glBindBuffer(target, id);}
	inline void gen() {glGenBuffers(1,&id);}
	inline void del() {glDeleteBuffers(1,&id);}
}; // GLProgramID

struct alignas(GLuint) GLProgramID : public _OpenGL_ID_Convertible<GLuint> { using _OpenGL_ID_Convertible<GLuint>::_OpenGL_ID_Convertible;
	inline void create() {id = glCreateProgram();}
	inline void binary(GLenum binaryFormat,const void *binary,GLsizei length) {
		glProgramBinary(id,binaryFormat,binary,length);
	}
	inline void bind() {glUseProgram(id);}
	inline void use() {glUseProgram(id);}
	inline void link() {glLinkProgram(id);}
	inline void del() {glDeleteProgram(id);}
	inline void attachShader(GLuint shader) {glAttachShader(id, shader);}
	inline GLint getUniform(const char* name) {return glGetUniformLocation(id,name);}
};

struct alignas(GLuint) GLShaderID : public _OpenGL_ID_Convertible<GLuint> { using _OpenGL_ID_Convertible<GLuint>::_OpenGL_ID_Convertible;
	inline void create(GLenum type) {id = glCreateShader(type);}
	inline void compile() {glCompileShader(id);}
	inline void source(std::string string) {
		const GLchar* const slist[1] = {string.c_str()};
		glShaderSource(id,1,slist,nullptr);
	}
	inline void del() {glDeleteShader(id);}
	inline void source(GLsizei count, const GLchar **strings, const GLint *lengths) {
		glShaderSource(id,count,strings,lengths);
	}
};

struct alignas(GLuint) GLVxArrayID : public _OpenGL_ID_Convertible<GLuint> { using _OpenGL_ID_Convertible<GLuint>::_OpenGL_ID_Convertible;
	inline void bind() {glBindVertexArray(id);}
	inline void gen() {glGenVertexArrays(1,&id);}
	inline void del() {glDeleteVertexArrays(1,&id);}
	inline void enableAttrib(GLuint index) {glEnableVertexArrayAttrib(id,index);}
	inline void disableAttrib(GLuint index) {glDisableVertexArrayAttrib(id,index);}
	inline void bindBuffer(GLuint vbIndex, GLuint buffer, GLintptr offset, GLsizei stride) {
		glVertexArrayVertexBuffer(id, vbIndex, buffer, offset, stride);
	}
	inline void bindBuffer(GLuint vbIndex, GLuint buffer, GLintptr offset, GLsizei stride, GLuint attrib) {
		glEnableVertexArrayAttrib(id,attrib);
		glVertexArrayVertexBuffer(id, vbIndex, buffer, offset, stride);
		glVertexArrayAttribBinding(id,attrib,vbIndex);
	}
	inline void attribFormat(GLuint attribIndex, GLint vecSize, GLenum type, GLboolean normalised=GL_FALSE,
			GLuint startRelativeOffset=0) {
		glVertexArrayAttribFormat(id, attribIndex, vecSize, type, normalised, startRelativeOffset);
	}
	inline void attribIFormat(GLuint attribIndex, GLint vecSize, GLenum type,
			GLuint startRelativeOffset=0) {
		glVertexArrayAttribIFormat(id, attribIndex, vecSize, type, startRelativeOffset);
	}
	inline void attribLFormat(GLuint attribIndex, GLint vecSize, GLenum type,
			GLuint startRelativeOffset=0) {
		glVertexArrayAttribLFormat(id, attribIndex, vecSize, type, startRelativeOffset);
	}
	inline void attribBuffer(GLuint attribIndex, GLuint bindingIndex) {
		glVertexArrayAttribBinding(id,attribIndex,bindingIndex);
	}
};




#endif /* GL_HPP_ */
