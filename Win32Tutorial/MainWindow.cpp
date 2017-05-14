#include "MainWindow.h"
#include "Helpers.h"

#ifdef NDEBUG
  #define DEVICE_CREATION_FLAGS 0
#else
  #define DEVICE_CREATION_FLAGS D3D11_CREATE_DEVICE_DEBUG
#endif

const LPCWSTR MainWindow::cClassName = L"MainWindow";
ATOM MainWindow::mClassId;
HINSTANCE MainWindow::mInstanceHandle;

struct VertexPositionColor
{
  FLOAT X, Y, Z;
  FLOAT R, G, B, A;
};

void MainWindow::ApplicationInit(HINSTANCE instanceHandle)
{
  mInstanceHandle = instanceHandle;

  WNDCLASSEXW wClassDescription = { 0 };  
  wClassDescription.cbSize = sizeof(WNDCLASSEX);
  wClassDescription.hInstance = mInstanceHandle;
  wClassDescription.lpfnWndProc = MainWindow::MessageHandler;
  wClassDescription.lpszClassName = cClassName;
  wClassDescription.style = CS_HREDRAW | CS_VREDRAW;
  wClassDescription.hCursor = LoadCursor(NULL, IDC_ARROW);
  wClassDescription.hbrBackground = (HBRUSH)COLOR_WINDOW;
  mClassId = RegisterClassExW(&wClassDescription);
}

MainWindow::MainWindow()
{
  CreateWindowExW(NULL, cClassName, L"MainWindow", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, mInstanceHandle, this);
  SetWindowLongPtr(mWindowHandle, GWLP_USERDATA, (LONG_PTR)this);

  InitializeRendering();

  SetTimer(mWindowHandle, cRenderTimerId, cRenderInterval, NULL);
}

LRESULT CALLBACK MainWindow::MessageHandler(HWND windowHandle, UINT message, WPARAM wParam, LPARAM lParam)
{
  MainWindow* wThis = nullptr;
  if (message == WM_CREATE)
  {
    auto wCreateStruct = (CREATESTRUCT*)lParam;
    wThis = (MainWindow*)wCreateStruct->lpCreateParams;
    SetWindowLongPtr(windowHandle, GWLP_USERDATA, (LONG_PTR)wThis);
    wThis->mWindowHandle = windowHandle;
  }
  else
  {
    wThis = (MainWindow*)GetWindowLongPtr(windowHandle, GWLP_USERDATA);;
  }
  
  switch (message)
  {
  case WM_DESTROY:
    PostQuitMessage(0);
    break;
  case WM_TIMER:
    switch (wParam)
    {
    case cRenderTimerId:
      wThis->RenderFrame();
      break;
    }
    break;
  case WM_SIZE:
    {
      int wWidth = LOWORD(lParam);
      int wHeight = HIWORD(lParam);

      wThis->PrepareBackBuffer(wWidth, wHeight);
    }
    break;
  default:
    return DefWindowProcW(windowHandle, message, wParam, lParam);
    break;
  }
  return 0;
}

void MainWindow::Show(INT options)
{
  ShowWindow(mWindowHandle, options);
  UpdateWindow(mWindowHandle);
}

void MainWindow::RenderFrame()
{
  //Print frame index
  TCHAR wTitle[128];
  StringCbPrintfW(wTitle, 128, L"Frame: %d", mFrameIndex++);
  SetWindowTextW(mWindowHandle, wTitle);

  double wAngle = mFrameIndex % 120 / 60.0 * M_PI;
  double wR = sin(wAngle);
  double wG = sin(wAngle + M_PI / 1.5);
  double wB = sin(wAngle - M_PI / 1.5);
  FLOAT wColor[] = { wR, wG, wB, 1 };
  mDeviceContext->ClearRenderTargetView(mRenderTargetView, wColor);

  mDeviceContext->RSSetViewports(1, &mViewport);
  mDeviceContext->VSSetShader(mVertexShader, 0, 0);
  mDeviceContext->PSSetShader(mPixelShader, 0, 0);
  mDeviceContext->IASetInputLayout(mInputLayout);
  mDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
  mDeviceContext->OMSetRenderTargets(1, &mRenderTargetView, nullptr);
  
  UINT wStride = sizeof(VertexPositionColor);
  UINT wOffset = 0;
  mDeviceContext->IASetVertexBuffers(0, 1, &mVertexBuffer, &wStride, &wOffset);
  mDeviceContext->Draw(3, 0);

  mSwapChain->Present(0, 0);
}

