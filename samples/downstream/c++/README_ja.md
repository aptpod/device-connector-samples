Downstream (C++)
================

このサンプルコードでは、C++でデバイスコネクターを作成して、文字列をintdashから受信します。

## 前提条件

### 実行環境
- intdash Edge 1.15.0 以降
- [Hello intdash! (c++)](../hello-intdash/README_ja.md) を実行可能（このサンプルコードが受信するデータを、別途送信するために使用します）

### コンパイル環境
- 実行環境向けのバイナリをコンパイルできるC++コンパイラ

## 手順

1. 実行環境用のコンパイラでコンパイルする

    このサンプルフォルダにある`downstream-connector.cpp`をコンパイルします。
    ```
    c++ downstream-connector.cpp -o downstream-connector
    ``` 
    （c++は対象のコンパイラに置き換えてください）

2. 設定ファイルに接続先の情報を書き込む

    このフォルダの manager.conf に、接続先情報を記載します。
    
    - `"host"` : 接続先サーバー（例：dummy.intdash.jp）
    - `"ctlr_id"` : 送信元エッジID（[Hello intdash! (c++)](../hello-intdash/README_ja.md)で設定した`"my_id"`のID）
    - `"my_token"` : 接続エッジトークン（例：0123456789ABCDEFGHIJ0123456789abcdefghij01234567）

3. 設定ファイルにデバイスコネクターのパスを書き込む

    このフォルダの manager.conf に、起動するデバイスコネクターのパスを記載します。変更箇所は下記です。

    - loggersオブジェクトの `"path"`: ビルドしたcustomized-loggerのフルパス

4. アップストリームの実行

    [Hello intdash! (c++)](../hello-intdash/README_ja.md) を実行してください。

5. ダウンストリームの実行

    下記コマンド{サンプルフォルダのフルパス}に適切なパスを設定し実行してください。 [^1]

    ```
    $ sudo LD_LIBRARY_PATH=/opt/vm2m/lib /opt/vm2m/sbin/intdash-edge-manager -C {サンプルフォルダのフルパス}/manager.conf -p /var/run/intdash-edge-manager2.pid
    ```

## 実行結果

標準出力に受信したデータの情報が表示されます。

```
Timestamp: 1.610239000, id: abc, data: Hello intdash!
Timestamp: 2.110509000, id: abc, data: Hello intdash!
```

受信できるデータは下記のデータです。

| 送信データの情報    　| 設定されている値          |
|:-------------------|:-----------------------|
| 送信元エッジ         | `"ctlr_id"`に設定したエッジ |
| チャンネル           | 100                    |
| データタイプ         | String (0x0a)          |
| データID            | abc                    |


### その他

標準出力に出力には様々なログが出力されます。通信に関する主要なログは下記になります。

| ログ                                                 | 意味                                                                              |
|:----------------------------------------------------|:----------------------------------------------------------------------------------|
| `Receive 2 units`      | intdash Edgeがサーバーから受信できた秒間データ数です。                          |

---
[^1]: このサンプルコードでは、複数の intdash Edge Agnet を一つのターゲットデバイス上で動作させるために、Agentが使用するファイルのパスは[Hello intdash! (c++)](../hello-intdash/README_ja.md)とは異なるパスになるように設定してあります。