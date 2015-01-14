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

#ifndef __XLI_PLATFORM_INPUTEVENTHANDLER_H__
#define __XLI_PLATFORM_INPUTEVENTHANDLER_H__

#include <Xli/Object.h>
#include <Xli/Vector2.h>

namespace Xli
{
    class Window;

/**
        \ingroup XliPlatform
    */
    enum Key
    {
        KeyUnknown = 0,

        KeyBackspace = 8,
        KeyTab = 9,

        KeyEnter = 13,

        KeyShift = 16,
        KeyCtrl = 17,
        KeyAlt = 18,
        KeyBreak = 19,
        KeyCapsLock = 20,

        KeyEscape = 27,

        KeySpace = 32,
        KeyPageUp = 33,
        KeyPageDown = 34,
        KeyEnd = 35,
        KeyHome = 36,
        KeyLeft = 37,
        KeyUp = 38,
        KeyRight = 39,
        KeyDown = 40,
        KeyInsert = 45,
        KeyDelete = 46,

        Key0 = 48,
        Key1 = 49,
        Key2 = 50,
        Key3 = 51,
        Key4 = 52,
        Key5 = 53,
        Key6 = 54,
        Key7 = 55,
        Key8 = 56,
        Key9 = 57,

        KeyA = 65,
        KeyB = 66,
        KeyC = 67,
        KeyD = 68,
        KeyE = 69,
        KeyF = 70,
        KeyG = 71,
        KeyH = 72,
        KeyI = 73,
        KeyJ = 74,
        KeyK = 75,
        KeyL = 76,
        KeyM = 77,
        KeyN = 78,
        KeyO = 79,
        KeyP = 80,
        KeyQ = 81,
        KeyR = 82,
        KeyS = 83,
        KeyT = 84,
        KeyU = 85,
        KeyV = 86,
        KeyW = 87,
        KeyX = 88,
        KeyY = 89,
        KeyZ = 90,

        KeyNumPad0 = 96,
        KeyNumPad1 = 97,
        KeyNumPad2 = 98,
        KeyNumPad3 = 99,
        KeyNumPad4 = 100,
        KeyNumPad5 = 101,
        KeyNumPad6 = 102,
        KeyNumPad7 = 103,
        KeyNumPad8 = 104,
        KeyNumPad9 = 105,

        KeyF1 = 112,
        KeyF2 = 113,
        KeyF3 = 114,
        KeyF4 = 115,
        KeyF5 = 116,
        KeyF6 = 117,
        KeyF7 = 118,
        KeyF8 = 119,
        KeyF9 = 120,
        KeyF10 = 121,
        KeyF11 = 122,
        KeyF12 = 123,

        KeyMenu = 200,
        BackButton = 201,
    };

    /**
        \ingroup XliPlatform

        Enumerates the standard mouse buttons.
    */
    enum MouseButton
    {
        MouseButtonUnknown = 0,

        /**
            Left mouse button
        */
        MouseButtonLeft = 1,

        /**
            Middle mouse button
        */
        MouseButtonMiddle = 2,

        /**
            Right mouse button
        */
        MouseButtonRight = 3,

        /**
            Extra mouse button 1 (aka Mouse4)
        */
        MouseButtonX1 = 4,

        /**
            Extra mouse button 2 (aka Mouse5)
        */
        MouseButtonX2 = 5
    };
    
    /**
        \ingroup XliPlatform
    */
    enum SystemCursor
    {
        SystemCursorNone = 0,
        SystemCursorArrow = 1,
        SystemCursorIBeam = 2,
        SystemCursorWait = 3,
        SystemCursorCrosshair = 4,
        SystemCursorWaitArrow = 5,
        SystemCursorSizeNWSE = 6,
        SystemCursorSizeNESW = 7,
        SystemCursorSizeWE = 8,
        SystemCursorSizeNS = 9,
        SystemCursorSizeAll = 10,
        SystemCursorNo = 11,
        SystemCursorHand = 12,
    };

    /**
        \ingroup XliPlatform
    */
    enum TextInputHint
    {
        TextInputHintDefault,
        TextInputHintEmail,
        TextInputHintURL,
        TextInputHintPhone,
        TextInputHintNumber
    };

    /**
        \ingroup XliPlatform
    */
    enum WindowFlags
    {
        WindowFlagsFixed = 0, ///< Fixed size window with a title bar and minimize and close buttons
        WindowFlagsBorderless = 1 << 0, ///< Borderless window without titlebar and buttons
        WindowFlagsResizeable = 1 << 1, ///< Resizeable window with a title bar and minimize, maximize and close buttons
        WindowFlagsFullscreen = 1 << 2, ///< Fullscreen window which should always be on top

        /// Disables power saver on mobile platforms. 
        /// This is useful to avoid the screen from going black for apps not using touch input
        /// -- such as sensor based apps, demos, video players, etc. On desktop platforms this is 
        /// the default behaviour when in fullscreen mode, but on mobile it must be explicitly 
        /// enabled for apps that needs it because it can potentially drain the battery.
        WindowFlagsDisablePowerSaver = 1 << 3,

        /// Disables running as background process on applicable platforms (i.e. Android)
        WindowFlagsDisableBackgroundProcess = 1 << 4,
    };
    
    /**
        \ingroup XliPlatform
    */
    enum WindowImplementation
    {
        WindowImplementationUnknown,
        WindowImplementationWin32,
        WindowImplementationAndroid,
        WindowImplementationSDL2,
    };
    
    /**
        \ingroup XliPlatform
    */
    class InputEventHandler: public Object
    {
    public:
        virtual bool OnKeyDown(Window* wnd, Key key);
        virtual bool OnKeyUp(Window* wnd, Key key);
        virtual bool OnTextInput(Window* wnd, const String& text);
        virtual bool OnKeyboardResized(Window* wnd);
        
        virtual bool OnMouseDown(Window* wnd, Vector2i pos, MouseButton button);
        virtual bool OnMouseUp(Window* wnd, Vector2i pos, MouseButton button);
        virtual bool OnMouseMove(Window* wnd, Vector2i pos);
        virtual bool OnMouseWheel(Window* wnd, Vector2i delta);
        
        virtual bool OnTouchDown(Window* wnd, Vector2 pos, int id);
        virtual bool OnTouchMove(Window* wnd, Vector2 pos, int id);
        virtual bool OnTouchUp(Window* wnd, Vector2 pos, int id);
        
        virtual void OnSizeChanged(Window* wnd);
        virtual void OnNativeHandleChanged(Window* wnd);
    };
}

#endif
