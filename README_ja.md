Device Connector Samples
========================

本リポジトリは、 intdash Edge Agnet 用のデバイスコネクターのサンプル集です。

デバイスコネクターは、各種デバイスとAgentの間でデータの仲介をするソフトウェアです。

本リポジトリには、用途（送信用か受信用か）、使用するデータタイプ、実装言語が異なるいくつかのデバイスコネクターを収録しています。

## サンプルを使用するための事前準備

本リポジトリに収録されているデバイスコネクターのサンプルを使用するには、intdash Edgeが必要です。

intdash Edge を使用するために必要な環境や、インストール方法、設定方法については、 [intdash Edge Agent デベロッパーガイド](https://docs.intdash.jp/manual/intdash-agent-developer-guide/latest/ja/intdash-agent-developer-guide-ja.pdf) を参照してください。

## サンプルコード

本リポジトリに収録されているサンプルは以下のとおりです。

| タイトル               | 概要                                                | Python | C++ | bash |
|:----------------------|:---------------------------------------------------|:------:|:---:|:----:|
| Hello intdash!     | デバイスコネクターを作成して、文字列をintdashサーバーに送信します。          | [o](samples/hello-intdash/python/README_ja.md) | [o](samples/hello-intdash/c++/README_ja.md) | [o](samples/hello-intdash/bash/README_ja.md) |
| Downstream         | デバイスコネクターを作成して、文字列をintdashサーバーから受信します。          | - | [o](samples/downstream/c++/README_ja.md) | - |
| [Motion JPEG](samples/mjpeg/README_ja.md) | プリセットのデバイスコネクターを使用して、UVCデバイスからMotionJPEGを取得し、intdashサーバーに送信します。 | - | - | - |
