
<table style="width:100%">
    <tr>
        <th width="100%" colspan="6"><img src="https://www.xilinx.com/content/dam/xilinx/imgs/press/media-kits/corporate/xilinx-logo.png" width="30%"/><h1>SDSoC 環境チュートリアル: OpenCV の xfOpenCV への移行</h2>
        </th>
    </tr>
    <tr>
    <td align="center"><a href="README.md">概要</a></td>
    <td align="center"><a>演習 1: OpenCV の xfOpenCV への移行</a></td>
    <td align="center"><a href="lab2-build-sdsoc-acceleration-project.md">演習 2: SDSoC アクセラレーション プロジェクトのビルド</a></td>
    </tr>
</table>



## 演習 1- OpenCV の xfOpenCV への移行

この演習では、CPU 用に記述された OpenCV プログラムを xfOpenCV ライブラリに移行して、reVISION プラットフォームでハードウェア アクセラレーション関数を使用する方法について説明します。 

>**:pushpin: 注記:**
>すべての OpenCV 関数が xfOpenCV ライブラリに複製されるわけではなく、関数の中には特定の手順を簡素にするために記述されたものもあります。

このチュートリアルのソース ファイルとテスト画像は [source.zip](./source.zip) ファイルに含まれます。このファイルを選択したディレクトリにダウンロードして抽出します。含まれるファイルおよびフォルダーは次のとおりです。

* `colordetect.cpp` - このチュートリアルで編集する C++ ソース ファイル。 
* `rock_landscape.jpg` - アプリケーションをテストするのに使用するテスト画像。 
* `solution` フォルダー - 完全に変換されたソース ファイル (参照用)。

ソース コードは、次に示すテスト画像のように、1920x1080 の入力画像で青、緑、オレンジを検出するかなり単純な色検出器です。含まれているコードおよび画像を使って、OpenCV 関数およびアプリケーション フローを ZCU102 reVISION プラットフォームに基づいた xfOpenCV 関数を使用して移行する方法を示します。このチュートリアルでは、次について学ぶことができます。

* OpenCV 関数およびフローを xfOpenCV 関数およびフローに移行します。
* ハードウェア アクセラレーションする関数を特定します。
* xfOpenCV コードをコンパイルするためのビルド環境を設定します。
* コードを ZCU102 reVISION プラットフォームで使用できるように変更します。

入力画像:
![Rock Landscape](./images/rock_landscape.jpg)

### 手順 1: ソース ファイルの作成と編集

コード エディターでソース コードを開いたら、ZCU102 ボードで実行できるように変換します。

1. ソース ファイルをダウンロードして抽出し、選択したコード エディターで `colordetect.cpp` ソース ファイルを開きます。 

    上記に示すように `colordetect.cpp` ファイルには `colordetect()` 関数が含まれ、アプリケーション用に `main()` 関数が含まれます。このチュートリアルでは、colordetect 関数のハードウェア アクセラレーションされたバージョンである `colordetect_accel()` を別のファイルおよびヘッダー ファイルに作成します。 

2. `colordetect.cpp` ファイルの一番上に xfOpenCV インクルード文を追加して reVISION プラットフォームがサポートされるようにします。`#include <iostream>` の下に次の 3 行があります。 

    ```c++
    #include <opencv2/opencv.hpp>
    #include <opencv2/highgui.hpp>
    #include <opencv2/imgproc.hpp>
    ```

    これを次のコードに置き換えます。

    ```c++
    #if __SDSCC__
    #undef __ARM_NEON__
    #undef __ARM_NEON
    #include "opencv2/highgui/highgui.hpp"
    #include "opencv2/imgproc/imgproc.hpp"
    #include <opencv2/core/core.hpp>
    #define __ARM_NEON__
    #define __ARM_NEON
    #else
    #include "opencv2/highgui/highgui.hpp"
    #include "opencv2/imgproc/imgproc.hpp"
    #include <opencv2/core/core.hpp>
    #endif
    ```

    `ARM_NEON` に関する最初の部分では、ZCU102 の Arm プロセッサ用に OpenCV インクルード/ライブラリのコンパイルが処理されています。`ARM_NEON` セクションの後では、ツールで使用する xfOpenCV インクルードを定義しています。これらのファイルを含めると、コンパイラがすべての必要な関数およびデータ型にアクセスできるようになります。 

    >**:pushpin: 注記:**
    >`__SDSCC__` は sds++ コンパイラで自動的に設定されます。 

