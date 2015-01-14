//
// Copyright (C) 2010-2014 Outracks Technologies
//
// Permission is hereby granted, free of charge, to any person obtaining a copy of this software and
// associated documentation files (the "Software"), to deal in the Software without restriction,
// including without limitation the rights to use, copy, modify, merge, publish, distribute,
// sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all copies or substantial
// portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT
// NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
// NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES
// OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
// CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
//

#include <XliGL.h>
#include <XliPlatform/PlatformSpecific/iOS.h>
#include <Xli/Shared.h>
#include "iGLContext.h"

namespace Xli
{
    namespace PlatformSpecific
    {
        
        iGLContext::~iGLContext()
        {
            Destroy();
        }

        void iGLContext::Initialize(CAEAGLLayer *layer)
        {
            layer_ = layer;
            context_
                = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES2];
            [EAGLContext setCurrentContext:context_];

            layer_.opaque = YES;

            //  NOTE: Unretained backing -- the default.
            //  Differs from prior SDL implementation.
            layer_.drawableProperties = [NSDictionary
                                         dictionaryWithObject:kEAGLColorFormatRGBA8
                                         forKey:kEAGLDrawablePropertyColorFormat];

            GLuint renderBuffer;
            glGenRenderbuffers(1, &renderBuffer);
            glBindRenderbuffer(GL_RENDERBUFFER, renderBuffer);
            [context_ renderbufferStorage:GL_RENDERBUFFER fromDrawable:layer_];

            GLint backingWidth, backingHeight;
            glGetRenderbufferParameteriv(
                GL_RENDERBUFFER, GL_RENDERBUFFER_WIDTH, &backingWidth);
            glGetRenderbufferParameteriv(
                GL_RENDERBUFFER, GL_RENDERBUFFER_HEIGHT, &backingHeight);

            GLuint frameBuffer;
            glGenFramebuffers(1, &frameBuffer);
            glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
            glFramebufferRenderbuffer(
                GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER,
                renderBuffer);

            GLuint depthStencilBuffer;
            glGenRenderbuffers(1, &depthStencilBuffer);
            glBindRenderbuffer(GL_RENDERBUFFER, depthStencilBuffer);
            glRenderbufferStorage(
                GL_RENDERBUFFER, GL_DEPTH24_STENCIL8_OES, backingWidth,
                backingHeight);
            glFramebufferRenderbuffer(
                GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER,
                depthStencilBuffer);
            glFramebufferRenderbuffer(
                GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_RENDERBUFFER,
                depthStencilBuffer);

            glBindRenderbuffer(GL_RENDERBUFFER, renderBuffer);
            glClearColor(0, 104.0/255.0, 55.0/255.0, 1.0);
            glClear(GL_COLOR_BUFFER_BIT);
        }

        void iGLContext::Destroy()
        {
            [context_ release];

            layer_ = 0;
            context_ = 0;
        }

        void iGLContext::GetAttributes(Xli::GLContextAttributes& result)
        {
            result = Xli::GLContextAttributes::Default();
        }

        Xli::Vector2i iGLContext::GetDrawableSize()
        {
            CGRect bounds = layer_.bounds;
            return Xli::Vector2i(bounds.size.width, bounds.size.height);
        }

        void iGLContext::MakeCurrent(Xli::Window *)
        {
            [EAGLContext setCurrentContext:context_];
        }

        bool iGLContext::IsCurrent()
        {
            return context_ == [EAGLContext currentContext];
        }

        void iGLContext::SwapBuffers()
        {
            [context_ presentRenderbuffer:GL_RENDERBUFFER];
        }

        // NOT Supported
        GLContext* iGLContext::CreateSharedContext()
        {
            XLI_THROW_NOT_SUPPORTED(XLI_FUNCTION);
        }

        void iGLContext::SetSwapInterval(int value)
        {
            XLI_THROW_NOT_SUPPORTED(XLI_FUNCTION);
        }

        int iGLContext::GetSwapInterval()
        {
            XLI_THROW_NOT_SUPPORTED(XLI_FUNCTION);
        }
    }
    
    GLContext* GLContext::Create(Window* wnd, const GLContextAttributes& attribs)
    {
        return new PlatformSpecific::iGLContext();
    }
}
