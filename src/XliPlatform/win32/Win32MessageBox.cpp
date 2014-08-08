#include <XliPlatform/MessageBox.h>
#include <XliPlatform/Window.h>
#include <XliPlatform/PlatformSpecific/Win32.h>
#include <Xli/Unicode.h>

namespace Xli
{
    DialogResult MessageBox::Show(Window* wnd, const String& message, const String& caption, DialogButtons buttons, int hints)
    {
        UINT type = 0;

        switch (buttons)
        {
        case DialogButtonsOK: type |= MB_OK; break;
        case DialogButtonsOKCancel: type |= MB_OKCANCEL; break;
        case DialogButtonsYesNo: type |= MB_YESNO; break;
        case DialogButtonsYesNoCancel: type |= MB_YESNOCANCEL; break;
        case DialogButtonsCancelTryContinue: type |= MB_CANCELTRYCONTINUE; break;
        default:
            XLI_THROW("Invalid button combination for MessageDialog");
        }

        if (hints & DialogHintError) type |= MB_ICONERROR;
        else if (hints & DialogHintInformation) type |= MB_ICONINFORMATION;
        else if (hints & DialogHintQuestion) type |= MB_ICONQUESTION;
        else if (hints & DialogHintWarning) type |= MB_ICONWARNING;
        
        if (hints & DialogHintButton1Default) type |= MB_DEFBUTTON1;
        else if (hints & DialogHintButton2Default) type |= MB_DEFBUTTON2;
        else if (hints & DialogHintButton3Default) type |= MB_DEFBUTTON3;

        Utf16String messageW = Unicode::Utf8To16(message);
        Utf16String captionW = Unicode::Utf8To16(caption);
        int result = MessageBoxW(Win32::GetWindowHandle(wnd), messageW.DataPtr(), captionW.DataPtr(), type);

        switch (result)
        {
        case IDOK: return DialogResultOK;
        case IDCANCEL: return DialogResultCancel;
        case IDCONTINUE: return DialogResultContinue;
        case IDIGNORE: return DialogResultContinue;
        case IDNO: return DialogResultNo;
        case IDTRYAGAIN: return DialogResultTryAgain;
        case IDYES: return DialogResultYes;
        default:
            XLI_THROW("MessageDialog failed: " + PlatformSpecific::Win32::GetLastErrorString());
        }
    }
}
