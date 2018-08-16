<table style="width:100%">
  <tr>
    <th width="100%" colspan="6"><img src="https://www.xilinx.com/content/dam/xilinx/imgs/press/media-kits/corporate/xilinx-logo.png" width="30%"/><h1>SDSoC 環境チュートリアル: 概要</h1>
</th>
  </tr>
  <tr>
    <td align="center"><a href="README.md">概要</a></td>
    <td align="center"><a href="lab-1-introduction-to-the-sdsoc-development-environment.md">演習 1: SDSoC 開発環境の概要</a></td>
    <td align="center"><a href="lab-2-performance-estimation.md">演習 2: パフォーマンスの見積もり</a></td>
    <td align="center">演習 3: アプリケーション コードの最適化</td>
    <td align="center"><a href="lab-4-optimize-the-accelerator-using-directives.md">演習 4: 指示子を使用したアクセラレータの最適化</a></td>
  </tr>
  <tr>
    <td align="center"><a href="lab-5-task-level-pipelining.md">演習 5: タスク レベルのパイプライン</a></td>
    <td align="center"><a href="lab-6-debug.md">演習 6: デバッグ</a></td>
    <td align="center"><a href="lab-7-hardware-debug.md">演習 7: ハードウェア デバッグ</a></td>
    <td align="center"><a href="lab-8-emulation.md">演習 8: エミュレーション</a></td>
    <td align="center"><a href="lab-9-installing-applications-from-github.md">演習 9: GitHub からのサンプルのインストール</a></td>
</table>


##演習 3: アプリケーション コードの最適化  

このチュートリアルでは、SDx 環境で生成されたハードウェア/ソフトウェア システムを最適化するためにコードを変更する方法について説明します。ビルド エラーの詳細を確認して、コードを修正する方法についても説明します。  

>**:pushpin: 注記:**  この演習は、ZC702 ボードがなくても終了できます。SDSoC 環境プロジェクトを作成する際に、推奨されている [Matrix Multiplication and Addition] テンプレートが見つからない場合は、ボードと使用可能なアプリケーションの 1 つを選択してください。たとえば、より小型の Zynq-7000 デバイスを含む MicroZed ボードなどのボードの場合、使用可能なテンプレートに [Matrix Multiplication and Addition (area reduced)] アプリケーションがリストされます。このチュートリアルでは、ボード上でアプリケーションを実行することはありません。ZC702 の手順に従うことで、学習目標が達成できるようになっています。  

<details>
<summary><strong>システム ポートおよび DMA の概要</strong></summary>

Zynq®-7000 SoC デバイス システムでは、ARM A9 プロセッサがアクセスするメモリに、オンチップ キャッシュと大型のオフチップ DDR メモリの 2 レベルがあります。プログラマブル ロジック側からは、ハードウェア デザインが作成されます。このハードウェア デザインには、ハードウェア関数がシステム インターフェイス ポートを介してプロセッサ システム メモリにデータを直接読み書きできるように、ダイレクト メモリ アクセス (DMA) ブロックが含まれることがあります。  
  

次の簡略化された図に示すように、Zynq デバイスのプロセッシング システム (PS) ブロックには 3 種類のシステム ポートが含まれ、プロセッサ メモリから Zynq デバイスのプログラマブル ロジックにデータを転送するために使用されます。これらの 3 種類のシステム ポートは、ハードウェアがコヒーレント方式でプロセッサの L2 キャッシュに直接アクセスできるようにするアクセラレータ コヒーレンシ ポート (ACP)、Asynchronous FIFO Interface (AFI) を使用してプロセッサ キャッシュをバイパスしてハードウェアから DDR メモリまたはオンチップ メモリにダイレクト バッファー アクセスを提供するハイ パフォーマンス ポート 0 ～ 3 (HP0 ～ 3)、およびプロセッサがハードウェア レジスタに対して読み出し/書き込みを実行できるようにする汎用 I/O (GP0/GP1) です。  

![](./images/zjf1517376261638.png)    

ARM A9 プロセッサで実行されるソフトウェアがハードウェア関数を呼び出す場合、実際には `sds++` で生成されたスタブ関数が呼び出され、3 種類のシステム ポート (GPx、ACP、AFI) を介してプロセッサ メモリからデータをハードウェア関数に送信し、ハードウェア関数からプロセッサ メモリにデータを戻す下位ドライバーが呼び出されます。  

次の表に、これらのシステム ポートとその特性を示します。`sds++` コンパイラでは、データ転送に最適なシステム ポートが自動的に選択されますが、プラグマを使用してこの選択を変更することもできます。  

