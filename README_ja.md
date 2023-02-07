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

| タイトル               | 概要                                                | Python | C++ | bash | 設定ファイル |
|:----------------------|:---------------------------------------------------|:------:|:------:|:---:|:----:|
| Hello intdash!     | intdashサーバーにデータを送信する場合のデバイスコネクターのサンプルです。文字列を生成してintdashサーバーに送信します。| [o](samples/hello-intdash/python) | [o](samples/hello-intdash/c++) | [o](samples/hello-intdash/bash) | - |
| Downstream         | intdashサーバーからデータを受信する場合のデバイスコネクターのサンプルです。intdashサーバーから受信した文字列を標準出力に出力します。| [o](samples/downstream/python) | [o](samples/downstream/c++) | - | - |
| Motion JPEG | プリセットのデバイスコネクターを使用するための設定ファイルのサンプルです。UVCデバイスからMotion JPEGを取得し、intdashサーバーに送信します。 | - | - | - | [o](samples/mjpeg) |





