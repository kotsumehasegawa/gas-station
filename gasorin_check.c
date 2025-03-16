/* ガソリンスタンド給油ゲーム */
/* 【成功条件】与えられた残りの時間・燃料を確認し、失敗条件を避けながら店を巡り給油して帰宅する
　 【失敗条件】「残り時間が00:00になる」「燃料が0になる」
　 【ガソリン代】車種で指定された燃料が支払い対象になる */
#include <stdio.h>		/* コンソール入出力で使用 */
#include <conio.h>		/* getchやkbhitなどに使用 */
#include <stdlib.h>		/* rand(),srand():乱数生成に使用 */
#include <time.h>		/* srand(time(NULL)):乱数シードを設定に使用 */
#include <string.h>		/* memcpyを使用 */
#include <windows.h>	/* Sleep(),system("cls")に使用 */

#define MAP_WIDTH 30		/* マップの幅(列数) */
#define MAP_HEIGHT 30		/* マップの高さ(行数) */

#define GAS_STATION 5		/* ガソリンスタンドの数 */
#define FUEL		3		/* 燃料の種類 */

enum {
	SPACE,
	ROAD,
	HOUSE,
	APOLLO,
	HOKUREN,
	COSMO,
	MODA,
	ENEOS,
	MY_CAR,
	STORE,
	CHECK,
};
typedef struct {
	int startMinutes;
	int startSeconds;
	time_t lastTime;
}TIME;
TIME t;

typedef struct {
	int flag;		// お店を何ヶ所巡ったか確認(全5ヶ所)
}StarStatus;
StarStatus star;

typedef struct{
	int y, x;			/* マップ上の車の位置 */
	int car_models;		/* 0 レギュラー車、1 ハイオク車、2 ディーゼル車 */
	double distance;	/* 走行距離 */
	double fuel_max;	/* ガソリンタンクの最大容量 */
	double fuel_now;	/* 現在のガソリンタンク残量 */
}CAR;
CAR car;

typedef struct {
	double price[GAS_STATION][FUEL];	/* 各ガソリンスタンドの料金 */
	double total_price;					/* ガソリンスタンドを利用した費用 */
}GAS_PRICE;
GAS_PRICE gp;

void random_price() {
	/* レギュラー 181.8、ハイオク　191.9、軽油　159.6から1.001～1.3を乗算してランダム価格を代入 */
	double all_fuel[FUEL] = { 181.8, 191.9, 159.6 };
	int i, j;
	for (i = 0; i < GAS_STATION; i++) {
		for (j = 0; j < FUEL; j++) {
			gp.price[i][j] = (((rand() % 300) * 0.001) + 1) * all_fuel[j];
		}
	}
}

void draw_status() {
	/* バッファオーバーフローやメモリ破壊などのリスクを回避する為ポインタを使用 */
	const char* cModel[] = {
		"レギュラー",
		"ハイオク",
		"ディーゼル"
	};
	/* 各種ステータスの表示 */
	printf("%s車　走行距離:%.1fkm　燃料:%.2f/%.2f L　ガソリン代:%.1f円　用事がある店：",
		cModel[car.car_models], car.distance, car.fuel_now, car.fuel_max, gp.total_price);
	for (int i = 0; i < GAS_STATION; i++) {
		if (star.flag > i) {
			printf("★");	/* お店に寄った数だけ表示 */
		} else {
			printf("☆");	/* まだお店に寄っていない数だけ表示 */
		}
	}
	printf("\n");
}

void draw_gas_station() {
	/* 
	文字列を配列に格納(各文字列はポインタとして扱う)
	各要素は文字列リテラル(charのポインタ)として扱われる為、メモリの管理が不要
	*/
	const char *brands[] = { 
		"(Ga)アポロステーション",
		"(Gb)ホクレン", 
		"(Gc)コスモ石油", 
		"(Gd)モダ石油", 
		"(Ge)ENEOS" 
	};
	/* 開始時にランダムで決定した価格とブランドを表示 */
	printf("レギュラー　ハイオク　　軽油　　ブランド名\n");
	for (int i = 0; i < GAS_STATION; i++) {
		for (int j = 0; j < FUEL; j++) {
			printf("　%.1f円 ", gp.price[i][j]);
		}
		printf("%s\n", brands[i]);
	}
}
// 色を変更する関数
void setColor(int color) {
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
}
/* マップを描く */
void draw_map(int map[MAP_HEIGHT][MAP_WIDTH]) {
	system("cls");		/* コンソール画面をクリア */
	printf("出来るだけお得にガソリンをいれて制限時間内に店を巡って家まで帰ろう\n");
	printf("残り時間： %02d:%02d　　店へ行くと＋30秒\n", t.startMinutes, t.startSeconds);
	printf("move【←　a 】【↓　s 】【↑　w 】【→　d 】【リセット　Esc 】\n");	/* 操作ボタン案内 */
	draw_status();
	map[car.y][car.x] = MY_CAR;		/* マップに車を表示させる為の値：8 */
	for (int i = 0; i < MAP_HEIGHT; i++) {
		for (int j = 0; j < MAP_WIDTH; j++) {
			switch (map[i][j]) {
				case SPACE: printf("口"); break;					/* 値：0　コース外エリア */
				case ROAD: printf("　"); break;						/* 値：1　道 */
				case HOUSE: setColor(10),  // 緑色（ゴール）
							printf("家"),
							setColor(7);   // 元の色に戻す
							break;									/* 値：2　家 */
				case APOLLO: printf("Ga"); break;					/* 値：3　アポロステーション(ガソリンスタンド) */
				case HOKUREN: printf("Gb"); break;					/* 値：4　ホクレン(ガソリンスタンド) */
				case COSMO: printf("Gc"); break;					/* 値：5　コスモ石油 */
				case MODA: printf("Gd"); break;						/* 値：6　モダ石油 */
				case ENEOS: printf("Ge"); break;					/* 値：7　エネオス(ガソリンスタンド) */
				case MY_CAR: setColor(12),  // 赤色
							 printf("車"),
							 setColor(7);   // 元の色に戻す
							 map[i][j] = 1; break;					/* 値：8　車(車を表示後、次回更新時に道を表示させる為1を代入) */
				case STORE: setColor(14),  // 黄色
							printf("店"),
							setColor(7);   // 元の色に戻す
							break;									/* 値：9　巡るお店 */
				case CHECK: printf("済"); break;					/* 値：10 用事を足したお店を「済」にする */
			}
		}
		printf("\n");
	}
	draw_gas_station();
}

