Downstream! (Python)
=======================

このデバイスコネクターは、intdashサーバーからデータを受信する（ダウンストリームを行う）ためのサンプルです。

ダウンストリームを行う場合、Agentはintdashサーバーからデータを受信すると、そのデータをデバイスコネクターに渡します。
このデバイスコネクターは、Agentから受け取ったデータを標準出力に出力します。

このデバイスコネクターはPythonで作成されています。

## 前提条件

### 実行環境
- intdash Edge Agent 1.20.0 以降
- Python3
- [Hello intdash! (Python)](../hello-intdash/README_ja.md) を実行可能であること（以下の使用手順では、「Hello intdash! (Python)」から送信されたデータをこのサンプル「Downstream (Python)」で受信します。）


## 手順

1. 設定ファイルに接続先の情報を書き込む

    このフォルダの manager.conf に、接続先情報を記載します。変更箇所は下記です。
    
    - `"host"` : 接続先サーバー（例：dummy.intdash.jp）
    - `"my_id"` : 受信側エッジのエッジUUID（例：01234567-0123-0123-0123-0123456789AB）。
    - `"my_secret"` : 受信側エッジのクライアントシークレット（例：0123456789ABCDEFGHIJ0123456789abcdefghij0123456789ABCDEFGHIJ0123）
    - `"ctlr_id"` : 送信元エッジID（[Hello intdash! (python)](../hello-intdash/README_ja.md)で設定した`"my_id"`のID）

2. 設定ファイルにデバイスコネクターのパスを書き込む

    このフォルダの manager.conf に、起動するデバイスコネクターのパスを記載します。変更箇所は下記です。

    - loggersオブジェクトの `"path"`: このサンプルフォルダにあるdownstream-connector.pyのフルパス

3. デバイスコネクターに実行権限を付与する

    下記コマンドの <サンプルフォルダのフルパス> に適切なパスを設定し、実行権限を付与してください。

    ```
    $ chmod 755 <サンプルフォルダのフルパス>/downstream-connector.py
    ````

4. アップストリームの実行

    [Hello intdash! (python)](../hello-intdash/README_ja.md) を実行してください。

5. 実行

    下記コマンドの <サンプルフォルダのフルパス> に適切なパスを設定し実行してください。

    ```
    $ sudo LD_LIBRARY_PATH=/opt/vm2m/lib /opt/vm2m/sbin/intdash-edge-manager -C <サンプルフォルダのフルパス>/manager.conf -p /var/run/intdash-edge-manager2.pid
    ```

## 実行結果


```
Timestamp: 1.262887000, id: abc, data: b'Hello intdash!'
Timestamp: 1.763174000, id: abc, data: b'Hello intdash!'
```

manager.confに設定されているとおり、以下のすべてに合致するデータポイントを受信します。受信したデータは標準出力に表示されます。

| 送信データの情報    　| 設定されている値          |
|:-------------------|:-----------------------|
| 送信元エッジ         | `"ctlr_id"`に設定したエッジ |
| チャンネル           | 100                    |
| データタイプ         | String (0x0a)          |
| データID            | abc                    |


### その他

標準出力には様々なログが出力されます。通信に関する主要なログは下記になります。

| ログ                                                 | 意味                                                                              |
|:----------------------------------------------------|:----------------------------------------------------------------------------------|
| `Receive 2 units`      | intdash Edgeがサーバーから受信できた秒間データ数です。                          |

---
[^1]: このサンプルでは、複数の intdash Edge Agnet を一つのターゲットデバイス上で動作させるために、Agentが使用するファイルのパスは[Hello intdash! (python)](../hello-intdash/README_ja.md)とは異なるパスになるように設定してあります。
