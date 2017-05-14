#include "Main.h"
#include "MainWindow.h"

INT APIENTRY wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, INT nCmdShow)
{
  MainWindow::ApplicationInit(hInstance);
  auto wMainWindow = new MainWindow();
  wMainWindow->Show(nCmdShow);

  //Event loop
  MSG wMessage;
  while (GetMessage(&wMessage, NULL, 0, 0))
  {
    TranslateMessage(&wMessage);
    DispatchMessage(&wMessage);
  }

  delete wMainWindow;
  return wMessage.wParam;
}