#ifndef __XLI_GL_CHECK_ERROR_H__
#define __XLI_GL_CHECK_ERROR_H__

#include <Xli/GLHeaders.h>
#include <Xli/Exception.h>

namespace Xli
{
	/**
		\addtogroup GL
		@{
	 */

	String GLGetErrorString(GLint err);

	static inline void GLCheckErrorImpl(const char* func, int line)
	{
		GLint err = glGetError();
		if (err == GL_NO_ERROR) return;
		XLI_BREAK_THROW("GL ERROR: " + GLGetErrorString(err) + " (in " + func + ":" + line + ")");
	}

#define GLCheckError() GLCheckErrorImpl(XLI_FUNC, __LINE__)

	/** @}*/
}


#endif
