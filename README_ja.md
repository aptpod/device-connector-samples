Device Connector Samples
========================

Device Connector Samples は、 intdash Edge Agnet のユースケースに合わせた、デバイスコネクターを使用するサンプル集です。

## 事前準備
### インストール
[intdash Edge Agent デベロッパーガイド](https://docs.intdash.jp/manual/intdash-agent-developer-guide/latest/ja/intdash-agent-developer-guide-ja.pdf) を参考にインストールしてください。

## サンプルコード

| タイトル               | 概要                                                | Python | C++ | bash |
|:----------------------|:---------------------------------------------------|:------:|:---:|:----:|
| Hello intdash!     | デバイスコネクターを作成して、文字列をintdashサーバーに送信します。          | [o](samples/hello-intdash/python/README_ja.md) | [o](samples/hello-intdash/c++/README_ja.md) | [o](samples/hello-intdash/bash/README_ja.md) |
| Downstream         | デバイスコネクターを作成して、文字列をintdashサーバーから受信します。          | - | [o](samples/downstream/c++/README_ja.md) | - |
| [Motion JPEG](samples/mjpeg/README_ja.md) | プリセットのデバイスコネクターを使用して、UVCデバイスからMotionJPEGを取得し、intdashサーバーに送信します。 | - | - | - |