void MainWindow::InitializeRendering()
{
  //Create double buffer and initialize device
  DXGI_SWAP_CHAIN_DESC wSwapChainDescription = { 0 };
  wSwapChainDescription.BufferCount = 1;
  wSwapChainDescription.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
  wSwapChainDescription.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
  wSwapChainDescription.OutputWindow = mWindowHandle;
  wSwapChainDescription.SampleDesc.Count = 1;
  wSwapChainDescription.Windowed = TRUE;

  D3D11CreateDeviceAndSwapChain(
    NULL,
    D3D_DRIVER_TYPE_HARDWARE,
    NULL,
    DEVICE_CREATION_FLAGS,
    NULL,
    NULL,
    D3D11_SDK_VERSION,
    &wSwapChainDescription,
    &mSwapChain,
    &mDevice,
    NULL,
    &mDeviceContext);

  //Create a triangle and load it to a vertex buffer
  VertexPositionColor wTriangleVertices[] =
  {
    { 0.0f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f },
    { 0.45f, -0.5, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f },
    { -0.45f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f }
  };

  D3D11_BUFFER_DESC wVertexBufferDescription = { 0 };
  wVertexBufferDescription.Usage = D3D11_USAGE_IMMUTABLE;
  wVertexBufferDescription.ByteWidth = sizeof(VertexPositionColor) * 3;
  wVertexBufferDescription.BindFlags = D3D11_BIND_VERTEX_BUFFER;
  wVertexBufferDescription.CPUAccessFlags = 0;

  D3D11_SUBRESOURCE_DATA wTriangleData = { 0 };
  wTriangleData.pSysMem = wTriangleVertices;

  mDevice->CreateBuffer(&wVertexBufferDescription, &wTriangleData, &mVertexBuffer);

  //Load shaders
  void* wData;
  long wLength;

  //Vertex shader
  LoadFile(L"VertexShader.cso", wData, wLength);
  mDevice->CreateVertexShader(wData, wLength, nullptr, &mVertexShader);
  
  //Input layout
  D3D11_INPUT_ELEMENT_DESC wInputLayoutDescription[] =
  {
    { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
  };

  mDevice->CreateInputLayout(wInputLayoutDescription, 2, wData, wLength, &mInputLayout);
  free(wData);

  //Pixel shader
  LoadFile(L"PixelShader.cso", wData, wLength);
  mDevice->CreatePixelShader(wData, wLength, nullptr, &mPixelShader);
  free(wData);
}

void MainWindow::PrepareBackBuffer(int width, int height)
{
  SafeRelease(mBackBufferTexture);
  SafeRelease(mRenderTargetView);

  mSwapChain->ResizeBuffers(2, width, height, DXGI_FORMAT_UNKNOWN, 0);
  mSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&mBackBufferTexture);
  mDevice->CreateRenderTargetView(mBackBufferTexture, NULL, &mRenderTargetView);
  
  mViewport.Width = width;
  mViewport.Height = height;
  mViewport.MinDepth = D3D11_MIN_DEPTH;
  mViewport.MaxDepth = D3D11_MAX_DEPTH;
}

MainWindow::~MainWindow()
{
  KillTimer(mWindowHandle, cRenderTimerId);

  SafeRelease(mDeviceContext);
  SafeRelease(mDevice);
  SafeRelease(mSwapChain);
  SafeRelease(mBackBufferTexture);
  SafeRelease(mRenderTargetView);
  SafeRelease(mVertexBuffer);
  SafeRelease(mVertexShader);
  SafeRelease(mPixelShader);
  SafeRelease(mInputLayout);

  DestroyWindow(mWindowHandle);
}
