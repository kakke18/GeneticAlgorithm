/************************ インクルード ***************************/
#include<stdio.h>
#include<stdlib.h>      // for rand(), srand()
#include<time.h>        // for time()
#include<conio.h>       // for getch()
#include<stdbool.h>     // for boolean


/************************ 定数宣言 ***************************/
#define IN         255   // 入力の種類(8ビット)
#define OUT          4   // 出力の種類(4通り)
#define POP        100   // 個体総数
#define CROSSOVER  0.7   // 交叉率
#define MUTATION  0.02   // 突然変異率


/******************* マップの情報の読み込み ******************/
#include"map3.c"         // int mapdata[WX][WY] などを初期化


/************************ 変数宣言 ***************************/
int map[WY][WX];                    // 作業エリア
int genotype[POP][IN];              // 遺伝子型 [No.][センサ入力値]
/* 行動出力の移動方向（OUT通り） */
int move_x[OUT] = { 0, 1, 0, -1 };  //x方向
int move_y[OUT] = { -1, 0, 1, 0 };  //y方向
int fitness[POP];                   // 個体の適応度


/***************** 関数のプロトタイプ宣言 ********************/
void disp_map();                    // マップ表示
void init_population();             // 個体群の初期化     
void disp_action(int n, int steps); // No.n の個体を steps 回だけ移動   
int  obtain_input(int x, int y);    // 周囲の状況を数値化 
void generation(int n);             // n 回世代交代
void calc_fitness(int steps);       // 各個体の適応度を算出 
void determine_next_generation();   // 次世代の個体群を算出 


/* メイン関数 */
int main() {
    int num, c; // 作業用変数

    /* BEGIN */
    printf("\n*** GA(遺伝的アルゴリズム)による即応プランの最適化デモ ***\n");

    /* 初期個体群の生成 */
    init_population();
    
    /* 初期個体の動きの例示 */
    printf("試しに初期個体No.0の動きを見てみましょう\n");
    disp_action(0, 100);                                        // No.0の個体を100step動かす
    printf("ランダムに作ったので全然ダメですね．．．\n");
    printf("===== それではルールをGAで進化させましょう =====\n");
    
    /* 最大世代数の入力 */
    printf("最大世代数(例：30000) = ");
    do{
        scanf("%d", &num);
    } while (num <= 0);

    /* num 回の世代交代で解を求める */
    generation(num);
    
    /* END */
    printf("\nどうですか？ GAの凄さが少しだけ分かったでしょ？ ");
    printf("おしまい．\n");
    printf("hit any key to finish:");
    c = getch();
    printf("\n");
    
    return 0;
}

/* 画面にキャラクタでマップを表示                                   */
/* 0:空白(　), 1:壁(■)，2:エージェント(●), 3:Goal(★) ，4:軌跡(　)   */
void disp_map() {
    for (int y = 0; y < WY; y++) {
        for (int x = 0; x < WX; x++) {
            switch( map[y][x]) {
                // ブランク
                case 0: 
                    printf("　");
                    break;  
                // 壁
                case 1:
                    printf("□");
                    break;  
                // エージェント
                case 2:
                    printf("●");
                    break;
                // ゴール  
                case 3:
                    printf("★");
                    break;  
                // 軌跡
                case 4:
                    printf("　");
                    break;  
            }
        }
        printf("\n");
    }
}

/* 個体群の遺伝子型をランダムに初期化 */
void init_population() {
    /* 乱数の初期化 */
    srand((unsigned)time(NULL));    // 時刻で初期化

    /* 遺伝子型の初期化 */
    for (int i = 0; i < POP; i++) {
        for (int j= 0; j < IN; j++) { 
            genotype[i][j] = rand() % 4;
        }
    }
}

