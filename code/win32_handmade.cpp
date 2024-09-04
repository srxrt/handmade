// Run the .\shell.bat file from an admin cmd
// run .\build.bat to build
// devenv ..\handmade_build\win32_handmade.exe to open vs
// ctrl+d selects the word at the cursor
// ctrl + -> jump over words


#include <windows.h>
#define local_persist static
#define global_variable static
#define internal static


//TODO: this is global for now
global_variable bool Running;
global_variable BITMAPINFO BitmapInfo;
global_variable void *BitmapMemory;
global_variable HBITMAP BitmapHandle;
global_variable HDC BitmapDeviceContext;

// DIB - device independent bitmap=> resizes the dib section, or creates if it does not exist
internal void
WIN32ResizeDIBSection(int Width, int Height){

    // TODO: free our DIBSection
    if(BitmapHandle){
        DeleteObject(BitmapHandle);
    }
   if(!BitmapDeviceContext){
        HDC BitmapDeviceContext = CreateCompatibleDC(0);
   }


    BitmapInfo.bmiHeader.biSize = sizeof(BitmapInfo.bmiHeader);
    BitmapInfo.bmiHeader.biWidth = Width;
    BitmapInfo.bmiHeader.biHeight = Height;
    BitmapInfo.bmiHeader.biPlanes = 1;
    BitmapInfo.bmiHeader.biBitCount = 32;
    BitmapInfo.bmiHeader.biCompression = BI_RGB;


    BitmapHandle =  CreateDIBSection(
        BitmapDeviceContext,&BitmapInfo,
        DIB_RGB_COLORS,
        &BitmapMemory,
        0,
        0
    );

}

internal void
WIN32UpdateWindow(HDC DeviceContext,int X,int Y,int Width,int Height){
     StretchDIBits(
        DeviceContext,
        X, Y, Width,Height,
        X, Y, Width,Height,
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
            int Width = ClientRect.bottom - ClientRect.top;
            int Height = ClientRect.right -ClientRect.left;
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
            WIN32UpdateWindow(DeviceContext,X,Y, Width, Height);
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
        HWND WindowHandle = CreateWindowEx(
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

        if(WindowHandle){
            MSG Message;
            Running = true;
            while(Running){
                BOOL MessageResult = GetMessageA(&Message, 0, 0, 0);
                if(MessageResult > 0){
                    TranslateMessage(&Message);
                    DispatchMessage(&Message);

                }else{
                    break;
                }


            }

        }else{
            //TODO: Logging
        }




    }else{
        //TODO logging
    }

    return 0;

}