3. `colordetect.cpp` を編集するのに使用したのと同じコード エディターで 2 つのファイル (`colordetect_accel.cpp` および `colordetect_accel.hpp`) を新しく作成します。 
   
4. `colordetect_accel.cpp` ファイルで新しいハードウェア アクセラレーションされた関数の関数シグネチャを定義します。ファイルの一番上に次を入力します。 
   
    ```c++
    void colordetect_accel() {}
    ```

5. `colordetect_accel.hpp` ヘッダー ファイルに次のコードを追加します。

    ```c++
    #include "hls_stream.h"
    #include "ap_int.h"
    #include "common/xf_common.h"
    #include "common/xf_utility.h"
    #include "imgproc/xf_inrange.hpp"
    #include "imgproc/xf_rgb2hsv.hpp"
    #include "imgproc/xf_erosion.hpp"
    #include "imgproc/xf_dilation.hpp"
    ```

6. `colordetect_accel.hpp` ファイルの前の `#include` 文の後に次のマクロを追加すると、コードがより読みやすくなり、xfOpenCV 関数呼び出しおよびオブジェクト インスタンシエーションの一部をテンプレートにできます。

    ```c++
    #define MAXCOLORS 3
    #define WIDTH 1920
    #define HEIGHT 1080
    ```

7. すべてのファイルを保存したら、後で編集できるように開いたままにしておきます。

### 手順 2: cv:Mat オブジェクトの xf:Mat への変換

1. OpenCV コードをハードウェア アクセラレーションされたプラットフォームに移行する際は、変換コードで使用されるオブジェクトをすべて特定する必要があります。`main()` および `colordetect()` 関数で `cv::Mat` オブジェクト (`in_img`、`out_img`、`mask1`、`mask2`、`mask3`、`_imgrange`、`_imghsv`) を見つけます。

    CPU で実行する場合、これらのオブジェクトにより、必要に応じてメモリをダイナミックに割り当てたり、割り当てを解除したりできるようになります。ただし、ハードウェア アクセラレーションをターゲットにする場合は、コンパイル時にメモリ要件を指定する必要があります。このため、xfOpenCV ライブラリには FPGA デバイスでメモリ割り当てを実行する `xf::Mat` オブジェクトが含まれます。ハードウェア アクセラレーションされた関数で使用される `cv::Mat` オブジェクトは、ユーザーが定義したか、xfOpenCV ライブラリからのものかに関係なく、同等オブジェクトの `xf::Mat` に置換する必要があります。  
    
    `main()` 関数では、この関数のコードが CPU で実行されるので、Linux システムへの入力/出力に `cv::Mat` を使用し続けることができます。ただし、アクセラレーション関数の場合は、`cv::Mat` オブジェクトからのデータを `xf::Mat` オブジェクトに変換する必要があります。

