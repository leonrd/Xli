#ifndef __XLI_MEDIA_IMAGE_H__
#define __XLI_MEDIA_IMAGE_H__

#include <Xli/Buffer.h>
#include <XliImage/Format.h>

namespace Xli
{
    class Bitmap;

    /**
        \ingroup XliImage
    */
    class Image: public DataAccessor
    {
    public:
        virtual int GetWidth() const = 0;
        virtual int GetHeight() const = 0;
        virtual int GetDepth() const = 0;
        virtual Format GetFormat() const = 0;
        virtual Bitmap* ToBitmap() = 0;

        virtual int GetSizeInBytes() const = 0;
        virtual const UInt8* GetDataPtr() const = 0;
    };
}

#endif
