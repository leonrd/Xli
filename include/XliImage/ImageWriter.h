#ifndef __XLI_MEDIA_IMAGE_WRITER_H__
#define __XLI_MEDIA_IMAGE_WRITER_H__

#include <XliImage/Format.h>
#include <XliImage/ProgressCallback.h>
#include <XliImage/Bitmap.h>

namespace Xli
{
    /**
        \ingroup XliImage
    */
    class ImageWriter: public Object
    {
    public:
        virtual ~ImageWriter() {}

        virtual int GetWidth() = 0;
        virtual int GetHeight() = 0;
        virtual int GetDepth() = 0;
        virtual int GetBufferSize() = 0;
        virtual Format GetFormat() = 0;

        virtual void Write(void* sourceBuffer, int pitch, ProgressCallback* callback = 0) = 0;
        virtual void WriteBitmap(Bitmap* bmp, ProgressCallback* callback = 0);
        virtual void WriteImage(Image* img, ProgressCallback* callback = 0);
    };
}


#endif