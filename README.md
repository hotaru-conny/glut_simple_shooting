# glut_simple_shooting
## 概要
大学2年の時に授業でglutを使った際に作成したシューティングゲームのプログラムです。

## 詳細
Visual Studioで、「NuGetパッケージの管理」から、nupengl.coreをインストールして使用します。
ソースファイルの拡張子はcppですが、コードはC言語のものになっています。

## 遊び方
Visual Studioで、デバッグなしで実行すると動作します。  
マウスで左右に移動し、クリックで弾を打ちます。  
ゲームオーバー画面はありませんが、Rキーを押すことで、ゲームをリセットして最初からプレイできます。  

![image](https://github.com/hotaru-conny/glut_simple_shooting/assets/63710354/a205affb-526c-493e-b24a-2f0ce0e1d0b0)

## コメント
プログラミングを始めてすぐのころに、GUIを作る方法が分からなかったので、glutの機能にワクワクしながら作成しました。  
しかし、オブジェクト指向やコードの分割を知らないままC言語で作ったので、1ファイルに処理が詰め込まれています。  
今となっては、オブジェクト指向が必要な理由を説明するための、反面教師的なプロジェクトとなりました。  
とはいえ、せっかく作ったものですし、大学の勉強でglutを使う方もいると思うので、参考に公開しておきます。  