2. `main()` 関数で、次の if 文の後にアクセラレーション関数用に入力および出力 `xf::Mat` オブジェクトを作成します。
   
    ```C++
    if (!in_img.data) {
	    return -1;
	}
    ```

    次のコードを追加します。

    ```c++
    xf::Mat<XF_8UC4, HEIGHT, WIDTH, XF_NPPC1> xfIn(in_img.rows, in_img.cols);
    xf::Mat<XF_8UC1, HEIGHT, WIDTH, XF_NPPC1> xfOut(in_img.rows, in_img.cols);
    ```

   これで、ハードウェア アクセラレーションされた関数の入力および出力用に `xf::Mat` オブジェクトのテンプレートが作成されます。オブジェクトのテンプレート化に関する一般的な情報は、『ザイリンクス OpenCV ユーザー ガイド』 ([UG1233](https://japan.xilinx.com/support/documentation/sw_manuals_j/xilinx2018_2/ug1233-xilinx-opencv-user-guide.pdf)) を参照してください。  

   作成した `xfIn` および `xfOut` オブジェクトには、次の属性があります。
   
   `XF_8UC4` - 8 ビットの unsigned-char 4 チャネル データ型を定義します。

   `XF_8UC1` - 8 ビットの unsigned-char 1 チャネル データ型を定義します。

   入力 `xf:Mat` は、`colorthresholding()` 関数が画像の 3 色コンポーネント (チャネル) で動作できるように、4 チャネル オブジェクトとして宣言されています。出力オブジェクトには 3 つのチャネルは必要ありません。

   `HEIGHT`/`WIDTH` - 入力画像のサイズを定義します。`colordetect_accel.hpp` ヘッダー ファイルで画像の HEIGHT および WIDTH マクロを **1920x1080** と定義しました。最大画像サイズをスタティックに定義して、関数パラメーターを使用して可変画像サイズを処理します。 

   `XF_NPPC1` - このテンプレート パラメーターは、各クロック サイクルで処理されるピクセル数が 1 であることをコンパイラに伝えます。

<!-- 
   OpenCV では、オブジェクトのダイナミックな側面により、読み出し/書き込みが必要に応じて処理されるので、このタイプのフローの場合、複数の `cv::Mat` オブジェクトを作成する必要はありません。アクセラレーションでは、ほとんどの C/C++ プログラムで使用される手続き型の方法では実行されないため、データがどのように動くかということと、データ サイズを知っておく必要があります。アクセラレーションを使用すると、並列で処理されるようになるので、読み出し/書き込みが同時に発生する可能性があります。-->

### 手順 3: アクセラレーション関数の作成

`main()` 関数を使用して、アクセラレーション関数の入力および出力に必要な `xf::Mat` オブジェクトを作成したら、アクセラレーション関数を記述できます。  ハードウェア アクセラレーションされた `colordetect_accel()` 関数は、ハードウェアでアクセラレーションされる xfOpenCV 関数を使用して、`colordetect()` 関数を複製します。 

このアクセラレーション関数を使用する場合、すべてを値渡しではなく、参照渡しする必要があります。参照渡しをすると、これらのパラメーターが関数へのストリーミング入力または出力のいずれかとして動作するようになりますが、両方同時に動作することはありません。 

`main()` 関数は `colordetect` 関数とアクセラレーションされた `colordetect_accel` 関数の両方を呼び出すので、Arm プロセッシングと FPGA アクセラレーション間のパフォーマンスを比較できます。

1. `colordetect_accel.cpp` ファイルで関数シグネチャを編集して、先ほど作成した `xf::Mat` オブジェクト特有のパラメーターを追加します。 

    ```c++
    void colordetect_accel( xf::Mat<XF_8UC4, HEIGHT, WIDTH, XF_NPPC1> &_src, xf::Mat<XF_8UC1, HEIGHT, WIDTH, XF_NPPC1> &_dst) {}
    ```

    その他のデータ型と同様、このテンプレートの値は関数に渡す値と同じにする必要があります。パラメーターを間違って入力すると、コンパイラでエラーになります。

2. `colordetect_accel()` 関数は HSV 色空間を使用するので、それに該当するしきい値を渡す必要もあります。次のように、関数シグネチャに `nLowThreshold` および `nHighThreshold` を追加します。

    ```c++
    void colordetect_accel( xf::Mat<XF_8UC4, HEIGHT, WIDTH, XF_NPPC1> &_src,
    xf::Mat<XF_8UC1, HEIGHT, WIDTH, XF_NPPC1> &_dst,
    unsigned char nLowThresh[3][3],
    unsigned char nHighThresh[3][3]) {}
    ```

    アクセラレーションをプログラマブル ロジックと一緒に使用する場合、コードで実際の回路が固定サイズで定義されるので、コンパイル時に配列サイズを指定する必要があります。たとえば、しきい値データが各次元で 3 要素の 2D 配列であるとわかっていれば、そのように定義します。

3. `xf::Mat` オブジェクトはダイナミックではないので、`xf::Mat` オブジェクトを宣言して、関数を介してデータ ストリームを渡す必要があります。関数の本体に、次のオブジェクトを追加します。

    ```C++
    xf::Mat<XF_8UC4, HEIGHT, WIDTH, XF_NPPC1> _hsv;
    xf::Mat<XF_8UC1, HEIGHT, WIDTH, XF_NPPC1> _range;
    xf::Mat<XF_8UC1, HEIGHT, WIDTH, XF_NPPC1> _erode;
    xf::Mat<XF_8UC1, HEIGHT, WIDTH, XF_NPPC1> _dilate1;
    xf::Mat<XF_8UC1, HEIGHT, WIDTH, XF_NPPC1> _dilate2;
    ```

4. これで、`colordetect_accel` 関数の動作を定義できるようになりました。元の `colordetect()` 関数の最初の動作は、入力画像の RGB 色空間から HSV への色変換 (`cv::cvtColor`) です。『ザイリンクス OpenCV ユーザー ガイド』 ([UG1233](https://japan.xilinx.com/support/documentation/sw_manuals_j/xilinx2018_2/ug1233-xilinx-opencv-user-guide.pdf)) には `cvtColor` 関数について記述されていませんが、「色検出」セクションには xfOpenCV ライブラリからの 4 つのハードウェア関数 (`xf::RGB2HSV` から記述) を使用した色検出アルゴリズムについての説明があります。`colordetect_accel` 関数にはその関数を使用して、色変換を実行します。 

    もう一方の `xf::Mat` オブジェクト定義の後、`xf::RGB2HSV` 関数にパラメーターを付けて `colordetect_accel` に追加します。

    ```c++
        xf::RGB2HSV<XF_8UC4, HEIGHT, WIDTH, XF_NPPC1>(_src, _hsv);
    ```

    関数およびオブジェクトは、特定の情報を含めてテンプレートにする必要があります。これで、正しいデータ型/サイズが使用されるようになります。この関数テンプレートは `xf::Mat` テンプレートと同じにしておかないと、実行時にアサート条件になります。

5. 次に `colordetect()` 関数コードはしきい値処理を実行し、出力されるマスクを一緒にまとめます。先ほどと同様、xfOpenCV には `cv::inRange` 関数がありませんが、この目的のみに作成された `xf::colorthresholding` があります。『ザイリンクス OpenCV ユーザー ガイド』 ([UG1233](https://japan.xilinx.com/support/documentation/sw_manuals_j/xilinx2018_2/ug1233-xilinx-opencv-user-guide.pdf)) の「色しきい値処理」セクションに記述されるように、次の点に注意してください。

    - テンプレート パラメーター MAXCOLORS
    - `low_thresh` および `high_thresh` は 1 次元配列
    - ソース テンプレートは `XF_8UC4` にする必要あり

    この関数の場合、3 チャネル データを使用するので、`xf::Mat` 入力オブジェクトは `XF_8UC4` である必要があります。これは、`MAXCOLORS` でしきい値処理している色の数 (このコードの場合 3) が決定されるからです。最後に `low_thresh` および `high_thresh` は 1D 配列にする必要があります。これにより、使用している `MAXCOLORS` の大きさに基づいて、配列を適切な low および high 値で調整できます。

    2D 配列を 1 次元に変換するには、インストール時に実行する方法とループを使用する方法の 2 つの方法があります。ここでは、簡単に説明するため、ループを使用する方法を説明します。先ほど追加した `xf::RGB2HSV` 関数の前に次のコードを追加します。

    ```c++
    unsigned char low_thresh[9];
    unsigned char high_thresh[9];

    for(int i = 0; i < 3; ++i)
    for(int j = 0; j < 3; ++j) {
        low_thresh[i*3+j] = nLowThresh[i][j];
        high_thresh[i*3+j] = nHighThresh[i][j];		
    }
    ```

    `colordetect_accel` 関数は、現時点で次のようになっているはずです。

    ```c++
    void colordetect_accel( xf::Mat<XF_8UC4, HEIGHT, WIDTH, XF_NPPC1> &_src,
    xf::Mat<XF_8UC1, HEIGHT, WIDTH, XF_NPPC1> &_dst,
    unsigned char nLowThresh[3][3],
    unsigned char nHighThresh[3][3]) {

        //Define Matrix objects for operations
        xf::Mat<XF_8UC4, HEIGHT, WIDTH, XF_NPPC1> _hsv;
        xf::Mat<XF_8UC1, HEIGHT, WIDTH, XF_NPPC1> _range;
        xf::Mat<XF_8UC1, HEIGHT, WIDTH, XF_NPPC1> _erode;
        xf::Mat<XF_8UC1, HEIGHT, WIDTH, XF_NPPC1> _dilate1;
        xf::Mat<XF_8UC1, HEIGHT, WIDTH, XF_NPPC1> _dilate2;

        //Convert 2d Array to 1d
        unsigned char low_thresh[9];
        unsigned char high_thresh[9];

        for(int i = 0; i < 3; ++i)
        for(int j = 0; j < 3; ++j) {
            low_thresh[i*3+j] = nLowThresh[i][j];
            high_thresh[i*3+j] = nHighThresh[i][j];		
        }

        //Begin color conversion...
        xf::RGB2HSV<XF_8UC4, HEIGHT, WIDTH, XF_NPPC1>(_src, _hsv);

    }
    ```

    これで、しきい値のための 1 次元配列ができたので、`xf::colorthresholding` を追加する必要があります。`xf::RGB2HSV` 関数の後に次のコードを追加します。

    ```c++
    xf::colorthresholding<XF_8UC4, XF_8UC1, MAXCOLORS, HEIGHT, WIDTH, XF_NPPC1>(_hsv, _range, low_thresh, high_thresh);
    ```

    この関数は、`XF_8UC4` データ型の入力を `XF_8UC1` (グレースケール) の出力に変換し、`MAXCOLORS` 3 をしきい値処理します。元の `colordetect()` コードと比較すると、基本的には同じ `cv::inRange` およびマスクのビット単位 `OR` 演算の組み合わせを実行していることになります。より多くの、または少ない色が検出されるような、これより複雑なコードの場合、これは計測可能です。単に、必要に応じて `MAXCOLORS` と `low_thresh` および `high_thresh` 配列を調整してください。

6.  `cv::erode` および `cv::dilate` 関数を使用する場合は、どのタイプのモルフォロジカル タイプを使用するのか、どのピクセル エリアを考慮するのかを指定する必要があります。この関数では、`xf::erode` および `xf::dilate` が `MORPH_RECT` および 3x3 ピクセルを使用しています。 

    OpenCV バージョンと同じようにこれらの関数を追加するには、`colorthresholding` 関数の後に次の行を挿入します。

    ```c++
    xf::erode<XF_BORDER_CONSTANT, XF_8UC1, HEIGHT, WIDTH, XF_NPPC1>(_range, _erode);
    xf::dilate<XF_BORDER_CONSTANT, XF_8UC1, HEIGHT, WIDTH, XF_NPPC1>(_erode, _dilate1);
    xf::dilate<XF_BORDER_CONSTANT, XF_8UC1, HEIGHT, WIDTH, XF_NPPC1>(_dilate1, _dilate2);
    xf::erode<XF_BORDER_CONSTANT, XF_8UC1, HEIGHT, WIDTH, XF_NPPC1>(_dilate2, _dst);
    ```

    新しいテンプレート パラメーター `XF_BORDER_CONSTANT` が使用されています。`cv::erode` および `cv::dilate` では、このパラメーターで境界サイズ (デフォルト値は `BORDER_CONSTANT`) が定義されます。xfOpenCV の場合、コンパイル時間から画像の境界が調整されるかどうかを判断する必要があります。 
    
    最後に、この関数のテンプレート値すべてがデータ入力および出力と同じになるようにします。違っていると、コンパイル エラーまたはランタイム エラーのいずれかになります。

    関数は、次のようになっているはずです。

    ```c++
    void colordetect_accel( xf::Mat<XF_8UC4, HEIGHT, WIDTH, XF_NPPC1> &_src,
    xf::Mat<XF_8UC1, HEIGHT, WIDTH, XF_NPPC1> &_dst,
    unsigned char nLowThresh[3][3],
    unsigned char nHighThresh[3][3]) {

        //Define Matrix objects for operations
        xf::Mat<XF_8UC4, HEIGHT, WIDTH, XF_NPPC1> _hsv;
        xf::Mat<XF_8UC1, HEIGHT, WIDTH, XF_NPPC1> _range;
        xf::Mat<XF_8UC1, HEIGHT, WIDTH, XF_NPPC1> _erode;
        xf::Mat<XF_8UC1, HEIGHT, WIDTH, XF_NPPC1> _dilate1;
        xf::Mat<XF_8UC1, HEIGHT, WIDTH, XF_NPPC1> _dilate2;

        //Convert 2d Array to 1d
        unsigned char low_thresh[9];
        unsigned char high_thresh[9];

        for(int i = 0; i < 3; ++i)
        for(int j = 0; j < 3; ++j) {
            low_thresh[i*3+j] = nLowThresh[i][j];
            high_thresh[i*3+j] = nHighThresh[i][j];		
        }

        //Begin color conversion...
        xf::RGB2HSV<XF_8UC4, HEIGHT, WIDTH, XF_NPPC1>(_src, _hsv);
        xf::colorthresholding<XF_8UC4, XF_8UC1, MAXCOLORS, HEIGHT, WIDTH, XF_NPPC1>(_hsv, _range, low_thresh, high_thresh);
        xf::erode<XF_BORDER_CONSTANT, XF_8UC1, HEIGHT, WIDTH, XF_NPPC1>(_range, _erode);
        xf::dilate<XF_BORDER_CONSTANT, XF_8UC1, HEIGHT, WIDTH, XF_NPPC1>(_erode, _dilate1);
        xf::dilate<XF_BORDER_CONSTANT, XF_8UC1, HEIGHT, WIDTH, XF_NPPC1>(_dilate1, _dilate2);
        xf::erode<XF_BORDER_CONSTANT, XF_8UC1, HEIGHT, WIDTH, XF_NPPC1>(_dilate2, _dst);
    }
    ```

7. アクセラレータが完了したら、`#define` の後に次の行を追加して、`colordetect_accel.hpp` に関数定義を追加します。

    ```c++
    void colordetect_accel( xf::Mat<XF_8UC4, HEIGHT, WIDTH, XF_NPPC1> &_src, xf::Mat<XF_8UC1, HEIGHT, WIDTH, XF_NPPC1> &_dst, unsigned char nLowThresh[3][3], unsigned char nHighThresh[3][3]);
    ```

8. `colordetect_accel.cpp` コード ファイルに `colordetect_accel.hpp` ヘッダー ファイルを含めます。ファイルの一番上に次の行を挿入します。 
   
    ```c++
    #include "colordetect_accel.hpp"
    ```

これで `colordetect_accel` 関数とヘッダー ファイルの定義は終了です。 

### 手順 4: Main() 関数のアップデート

次は、`colordetect_accel` を `main` 関数に追加します。 

1. `colordetect_accel.hpp` を `colordetect.cpp` に含める必要があります。さまざまな OpenCV インクルード文の後、ファイルの一番上に次を追加します。

    ```c++
    #include "colordetect_accel.hpp"
    ```

    `colordetect_accel` 関数を呼び出す前に、入力画像 `in_img` を `xfIn` 入力オブジェクトにコピーする必要があります。`cv::Mat` オブジェクトから `xf::Mat` オブジェクトにデータをコピーする `copyTo` と呼ばれる `xf::Mat` メンバー関数を使用します。これは `xfIn` のインスタンシエーションの後に使用します。 

2.  `xfIn` オブジェクトの作成後、`main()` に次の行を追加します。

    ```c++
    xfIn.copyTo(in_img.data);
    ```

3. これで `colordetect()` 関数の下にハードウェア アクセラレーションされた関数をインスタンシエートできるようになりました。

    ```c++
    colordetect_accel(xfIn,xfOut,nLowThresh,nHighThresh);
    ```

4. Arm およびプログラマブル ロジックの出力を比較するには、`copyFrom` を使用して `xfOut` からデータを別の `cv::Mat` オブジェクトにコピーします。`colordetect_accel` 関数の直後に次の行を追加します。

    ```c++
    cv::Mat accel_out(height, width, CV_8U);
    accel_out.data = xfOut.copyFrom();
    ```

5. `cv::imwrite` 関数を使用して出力画像を保存します。もう 1 つの `cv::imwrite` 関数直後に次を追加します。

    ```c++
    cv::imwrite("accel_out.png",accel_out);
    ```

    最後の変更で、`main()` 関数は次のようになるはずです。

    ```c++
    int main(int argc, char **argv)
    {
        //Create the input/output cv::Mat objects
        cv::Mat in_img, out_img;
        cv::Mat imghsv, imgrange, imgerode, imgdilate1, imgdilate2;

        // Define the low and high thresholds
        // Want to grab 3 colors (Blue, Green, Orange) for teh input image
        unsigned char nLowThresh[3][3] = { { 110, 150, 20 }, // Lower boundary for Blue
                                        { 38, 0, 20 }, // Lower boundary for Green
                                        { 10, 150, 20 } }; // Lower boundary for Orange
        unsigned char nHighThresh[3][3] = { { 130, 255, 255 }, // Upper boundary for Blue
                                            { 75, 125, 255 }, // Upper boundary for Green
                                            { 25, 255, 255 } }; // Upper boundary for Orange

                            // Read in the commandline for an image
        in_img = cv::imread(argv[1], 1);
        if (!in_img.data) {
            return -1;
        }

        // Create input output images for the hardware function
        xf::Mat<XF_8UC4, HEIGHT, WIDTH, XF_NPPC1> xfIn(in_img.rows, in_img.cols);
        xfIn.copyTo(in_img.data);
        xf::Mat<XF_8UC1, HEIGHT, WIDTH, XF_NPPC1> xfOut(in_img.rows, in_img.cols);

        // Create the output image to match the input image (CV_8U)
        int height = in_img.rows;
        int width = in_img.cols;
        out_img.create(height, width, CV_8U);

        // Run the input and thresholds into the colordect function
        colordetect(in_img, out_img, nLowThresh, nHighThresh);

        // Call the hardware accelerated function
        colordetect_accel(xfIn,xfOut,nLowThresh,nHighThresh);

        //Copy xfOut image to cv::Mat object
        cv::Mat accel_out(height, width, CV_8U);
        accel_out.data = xfOut.copyFrom();

        // Write out the input image and the output image
        cv::imwrite("output.png", out_img);
        cv::imwrite("input.png", in_img);
        cv::imwrite("accel_out.png",accel_out);

        return 0;
    }
    ```

6. ファイルを保存して閉じます。

### まとめ

使用される OpenCV オブジェクトを見て、xfOpenCV のテンプレート化方法を理解すると、どのオブジェクトを `cv::Mat` から `xf::Mat` に移行し、`xf::erode` などの一般的な関数と `xf::RGB2HSV` および `xf::colorthresholding` などの複雑な関数をどのように使用して元の OpenCV コードのデザインおよびフローを作成し直すのかが簡単にわかります。

元の `colordetect()` と新しい `colordetect_accel()` を比較すると、フローは同じでも使用される関数が違うことがわかります。

この演習では、OpenCV 関数の `colordetect()` を xfOpenCV 関数を使用してハードウェア アクセラレーションした関数 `colordetect_accel()` に変換しました。実行した内容は、次のとおりです。

- 最適化された xfOpenCV を使用するために OpenCV コードを見つけて変更しました。
- ダイナミックな cv::Mat オブジェクトをテンプレート化した xf:Mat オブジェクトに変換しました。
- テンプレート化した xfOpenCV 関数を呼び出します。 

<hr/>

:arrow_forward:**次のトピック:**  [演習 2 - SDSoC アクセラレーション プロジェクトのビルド](lab2-build-sdsoc-acceleration-project.md)

:arrow_backward:**前のトピック:**  [概要](README.md)

---
<p align="center"><sup>Copyright&copy; 2018 Xilinx</sup></p>

この資料は表記のバージョンの英語版を翻訳したもので、内容に相違が生じる場合には原文を優先します。資料によっては英語版の更新に対応していないものがあります。日本語版は参考用としてご使用の上、最新情報につきましては、必ず最新英語版をご参照ください。