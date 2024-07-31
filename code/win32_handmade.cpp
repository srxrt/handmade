// Run the .\shell.bat file from an admin cmd
// run .\build.bat to build
// devenv ..\handmade_build\win32_handmade.exe to open vs
// ctrl+d selects the word at the cursor
// ctrl + -> jump over words


#include <windows.h>


LRESULT CALLBACK MainWindowCallback(HWND Window,
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
            OutputDebugString("WM_SIZE\n");

        }break;
        case WM_DESTROY:
        {
            OutputDebugString("WM_DESTROY\n");

        }break;
        case WM_CLOSE:
        {
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
            static DWORD Operation = WHITENESS;
            PatBlt(DeviceContext, X, Y, Width, Height, Operation);
            if(Operation == WHITENESS){
                Operation = BLACKNESS;
            }else{
                Operation = WHITENESS;
            }
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
    WindowClass.lpfnWndProc = MainWindowCallback;
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

            for(;;){
                BOOL MessageResult = GetMessage(&Message, 0, 0, 0);
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

