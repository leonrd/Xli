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

#include <Xli/File.h>
#include <Xli/Shared.h>
#include <XliImage/Jpeg.h>
#include <XliImage/ImageReader.h>

extern "C"
{
#include <jpeglib.h>
}

namespace Xli
{
    class JpegReader: public ImageReader
    {
        struct ClientData
        {
            Shared<Stream> File;
            UInt8 Buf[1024];
            ClientData(Stream* f): File(f) {}
        };

        ClientData cd;

        static void initSource (j_decompress_ptr cinfo)
        {
        }

        static boolean fillInputBuffer (j_decompress_ptr cinfo)
        {
            ClientData* cd = reinterpret_cast<ClientData*>(cinfo->client_data);
            struct jpeg_source_mgr *jsrc = cinfo->src;

            jsrc->next_input_byte = cd->Buf;
            jsrc->bytes_in_buffer = (size_t)cd->File->Read(cd->Buf, 1, 1024);

            if (!jsrc->bytes_in_buffer)
            {
                static const JOCTET eoi_buffer[2] = { 0xFF, JPEG_EOI };
                jsrc->bytes_in_buffer = 2;
                jsrc->next_input_byte = eoi_buffer;
            }

            return TRUE;
        }

        static void skipInputData(j_decompress_ptr cinfo, long num_bytes)
        {
            ClientData* cd = reinterpret_cast<ClientData*>(cinfo->client_data);
            struct jpeg_source_mgr *jsrc = cinfo->src;

            if ((size_t)num_bytes > jsrc->bytes_in_buffer)
            {
                cd->File->Seek((int)num_bytes - (int)jsrc->bytes_in_buffer, SeekOriginCurrent);
                fillInputBuffer(cinfo);
            }
            else
            {
                jsrc->next_input_byte += num_bytes;
                jsrc->bytes_in_buffer -= num_bytes;
            }
        }

        static void termSource (j_decompress_ptr cinfo)
        {
        }

        static void errorExit(j_common_ptr cinfo)
        {
            char buf[JMSG_LENGTH_MAX];
            (*cinfo->err->format_message)(cinfo, buf);
            XLI_THROW(String("Error while reading JPEG file: ") + buf);
        }

        jpeg_decompress_struct cinfo;
        jpeg_source_mgr jsrc;
        jpeg_error_mgr pub;

        int width, height, comps;
        Format format;

    public:
        JpegReader(Stream* input): cd(input)
        {
            jpeg_create_decompress (&cinfo);

            cinfo.err = jpeg_std_error (&pub);
            cinfo.client_data = (void*)&cd;
            cinfo.src = &jsrc;

            pub.error_exit = errorExit;

            /* configure source manager */
            jsrc.next_input_byte = 0;
            jsrc.bytes_in_buffer = 0;
            jsrc.init_source = initSource;
            jsrc.fill_input_buffer = fillInputBuffer;
            jsrc.skip_input_data = skipInputData;
            jsrc.resync_to_restart = jpeg_resync_to_restart;
            jsrc.term_source = termSource;

            /* read file's header and prepare for decompression */
            jpeg_read_header (&cinfo, TRUE);

            width = cinfo.image_width;
            height = cinfo.image_height;
            comps = cinfo.num_components;

            switch (comps)
            {
            case 1:
                format = FormatL_8_UInt_Normalize;
                break;

            case 3:
                format = FormatRGB_8_8_8_UInt_Normalize;
                break;

            default:
                XLI_THROW("Unsupported JPEG pixel format");
                break;
            }
        }

        virtual ~JpegReader()
        {
            jpeg_destroy_decompress (&cinfo);
        }

        virtual int GetWidth()
        {
            return width;
        }

        virtual int GetHeight()
        {
            return height;
        }

        virtual int GetDepth()
        {
            return 0;
        }

        virtual int GetBufferSize()
        {
            return width * height * comps;
        }

        virtual Format GetFormat()
        {
            return format;
        }

        virtual void Read(void* targetBuffer, ProgressCallback* callback)
        {
            JSAMPROW j = (JSAMPROW)targetBuffer;
            int pitch = width*comps;
            jpeg_start_decompress (&cinfo);

            /* read scanlines */
            for (int i = 0; i < height; ++i)
            {
                if (callback) callback->Update(i, height);
                jpeg_read_scanlines (&cinfo, &j, 1);
                j += pitch;
            }

            jpeg_finish_decompress (&cinfo);
            if (callback) callback->Update(height, height);
        }
    };

    ImageReader* Jpeg::CreateReader(Stream* input)
    {
        return new JpegReader(input);
    }

    Bitmap* Jpeg::Load(Stream* input)
    {
        JpegReader r(input);
        return r.ReadBitmap();
    }

    Bitmap* Jpeg::Load(const String& filename)
    {
        File f(filename, FileModeRead);
        return Load(&f);
    }
}
