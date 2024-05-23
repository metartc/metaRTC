import 'package:flutter/material.dart';
import 'package:flutter/services.dart';



void main() {
  runApp(const YangMetaplayerApp());
}

class YangMetaplayerApp extends StatelessWidget {
  const YangMetaplayerApp({super.key});
  @override
  Widget build(BuildContext context) {
    return MaterialApp(
      title: 'metaplayer-flutter',
      theme: ThemeData(

        colorScheme: ColorScheme.fromSeed(seedColor: Colors.deepPurple),
        useMaterial3: true,
      ),
      home: const YangMetaplayerHome(title: 'metaplayer_flutter'),
    );
  }



}

class YangMetaplayerHome extends StatefulWidget {
  const YangMetaplayerHome({super.key, required this.title});
  final String title;

  @override
  State<YangMetaplayerHome> createState() => _YangMetaplayerHomeState();
}

class _YangMetaplayerHomeState extends State<YangMetaplayerHome> {
  static const int textureWidth = 960;
  static const int textureHeight = 540;
  static const MethodChannel channel =  MethodChannel('yang_channel_texture');
  String m_b_text="play";
  bool m_isStart=false;
  TextEditingController m_urlText = new TextEditingController(text:"webrtc://192.168.3.5/live/livestream");
 
  final Future<int?> m_textureId =
      channel.invokeMethod('create', <int>[textureWidth, textureHeight]);

  Future<void> play() async {
      String url=m_urlText.text;
    await channel.invokeMethod('playRtc', <String>[url]);
    setState(() {
      m_b_text="stop";
    });
    
  }

  Future<void> stopRtc() async {

    await channel.invokeMethod('closeRtc');
    setState(() {
       m_b_text="play";
    });
   
  }

void play_click(){

    if(!m_isStart){
       play();
      m_isStart=!m_isStart;
    }else{
      stopRtc();
      m_isStart=!m_isStart;
    }
   
  }



  @override
  void dispose(){
  
    super.dispose();
  }

  @override
  Widget build(BuildContext context) {
  
    
    return Scaffold(
      appBar: AppBar(
        backgroundColor: Theme.of(context).colorScheme.inversePrimary,
        title: Text(widget.title),
      ),
      body: Container(
        child: Column(
        children: <Widget>[Row(

          mainAxisAlignment: MainAxisAlignment.start,
          children: <Widget>[
            const Text(
              'URL:',
            ),
             Expanded(
              child:TextField(
                  controller:m_urlText,
                  decoration: InputDecoration(
                    hintText: 'webrtc://127.0.0.1/live/livestream',
                  ),
              ),
            ),
             FloatingActionButton(
              onPressed:  play_click,
              tooltip: 'play(stop) rtc',
              child: Text(m_b_text),
            ), 
          ],
        ),
      
        FutureBuilder<int?>(
              future: m_textureId,
              builder: (BuildContext context, AsyncSnapshot<int?> snapshot) {
                if (snapshot.hasData) {
                  if (snapshot.data != null) {
                    return SizedBox(
                      width: textureWidth.toDouble(),
                      height: textureHeight.toDouble(),
                      child: Texture(textureId: snapshot.data!),
                    );
                  } else {
                    return const Text('Error creating texture');
                  }
                } else {
                  return const Text('Creating texture...');
                }
              },
            )
        ]
        )
      ),
      // This trailing comma makes auto-formatting nicer for build methods.
    );
  }
}
