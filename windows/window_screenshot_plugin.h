#ifndef FLUTTER_PLUGIN_WINDOW_SCREENSHOT_PLUGIN_H_
#define FLUTTER_PLUGIN_WINDOW_SCREENSHOT_PLUGIN_H_

#include <flutter/method_channel.h>
#include <flutter/plugin_registrar_windows.h>

#include <memory>


namespace window_screenshot {

class WindowScreenshotPlugin : public flutter::Plugin {
 public:
  static void RegisterWithRegistrar(flutter::PluginRegistrarWindows *registrar);

  WindowScreenshotPlugin();

  virtual ~WindowScreenshotPlugin();

  // Disallow copy and assign.
  WindowScreenshotPlugin(const WindowScreenshotPlugin&) = delete;
  WindowScreenshotPlugin& operator=(const WindowScreenshotPlugin&) = delete;

 private:
  // Called when a method is called on this plugin's channel from Dart.
  void HandleMethodCall(
      const flutter::MethodCall<flutter::EncodableValue> &method_call,
      std::unique_ptr<flutter::MethodResult<flutter::EncodableValue>> result);
};

}  // namespace window_screenshot

#endif  // FLUTTER_PLUGIN_WINDOW_SCREENSHOT_PLUGIN_H_
