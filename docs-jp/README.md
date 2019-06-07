<p align="right">
<a href="../README.md">English</a> | <a>日本語</a>
</p>
<table width="100%">
  <tr width="100%">
    <td align="center"><img src="https://www.xilinx.com/content/dam/xilinx/imgs/press/media-kits/corporate/xilinx-logo.png" width="30%"/><h1>2018.3 SDSoC™ 開発環境チュートリアル</h1>
    <a href="https://github.com/Xilinx/SDSoC-Tutorials/branches/all">ほかのバージョンを参照</a>
    </td>
 </tr>
 </table>

 <table style="width:100%">
 <tr>
 <td width="35%" align="center"><b>チュートリアル</b>
 <td width="65%" align="center"><b>説明</b>
 </tr>
 <tr>
 <td align="center"><a href="./getting-started-tutorial/README.md">概要</a></td>
 <td>このチュートリアルでは、SDx™ IDE (Integrated Development Environment) ツールを使用してソフトウェア関数をアクセラレーションするために使用可能な SDSoC 環境プラットフォームの作成方法を説明します。</td>
 </tr>
 <tr>
 <td align="center"><a href="./opencv-to-xfopencv-migration-tutorial/README.md">OpenCV から xfOpenCV への移行</a></td>
 <td>このチュートリアルでは、OpenCV を xfOpenCV (ザイリンクス SoC および FPGA 用に最適化されたハードウェア アクセラレーション済みの関数) へ移行する方法について説明します。xfOpenCV を使用するように最適化すると、コードがエンベデッド GPU よりも最大で 40 倍、CPU よりも 100 倍高速に実行できます。</td>
 </tr>
  <tr>
 <td align="center"><a href="./platform-creation-tutorial/README.md">プラットフォームの作成</a></td>
 <td>このチュートリアルでは、SDSoC プラットフォームの作成方法を説明し、そのプラットフォーム上で SDSoC アプリケーション例を作成して実行します。</td>
 </tr>
   </table>

## 概要

SDSoC (Software-Defined System-On-Chip) 環境は、Zynq-7000 SoC および Zynq UltraScale+ MPSoC プラットフォームを使用してヘテロジニアス エンベデッド システムをインプリメントするための Eclipse ベースの統合設計環境 (IDE) です。SDSoC 環境では、ソフトウェア エンジニアおよびシステム アーキテクト用に、使いやすい Eclipse ベースの IDE を使用したエンベデッド C/C++ アプリケーション開発環境と、ヘテロジニアス Zynq SoC 開発用の包括的なデザイン ツールが提供されています。SDSoC 環境には、プログラマブル ロジックでの自動ソフトウェア アクセラレーションや、システム接続の自動生成などを実行する、フル システム最適化 C/C++/OpenCL コンパイラが含まれます。SDSoC 環境内のプログラミング モデルは、ソフトウェア エンジニアが簡単に理解できるように設計されています。アプリケーションは C/C++/OpenCL コードで記述され、プログラマがターゲット プラットフォームとハードウェアにコンパイルするアプリケーション内の関数のサブセットを特定します。この後、SDSoC システム コンパイラによりアプリケーションがハードウェアとソフトウェアにコンパイルされ、ファームウェア、オペレーティング システム、アプリケーション実行ファイルを含むブート イメージを含めた完全なエンベデッド システムが Zynq デバイスにインプリメントされます。

SDSoC 環境では、C/C++/OpenCL 関数の Zynq デバイス内の Arm CPU だけでなくプログラマブル ロジック ファブリックへのクロスコンパイルおよびリンクを含め、ソフトウェア抽象層を増加することによりハードウェアが抽象化されます。プログラマブル ハードウェアで実行するプログラム関数のユーザー仕様に基づいて、プログラム解析、タスク スケジューリング、プログラマブル ロジックおよびエンベデッド CPU へのバインディングが実行されるほか、ハードウェアおよびソフトウェア コード生成により、ハードウェアとソフトウェア コンポーネント間の通信および連携が調整されます。  

SDSoC 環境 2018.2 リリースでは、Zynq-7000 SoC を搭載した ZC702、ZC706、および Zed 開発ボード、Zynq UltraScale+ MPSoC を搭載した ZCU102 開発ボードがサポートされています。その他のプラットフォームは、パートナーから入手できます。詳細は、[SDSoC 環境のウェブ ページ](https://www.xilinx.com/products/design-tools/software-zone/sdsoc.md)を参照してください。

<hr/>
<p align="center"><sup>Copyright&copy; 2019-2019 Xilinx</sup></p>

この資料は表記のバージョンの英語版を翻訳したもので、内容に相違が生じる場合には原文を優先します。資料によっては英語版の更新に対応していないものがあります。日本語版は参考用としてご使用の上、最新情報につきましては、必ず最新英語版をご参照ください。
