/*    // =========================================================================
    // FileName:   win32_handmade.cpp
    // Date:       1 March 2015
    // Revision:   3 March 2015
    // Creator:    Lyle Denman @lobsterhands
    // =========================================================================
*/

#include <windows.h>
#include <stdint.h>

// Lyle: these are for building; will be changed later
#define internal static             // local to specific source file
#define local_persist static
#define global_variable static

typedef int8_t  int8;
typedef int16_t int16;
typedef int32_t int32;
typedef int64_t int64;

typedef uint8_t  uint8;
typedef uint16_t uint16;
typedef uint32_t uint32;
typedef uint64_t uint64;

// Lyle: This is a static global (global for now)
global_variable bool Running;

global_variable BITMAPINFO BitmapInfo;
global_variable void *BitmapMemory;
global_variable int BitmapWidth;
global_variable int BitmapHeight;
global_variable int BytesPerPixel = 4;

internal void
RenderWeirdGradient(int XOffset, int YOffset)
{
    int Width = BitmapWidth;
    int Height = BitmapHeight;

    int Pitch = Width*BytesPerPixel;
    uint8 *Row = (uint8 *)BitmapMemory;
    for (int Y = 0; Y < BitmapHeight; ++Y) // Casey does a ++Y here; why?
    {
        uint32 *Pixel = (uint32 *)Row;
        for (int X = 0; X < BitmapWidth; ++X)
        {
            uint8 Blue = (X + XOffset);
            uint8 Green = (Y + YOffset);

            *Pixel++ = ((Green << 8) | Blue);
        }  

        Row += Pitch;
    }
}

internal void
Win32ResizeDIBSection(int Width, int Height)         // Windows bitmap display
{
    // Lyle: Maybe don't free first; free after then free first if that fails
    if(BitmapMemory)
    {
        VirtualFree(BitmapMemory, 0, MEM_RELEASE);
    }

    BitmapWidth = Width;
    BitmapHeight = Height;

    BitmapInfo.bmiHeader.biSize = sizeof(BitmapInfo.bmiHeader);
    BitmapInfo.bmiHeader.biWidth = BitmapWidth;
    BitmapInfo.bmiHeader.biHeight = -BitmapHeight;  // Neg biHeight = top-down DIB
    BitmapInfo.bmiHeader.biPlanes = 1;
    BitmapInfo.bmiHeader.biBitCount = 32;
    BitmapInfo.bmiHeader.biCompression = BI_RGB;

    int BitmapMemorySize = (BitmapWidth * BitmapHeight) *  BytesPerPixel;
    BitmapMemory = VirtualAlloc(0, BitmapMemorySize, MEM_COMMIT, PAGE_READWRITE);

    // TODO(Lyle): Probably clear this to black
}

internal void
Win32UpdateWindow(HDC DeviceContext, RECT *ClientRect, int X, int Y, int Width, int Height)
{
    int WindowWidth = ClientRect->right - ClientRect->left;
    int WindowHeight = ClientRect->bottom - ClientRect->top;
    StretchDIBits(DeviceContext,
                  0, 0, BitmapWidth, BitmapHeight,  // Destination
                  0, 0, WindowWidth, WindowHeight,  // Source
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
            RECT ClientRect;
            GetClientRect(Window, &ClientRect);
            Win32UpdateWindow(DeviceContext, &ClientRect, X, Y, Width, Height);
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
        HWND Window = 
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
        if(Window)
        {
            Running = true;
            int XOffset = 0;
            int YOffset = 0;
            while(Running)
            {
                MSG Message;
                while(PeekMessage(&Message, 0, 0, 0, PM_REMOVE))
                {
                    if(Message.message == WM_QUIT)
                    {
                        Running = false;
                    }

                    TranslateMessage(&Message);
                    DispatchMessageA(&Message);
                }
                RenderWeirdGradient(XOffset, YOffset);
                
                HDC DeviceContext = GetDC(Window);
                RECT ClientRect;
                GetClientRect(Window, &ClientRect);
                int WindowWidth = ClientRect.right - ClientRect.left;
                int WindowHeight = ClientRect.bottom - ClientRect.top;
                Win32UpdateWindow(DeviceContext, &ClientRect, 0, 0, WindowWidth, WindowHeight);
                ReleaseDC(Window, DeviceContext);

                ++XOffset;
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