/* No.n の個体を steps 回だけ動かす．                        */
/* 途中で壁にぶつかったり，１回居た場所に戻ったり，ゴールに */
/* 到達したら steps 回に到達していなくても表示を終了する．  */
void disp_action(int n, int steps) {
    int dx, dy;         // 移動先の座標
    int x, y, i;        // 作業用変数
    bool finish;        // 終了フラグ
    int input, output;  // 入力・出力を数値化したもの */
    int fit, kbd;

    /* 変数の初期化 */
    i = 1;
    finish = false;

    /* マップの初期化 */
    for (int y = 0; y < WY; y++) {
        for (int x = 0; x < WX; x++) {
            map[y][x] = mapdata[y][x];
        }
    }
    map[START_Y][START_X] = 2;    // スタート地点の設定    */
    map[GOAL_Y][GOAL_X]   = 3;    // ゴール地点の設定      */
    x = START_X;                  // 現在地のX座標の初期化 */
    y = START_Y;                  // 現在地のY座標の初期化 */

    /* 個体No.n を steps 回だけ動かす */
    do {
        /* 画面に step，入力信号，出力信号 を表示 */
        printf("step:%d  ", i);
        input = obtain_input(x, y);             // 入力パターンの調査
        printf("入力(周囲の状況):%d, ", input);
        output = genotype[n][input];            // 行動出力
        printf("出力(次の移動方向):");
        switch(output) {
            case 0:
                printf("↑\n");
                break;
            case 1:
                printf("→\n");
                break;
            case 2:
                printf("↓\n");
                break;
            case 3:
                printf("←\n");
                break;
        }

        /* マップを表示する */
        disp_map();
        
        /* 次の場所を決める */
        dx = x + move_x[output];    // 次の場所(仮)
        dy = y + move_y[output];    // 次の場所(仮)

        /* 次の場所が壁(=1)，通過軌跡(=4)以外なら動かす */
        if (dx > 0 && dx < WX - 1 && dy > 0 && dy < WY - 1 && ( map[dy][dx] != 1 && map[dy][dx] != 4)) {
            map[y][x] = 4;  // 今居る場所を通過軌跡(=4)にする
            x = dx;         // 現在地のX座標を更新する
            y = dy;         // 現在地をY座標を更新する
            map[y][x] = 2;  // 移動先の数値を直す

            /* ゴールに到達したら表示する */
            if (x == GOAL_X && y == GOAL_Y) {
                printf("GOAL!!! ｬｯﾀ─ヽ(*´ｖ｀*)ﾉ─ｧｧ!!\n");
            }
        }
        else {
            finish = true;  // 即終了
            //壁にぶつかった
            if (map[dy][dx] == 1) {
                printf("壁にぶつかりました！ ｡･ﾟ･(*ﾉД`*)･ﾟ･。\n");
            }
            //一歩前に戻った
            else {
                printf("１度通ったマスに戻ってしまいました！ ｡･ﾟ･(*ﾉД`*)･ﾟ･。\n");
            }
        }

        /* キーボード入力待ちにする */
        printf("hit any key (強制終了したいときはスペースキー):");
        kbd = getch();
		if (kbd == 32) {
            exit(1);
        }
        printf("\n\n");

        /* ステップを増やす */
        i++;
    } while ((x != GOAL_X || y != GOAL_Y) && i <= steps && !finish);

    /* 最終的な適応度(評価値，ゴールとの現在位置との差で計算)*/
    fit = 1 + abs(GOAL_X - START_X) + abs(GOAL_Y - START_Y ) - abs(GOAL_X - x) - abs(GOAL_Y - y);

    /* fit < 0 なら fit = 0 とする */
    if (fit < 0) {
        fit = 0;
    }

    printf("この個体の適応度 = %d\n", fit);
}

/* 座標(x,y)に居るときの周囲を 0～255 で表して返す */
/*  0  1  2      エージェント(A)の周囲8マスを左のように0～7bit と */
/*  7  A  3      みなし，壁があれば1，なければ0として [0, 255] で */
/*  6  5  4      数値化している．                                 */
int obtain_input(int x, int y) {
    int shift_x[8] = {-1, 0, 1, 1, 1, 0, -1, -1};  // Xのシフト量
    int shift_y[8] = {-1, -1, -1, 0, 1, 1, 1, 0};  // Yのシフト量
    int input_value;                                // 作業変数

    /* 変数の初期化 */
    input_value = 0;
 
    /* エージェントの周囲を8ビットで数値化している */
    for (int i = 0; i < 8; i++) {
        input_value = input_value * 2;
        if (map[y + shift_y[i]][x + shift_x[i]] == 1) {
            input_value++;
        }
    }
    return input_value;
}

