/*    // =========================================================================
    // FileName:   win32_handmade.cpp
    // Date:       1 March 2015
    // Revision:   3 March 2015
    // Creator:    Lyle Denman @lobsterhands
    // =========================================================================
*/

#include <windows.h>

// Lyle: these are for building; will be changed later
#define internal static             // local to specific source file
#define local_persist static
#define global_variable static

// Lyle: This is a static global (global for now)
global_variable bool Running;

global_variable BITMAPINFO BitmapInfo;
global_variable void *BitmapMemory;
global_variable HBITMAP BitmapHandle;
global_variable HDC BitmapDeviceContext;

internal void
Win32ResizeDIBSection(int Width, int Height)         // Windows bitmap display
{
    // Lyle: Maybe don't free first; free after then free first if that fails
    if(BitmapHandle)
    {
        DeleteObject(BitmapHandle);
    }
    
    if(!BitmapDeviceContext)
    {
        // Lyle: should we recreate these under certain circumstances?
        BitmapDeviceContext = CreateCompatibleDC(0);
    }

    BitmapInfo.bmiHeader.biSize = sizeof(BitmapInfo.bmiHeader);
    BitmapInfo.bmiHeader.biWidth = Width;
    BitmapInfo.bmiHeader.biHeight = Height;
    BitmapInfo.bmiHeader.biPlanes = 1;
    BitmapInfo.bmiHeader.biBitCount = 32;
    BitmapInfo.bmiHeader.biCompression = BI_RGB;
    // BitmapInfo.biSizeImage = 0;          // Lyle: These are cleared for free 
    // BitmapInfo.biXPelsPerMeter = 0;      // when BitmapInfo is initialized; 
    // BitmapInfo.biYPelsPerMeter = 0;      // remove this code/note
    // BitmapInfo.biClrUsed = 0;
    // BitmapInfo.biClrImportant = 0;

    // Lyle: Maybe we can just allocate this ourselves
    BitmapHandle = CreateDIBSection(
        BitmapDeviceContext, &BitmapInfo,
        DIB_RGB_COLORS,
        &BitmapMemory,
        0, 0);
}

internal void
Win32UpdateWindow(HDC DeviceContext, int X, int Y, int Width, int Height)
{
    StretchDIBits(DeviceContext,
                  X, Y, Width, Height,    // Destination
                  X, Y, Width, Height,    // Source
                  BitmapMemory,
                  &BitmapInfo,
                  DIB_RGB_COLORS, SRCCOPY);   // MSDN for info
}

LRESULT CALLBACK 
Win32MainWindowCallback(HWND Window,      // Function originally: WindowProc
                        UINT Message,            // Message Windows is asking to handle
                        WPARAM WParam, 
                        LPARAM LParam)
{
    LRESULT Result = 0;       // An LRESULT returns saying what we did with the message

    switch(Message)
    {
        case WM_SIZE:
        {
            RECT ClientRect;
            GetClientRect(Window, &ClientRect);
            int Width = ClientRect.right - ClientRect.left;
            int Height = ClientRect.bottom - ClientRect.top;
            Win32ResizeDIBSection(Width, Height);
        } break;

        case WM_CLOSE:
        {
            // Lyle: Handle this with a msg to the user?
            Running = false;
        } break;

        case WM_ACTIVATEAPP:
        {
            OutputDebugStringA("WM_ACTIVATEAPP\n");
        } break;


        case WM_DESTROY:
        {
            // Lyle: Handle this as an error - recreate window?
            Running = false;            
        } break;

        case WM_PAINT:
        {
            PAINTSTRUCT Paint;
            HDC DeviceContext = BeginPaint(Window, &Paint);
            int X = Paint.rcPaint.left;
            int Y = Paint.rcPaint.top;
            int Width = Paint.rcPaint.right - Paint.rcPaint.left;
            int Height = Paint.rcPaint.bottom - Paint.rcPaint.top;
            Win32UpdateWindow(DeviceContext, X, Y, Width, Height);
            EndPaint(Window, &Paint);
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

    WindowClass.lpfnWndProc = Win32MainWindowCallback;     // Pointer to a function; window events
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
            Running = true;
            while(Running)
            {
                MSG Message;
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
            // Lyle: Logging
        }
    } 
    else
    {
        // Lyle: Logging
    }

    return (0);
}
