/*    // =========================================================================
    // FileName:   win32_handmade.cpp
    // Date:       1 March 2015
    // Revision:   3 March 2015
    // Creator:    Lyle Denman @lobsterhands
    // =========================================================================
*/

#include <windows.h>

LRESULT CALLBACK MainWindowCallback(     // Originally: WindowProc callback
    HWND Window,        // Handle to a window
    UINT Message,       // Message Windows is asking to handle
    WPARAM WParam,      // 
    LPARAM LParam)
{
    LRESULT Result = 0;       // An LRESULT returns saying what we did with the message

    switch(Message)
    {
        case WM_SIZE:
        {
            OutputDebugStringA("WM_SIZE\n");
        } break;

        case WM_DESTROY:
        {
            OutputDebugStringA("WM_DESTROY\n");
        } break;

        case WM_CLOSE:
        {
            OutputDebugStringA("WM_CLOSE\n");
        } break;

        case WM_ACTIVATEAPP:
        {
            OutputDebugStringA("WM_ACTIVATEAPP\n");
        } break;

        case WM_PAINT:
        {
            PAINTSTRUCT Paint;
            HDC DeviceContext = BeginPaint(Window, &Paint);
            int X = Paint.rcPaint.left;
            int Y = Paint.rcPaint.top;
            int Width = Paint.rcPaint.right - Paint.rcPaint.left;
            int Height = Paint.rcPaint.bottom - Paint.rcPaint.top;
            static DWORD Operation = WHITENESS;
            PatBlt(DeviceContext, X, Y, Width, Height, Operation);
            EndPaint(Window, &Paint);
            if(Operation == WHITENESS)
            {
                Operation = BLACKNESS;
            }
            else
            {
                Operation = WHITENESS;
            }
        } break;

        default:
        {
            // OutputDebugStringA("default\n")
            Result = DefWindowProc(Window, Message, WParam, LParam);
        } break;
    }

    return(Result);
}

int CALLBACK 
WinMain(
    HINSTANCE Instance,
    HINSTANCE PrevInstance,
    LPSTR CommandLine,
    int ShowCode)
{
    WNDCLASS WindowClass = {};      // This is all about the game window itself

    WindowClass.lpfnWndProc = MainWindowCallback;     // Pointer to a function; window events
    WindowClass.hInstance = Instance;  // if !WinMain, set to GetModuleHandle(0)
//  WindowClass.hIcon;
    WindowClass.lpszClassName = "HandmadeHeroWindowClass";  // Window class name

    // RegisterClass(&WindowClass);

    if(RegisterClassA(&WindowClass))
    {
        HWND WindowHandle = 
        CreateWindowEx(
            0,      // DWORD dwExStyle; no need here
            WindowClass.lpszClassName,       // LPCTSTR lpClassName,
            "Handmade Hero",                // LPCTSTR lpWindowName,
            WS_OVERLAPPEDWINDOW|WS_VISIBLE, // DWORD dwStyle; built-in goodies
            CW_USEDEFAULT,                  // int x,
            CW_USEDEFAULT,                  // int y,
            CW_USEDEFAULT,                  // int nWidth,
            CW_USEDEFAULT,                  // int nHeight,
            0,                              // HWND hWndParent,
            0,                              // HMENU hMenu,
            Instance,                       // HINSTANCE hInstance,
            0);                             // LPVOID lpParam
        if(WindowHandle)
        {
            MSG Message;
            for(;;)
            {
                BOOL MessageResult = GetMessageA(&Message, 0, 0, 0);
                if(MessageResult > 0)
                {
                    TranslateMessage(&Message);
                    DispatchMessageA(&Message);
                }
                else
                {
                    break;
                }
            }
        }
        else
        {
            // @lob: Logging
        }
    } 
    else
    {
        // @lob: Logging
    }

    return (0);
}