/* n 世代だけ世代交代させる */
void generation(int n) {
    int max, maxnum, kbd;   // 作業変数 
    int num_gene;           //世代数
    int maximum;            // 理論上の最大適応度

    /* 変数初期化 */
    num_gene = 0;

    /* 理論上の最大適応度（今回の適応度定義式による）  */
    /* （現在位置がゴール位置に一致したとき最大になる）*/
    maximum = 1 + abs(GOAL_X - START_X) + abs(GOAL_Y - START_Y);

    /* 初期個体群の適応度を求める(引数は動かす最大step数) */
    calc_fitness(100);

    /* 初期個体群の最大適応度を表示 */
    max = fitness[0];
    maxnum = 0;
    for (int i = 1; i < POP; i++) {    // 全個体をスキャンして値を更新
        if (fitness[i] > max) {
            max = fitness[i];
            maxnum = i;
        }
    }
    printf("世代 No.0:最大適応度 = %d\n", max);

    /* 世代交代 */
    do {
        /* 世代数を増やす */
        num_gene++;

        /* 次世代の POP 個の個体を求める */
        determine_next_generation();
        
        /* 各個体の適応度を求める(引数は動かす最大step数) */
        calc_fitness(100);

        /* 最大適応度を求める */
        max = fitness[0];
        maxnum = 0;
        for (int i = 1; i < POP; i++) {    // 全個体をスキャンして値を更新
            if (fitness[i] > max) {
                max = fitness[i];
                maxnum = i;
            }
        }
        if (num_gene % 100 == 0) {    // 100世代ごとに画面表示 */
            printf("世代 No.%d:最大適応度 = %d\n", num_gene, max);
        }
    } while (max < maximum && num_gene < n);

    if (max == maximum) {
        printf("世代 No.%d:最大適応度 = %d\n", num_gene, max);
        printf("ゴールに到達するルールが得られたので世代交代を終了しました．\n");
    }
    printf("得られた解を示します．hit any key:");

    /* キーボード入力待ちにする */
    kbd = getch();
    printf("\n\n");
    printf("最優秀個体 No.%d の適応度 = %d\n", maxnum, max);

    /* 最優秀個体の動作を表示 */
    disp_action(maxnum, 100);

    /* 得られたルールの画面への表示 */
    printf("\n得られた解(ルール(遺伝子型))は次の通りです\n");
    printf("（入力0から入力%dまでのそれぞれの行動出力です）\n", IN);
    for (int i = 0; i < IN; i++) {
        printf("%d", genotype[maxnum][i]);
        /* 改行 */
        if (i % ((IN + 1) / 8) == 0) {
            printf("\n");
        }
    }
    printf("\n");
}

/* 各個体を steps だけ動かしたときの適応度 fitness[0～POP-1] を求める */
void calc_fitness(int steps) {
    int x, y, i, fit, kbd;  // 作業変数
    bool finish;            // 終了フラグ
    int dx, dy;             // 移動先の座標 
    int input, output;      // 入力・出力を数値化したもの

    for (int n = 0; n < POP; n++) {
        /* 変数初期化 */
        i = 1;
        finish = false;

        /* マップの初期化 */
        for (y = 0; y < WY; y++) {
            for (x = 0; x < WX; x++) {
                map[y][x] = mapdata[y][x];
            }
        }
        map[START_Y][START_X] = 2;  // スタート地点の設定
        map[GOAL_Y][GOAL_X] = 3;    // ゴール地点の設定
        x = START_X;                // 現在地のX座標の初期化
        y = START_Y;                // 現在地のY座標の初期化

        /* 個体No.n を steps 回だけ動かす */
        do {
            input = obtain_input(x, y);     // 入力パターンの調査
            output = genotype[n][input];    // 行動出力
            
            /* 次の場所を決める */
            dx = x + move_x[output]; 
            dy = y + move_y[output];

            /* 次の場所が壁(=1)，通過軌跡(=4)以外なら動かす */
            if (dx > 0 && dx < WX - 1 && dy > 0 && dy < WY - 1 && ( map[dy][dx] != 1 && map[dy][dx] != 4)) {
                map[y][x] = 4;  /* 今居る場所を通過軌跡(=4)にする */
                x = dx;         /* 現在地のX座標を更新する */
                y = dy;         /* 現在地をY座標を更新する */
                map[y][x] = 2;  /* 移動先の数値を直す      */
            }
            else {
                finish = true;
            }

            /* ステップを増やす */
            i++;
        } while ((x != GOAL_X || y != GOAL_Y) && i <= steps && !finish);

        /* 最終的な適応度（評価値，ゴールとの現在位置との差で計算）*/
        fitness[n] = 1 + abs(GOAL_X - START_X) + abs(GOAL_Y - START_Y ) - abs(GOAL_X - x) - abs(GOAL_Y - y);

        /* fitnness < 0 なら fitness = 0 とする */
        if (fitness[n] < 0) {
            fitness[n] = 0;
        }
    }
}

