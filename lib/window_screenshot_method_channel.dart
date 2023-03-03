import 'package:flutter/foundation.dart';
import 'package:flutter/services.dart';

import 'window_screenshot_platform_interface.dart';

/// An implementation of [WindowScreenshotPlatform] that uses method channels.
class MethodChannelWindowScreenshot extends WindowScreenshotPlatform {
  /// The method channel used to interact with the native platform.
  @visibleForTesting
  final methodChannel = const MethodChannel('window_screenshot');

  @override
  Future<String?> getPlatformVersion() async {
    final version = await methodChannel.invokeMethod<String>('getPlatformVersion');
    return version;
  }

  @override
  Future<List<String>?> getMonitors() async {
    final monitors = await methodChannel.invokeMethod("getMonitors");

    List<String> monitorsAsStr = [];
    for (dynamic cur in monitors){
      //print("${cur.toString()} has the run type${cur.runtimeType.toString()}");
      monitorsAsStr.add(cur as String);
    }
    return monitorsAsStr;
  }

  @override
  Future<Uint8List?> getMonitorScreenshot(int monitorNumber,int delayTimerInSeconds) async {
    final screenshot = await methodChannel.invokeMethod("getMonitorScreenshot",{"monitorNumber":monitorNumber,"delayTimerInSeconds":delayTimerInSeconds});
    //print(monitors);
    return screenshot;
  }
}