void startCar(int start) {
	/* 車の初期開始位置をランダムで */
	switch (start) {
		case 0: car.y = 1, car.x = 1; break;
		case 1: car.y = 1, car.x = 20; break;
		case 2: car.y = 10, car.x = 1; break;
		case 3: car.y = 25, car.x = 3; break;
		case 4: car.y = 25, car.x = 5; break;
		case 5: car.y = 15, car.x = 14; break;
		case 6: car.y = 28, car.x = 16; break;
		case 7: car.y = 18, car.x = 8; break;
		case 8: car.y = 15, car.x = 28; break;
		case 9: car.y = 8, car.x = 14; break;
	}
}

void gameStart(int map[MAP_HEIGHT][MAP_WIDTH], int tmpMap[MAP_HEIGHT][MAP_WIDTH]) {
	car.distance = 0.0;				/* 走行距離の初期化 */
	car.fuel_max = (rand() % 8 + 3) * 10.00;	/* ガソリンタンクの容量ランダムで(30～100L) */
	car.fuel_now = (rand() % 8 + 3) * 0.10;		/* 現在のガソリンタンク残量をランダムで(0.3～1.0L) */
	gp.total_price = 0;				/* 現在の燃料購入費 */
	car.car_models = (rand() % 3);	/* 車種をランダムで選択 */
	star.flag = 0;					/* 寄るお店の数の初期値を設定 */
	t.startMinutes = 1;			/* 残り時間1分に設定 */
	t.startSeconds = 0;			/* 残り時間0秒に設定 */
	memcpy(map, tmpMap, sizeof(int) * MAP_HEIGHT * MAP_WIDTH);	/* 初期状態のマップデータをコピー */
	random_price();					/* 燃料価格をランダムで設定 */
	startCar(rand() % 10);			/* スタート位置をランダムで設定 */
}

/* 時間を加算する関数 */
void add_time(int* minutes, int* seconds, int add_sec) {
	*seconds += add_sec;
	while (*seconds >= 60) {
		*seconds -= 60;
		(*minutes)++;
	}
}

