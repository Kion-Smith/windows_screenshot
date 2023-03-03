
import 'dart:typed_data';

import 'window_screenshot_platform_interface.dart';

class WindowScreenshot {
  Future<String?> getPlatformVersion() {
    return WindowScreenshotPlatform.instance.getPlatformVersion();
  }

  Future<List<String>?> getMonitors() {
    return WindowScreenshotPlatform.instance.getMonitors();
  }

  Future<Uint8List?> getMonitorScreenshot(int monitorNumber,int delayTimerInSeconds) async {
    return WindowScreenshotPlatform.instance.getMonitorScreenshot(monitorNumber,delayTimerInSeconds);
  }
}
