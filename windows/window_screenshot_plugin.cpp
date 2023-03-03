#include "window_screenshot_plugin.h"

// This must be included before many other Windows headers.
#include <windows.h>

// For getPlatformVersion; remove unless needed for your plugin implementation.
#include <VersionHelpers.h>

#include <flutter/method_channel.h>
#include <flutter/plugin_registrar_windows.h>
#include <flutter/standard_method_codec.h>

#include <memory>
#include <sstream>

#include <gdiplus.h>
#include <atlimage.h>
#include <fstream>

namespace window_screenshot {

    typedef struct tagMonData
    {
        int current;
        MONITORINFOEXW* info;
    } MonData;

    //Note these need to be called the samet
    const char* screenshotFilename_char = "screenshot.bmp";
    LPCWSTR screenshotFilename_wchar = L"screenshot.bmp";

// static
void WindowScreenshotPlugin::RegisterWithRegistrar(
    flutter::PluginRegistrarWindows *registrar) {
  auto channel =
      std::make_unique<flutter::MethodChannel<flutter::EncodableValue>>(
          registrar->messenger(), "window_screenshot",
          &flutter::StandardMethodCodec::GetInstance());

  auto plugin = std::make_unique<WindowScreenshotPlugin>();

  channel->SetMethodCallHandler(
      [plugin_pointer = plugin.get()](const auto &call, auto result) {
        plugin_pointer->HandleMethodCall(call, std::move(result));
      });

  registrar->AddPlugin(std::move(plugin));
}


WindowScreenshotPlugin::WindowScreenshotPlugin() {}

WindowScreenshotPlugin::~WindowScreenshotPlugin() {}

BOOL EnumProc(HMONITOR hMonitor, HDC hdcMonitor, LPRECT lprcMonitor, LPARAM dwData)
{
    MonData* data = (MonData*)dwData;
    data->info[data->current].cbSize = sizeof(MONITORINFOEXW);
    return GetMonitorInfoW(hMonitor, &(data->info[data->current++]));
}

BOOL GetAllMonitorInfo(MonData* data)
{
    return EnumDisplayMonitors(NULL, NULL, (MONITORENUMPROC)(&EnumProc), (LPARAM)(data));
}

std::string WStringToString(const std::wstring& wstr)
{
    std::string str;
    size_t size;
    str.resize(wstr.length());
    wcstombs_s(&size, &str[0], str.size() + 1, wstr.c_str(), wstr.size());
    return str;
}

BOOL SaveBmp(HBITMAP hBitmap, LPCWSTR FileName)
{
    HDC hDC;
    int iBits;
    WORD wBitCount;
    DWORD dwPaletteSize = 0, dwBmBitsSize = 0, dwDIBSize = 0, dwWritten = 0;
    BITMAP Bitmap;
    BITMAPFILEHEADER bmfHdr;
    BITMAPINFOHEADER bi; 
    LPBITMAPINFOHEADER lpbi;
    HANDLE fh, hDib, hPal, hOldPal = NULL;

    hDC = CreateDC(_T("DISPLAY"), NULL, NULL, NULL);
    iBits = GetDeviceCaps(hDC, BITSPIXEL) * GetDeviceCaps(hDC, PLANES);
    DeleteDC(hDC);
    if (iBits <= 1) wBitCount = 1;
    else if (iBits <= 4) wBitCount = 4;
    else if (iBits <= 8) wBitCount = 8;
    else wBitCount = 24;

    GetObject(hBitmap, sizeof(Bitmap), (LPSTR)&Bitmap);
    bi.biSize = sizeof(BITMAPINFOHEADER);
    bi.biWidth = Bitmap.bmWidth;
    bi.biHeight = Bitmap.bmHeight;
    bi.biPlanes = 1;
    bi.biBitCount = wBitCount;
    bi.biCompression = BI_RGB;
    bi.biSizeImage = 0;
    bi.biXPelsPerMeter = 0;
    bi.biYPelsPerMeter = 0;
    bi.biClrImportant = 0;
    bi.biClrUsed = 0;

    dwBmBitsSize = ((Bitmap.bmWidth * wBitCount + 31) / 32) * 4 * Bitmap.bmHeight;

    hDib = GlobalAlloc(GHND, dwBmBitsSize + dwPaletteSize + sizeof(BITMAPINFOHEADER));
    lpbi = (LPBITMAPINFOHEADER)GlobalLock(hDib);
    *lpbi = bi;
    hPal = GetStockObject(DEFAULT_PALETTE);
    if (hPal)
    {
        hDC = ::GetDC(NULL);
        hOldPal = ::SelectPalette(hDC, (HPALETTE)hPal, FALSE);
        RealizePalette(hDC);
    }

    GetDIBits(hDC, hBitmap, 0, (UINT)Bitmap.bmHeight, (LPSTR)lpbi + sizeof(BITMAPINFOHEADER)
        + dwPaletteSize, (BITMAPINFO*)lpbi, DIB_RGB_COLORS);

    if (hOldPal)
    {
        ::SelectPalette(hDC, (HPALETTE)hOldPal, TRUE);
        RealizePalette(hDC);
        ::ReleaseDC(NULL, hDC);
    }
    fh = CreateFile(FileName, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS,
        FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN, NULL);

    if (fh == INVALID_HANDLE_VALUE) return FALSE;

    bmfHdr.bfType = 0x4D42; // "BM" 
    dwDIBSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + dwPaletteSize + dwBmBitsSize;
    bmfHdr.bfSize = dwDIBSize;
    bmfHdr.bfReserved1 = 0;
    bmfHdr.bfReserved2 = 0;
    bmfHdr.bfOffBits = (DWORD)sizeof(BITMAPFILEHEADER) + (DWORD)sizeof(BITMAPINFOHEADER) + dwPaletteSize;
    WriteFile(fh, (LPSTR)&bmfHdr, sizeof(BITMAPFILEHEADER), &dwWritten, NULL);
    WriteFile(fh, (LPSTR)lpbi, dwDIBSize, &dwWritten, NULL);
    GlobalUnlock(hDib);
    GlobalFree(hDib);
    CloseHandle(fh);
    return TRUE;
}

void getScreenshot(int x1,int y1,int x2, int y2)
{
    int w, h;
    w = x2 - x1;
    h = y2 - y1;

    // copy screen to bitmap
    HDC     hScreen = GetDC(NULL);
    HDC     hDC = CreateCompatibleDC(hScreen);

    HBITMAP hBitmap = CreateCompatibleBitmap(hScreen, w, h);
    HGDIOBJ old_obj = SelectObject(hDC, hBitmap);
    BitBlt(hDC, 0, 0, w, h, hScreen, x1, y1, SRCCOPY);

    LPCWSTR filename = screenshotFilename_wchar;
    SaveBmp(hBitmap, filename);

    SelectObject(hDC, old_obj);
    DeleteDC(hDC);
    ReleaseDC(NULL, hScreen);
    DeleteObject(hBitmap);
}
  
std::vector<BYTE> readFile(const char* filename)
{
    // open the file:
    std::ifstream file(filename, std::ios::binary);

    // Stop eating new lines in binary mode!!!
    file.unsetf(std::ios::skipws);

    // get its size:
    std::streampos fileSize;

    file.seekg(0, std::ios::end);
    fileSize = file.tellg();
    file.seekg(0, std::ios::beg);

    // reserve capacity
    std::vector<BYTE> vec;
    vec.reserve(fileSize);

    // read the data:
    vec.insert(vec.begin(),
        std::istream_iterator<BYTE>(file),
        std::istream_iterator<BYTE>());

    return vec;
}

bool isKeyDown(int key)
{
    return (GetAsyncKeyState(key) & (1 << 16));
}

void WindowScreenshotPlugin::HandleMethodCall(
    const flutter::MethodCall<flutter::EncodableValue> &method_call,
    std::unique_ptr<flutter::MethodResult<flutter::EncodableValue>> result) 
{
  if (method_call.method_name().compare("getMonitors") == 0)
  {

      int cMonitors = GetSystemMetrics(SM_CMONITORS);
      MonData data;
      data.current = 0;
      data.info = (MONITORINFOEXW*)calloc(cMonitors, sizeof(MONITORINFOEXW));

      if (!GetAllMonitorInfo(&data)) { result->NotImplemented(); }

      std::vector<flutter::EncodableValue> forFlutter;
      for (int i = 0; i < cMonitors; i++)
      {
          std:: wstring deviceName(data.info[i].szDevice);          
          forFlutter.push_back(flutter::EncodableValue(WStringToString(deviceName)));
      }

    result->Success(flutter::EncodableList(forFlutter));
    free(data.info);
  } 
  else if (method_call.method_name().compare("getMonitorScreenshot") == 0)
  {
      const auto* arguments = std::get_if<flutter::EncodableMap>(method_call.arguments());
      
      auto monitor_arg = arguments->find(flutter::EncodableValue("monitorNumber"));
      auto timeToWait_arg = arguments->find(flutter::EncodableValue("delayTimerInSeconds"));

      int monitor  = -1;
      if (monitor_arg != arguments->end())
      {
          monitor = std::get<int>(monitor_arg->second);
      }

      int timeToWait = -1;
      if (timeToWait_arg != arguments->end())
      {
          timeToWait = std::get<int>(timeToWait_arg->second);
      }

      if (monitor == -1) {
          result->Error("Invalid mointor value");
      }

      if (timeToWait == -1) {
          result->Error("Invalid timeToWait value");
      }

      int cMonitors = GetSystemMetrics(SM_CMONITORS);
      MonData data;
      data.current = 0;
      data.info = (MONITORINFOEXW*)calloc(cMonitors, sizeof(MONITORINFOEXW));

      if (!GetAllMonitorInfo(&data)) { result->NotImplemented(); }

      if (monitor < cMonitors && data.info != nullptr)
      {   
          //Sleep(timeToWait*1000);
          while (!isKeyDown(VK_F5)) 
          {
              if (isKeyDown(VK_F9)) 
              {
                  result->Error("Canceled Keyboard Action");
                  break;
              }
          }
          getScreenshot(data.info[monitor].rcMonitor.left, data.info[monitor].rcMonitor.top, data.info[monitor].rcMonitor.right, data.info[monitor].rcMonitor.bottom);
          std::vector<BYTE> screenshotAsUint8List = readFile(screenshotFilename_char);
          remove(screenshotFilename_char);
          
          result->Success(flutter::EncodableValue(screenshotAsUint8List));
      }
      else {
          result->Error("Monitor could not be found");
      }
  }
  else {
    result->NotImplemented();
  }

}


}  // namespace window_screenshot