int main(void) {
	srand((unsigned int)time(NULL));	/* シード設定(毎回異なる乱数を生成) */

	/* マップデータ(0 コース外、1 道、2 家、3～7 ガソリンスタンド、 8 車、 9 店) */
	int map[MAP_HEIGHT][MAP_WIDTH] = {
		//	列のカウント		   10							 20							   30(値29)
		{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
		{0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0},
		{0, 1, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 1, 1, 1, 0},
		{9, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 1, 4, 1, 0},
		{0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 9, 0, 0, 0, 1, 0, 0, 0, 1, 1, 0, 0, 1, 0},
		{0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0},
		{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 1, 0, 0, 0, 1, 0, 0},
		{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 1, 0, 0},
		{0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0},
		{0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0},	// 10行目
		{0, 1, 1, 0, 2, 1, 0, 1, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 1, 0},
		{0, 0, 1, 0, 0, 1, 0, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 1, 0},
		{0, 0, 1, 0, 0, 1, 1, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0},
		{0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0},
		{0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0},
		{0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0},
		{0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0},
		{0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0},
		{0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 1, 1, 1, 0, 0, 0, 1, 0, 0},
		{0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 0, 0, 1, 0, 7, 1, 0, 1, 0, 0, 0, 1, 0, 0},	// 20行目
		{0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 1, 9, 0},
		{0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 9, 0, 1, 0, 0, 0, 1, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0},
		{0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0},
		{0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0},
		{0, 1, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 1, 0, 1, 1, 0, 1, 1, 1, 1, 0, 0, 1, 0, 0},
		{0, 1, 0, 1, 0, 1, 0, 9, 1, 0, 0, 0, 0, 6, 1, 0, 1, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 1, 0},
		{0, 1, 0, 1, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 1, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 0, 1, 0},
		{0, 1, 0, 1, 0, 1, 0, 1, 1, 1, 1, 0, 0, 1, 1, 0, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 5, 0, 1, 0},
		{0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0},
		{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}	// 30行目(値29)
	};
	int tmpMap[MAP_HEIGHT][MAP_WIDTH];	/* リセット用マップの初期化 */
	memcpy(tmpMap, map, sizeof(int) * MAP_HEIGHT * MAP_WIDTH);	/* tmpMapにmapデータをコピー */
	gameStart(map, tmpMap);		/* 開始時の各種データを設定 */
	draw_map(map);				/* マップを表示 */
	int rflag;					/* リセット用flagの宣言 */
	t.lastTime = time(NULL);

	while (t.startMinutes > 0 || t.startSeconds >= 0) {
		system("cls");
		draw_map(map);
		/* 残り時間00:00の時、ゲームオーバーを表示して終了させる */
		if (t.startMinutes == 0 && t.startSeconds == 0) {
			printf("　　　　　Game Over　時間切れ\n");
			return 0;
		}
		/* 1秒ごとに時間を減らす */
		if (time(NULL) - t.lastTime >= 1) {
			if (t.startSeconds >= 0) {
				t.startSeconds--;		/* 1秒減らす */
				if (t.startMinutes > 0 && t.startSeconds == -1) {
					t.startMinutes--;		/* 1分減らす */
					t.startSeconds = 59;	/* 59秒をセット */
				}
			}
			t.lastTime = time(NULL);
		}
			
		if (kbhit()) {						/* もし何かキーボード入力が行われた時 */
			CAR lastCar = car;				/* 現在の車の位置を保存 */
			switch (getch()) {				/* キー入力を受け取り、表示させずに処理 */
				case 'w': car.y--, rflag = 0; break;	/* 上に移動 */
				case 'a': car.x--, rflag = 0; break;	/* 左に移動 */
				case 'd': car.x++, rflag = 0; break;	/* 右に移動 */
				case 's': car.y++, rflag = 0; break;	/* 下に移動 */
				case 27: gameStart(map, tmpMap), rflag = 1; break;		/* ESCキーでリセット */
			}

			/* 条件を達成した後、家に帰宅出来たらCLEAR */
			if (star.flag == 5) {
				if (map[car.y][car.x] == 2) {
					draw_map(map);
					printf("\n　　　　　CLEAR\n");
					break;
				}
			}

			switch (map[car.y][car.x]) {		/* 移動した先が道以外だったら */
				case 0: car = lastCar; break;	/* コース外だったら車の位置を戻す */
				case 2: car = lastCar; break;	/* 家だったら車の位置を戻す */
					/* ガソリンスタンドだったら車種タイプの燃料満タンにして料金加算し、車の位置を戻す */
				case 3: car = lastCar,			/* アポロステーション */
					gp.total_price += (car.fuel_max - car.fuel_now) * gp.price[0][car.car_models],
					car.fuel_now = car.fuel_max;
					break;
				case 4: car = lastCar,			/* ホクレン */
					gp.total_price += (car.fuel_max - car.fuel_now) * gp.price[1][car.car_models],
					car.fuel_now = car.fuel_max;
					break;
				case 5: car = lastCar,			/* コスモ石油 */
					gp.total_price += (car.fuel_max - car.fuel_now) * gp.price[2][car.car_models],
					car.fuel_now = car.fuel_max;
					break;
				case 6: car = lastCar,			/* モダ石油 */
					gp.total_price += (car.fuel_max - car.fuel_now) * gp.price[3][car.car_models],
					car.fuel_now = car.fuel_max;
					break;
				case 7: car = lastCar,			/* ENEOS */
					gp.total_price += (car.fuel_max - car.fuel_now) * gp.price[4][car.car_models],
					car.fuel_now = car.fuel_max;
					break;
					/* 店だったらステータスを更新して「店」を「済」にし、車の位置を戻す */
				case 9: map[car.y][car.x] = 10,
					car = lastCar,
					star.flag++;
					add_time(&t.startMinutes, &t.startSeconds, 30);	/* 店に寄ると30秒加算 */
					break;
				case 10: car = lastCar; break;	/* 「済」だったら車の位置を戻す */
					/* それ以外なら走行距離やタンク残量を操作
					　 ただし、リセット(Escキー)されていない時のみ */
				default: if (rflag == 0) {
					car.distance += 0.1, car.fuel_now -= 0.01;
				}
			}

			draw_map(map);
			/* 燃料がなくなるとそこで終了 */
			if (car.fuel_now <= 0) {
				printf("\n　　　　　GAME OVER　燃料切れです\n");
				break;
			}
		}
		Sleep(250);		/* 0.25秒待機。CPU負担軽減と目のチカチカ軽減。1秒間に3～4マス進める。 */
	}
	return 0;
}