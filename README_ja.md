Device Connector Samples
========================

本リポジトリは、 intdash Edge Agent用デバイスコネクターのサンプル集です。

デバイスコネクターは、各種デバイスと intdash Edge Agent（以下Agentと呼びます）の間でデータの仲介をするソフトウェアです。

本リポジトリには、用途（送信用か受信用か）、使用するデータタイプ、実装言語が異なるいくつかのデバイスコネクターを収録しています。

## サンプルを使用するための事前準備

本リポジトリに収録されているデバイスコネクターのサンプルを使用するには、intdash Edge Agentが必要です。

intdash Edge Agentを使用するために必要な環境や、インストール方法、設定方法については、 [intdash Edge Agent デベロッパーガイド](https://docs.intdash.jp/manual/intdash-agent-developer-guide/latest/ja/intdash-agent-developer-guide-ja.pdf) を参照してください。

## サンプルコード

本リポジトリに収録されているサンプルは以下のとおりです。

| タイトル               | 概要                                                | Python | C++ | bash |
|:----------------------|:---------------------------------------------------|:------:|:---:|:----:|
| Hello intdash!     | intdashサーバーにデータを送信する場合の基本のデバイスコネクターのサンプルです。文字列を生成してintdashサーバーに送信します。| [o](samples/hello-intdash/python/README_ja.md) | [o](samples/hello-intdash/c++/README_ja.md) | [o](samples/hello-intdash/bash/README_ja.md) |
| Downstream         | intdashサーバーからデータを受信する場合の基本のデバイスコネクターのサンプルです。intdashサーバーから受信した文字列を出力します。| - | [o](samples/downstream/c++/README_ja.md) | - |
| [Motion JPEG](samples/mjpeg/README_ja.md) | プリセットのデバイスコネクターを使用するサンプルです。UVCデバイスからMotion JPEGを取得し、intdashサーバーに送信します。 | - | - | - |





