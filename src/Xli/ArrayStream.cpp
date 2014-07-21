#include <Xli/ArrayStream.h>
#include <cstring>

namespace Xli
{
    ArrayStream::ArrayStream(int initCapacity)
    {
        buf.Reserve(initCapacity);
    }

    bool ArrayStream::CanWrite() const
    {
        return true;
    }

    void ArrayStream::Write(const void* src, int elmSize, int elmCount)
    {
        buf.Expand();
        int loc = buf.Length();
        buf.Resize(loc + elmSize * elmCount);
        memcpy(buf.DataPtr() + loc, src, elmSize * elmCount);
    }

    int ArrayStream::GetLength() const
    {
        return buf.Length();
    }

    const UInt8* ArrayStream::GetDataPtr() const
    {
        return buf.DataPtr();
    }

    Buffer* ArrayStream::CopyData() const
    {
        return Buffer::CopyFrom(buf.DataPtr(), buf.Length());
    }

    void ArrayStream::Clear()
    {
        buf.Clear();
    }
}