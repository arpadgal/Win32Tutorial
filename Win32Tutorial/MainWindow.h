#pragma once
class MainWindow
{
private:
  static const LPCWSTR cClassName;
  static ATOM mClassId;
  static HINSTANCE mInstanceHandle;
  HWND mWindowHandle;
  static LRESULT CALLBACK MessageHandler(HWND windowHandle, UINT message, WPARAM wParam, LPARAM lParam);

  static const INT_PTR cRenderTimerId = 0;
  static const UINT cRenderInterval = 15;
  INT mFrameIndex = 0;
  void RenderFrame();

  IDXGISwapChain *mSwapChain;
  ID3D11Device *mDevice;
  ID3D11DeviceContext *mDeviceContext;
  ID3D11Texture2D *mBackBufferTexture;
  ID3D11RenderTargetView *mRenderTargetView;
  ID3D11Buffer *mVertexBuffer;
  ID3D11VertexShader *mVertexShader;
  ID3D11PixelShader *mPixelShader;
  ID3D11InputLayout *mInputLayout;
  D3D11_VIEWPORT mViewport;
  void InitializeRendering();
  void PrepareBackBuffer(int width, int height);
public:
  static void ApplicationInit(HINSTANCE instanceHandle);
  MainWindow();
  void Show(INT options);
  ~MainWindow();
};

