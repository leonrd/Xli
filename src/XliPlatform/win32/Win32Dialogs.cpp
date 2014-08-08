#include <XliPlatform/CommonDialogs.h>
#include <XliPlatform/Disk.h>
#include <Xli/StringBuilder.h>
#include <Xli/Path.h>
#include <XliPlatform/MessageBox.h>
#include <XliPlatform/PlatformSpecific/Win32.h>
#include <XliPlatform/Window.h>
#include <Xli/Unicode.h>
#include <CommDlg.h>

namespace Xli
{
    static String FixExtension(const String& str)
    {
        return str.Trim('.').Trim('*');
    }

    static void InitOptions(Window* parent, const FileDialogOptions& options, bool mustExist, OPENFILENAMEW& ofn, WCHAR fnbufW[4096], Utf16String& filterW, Utf16String& defW, Utf16String& dirW, Utf16String& captionW)
    {
        fnbufW[0] = '\0';

        ZeroMemory(&ofn, sizeof(OPENFILENAMEW));
        ofn.lStructSize = sizeof(OPENFILENAMEW);

        defW = Unicode::Utf8To16(FixExtension(options.DefaultExtension));
        dirW = Unicode::Utf8To16(options.DefaultFolder);
        captionW = Unicode::Utf8To16(options.Caption);

        for (int i = 0; i < dirW.Length(); i++)
            if (dirW[i] == '/') 
                dirW[i] = '\\';

        if (options.FileExtensions.Length())
        {
            ofn.nFilterIndex = 1;

            StringBuilder fb;
            for (int i = 0; i < options.FileExtensions.Length(); i++)
            {
                String ext = FixExtension(options.FileExtensions[i].Extension);

                if (ext.Length()) 
                    ext = "*." + ext;
                else 
                    ext = "*.*";

                fb.Append(options.FileExtensions[i].Description);
                fb.Append(" (" + ext + ")");
                fb.Append('\0');
                fb.Append(ext);
                fb.Append('\0');

                if (options.FileExtensions[i].Extension == options.DefaultExtension)
                    ofn.nFilterIndex = i + 1;
            }

            fb.Append('\0');
            filterW = Unicode::Utf8To16(fb.GetString());
        }
        else
        {
            ofn.nFilterIndex = 1;
            filterW = Unicode::Utf8To16("All files (*.*)\0*.*\0\0");
        }
        
        if (options.DefaultFile.Length())
        {
            Utf16String defaultFileW = Unicode::Utf8To16(options.DefaultFile);
            memcpy(fnbufW, defaultFileW.DataPtr(), defaultFileW.Length() * 2 + 2);
        }
        
        ofn.hwndOwner = Win32::GetWindowHandle(parent);
        ofn.hInstance = GetModuleHandle(NULL);
        ofn.lpstrFilter = filterW.DataPtr();
        ofn.lpstrFile = fnbufW;
        ofn.nMaxFile = 4096;
        ofn.lpstrInitialDir = dirW.DataPtr();
        ofn.lpstrTitle = captionW.DataPtr();

        if (mustExist)
            ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_ENABLESIZING;
        else
            ofn.Flags = OFN_OVERWRITEPROMPT | OFN_ENABLESIZING;

        ofn.lpstrDefExt = defW.DataPtr();
    }

    static bool EndFileDialog(bool ret, WCHAR* fnbufW, const String& cd, String& result)
    {
        if (ret)
        {
            result = Unicode::Utf16To8(fnbufW);

            for (int i = 0; i < result.Length(); i++)
                if (result[i] == '\\') 
                    result[i] = '/';
        }

        Disk->SetCurrentDirectory(cd);
        return ret;
    }

    bool CommonDialogs::OpenFile(Window* parent, const FileDialogOptions& options, String& result)
    {
        WCHAR fnbufW[4096];
        OPENFILENAMEW ofn;
        Utf16String filterW, defW, dirW, captionW;
        String cd = Disk->GetCurrentDirectory();
        InitOptions(parent, options, true, ofn, fnbufW, filterW, defW, dirW, captionW);
        return EndFileDialog(GetOpenFileNameW(&ofn) == TRUE, fnbufW, cd, result);
    }

    bool CommonDialogs::SaveFile(Window* parent, const FileDialogOptions& options, String& result)
    {
        WCHAR fnbufW[4096];
        OPENFILENAMEW ofn;
        Utf16String filterW, defW, dirW, captionW;
        String cd = Disk->GetCurrentDirectory();
        InitOptions(parent, options, false, ofn, fnbufW, filterW, defW, dirW, captionW);
        return EndFileDialog(GetSaveFileNameW(&ofn) == TRUE, fnbufW, cd, result);
    }
}
