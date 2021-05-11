Customize DeviceConnector Cpp Sample
====================================

このサンプルでは、C++でカスタムDeviceConnectorを使用して、文字列をintdashに送信する方法を紹介します。

## 前提条件

- intdash Edge 1.15.0
- intdash Edge のインストール先は /opt/vm2m

## 手順

1. ターゲット環境用のコンパイラでコンパイルする

    このサンプルフォルダにある`customized-logger.cpp`をコンパイルします。
    ```
    c++ customized-logger.cpp -o customized-logger
    ``` 
    （c++は対象のコンパイラに置き換えてください）

2. 設定ファイルに接続先の情報を書き込む

    このフォルダの manager.conf に、接続先情報を記載します。変更箇所は下記です。２箇所ずつある事に注意してください。
    
    - `"host"` : 接続先サーバー、２箇所（例：dummy.intdash.jp）
    - `"my_id"` : 接続エッジID、２箇所（例：01234567-0123-0123-0123-0123456789AB）
    - `"my_token"` : 接続エッジトークン、２箇所（例：0123456789ABCDEFGHIJ0123456789abcdefghij01234567）

3. 設定ファイルにカスタムDeviceConnectorパスを書き込む

    このフォルダの manager.conf に、起動するDeviceConnectorのパスを記載します。変更箇所は下記です。

    - loggersオブジェクトの `"path"`: ビルドしたcustomized-loggerのフルパス

4. 実行モジュールの存在確認をする

    下記コマンドで必要なモジュールが/opt/vm2mにインストールされていることを確認します。

    ```
    $ test -x /opt/vm2m/sbin/intdash-edge-manager && echo OK || echo NG
    $ test -x /opt/vm2m/sbin/intdash-edge-client && echo OK || echo NG
    $ test -x /opt/vm2m/sbin/intdash-edge-logger && echo OK || echo NG
    $ test -f /opt/vm2m/lib/mod_http.so && echo OK || echo NG
    $ test -f /opt/vm2m/lib/mod_websocket.v2.so && echo OK || echo NG
    ```

5. 実行

    intdash Edgeの実行します。下記コマンド<サンプルフォルダのフルパス>に適切なパスを設定し実行してください。

    ```
    $ sudo LD_LIBRARY_PATH=/opt/vm2m/lib /opt/vm2m/sbin/intdash-edge-manager -C <サンプルフォルダのフルパス>/manager.conf
    ```

## 実行結果

intdashサーバーに下記データが送信されます。

| 送信データの情報    　| 設定されている値          |
|:-------------------|:-----------------------|
| 送信元エッジ         | `"my_id"`に設定したエッジ |
| チャンネル           | 100                    |
| データタイプ         | String (0x0a)          |
| データID            | abc                    |
| データ              | Hello intdash!         |
| 送信タイミング       | 約500[msec]毎に20回     |


### その他

標準出力に出力には様々なログが出力されます。通信に関する主要なログは下記になります。

| ログ                                                 | 意味                                                                              |
|:----------------------------------------------------|:----------------------------------------------------------------------------------|
| `REALTIME 15 units and 0 basetimes with ID:10`      | intdash Edgeがサーバーにリアルタイムで送信できた秒間データ数です。                          |
