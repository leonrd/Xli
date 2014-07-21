#ifndef __XLI_MEDIA_BITMAP_H__
#define __XLI_MEDIA_BITMAP_H__

#include <Xli/Buffer.h>
#include <Xli/Vector2.h>
#include <Xli/Vector4.h>
#include <Xli/Rectangle.h>
#include <XliImage/Image.h>

namespace Xli
{
    /**
        \ingroup XliImage
    */
    enum RotateAngle
    {
        RotateAngle90CW,
        RotateAngle90CCW,
        RotateAngle180,
    };

    /**
        \ingroup XliImage
    */
    class Bitmap: public Image
    {
        Format format;
        int width, height, pitch;
        DataAccessor* dataOwner;
        UInt8* data;

    public:
        Bitmap(int width, int height, Format format);
        Bitmap(int width, int height, Format format, DataAccessor* data, int pitch, int offset);
        virtual ~Bitmap();

        Format GetFormat() const;
        int GetWidth() const;
        int GetHeight() const;
        int GetDepth() const;
        int GetPitch() const;
        int GetComponentCount() const;
        int GetBytesPerPixel() const;
        Recti GetRect() const;

        Vector4u8 GetPixelColor(int x, int y);
        void SetPixelColor(int x, int y, const Vector4u8& color);

        /**
            Returns a version of this bitmap where pixels are packed (pitch = pixelsize*width).
            If this bitmap is already packed, then a new reference is added and this is returned.
        */
        Bitmap* Pack();
        Bitmap* ToBitmap();
        Bitmap* SubBitmap(const Recti& rect);
        Bitmap* ConvertTo(Format format);
        Bitmap* Rotate(RotateAngle angle);

        Bitmap* DownSample2x2();
        void GenerateMipMaps(Array<Bitmap*>& chain);

        void Clear();
        void Blit(Bitmap* src, Recti srcRect, Vector2i dstPos);

        virtual int GetSizeInBytes() const;
        
        UInt8* GetDataPtr();
        virtual const UInt8* GetDataPtr() const;

        UInt8* GetScanlinePtr(int y);
        const UInt8* GetScanlinePtr(int y) const;

        UInt8* GetPixelPtr(int x, int y);
        const UInt8* GetPixelPtr(int x, int y) const;

        DataAccessor* GetDataAccessor();
    };
}

#endif