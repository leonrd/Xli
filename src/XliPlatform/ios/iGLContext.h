/* * Copyright (C) 2010-2014 Outracks Technologies
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this software and
 * associated documentation files (the "Software"), to deal in the Software without restriction,
 * including without limitation the rights to use, copy, modify, merge, publish, distribute,
 * sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies or substantial
 * portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT
 * NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES
 * OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#ifndef __XLI_GL_IGL_CONTEXT_H__
#define __XLI_GL_IGL_CONTEXT_H__

#include <Xli/Object.h>
#include <Xli/Vector4.h>
#include <XliPlatform/PlatformSpecific/iOS.h>
#include <Xli/Shared.h>
#include <XliGL/GLContext.h>
#include <XliGL.h>

#include <UIKit/UIKit.h>
#include <OpenGLES/ES2/gl.h>
#include <OpenGLES/ES2/glext.h>
#include <QuartzCore/QuartzCore.h>


namespace Xli
{
    namespace PlatformSpecific
    {
        class iGLContext : public Xli::GLContext
        {
        public:
            iGLContext();
            virtual ~iGLContext();

            void Initialize(CAEAGLLayer* layer);

            virtual GLContext* CreateSharedContext();

            virtual void GetAttributes(GLContextAttributes& result);
            virtual Vector2i GetDrawableSize();

            virtual void MakeCurrent(Window* window);
            virtual bool IsCurrent();
            virtual void Destroy();
            
            virtual void SwapBuffers();

            virtual void SetSwapInterval(int value);
            virtual int GetSwapInterval();
        private:
            CAEAGLLayer* layer_;
            EAGLContext* context_;
        };
    }
}

#endif
