audios.bgm.dataset.volume = 0.5;
audios.snd.dataset.volume = 0.5;
let scriptText = `
/img:novel1.jpg:0
/start
/bgm:novel1.mp3
画面を押す、Enter、→キーで次のテキストに進みます
←キーで戻ります/
必要なければ先へ進んでください
右下の小さいボタンから任意のテキストに飛ぶことができます/
Autoボタンでテキストを自動で進み、時間は音声または文字数で決まります。もう一度右下のボタンを押すことで停止します
ここから、何が出来るかサンプルを示していきます

音声付きテキストです。<span style="color:#8f8">タグも使えます</span>/snd:novel3.mp3
改行がある音声付きテキストです/
サンプルテキスト/snd:novel3.mp3

/bgm:novel2.mp3
/img:novel1.jpg,novel2.jpg
BGM、画像を変更しました
/bgm:novel1.mp3:2000
BGMを2秒かけて変更しました
/bgm:
BGMを停止しました。再び再生します
/bgm:novel2.mp3
再生しました
/bgm::2000
BGMを2秒かけて停止しました
/bgm:novel1.mp3:2000
BGMを再生しました。2秒で指定しましたが、停止していたので即座に再生されます

/snd:novel3.mp3
音声を再生しました/
素早く次の操作を行うと再生が終わる前に停止しますので、←キーで戻って試してもよいでしょう

次に、画像を変更します。違いが分かりにくいのでよく見るようにしてください
/img:novel2.jpg:1000
画像を1秒かけて変更しました
/img:novel1.jpg,novel2.jpg::50
画像を変更しました(ループ時間50ms)
/video:novel1.mp4
動画をループなしで再生しました
/color:#008
背景色を変更しました
/img:novel1.jpg,novel2.jpg:1000:500
画像を1秒かけて変更しました(ループ時間500ms)
/video:novel1.mp4:1000:1
ループ動画に1秒かけて変更しました。他と同様、引数は省略できます
/color:#080:1000
背景色を1秒かけて変更しました/
次に、2秒待機します
/wait:2000
2秒待機しました

/color:#800:500
/img:novel2.jpg:500
/color:#080:500
/img:novel1.jpg:500
/bgm:novel2.mp3
連続で変更するとこのような演出ができます/
以上です
`;
