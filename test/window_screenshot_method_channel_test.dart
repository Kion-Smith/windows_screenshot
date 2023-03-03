import 'package:flutter/services.dart';
import 'package:flutter_test/flutter_test.dart';
import 'package:window_screenshot/window_screenshot_method_channel.dart';

void main() {
  MethodChannelWindowScreenshot platform = MethodChannelWindowScreenshot();
  const MethodChannel channel = MethodChannel('window_screenshot');

  TestWidgetsFlutterBinding.ensureInitialized();

  setUp(() {
    channel.setMockMethodCallHandler((MethodCall methodCall) async {
      return '42';
    });
  });

  tearDown(() {
    channel.setMockMethodCallHandler(null);
  });

  test('getPlatformVersion', () async {
    expect(await platform.getPlatformVersion(), '42');
  });
}