/* シンプルＧＡ方式（ルーレット選択）＋エリート保存で次世代の個体を求める    */
/* ここでは普通にルーレット選択してから，0番の個体にエリートを戻すこととする */
void determine_next_generation() {
    int elitest[IN];                // エリート
    int max, maxnum, sum, random;   // 作業用変数
    int new_genotype[POP][IN];      // 新しい個体
    int result_roulette;            // ルーレットの結果
    int tmp[IN];                   // 一点交差用の作業変数

    /* 変数初期化 */
    max = fitness[0];
    maxnum = 0;
    sum = 0;

    /*---------- エリート選択 ----------*/
    /* fitness の最大値を探索 */
    for (int i = 1; i < POP; i++) {
        if (fitness[i] > max ) {
            max = fitness[i];
            maxnum = i;
        }
    }
    /* eletest に代入 */
    for (int i = 0; i < IN; i++) {
        elitest[i] = genotype[maxnum][i];
    }
    /*---------------------------------*/

    /*---------- ルーレット選択 ----------*/
    /* 全ての個体値の適応度の和 sum を算出 */
    for (int i = 0; i < POP; i++) {
        sum += fitness[i];
    }
    /* 新しい個体を生成 */
    for (int i = 0; i < POP; i++) {
        /* [0,sum-1]の乱数を算出 */
        random = rand() % sum;
        /* ルーレットの位置を算出 */
        for (int j = 0; j < POP; j++) {
            random -= fitness[j];
            if (random < 0) {
                result_roulette = j;
                break;
            }
        }
        /* 算出したルーレットの位置 result_roulette 番目の個体 genotype[][] を 新しい個体 new_genotype[][] に保存 */
        for (int j = 0; j < IN; j++){
            new_genotype[i][j] = genotype[result_roulette][j];
        }
    }
    /*-----------------------------------*/

    /*---------- 一点交差 ----------*/
    for (int i = 0; i < POP; i += 2) {
        /* 交差率 */
        if ((double) rand() / RAND_MAX < CROSSOVER) {
            /* 交差点の位置を算出 */
            random = rand() % IN;

            /* 遺伝子型を分割 */
            for(int j = random; j < IN; j++) {
                tmp[j] = new_genotype[i][j];                    // 親1の後半
                new_genotype[i][j] = new_genotype[i + 1][j];    // 親1の後半を親2の後半で上書き
                new_genotype[i + 1][j] = tmp[j];                // 親2の後半を親1の後半で上書き
            }
        }
    }
    /*-----------------------------*/

    /*---------- 突然変異 ----------*/
    for (int i = 0; i < POP; i++) {
        for (int j = 0; j < IN; j++) {
            /* 突然変異率 */
            if ((double) rand() / RAND_MAX < MUTATION) {
                new_genotype[i][j] = rand() % 4;
            }
        }
    }
    /*-----------------------------*/

    /*---------- エリート保存 ----------*/
    for (int i = 0; i < IN; i++) {
        new_genotype[0][i] = elitest[i];
    }
    /*---------------------------------*/

    /* new_genotype を genotype にコピー */
    for (int i = 0; i < POP; i++) {
        for (int j = 0; j < IN; j++) {
            genotype[i][j] = new_genotype[i][j];
        }
    }
}
