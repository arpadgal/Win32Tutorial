
#define SafeRelease(p) if(p) { (p)->Release(); (p) = nullptr; }

void LoadFile(LPCWSTR path, LPVOID &data, LONG &length)
{
  TCHAR wAbsolutePath[MAX_PATH];
  GetModuleFileName(NULL, wAbsolutePath, MAX_PATH);
  wAbsolutePath[wcsrchr(wAbsolutePath, L'\\') - wAbsolutePath + 1] = L'\0';
  wcscat_s(wAbsolutePath, path);

  FILE* wFileHandle = nullptr;
  _wfopen_s(&wFileHandle, wAbsolutePath, L"rb");
  fseek(wFileHandle, 0, SEEK_END);
  length = ftell(wFileHandle);
  fseek(wFileHandle, 0, SEEK_SET);
  data = malloc(length);
  fread_s(data, length, length, 1, wFileHandle);
  fclose(wFileHandle);
}