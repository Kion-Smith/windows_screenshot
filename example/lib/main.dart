import 'dart:convert';

import 'package:flutter/material.dart';
import 'dart:async';

import 'package:flutter/services.dart';
import 'package:window_screenshot/window_screenshot.dart';
import 'dart:ui' as ui;
void main() {
  runApp(const MyApp());
}

class MyApp extends StatefulWidget {
  const MyApp({super.key});

  @override
  State<MyApp> createState() => _MyAppState();
}

class _MyAppState extends State<MyApp> {
  List<String> _listOfWindows = ['No Window data'];
  Uint8List? _image;
  final _windowScreenshotPlugin = WindowScreenshot();

  @override
  void initState() {
    super.initState();
    initPlatformState();
  }

  // Platform messages are asynchronous, so we initialize in an async method.
  Future<void> initPlatformState() async {
    List<String> listOfWindows;
    Uint8List? value;
    // Platform messages may fail, so we use a try/catch PlatformException.
    // We also handle the message potentially returning null.
    try {
      listOfWindows =
          await _windowScreenshotPlugin.getMonitors() ?? [];

      value = await _windowScreenshotPlugin.getMonitorScreenshot(0, 5);
    } on PlatformException {
      listOfWindows = ['Failed to get platform version.'];
    }

    // If the widget was removed from the tree while the asynchronous platform
    // message was in flight, we want to discard the reply rather than calling
    // setState to update our non-existent appearance.
    if (!mounted) return;

    setState(() {
      _listOfWindows = listOfWindows;
      _image = value;
    });
  }

  @override
  Widget build(BuildContext context) {
    // double w= MediaQuery.of(context).size.width*.7;
    // double h= MediaQuery.of(context).size.height*.7;
    return MaterialApp(
      home: Scaffold(
        appBar: AppBar(
          title: const Text('Plugin example app'),
        ),
        body: Center(
          child: Column(children: [
          Text('Running on: ${_listOfWindows.toString()}\n'),
            _image == null ? Container() : SizedBox( width: 1920/2, height: 1080/2, child:Image.memory(_image!)),
          ],),
        ),
      ),
    );
  }

  Future<ui.Image> loadImage(Uint8List img) async {
    final Completer<ui.Image> completer = Completer();
    ui.decodeImageFromList(img, (ui.Image img) {
      return completer.complete(img);
    });
    return completer.future;
  }



}
