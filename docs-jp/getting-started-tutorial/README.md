<p align="right">
<a href="../../getting-started-tutorial/README.md">English</a> | <a>日本語</a>
</p>

<table style="width:100%">
  <tr>
<td align="center" width="100%" colspan="6"><img src="https://www.xilinx.com/content/dam/xilinx/imgs/press/media-kits/corporate/xilinx-logo.png" width="30%"/><h1>2018.3 SDSoC™ 開発環境チュートリアル</h1>
<a href="https://github.com/Xilinx/SDSoC-Tutorials/branches/all">ほかのバージョンを参照</a>
</td>
  </tr>
  <tr>
    <td colspan="5" align="center"><h2>概要</h2></td>
  </tr>
    <tr>
    <td align="center">概要</td>
    <td align="center"><a href="lab-1-introduction-to-the-sdsoc-development-environment.md">演習 1: SDSoC 開発環境の概要</a></td>
    <td align="center"><a href="lab-2-performance-estimation.md">演習 2: パフォーマンスの見積もり</a></td>
    <td align="center"><a href="lab-3-optimize-the-application-code.md">演習 3: アプリケーション コードの最適化</a></td>
    <td align="center"><a href="lab-4-optimize-the-accelerator-using-directives.md">演習 4: 指示子を使用したアクセラレータの最適化</a></td>
  </tr>
  <tr>
    <td align="center"><a href="lab-5-task-level-pipelining.md">演習 5: タスク レベルのパイプライン</a></td>
    <td align="center"><a href="lab-6-debug.md">演習 6: デバッグ</a></td>
    <td align="center"><a href="lab-7-hardware-debug.md">演習 7: ハードウェア デバッグ</a></td>
    <td align="center"><a href="lab-8-emulation.md">演習 8: エミュレーション</a></td>
    <td align="center"><a href="lab-9-installing-applications-from-github.md">演習 9: GitHub からのサンプルのインストール</a></td>
    </tr>
</table>

## 概要  

このチュートリアルでは、SDx&trade; IDE (Integrated Design Environment) ツールを使用してソフトウェア関数をアクセラレーションするために使用可能な SDSoC&trade; プラットフォームの作成方法を説明します。

>**:pushpin: 注記:**  このチュートリアルには、複数の演習が含まれます。それぞれで大まかな手順が説明された後、細かい手順が説明されていますので、スキル レベルに合った方の手順を参照してください。大まかな手順を終了するのにヘルプが必要な場合は詳細な手順を参照したり、細かい手順を飛ばして次の大まかな手順に進んだりできます。  

## ハードウェアおよびソフトウェア要件

このチュートリアルを実行するには、SDx IDE v2018.2 をインストールしておく必要があります。インストール手順は、『SDx 環境リリース ノート、インストール、およびライセンス ガイド』 ([UG1238](https://japan.xilinx.com/cgi-bin/docs/rdoc?v=2018.2;d=ug1238-sdx-rnil.pdf)) を参照してください。SDx ツールをインストールすると、SDx IDE と必要な Vivado Design Suite と SDK (ソフトウェア開発キット) ソフトウェアがインストールされます。

## 関連情報
 - <a href="lab-1-introduction-to-the-sdsoc-development-environment.md">演習 1: SDSoC 開発環境の概要</a>
 - <a href="lab-2-performance-estimation.md">演習 2: パフォーマンスの見積もり</a>
 - <a href="lab-3-optimize-the-application-code.md">演習 3: アプリケーション コードの最適化</a>
 - <a href="lab-4-optimize-the-accelerator-using-directives.md">演習 4: 指示子を使用したアクセラレータの最適化</a>
 - <a href="lab-5-task-level-pipelining.md">演習 5: タスク レベルのパイプライン</a>
 - <a href="lab-6-debug.md">演習 6: デバッグ</a>
 - <a href="lab-7-hardware-debug.md">演習 7: ハードウェア デバッグ</a>
 - <a href="lab-8-emulation.md">演習 8: エミュレーション</a>
 - <a href="lab-9-installing-applications-from-github.md">演習 9: GitHub からのサンプルのインストール</a>


<hr/>
<p align="center"><sup>Copyright&copy; 2019-2019 Xilinx</sup></p>

この資料は表記のバージョンの英語版を翻訳したもので、内容に相違が生じる場合には原文を優先します。資料によっては英語版の更新に対応していないものがあります。日本語版は参考用としてご使用の上、最新情報につきましては、必ず最新英語版をご参照ください。