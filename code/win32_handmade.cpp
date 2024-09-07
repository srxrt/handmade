// Run the .\shell.bat file from an admin cmd
// run .\build.bat to build
// devenv ..\handmade_build\win32_handmade.exe to open vs
// ctrl+d selects the word at the cursor
// ctrl + -> jump over words


#include <windows.h>
#include <stdint.h>
#include <iostream>

#define local_persist static
#define global_variable static
#define internal static

typedef uint8_t uint8;
typedef uint16_t uint16;
typedef uint32_t uint32;
typedef uint64_t uint64;


//TODO: this is global for now
global_variable bool Running;
global_variable BITMAPINFO BitmapInfo;
global_variable void *BitmapMemory;
global_variable int BitmapWidth ;
global_variable int BitmapHeight;
global_variable int BytesPerPixel = 4;

internal void
RenderWeirdGradient(int XOffset, int YOffset){

    int Width = BitmapWidth;
    int Height = BitmapHeight;
    int Pitch = Width*BytesPerPixel;
    uint8 *Row = (uint8*) BitmapMemory;
    for(int Y=0; Y<BitmapHeight; ++Y)
    {
        uint32 *Pixel = (uint32*)Row;
        for(int X=0; X<BitmapWidth; ++X)
        {
            uint8 Blue = (X+XOffset);
            uint8 Green = (Y+YOffset);

            *Pixel++ = ((Green << 8) | Blue);

        }

        Row += Pitch;
    }
}
// DIB - device independent bitmap=> resizes the dib section, or creates if it does not exist
internal void
WIN32ResizeDIBSection(int Width, int Height){

    if(BitmapMemory){
        VirtualFree(BitmapMemory,0, MEM_RELEASE);
    }

    BitmapWidth = Width;
    BitmapHeight=  Height;

    // TODO: free our DIBSection
    BitmapInfo.bmiHeader.biSize = sizeof(BitmapInfo.bmiHeader);
    BitmapInfo.bmiHeader.biWidth = BitmapWidth;
    BitmapInfo.bmiHeader.biHeight = -BitmapHeight;
    BitmapInfo.bmiHeader.biPlanes = 1;
    BitmapInfo.bmiHeader.biBitCount = 32;
    BitmapInfo.bmiHeader.biCompression = BI_RGB;


    int BitmapMemorySize =(BitmapWidth*BitmapHeight)*BytesPerPixel ;
    BitmapMemory = VirtualAlloc(0,BitmapMemorySize,MEM_COMMIT, PAGE_READWRITE);

    // TODO: Probably clear this to black

}

internal void
WIN32UpdateWindow(HDC DeviceContext,RECT *WindowRect){

        int WindowWidth = WindowRect->right - WindowRect->left;
        int WindowHeight = WindowRect->bottom - WindowRect->top;

     StretchDIBits(
        DeviceContext,
        /*
        X, Y, Width,Height,
        X, Y, Width,Height,
        */
        0, 0, BitmapWidth, BitmapHeight,
        0, 0, WindowWidth, WindowHeight,
        BitmapMemory,
        &BitmapInfo,
        DIB_RGB_COLORS,
        SRCCOPY
    );

 }


LRESULT CALLBACK
WIN32MainWindowCallback(HWND Window,
                        UINT Message,
                        WPARAM WParam,
                        LPARAM LParam)
{

    LRESULT Result = 0;
    //ctrl +] to indent selected lines
    switch (Message)
    {
        case WM_SIZE:
        {

            RECT ClientRect;
            GetClientRect(Window, &ClientRect);
            int Height = ClientRect.bottom - ClientRect.top;
            int Width = ClientRect.right - ClientRect.left;
            WIN32ResizeDIBSection(Width, Height);


        }break;
        case WM_DESTROY:
        {
            Running = false;
            OutputDebugString("WM_DESTROY\n");

        }break;
        case WM_CLOSE:
        {
            Running = false;
            OutputDebugString("WM_CLOSE\n");

        }break;
        case WM_ACTIVATEAPP:
        {
            OutputDebugString("WM_ACTIVATEAPP\n");

        }break;
        case WM_PAINT:
        {
            PAINTSTRUCT Paint;
            HDC DeviceContext = BeginPaint(Window, &Paint);
            int X = Paint.rcPaint.left;
            int Y = Paint.rcPaint.top;
            int Width = Paint.rcPaint.bottom - Paint.rcPaint.top;
            int Height = Paint.rcPaint.right - Paint.rcPaint.left;
            RECT ClientRect;
            GetClientRect(Window, &ClientRect);
            WIN32UpdateWindow(DeviceContext,&ClientRect);
            EndPaint(Window, &Paint);


        }break;
        default: {
            OutputDebugString("DEFAULT\n");
            Result = DefWindowProc(Window, Message, WParam, LParam);
        }break;

        return Result;
    }

}




int CALLBACK WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR lpCmdLine,
                     int nCmdShow)
{
    //WNDCLASS is a struct, you can look up
    WNDCLASS WindowClass = {}; //this initialisez all of the values of the struct to zero;
    WindowClass.style = CS_OWNDC|CS_HREDRAW|CS_VREDRAW;
    WindowClass.lpfnWndProc =WIN32MainWindowCallback;
    WindowClass.hInstance = hInstance;
    WindowClass.lpszClassName = "HandmadeHeroWindowClass";

    if(RegisterClass(&WindowClass)){
        HWND Window = CreateWindowEx(
                                            0,
                                            WindowClass.lpszClassName,
                                            "Handmade Hero",
                                            WS_OVERLAPPEDWINDOW|WS_VISIBLE,
                                            CW_USEDEFAULT,
                                            CW_USEDEFAULT,
                                            CW_USEDEFAULT,
                                            CW_USEDEFAULT,
                                            0,
                                            0,
                                            hInstance,
                                            0
                                            );

        if(Window){
            Running = true;
                int XOffset = 0;
                int YOffset = 0;
            while(Running){
                MSG Message;
                while(PeekMessage(&Message, 0, 0, 0,PM_REMOVE)){
                    if(Message.message == WM_QUIT){
                        Running = false;
                    }
                    TranslateMessage(&Message);
                    DispatchMessage(&Message);
                }

                RenderWeirdGradient(XOffset,YOffset);

                HDC DeviceContext = GetDC(Window);
                RECT ClientRect;
                GetClientRect(Window, &ClientRect);
                WIN32UpdateWindow(DeviceContext, &ClientRect);
                ReleaseDC(Window, DeviceContext);
                ++XOffset;
                //++YOffset;
            }

        }else{
            //TODO: Logging
        }




    }else{
        //TODO logging
    }

    return 0;

}

