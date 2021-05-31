Downstream (C++)
================

このデバイスコネクターは、intdashサーバーからデータを受信する（ダウンストリームを行う）ためのサンプルです。

ダウンストリームを行う場合、Agentはintdashサーバーからデータを受信すると、そのデータをデバイスコネクターに渡します。
このデバイスコネクターは、Agentから受け取ったデータを標準出力に出力します。

このデバイスコネクターはC++で作成されています。


## 前提条件

### 実行環境
- intdash Edge 1.15.0 以降
- [Hello intdash! (C++)](../hello-intdash/README_ja.md) を実行可能であること（以下の使用手順では、「Hello intdash! (C++)」から送信されたデータをこのサンプル「Downstream (C++)」で受信します。）

### コンパイル環境
- 実行環境向けのバイナリをコンパイルできるC++コンパイラ

## 使用手順

1. このサンプルフォルダにある`downstream-connector.cpp`を、実行環境用のコンパイラでコンパイルします。
    ```
    c++ downstream-connector.cpp -o downstream-connector
    ``` 
    （c++は対象のコンパイラに置き換えてください）

2. Agent用の設定ファイル（manager.conf）で、接続に関する情報を設定します。

    - `"host"` : 接続先サーバー（例：dummy.intdash.jp）
    - `"ctlr_id"` : 送信側エッジのUUID（[Hello intdash! (Ｃ++)](../hello-intdash/README_ja.md)で設定した`"my_id"`）
    - `"my_token"` : 受信側エッジのエッジトークン（例：0123456789ABCDEFGHIJ0123456789abcdefghij01234567）

3. Agent用の設定ファイル（manager.conf）で、使用するデバイスコネクター（downstream-connector）のパスを指定します。設定する箇所は以下のとおりです。

    - loggersオブジェクトの `"path"`: ビルドしたdownstream-connectorのフルパス

4. 送信側エッジアカウントを使ってアップストリームを実行します。

    [Hello intdash! (c++)](../hello-intdash/README_ja.md) を実行してください。

5. ダウンストリームを実行します。

    以下のコマンドで、intdash Edgeを起動します。その際、設定ファイルとして、手順2～3で準備したmanager.confを指定します。
    <サンプルフォルダーのフルパス> には適切なパスを設定してください。 [^1]

    ```
    $ sudo LD_LIBRARY_PATH=/opt/vm2m/lib /opt/vm2m/sbin/intdash-edge-manager -C <サンプルフォルダーのフルパス>/manager.conf -p /var/run/intdash-edge-manager2.pid
    ```

## 実行結果


```
Timestamp: 1.610239000, id: abc, data: Hello intdash!
Timestamp: 2.110509000, id: abc, data: Hello intdash!
```

manager.confに設定されているとおり、以下のすべてに合致するユニットを受信します。受信したデータは標準出力に表示されます。


| フィールド            | 設定されている値          |
|:-------------------|:-----------------------|
| 送信元エッジ         | `"ctlr_id"`に設定したエッジ |
| チャンネル           | 100                    |
| データタイプ         | String (0x0a)          |
| データID            | abc                    |


### その他

標準出力にはさまざまなログメッセージが出力されます。通信に関する主なログメッセージは以下のとおりです。

| ログメッセージ                                          | 意味                                                                              |
|:----------------------------------------------------|:----------------------------------------------------------------------------------|
| `Receive 2 units`      | intdash Edgeがサーバーから受信できた秒間ユニット数です。                          |

---
[^1]: このサンプルでは、複数の intdash Edge Agnet を1つPC上で動作させるため、Agentが使用する設定ファイルのパスは[Hello intdash! (C++)](../hello-intdash/README_ja.md)とは異なるパスにしてあります。