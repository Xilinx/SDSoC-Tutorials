<table style="width:100%">
  <tr>
    <th width="100%" colspan="6"><img src="https://www.xilinx.com/content/dam/xilinx/imgs/press/media-kits/corporate/xilinx-logo.png" width="30%"/><h1>SDSoC 環境チュートリアル: 概要</h1>
</th>
  </tr>
  <tr>
    <td align="center"><a href="README.md">概要</a></td>
    <td align="center"><a href="lab-1-introduction-to-the-sdsoc-development-environment.md">演習 1: SDSoC 開発環境の概要</a></td>
    <td align="center"><a href="lab-2-performance-estimation.md">演習 2: パフォーマンスの見積もり</a></td>
    <td align="center"><a href="lab-3-optimize-the-application-code.md">演習 3: アプリケーション コードの最適化</a></td>
    <td align="center"><a href="lab-4-optimize-the-accelerator-using-directives.md">演習 4: 指示子を使用したアクセラレータの最適化</a></td>
  </tr>
  <tr>
    <td align="center"><a href="lab-5-task-level-pipelining.md">演習 5: タスク レベルのパイプライン</a></td>
    <td align="center"><a href="lab-6-debug.md">演習 6: デバッグ</a></td>
    <td align="center"><a href="lab-7-hardware-debug.md">演習 7: ハードウェア デバッグ</a></td>
    <td align="center">演習 8: エミュレーション</td>
    <td align="center"><a href="lab-9-installing-applications-from-github.md">演習 9: GitHub からのサンプルのインストール</a></td>
</table>

## 演習 8: エミュレーション  

このチュートリアルでは、SDx IDE でエミュレーション機能を使用する方法を示します。デバッガーを使用してデータ転送を可視化するには、SDSoC エミュレーターでアプリケーションを実行することをお勧めします。これにより、システム停止などの問題がわかるので、シミュレーション波形ビューで関連するデータ転送を調べて、そのデータ転送に関連するハードウェア ブロックの信号にアクセスできます。  

まず、デザインの OS とプラットフォームを指定し、プログラムでエミュレーションを実行します。このチュートリアルでは、アクセラレーションされたシステムで実行しているアプリケーションをデバッグします。  

**:pushpin: 注記:**  このチュートリアルは、ZC702 ボードがなくても終了できます。SDx プロジェクトを作成する際は、推奨されているエミュレーション アプリケーションのテンプレートが見つからない場合でも、ボードと使用可能なテンプレートの 1 つを選択してください。  

<details>
<summary><strong>手順 1: エミュレーションを実行するプロジェクトの作成</strong></summary>  

[Emulation Example] デザイン テンプレートを使用して ZC702 プラットフォームおよび Linux OS の新しい SDx プロジェクト (`lab8`) を作成します。SDx IDE でプロジェクトを作成する手順は、次のとおりです。  

  1. SDx IDE が起動します。  
  2. [File] → [New] → [SDx Project] をクリックします。  
  3. [Project Type] ページでは、デフォルトで [Application Project] がオンになっています。[Next] をクリックします。  
  4. [Project name] フィールドにプロジェクト名を指定します (例: `lab8`)。[Next] をクリックします。  
  5. [Platform] から [zc702] を選択します。[Next] をクリックします。  
  6. [System Configuration] ドロップダウン リストから [Linux] を選択します。[Next] をクリックします。  
  7. [Available Templates] から [Emulation Example] を選択し、[Finish] をクリックします。  
  8. [lab8] タブをクリックして SDx プロジェクト設定を選択します (タブが表示されていない場合は、[Project Explorer] で `project.sdx` ファイルをダブルクリックします)。[HW functions] パネルで、プロジェクトが作成されたときに mmult_accel 関数がハードウェア関数としてマークされていることを確認します。  
  9. ハードウェア関数が削除されていたり、マークされていない場合は、[Add HW Functions] アイコンをクリックして表示されたダイアログ ボックス内でハードウェア関数を指定します。  
  10. [SDx Project Settings] の [Active build configuration] プルダウン メニューから [Debug] を、[Target] プルダウン メニューから [Emulation] を選択します。[Emulation] モデルには、[Debug] と [Optimized] の 2 つのオプションがあります。[Debug] オプションを選択して、デバッグ情報のキャプチャを有効にします。デバッグ情報なしにしてエミュレーションを速めるには、[Optimized] プルダウン メニューの方を選択します。この演習では、デフォルト オプションの [Debug] を使用します。  
  11. [Target] に [Emulation] を選択すると、[Generate SD card image] は淡色表示になります。  

      ![](./images/fpd1527885363339.png)  

  12. [Generate emulation model] オプションをオンにしたまま、ビルド シンボルをクリックしてアプリケーションを構築します。  

</details>

<details>
<summary><strong>手順 2: エミュレーターの開始</strong></summary>

  1. [Xilinx] → [Start/Stop Emulator] をクリックします。  
  2. [Emulation] ダイアログ ボックスが開きます。プロジェクトと設定を選択します。  

     ![](./images/hra1517374817424.png)  

  3. 波形を表示するかどうかを選択します。波形を表示するようにすると、Vivado ツール セッションが開始されてシミュレーション ウィンドウが表示され、デザイン内のさまざまな信号の波形を確認できます。波形を表示しないようにするとエミュレーションが速く実行されます。[Show the Waveform] オプションをオンにします。  
  4. [Start] をクリックします。これは、ボードをオンにしたのと同じことです。  
  5. 波形ビューアーで表示する必要のある信号を追加します。これには、Vivado の [Scope] ウィンドウで関数を選択し、右クリックで [Add to Wave Window] をクリックします。その関数内の信号が波形ビューアーに追加されます。  

     ![](./images/wnf1527886268336.png)  

  6. 信号を選択したら、[Run All] または [Run for] ボタンクリックして、プログラマブル ロジック シミュレーションを開始します。  

</details>

<details>
<summary><strong>手順 3: アプリケーションの実行</strong></summary>

エミュレーターは、開始されるまで数秒かかります。エミュレーションの開始方法:  

  1. SDx IDE で [lab8] を右クリックして [Debug As] → [Launch on Emulator (SDx Application Debugger)] をクリックします。  

     ![](./images/kip1517375349372.png)  

  2. [Confirm Perspective Switch] ダイアログ ボックスが表示されます。[Yes] をクリックしてパースペクティブを切り替えます。  
  3. パースペクティブを [Debug] に切り替えると、コードを実際のハードウェアで実行しつつデバッグできます。  
  4. ツールバーの [Resume] アイコンクをクリックしてコードを実行します。  
  5. Vivado XSim シミュレータの波形ビューアーには、さまざまな信号のステートが表示されます。

</details>

### まとめ  
このチュートリアルを終了すると、次ができるようになります。

  * SDx IDE を使用して、エミュレーション モードでアプリケーションをダウンロードして実行。  
  * オプションで SDx IDE でソース コードをステップ スルーして、さまざまなレジスタおよびメモリを確認。  

<hr/>
<p align="center"><sup>Copyright&copy; 2018 Xilinx</sup></p>

この資料は表記のバージョンの英語版を翻訳したもので、内容に相違が生じる場合には原文を優先します。資料によっては英語版の更新に対応していないものがあります。日本語版は参考用としてご使用の上、最新情報につきましては、必ず最新英語版をご参照ください。