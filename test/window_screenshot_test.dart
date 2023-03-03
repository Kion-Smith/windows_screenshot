import 'package:flutter_test/flutter_test.dart';
import 'package:window_screenshot/window_screenshot.dart';
import 'package:window_screenshot/window_screenshot_platform_interface.dart';
import 'package:window_screenshot/window_screenshot_method_channel.dart';
import 'package:plugin_platform_interface/plugin_platform_interface.dart';

// class MockWindowScreenshotPlatform
//     with MockPlatformInterfaceMixin
//     implements WindowScreenshotPlatform {
//
//   @override
//   Future<String?> getPlatformVersion() => Future.value('42');
// }
//
// void main() {
//   final WindowScreenshotPlatform initialPlatform = WindowScreenshotPlatform.instance;
//
//   test('$MethodChannelWindowScreenshot is the default instance', () {
//     expect(initialPlatform, isInstanceOf<MethodChannelWindowScreenshot>());
//   });
//
//   test('getPlatformVersion', () async {
//     WindowScreenshot windowScreenshotPlugin = WindowScreenshot();
//     MockWindowScreenshotPlatform fakePlatform = MockWindowScreenshotPlatform();
//     WindowScreenshotPlatform.instance = fakePlatform;
//
//     expect(await windowScreenshotPlugin.getPlatformVersion(), '42');
//   });
//}