| システム ポート プロパティ        | プロパティ  |
| ----------------------------- |-------------|
| ACP  | ハードウェア関数には、PS L2 を介した DDR へのキャッシュ コヒーレンシ アクセスがあります。|
| AFI (HP) | ハードウェア関数には、PS メモリ コントローラーを介した DDR への非キャッシュ コヒーレンシ アクセスがあります。|
| GP | プロセッサがハードウェア関数のデータを直接読み出し/書き込み。大型データ転送には不向き。|
|MIG | ハードウェア関数は、MIG IP メモリ コントローラーを介して PL から DDR にアクセスします。|


>**:pushpin: 注記:**  最適化の詳細は、『SDSoC 環境プロファイリングおよび最適化ガイド』 ([UG1235](https://japan.xilinx.com/cgi-bin/docs/rdoc?v=2018.2;d=ug1235-sdsoc-optimization-guide.pdf)) を参照してください。  

</details>

<details>
<summary><strong>手順 1: 新規プロジェクトの作成</strong></summary>

  1. [Matrix Multiplication and Addition] デザイン テンプレートを使用して、[ZC702 platform] および [Linux] システム コンフィギュレーションで新しい SDx™ IDE プロジェクト (`lab3`) を作成します。   

  2. [lab3] タブをクリックして [SDx Project Settings] を開きます。タブが表示されていない場合は、[Project Explorer] タブの [lab3] プロジェクトの下の project.sdx ファイルをダブルクリックします。   

  3. [HW functions] パネルには、`madd` および `mmult` 関数がハードウェア アクセラレーション用にマークされた関数のリストに表示されます。   

  4. 最高のランタイム パフォーマンスにするには、[Active Build Configuration] オプションをクリックして [Release] を選択して [Release] コンフィギュレーションに切り替えます。または、[Build] アイコンから [Release] を選択するか、プロジェクトを右クリックして [Build Configurations] → [Set Active] → [Release] をクリックします。[Release] ビルド コンフィギュレーションでは、[Debug] ビルド コンフィギュレーションよりも高いコンパイラ最適化設定が使用されます。

</details>

<details>
<summary><strong>手順 2: システム ポートの指定</strong></summary>

sys_port プラグマを使用すると、SDSoC システム コンパイラ ポートの代わりに、ACP または Zynq-7000 SoC Processing System (PS) の AFI ポートのいずれかを選択してプロセッサ メモリにアクセスできます。  

  1. SDx システム コンパイラで生成されるシステムの構造を検証するために SD カードのブート イメージを生成する必要はないので、プロジェクト リンカー オプションをビットストリーム、ブート イメージ、ビルドが生成されないように設定します。  
     1. [lab3] タブをクリックして [SDx Project Settings] を開きます。  
     2. [Generate SD card image] チェック ボックスをオフにします。  

  2. [Project Explorer] タブのプロジェクトの最上位フォルダーを右クリックして [Build Project] クリックします。  

  3. ビルドが終了したら、[Assistant] ビューで lab3 プロジェクトの [Data Motion Network Report] をクリックして、データ モーション ネットワーク レポートを確認します。このレポートには、各ハードウェア関数のハードウェア/ソフトウェア接続性を記述する表が含まれます。  

     一番右の [Connection] 列には、行列乗算の入力配列に割り当てられた DMA のタイプ (AXIDMA_SIMPLE= simple DMA) と使用された Processing System 7 IP ポートが示されます。次の図は、sys_port プラグマを追加する前の data_motion.md ファイルの一部を表示しています。

     ![](./images/dfd1527614240611.png)  

  4. sys_port プラグマを追加します。  
     1. [Project Explorer] タブで src フォルダーの下の `mmultadd.h` ファイルをダブルクリックしてソース エディターでファイルを開きます。  
     2. `mmult` 関数の宣言直前に、次を挿入して、各入力配列それぞれに異なるシステム ポートを指定します。  
        `#pragma SDS data sys_port(A:ACP, B:AFI)`  

        ![](./images/sno1517376007043.png)  

    3. ファイルを保存します。

  5. プロジェクトの最上位フォルダーを右クリックして [Build Project] をクリックします。  

  6. ビルドが終了したら、データ モーション ネットワーク レポート (`data_motion.md` ファイル) を表示しているタブをクリックします。  

  7. [Data Motion Network Report] ペイン内をクリックし、コンテキスト メニューから [Refresh] をクリックします。  

     ![](./images/gqw1527616464174.png)  

     [Connection] 列に、行列乗算の各入力/出力配列に割り当てられたシステム ポートが表示されます。  

  8. プラグマ `#pragma SDS data sys_port(A:ACP, B:AFI)` を削除してファイルを保存します。

</details>

<details>
<summary><strong>手順 3: システム コンフポートの指定</strong></summary>

  sys_port プラグマを使用すると、SDSoC システム コンパイラ ポートの代わりに、ACP または Zynq-7000 SoC Processing System (PS) の AFI ポートのいずれかを選択してプロセッサ メモリにアクセスできます。  

  1. SDx システム コンパイラで生成されるシステムの構造を検証するために SD カードのブート イメージを生成する必要はないので、プロジェクト リンカー オプションをビットストリーム、ブート イメージ、ビルドが生成されないように設定します。  
     1. [lab3] タブをクリックして [SDx Project Settings] を開きます。  
     2. [Generate bitstream] および [Generate SD card image] チェック ボックスをオフにします。  

  2. [Project Explorer] タブのプロジェクトの最上位フォルダーを右クリックして [Build Project] クリックします。  

  3. ビルドが終了したら、[Reports] パネルで [Data Motion Network Report] をクリックして、データ モーション ネットワーク レポートを確認します。このレポートには、各ハードウェア関数のハードウェア/ソフトウェア接続性を記述する表が含まれます。  

     一番右の [Connection] 列には、行列乗算の入力配列に割り当てられた DMA のタイプ (AXIDMA_SIMPLE= simple DMA) と使用された Processing System 7 IP ポートが示されます。次の図は、sys_port プラグマを追加する前の `data_motion.md` ファイルの一部を表示しています。  

     ![](./images/plg1517376007047.png)  

  4. sys_port プラグマを追加します。  
     1. [Project Explorer] タブで src フォルダーの下の mmultadd.h ファイルをダブルクリックしてソース エディターでファイルを開きます。  
     2. `mmult` 関数の宣言直前に、次を挿入して、各入力配列それぞれに異なるシステム ポートを指定します。
	 
        `#pragma SDS data sys_port(A:ACP, B:AFI)`  

        ![](./images/sno1517376007043.png)

    3. ファイルを保存します。


  5. プロジェクトの最上位フォルダーを右クリックして [Build Project] をクリックします。  
  6. ビルドが終了したら、データ モーション ネットワーク レポート (data_motion.md ファイル) を表示しているタブをクリックします。  
  7. [Data Motion Network Report] ペイン内をクリックし、コンテキスト メニューから [Refresh] をクリックします。  

     ![](./images/prf1517375349351.png)  

     [Connection] 列に、行列乗算の各入力/出力配列に割り当てられたシステム ポートが表示されます。  

  8. プラグマ `#pragma SDS data sys_port(A:ACP, B:AFI)` を削除してファイルを保存します。  

</details>

<details>
<summary><strong>手順 4: エラーのレポート</strong></summary>

  次の手順を実行してエラーを発生させることができます。SDx IDE でエラーがどのように示されるかを確認してください。  

  1. src フォルダーからソース ファイル main.cpp を開き、ファイルの最後の方にある `std::cout` 文の最後のセミコロン (;) を削除します。  
     行の左端に黄色のボックスが表示されます。  

     ![](./images/qsw1517376007052.png)  

  2. 黄色のボックスにカーソルを置くと、ツール ヒントにセミコロンが足りないことが示されます。      
  3. セミコロンを挿入すると、黄色のボックスは消えます。  
  4. `std::cout` を `std::cou` に変更し、行の左端にピンク色のボックスが表示されることを確認します。  

     ![](./images/xpt1517376007008.png)  
  5. ピンク色のボックスにカーソルを置くと、`std::cou` ではなく正しい表記 `std::cout` が表示されます。  

     ![](./images/zey1517376007045.png)  

  6. `std::cou` を `std::cout` に変更してエラーを修正します。  
  7. `main()` で使用される変数を宣言する行をコメントアウトして、別のエラーを発生させます。  

     ![](./images/pjv1517375659059.png)  

  8. プロジェクトを保存してビルドします。ビルドが終了するまで待つ必要はありません。  
  9. コンソールをスクロールすると、エラー メッセージを確認できます。`Release/_sds/reports/sds_main.log` および `Release/_sds/reports/sds_mmult.log` ファイルを開いて、詳細なエラー レポートを確認します。  

     ![](./images/akh1517376007035.png)  

  10. 変数が宣言されている行をコメントアウトします。  

</details>

<details>
<summary><strong>手順 5: その他の演習</strong></summary>

  >**:pushpin: 注記:**  このセクションの手順は、オプションです。  

  Linux がアプリケーションのターゲット OS として使用される場合、アプリケーションのメモリ割り当ては Linux とサポートされるライブラリで処理されます。スコープ内のスタックで配列を宣言する場合 (`int a[10000];`) や、標準の `malloc()` 関数を使用してダイナミックに割り当てる場合は、プロセッサと Linux により提供される仮想アドレス空間の連続するメモリのセクションが取得されます。このバッファーは、通常物理アドレス空間の複数の不連続ページに分割され、Linux でソフトウェアがその配列にアクセスするたびに仮想/物理アドレス変換が自動的に処理されます。  
  ただし、ハードウェア関数および DMA は物理アドレス空間にのみアクセスできるので、ソフトウェア ドライバーは各配列を仮想アドレスから物理アドレスに明示的に変換し、この物理アドレスを DMA またはハードウェア関数に供給する必要があります。各配列が物理アドレス空間の複数の不連続ページに分散していることがあるので、ドライバーは DMA に物理ページ アドレスのリストを供給する必要があります。1 つの配列用にページのリストを処理できる DMA はスキャッター ギャザー DMA と呼ばれ、1 つの物理アドレスのみを処理できる DMA はシンプル DMA と呼ばれます。シンプル DMA は、エリアとパフォーマンスの面ではスキャッター ギャザー DMA よりも安価ですが、`sds_alloc()` という特別のアロケーターを使用して各配列ごとに物理的に隣接するメモリを取得する必要があります。  
    

  [演習 1](lab-1-introduction-to-the-sdsoc-development-environment.md) では、`mult_add` テンプレートを使用してシンプル DMA を使用できるようにしています。次の演習では、プラグマを使用してスキャッター ギャザー DAM や AXIFIFO などのほかのデータ ムーバーが使用されるようにします。ソース コードで `sds_alloc()` を `malloc()` にを変更し、スキャッター ギャザー DMA が自動的に選択されることを確認します。  


  <details>
  <summary><strong>データ ムーバー選択の制御</strong></summary>  

  この演習では、lab3 のソース コードにデータ ムーバー プラグマを追加して、ハードウェアとソフトウェア間での配列の転送に使用するデータ ムーバーを指定します。その後プロジェクトをビルドして、生成されたレポート (`data_motion.md`) を確認して、これらのプラグマの効果を確認します。ビルドでハードウェアが合成されないように、ビットストリームおよびブート ファイルの生成はオフにしておきます。  

  データ ムーバー プラグマを追加して各配列に使用されるデータ ムーバーのタイプを指定するには、次の手順に従います。  
    

  1. [Project Explorer] タブで lab3/src の下の `mmultadd.h` をダブルクリックします。  
  2. `mmult` 関数宣言の上に次の行を挿入して各配列に別のデータ ムーバーを指定し、ファイルを保存します。  
     `#pragma SDS data data_mover(A:AXIDMA_SG, B:AXIDMA_SIMPLE, C:AXIFIFO)`  

  3. プロジェクトの最上位フォルダーを右クリックして、[Build Project] をクリックします。  

     >**:information_source: 重要:**ビルド プロセスが完了するまでに、約 5 ～ 10 分かかります。  
    

  4. ビルドが完了したら、[Project Explorer] の [Reports] タブで [Data Motion Report] をダブルクリックして開きます。  
     一番右の [Connection] 列に、行列乗算の各入力/出力配列に割り当てられたデータ ムーバーが示されます。  

     >**:pushpin: 注記:**  [Pragmas] 列には、使用されたプラグマがリストされます。`AXIFIFO` データ ムーバーは `M_AXI_GP0` ポートに割り当てられ、その他 2 つのデータ ムーバーは `S_AXI_ACP` に関連付けられています。  

     ![](./images/znz1517376007025.png)  

  5. 手順 2 で入力したプラグマ #pragma SDS data data_mover(A:AXIDMA_SG, B:AXIDMA_SIMPLE, C:AXIFIFO) を削除し、ファイルを保存します。

  </details>

  <details>
  <summary><strong>sds_alloc() を malloc() に変更</strong></summary>  

  この演習では、lab3 のソース ファイルで `sds_alloc()` を `malloc()` に変更し、データ ムーバーがシンプル DMA からスキャッター ギャザー DMA に変更されることを確認します。  

  1. [Project Explorer] タブで src フォルダーの下の `main.cpp` をダブルクリックし、ソース エディター ビューで開きます。  
  2. バッファーが `sds_alloc()` で割り当てられている行をすべて検索して、`sds_alloc()` を `malloc()` に置き換えます。また、すべての `sds_free()` 呼び出しも `free()` に置換します。  
  3. ファイルを保存します。  
  4. プロジェクトの最上位フォルダーを右クリックして、[Build Project] をクリックします。  

     >**:information_source: 重要:**ビルド プロセスが完了するまでに、約 5 ～ 10 分かかります。  

  5. ビルドが完了したら、[Project Explorer] タブで `Release/_sds/reports/data_motion.md` をダブルクリックして開きます。  
  6. 一番右の [Connection] 行に、行列乗算の入力配列に割り当てられた DMA のタイプ (`AXIDMA_SG` = スキャッター ギャザー DMA) と使用された Processing System 7 IP ポート (`S_AXI_ACP`) が示されます。[Accelerator Callsites] の表には、各転送に使用されるメモリ割り当てが連続なのか、ページなのかが示されます。  

     ![](./images/hsc1517376007065.png)  

  7. 手順 2 の変更をすべて取り消して、ファイルを保存します。

  </details>
  <details>
  <summary><strong>転送されるデータ量を制御するプラグマの追加</strong></summary>

  この演習では、別のテンプレートを使用してコピー プラグマを使用する方法を示します。このテンプレートでは、M と呼ばれる追加のパラメーターが行列乗算関数に渡されます。このパラメーターを使用すると、最大 32*32 までの任意サイズ M*M の正方行列 2 つを乗算する行列乗算関数を使用できます。この行列の最上位の割り当てにより、最大 32x32 までのサイズの行列が作成されます。SDSoC™ では、M パラメーターで行列乗算関数で乗算する行列のサイズが指定され、data copy プラグマで最大の行列サイズではなく、実際の行列サイズに相当するデータ量を転送することが指定されます。  

  1. SDx 環境を起動し、可変データ サイズを使用する行列乗算デザイン (matrix multiplication with variable data size) テンプレートを使用して、ZC702 と Linux プラットフォーム用の新しいプロジェクトを作成します。  
     1. [File] → [New] → [SDx Project] をクリックします。  
     2. New Project ウィザードにプロジェクトの名前 (たとえば `lab3a`) を入力します。  
     3. [zc702] および [Linux] を選択します。  
     4. [Next] をクリックします。  
     5. [Available Templates] から [Matrix Multiplication Data Size] を選択し、[Finish] をクリックします。  
     6. `mmult_accel` 関数はハードウェア アクセラレーション用にマークされていることに注意してください。  

  2. [Options] パネルで [Generate bitstream] と [Generate SD Card Image] をオフにして、ビットストリームとブート ファイルが生成されないようにプロジェクトを設定します。  

  3. data copy プラグマがコードに含まれています。data copy プラグマを確認するには、[Project Explorer] タブで mmult_accel.h (src フォルダーの下) をダブルクリックしてソース エディター ビューで開きます。  

     次のように、各配列に異なるデータ コピー サイズを指定します。プラグマでは、関数のスカラー引数のどれでも使用してデータ コピー サイズを指定できます。ここでは、サイズを指定するのに M を使用しています。  

     ```
      #pragma SDS data copy(A[0:M*M], B[0:M*M], C[0:M*M])  
      #pragma SDS data access_pattern(A:SEQUENTIAL, B:SEQUENTIAL, C:SEQUENTIAL)   
      void mmult_accel (float A[N*N],  
                        float B[N*N],  
                        float C[N*N],  
                        int M);  
     ```
  4. プロジェクトの最上位フォルダーを右クリックして、[Build Project] をクリックします。

  5. ビルドが完了したら、[Project Explorer] の [Reports] タブで [Data Motion Network Report] をダブルクリックして開きます。  
  6. 右から 2 番目の [Pragmas] 列に、各配列のデータ転送の長さが表示されます。2 つ目の表には、各ハードウェア関数呼び出しサイトの転送サイズが示されます。  

     ![](./images/pvd1517375658992.png)  

  </details>   
  </details>

### まとめ

このチュートリアルを終了すると、次ができるようになります。

  * プラグマを使用してデータ転送用の ACP または AFI ポートを選択。
  * SDSoC 環境のエラー検出とレポート機能を確認。  

その他の演習を実行すると、次も学ぶことができます。  

  * プラグマを使用してハードウェア関数の引数に別のデータ ムーバーを選択。  
  * `sds_alloc()` の使用について理解。  
  * プラグマを使用してハードウェア関数から転送またはハードウェア関数へ転送されるデータ エレメント数を制御。  
<hr/>
<p align="center"><sup>Copyright&copy; 2018 Xilinx</sup></p>
