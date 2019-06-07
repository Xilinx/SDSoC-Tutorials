<p align="right">
<a href="../../opencv-to-xfopencv-migration-tutorial/README.md">English</a> | <a>日本語</a>
</p>

<!-- <div style="page-break-after: always;"></div> -->
<!-- <div style="display: none;" media="print"> -->
<table style="width:100%">
    <tr>
        <th width="100%" colspan="6"><img src="https://www.xilinx.com/content/dam/xilinx/imgs/press/media-kits/corporate/xilinx-logo.png" width="30%"/><h1>SDSoC 環境チュートリアル: OpenCV の xfOpenCV への移行</h2>
        </th>
    </tr>
    <tr>
    <td align="center"><a >概要</a></td>
    <td align="center"><a href="lab-1-migrate-opencv-to-xfopencv.md">演習 1: OpenCV の xfOpenCV への移行</a></td>
    <td align="center"><a href="lab2-build-sdsoc-acceleration-project.md">演習 2: SDSoC アクセラレーション プロジェクトのビルド</a></td>
    </tr>
</table>
<!-- </div> -->

# 概要

xfOpenCV は、ザイリンクス SoC および FPGA 用に最適化されたハードウェア アクセラレーション済みの OpenCV 関数です。関数は、高位合成 (HLS) 用に完全に C/C++ で記述されています。  この演習では、SDx 2018.2 およびリリースされている ZCU102 reVISION プラットフォームを使用します。

OpenCV と xfOpenCV の主な違いは、典型的な OpenCV が CPU (x86、Arm など) での実行を目的に記述され、xfOpenCV がザイリンクス SoCs および FPGA での実行を目的に記述されて最適化されている点にあります。SoC および FPGA を最適化すると、コードはエンベデッド GPU よりも最大で 40 倍、CPU よりも 100 倍速く実行できます。すべてのコードが C/C++ で記述されるので、ユーザーのコンピューター ビジョン関数に合わせてカスタマイズしやすくなっています。

**:pushpin: 注記**
**このチュートリアルは、[reVISION 入門ガイド](https://github.com/Xilinx/Revision-Getting-Started-Guide/tree/2018.2-ja) で説明される OpenCV および reVISION プラットフォームには既に精通していると想定して説明をします。先に進める前に、このガイドをご確認ください。**

## 改定履歴
これは、このチュートリアルの初版です。

## 変更履歴
### 2018.2
初版。

## サポート
チュートリアル デザインのテクニカル サポートについては、次を参照してください。

* [ザイリンクス コミュニティ フォーラム](https://forums.xilinx.com/): テクニカルな詳細および問題に関する質問またはご意見を集めています。新しいトピックを上げる前に、既存の同様のトピックがないかどうかをご確認ください。新しいトピックを上げる必要がある場合は、その問題および質問に該当するサブフォーラムを選択し、チュートリアルの内容を具体的に示してください。

<hr/>

:arrow_forward:**次のトピック:**  [演習 1 - OpenCV の xfOpenCV への移行](lab-1-migrate-opencv-to-xfopencv.md)

<hr/>
<p align="center"><sup>Copyright&copy; 2018 Xilinx</sup></p>

この資料は表記のバージョンの英語版を翻訳したもので、内容に相違が生じる場合には原文を優先します。資料によっては英語版の更新に対応していないものがあります。日本語版は参考用としてご使用の上、最新情報につきましては、必ず最新英語版をご参照ください。