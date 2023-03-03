#include "include/window_screenshot/window_screenshot_plugin_c_api.h"

#include <flutter/plugin_registrar_windows.h>

#include "window_screenshot_plugin.h"

void WindowScreenshotPluginCApiRegisterWithRegistrar(
    FlutterDesktopPluginRegistrarRef registrar) {
  window_screenshot::WindowScreenshotPlugin::RegisterWithRegistrar(
      flutter::PluginRegistrarManager::GetInstance()
          ->GetRegistrar<flutter::PluginRegistrarWindows>(registrar));
}
