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

#include <Xli/Thread.h>
#include <Xli/Console.h>
#include <pthread.h>
#include <time.h>
#include <cstdlib>

namespace Xli
{
    struct PthreadData
    {
        void (*Entrypoint)(void*);
        void* Arg;
    };

    void* PthreadFunc(void* data)
    {
        PthreadData* pdata = (PthreadData*)data;
        pdata->Entrypoint(pdata->Arg);
        free(data);
        return NULL;
    }

    ThreadHandle CreateThread(void (*entrypoint)(void*), void* arg)
    {
        pthread_attr_t attr;
        pthread_attr_init(&attr);
        pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

        PthreadData* data = (PthreadData*)malloc(sizeof(PthreadData));
        data->Entrypoint = entrypoint;
        data->Arg = arg;

        pthread_t result;
        if (pthread_create(&result, &attr, PthreadFunc, data))    
            XLI_THROW("Failed to create thread");

        pthread_attr_destroy(&attr);
        return (ThreadHandle)result;
    }

    void WaitForThread(ThreadHandle handle)
    {
        pthread_t thread = (pthread_t)handle;
        int result = pthread_join(thread, NULL);
        
        if (result) 
            Err->WriteLine("XLI ERROR: pthread_join failed: " + String::HexFromInt(result));
    }

    void* GetCurrentThread()
    {
        return (void*)pthread_self();
    }

    void Sleep(int ms)
    {
        struct timespec t, r;
        t.tv_sec = ms / 1000;
        t.tv_nsec = (ms % 1000) * 1000 * 1000;
        
        int result = nanosleep(&t, &r);
        
        if (result)
            Err->WriteLine("XLI ERROR: nanosleep failed: " + String::HexFromInt(result));
    }
}
