/* �K�\�����X�^���h�����Q�[�� */
/* �y���������z�^����ꂽ�c��̎��ԁE�R�����m�F���A���s����������Ȃ���X�����苋�����ċA���
�@ �y���s�����z�u�c�莞�Ԃ�00:00�ɂȂ�v�u�R����0�ɂȂ�v
�@ �y�K�\������z�Ԏ�Ŏw�肳�ꂽ�R�����x�����ΏۂɂȂ� */
#include <stdio.h>		/* �R���\�[�����o�͂Ŏg�p */
#include <conio.h>		/* getch��kbhit�ȂǂɎg�p */
#include <stdlib.h>		/* rand(),srand():���������Ɏg�p */
#include <time.h>		/* srand(time(NULL)):�����V�[�h��ݒ�Ɏg�p */
#include <string.h>		/* memcpy���g�p */
#include <windows.h>	/* Sleep(),system("cls")�Ɏg�p */

#define MAP_WIDTH 30		/* �}�b�v�̕�(��) */
#define MAP_HEIGHT 30		/* �}�b�v�̍���(�s��) */

#define GAS_STATION 5		/* �K�\�����X�^���h�̐� */
#define FUEL		3		/* �R���̎�� */

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
	int flag;		// ���X�����������������m�F(�S5����)
}StarStatus;
StarStatus star;

typedef struct{
	int y, x;			/* �}�b�v��̎Ԃ̈ʒu */
	int car_models;		/* 0 ���M�����[�ԁA1 �n�C�I�N�ԁA2 �f�B�[�[���� */
	double distance;	/* ���s���� */
	double fuel_max;	/* �K�\�����^���N�̍ő�e�� */
	double fuel_now;	/* ���݂̃K�\�����^���N�c�� */
}CAR;
CAR car;

typedef struct {
	double price[GAS_STATION][FUEL];	/* �e�K�\�����X�^���h�̗��� */
	double total_price;					/* �K�\�����X�^���h�𗘗p������p */
}GAS_PRICE;
GAS_PRICE gp;

void random_price() {
	/* ���M�����[ 181.8�A�n�C�I�N�@191.9�A�y���@159.6����1.001�`1.3����Z���ă����_�����i���� */
	double all_fuel[FUEL] = { 181.8, 191.9, 159.6 };
	int i, j;
	for (i = 0; i < GAS_STATION; i++) {
		for (j = 0; j < FUEL; j++) {
			gp.price[i][j] = (((rand() % 300) * 0.001) + 1) * all_fuel[j];
		}
	}
}

void draw_status() {
	/* �o�b�t�@�I�[�o�[�t���[�⃁�����j��Ȃǂ̃��X�N���������׃|�C���^���g�p */
	const char* cModel[] = {
		"���M�����[",
		"�n�C�I�N",
		"�f�B�[�[��"
	};
	/* �e��X�e�[�^�X�̕\�� */
	printf("%s�ԁ@���s����:%.1fkm�@�R��:%.2f/%.2f L�@�K�\������:%.1f�~�@�p��������X�F",
		cModel[car.car_models], car.distance, car.fuel_now, car.fuel_max, gp.total_price);
	for (int i = 0; i < GAS_STATION; i++) {
		if (star.flag > i) {
			printf("��");	/* ���X�Ɋ�����������\�� */
		} else {
			printf("��");	/* �܂����X�Ɋ���Ă��Ȃ��������\�� */
		}
	}
	printf("\n");
}

void draw_gas_station() {
	/* 
	�������z��Ɋi�[(�e������̓|�C���^�Ƃ��Ĉ���)
	�e�v�f�͕����񃊃e����(char�̃|�C���^)�Ƃ��Ĉ�����ׁA�������̊Ǘ����s�v
	*/
	const char *brands[] = { 
		"(Ga)�A�|���X�e�[�V����",
		"(Gb)�z�N����", 
		"(Gc)�R�X���Ζ�", 
		"(Gd)���_�Ζ�", 
		"(Ge)ENEOS" 
	};
	/* �J�n���Ƀ����_���Ō��肵�����i�ƃu�����h��\�� */
	printf("���M�����[�@�n�C�I�N�@�@�y���@�@�u�����h��\n");
	for (int i = 0; i < GAS_STATION; i++) {
		for (int j = 0; j < FUEL; j++) {
			printf("�@%.1f�~ ", gp.price[i][j]);
		}
		printf("%s\n", brands[i]);
	}
}
// �F��ύX����֐�
void setColor(int color) {
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
}
/* �}�b�v��`�� */
void draw_map(int map[MAP_HEIGHT][MAP_WIDTH]) {
	system("cls");		/* �R���\�[����ʂ��N���A */
	printf("�o���邾�������ɃK�\����������Đ������ԓ��ɓX�������ĉƂ܂ŋA�낤\n");
	printf("�c�莞�ԁF %02d:%02d�@�@�X�֍s���Ɓ{30�b\n", t.startMinutes, t.startSeconds);
	printf("move�y���@a �z�y���@s �z�y���@w �z�y���@d �z�y���Z�b�g�@Esc �z\n");	/* ����{�^���ē� */
	draw_status();
	map[car.y][car.x] = MY_CAR;		/* �}�b�v�ɎԂ�\��������ׂ̒l�F8 */
	for (int i = 0; i < MAP_HEIGHT; i++) {
		for (int j = 0; j < MAP_WIDTH; j++) {
			switch (map[i][j]) {
				case SPACE: printf("��"); break;					/* �l�F0�@�R�[�X�O�G���A */
				case ROAD: printf("�@"); break;						/* �l�F1�@�� */
				case HOUSE: setColor(10),  // �ΐF�i�S�[���j
							printf("��"),
							setColor(7);   // ���̐F�ɖ߂�
							break;									/* �l�F2�@�� */
				case APOLLO: printf("Ga"); break;					/* �l�F3�@�A�|���X�e�[�V����(�K�\�����X�^���h) */
				case HOKUREN: printf("Gb"); break;					/* �l�F4�@�z�N����(�K�\�����X�^���h) */
				case COSMO: printf("Gc"); break;					/* �l�F5�@�R�X���Ζ� */
				case MODA: printf("Gd"); break;						/* �l�F6�@���_�Ζ� */
				case ENEOS: printf("Ge"); break;					/* �l�F7�@�G�l�I�X(�K�\�����X�^���h) */
				case MY_CAR: setColor(12),  // �ԐF
							 printf("��"),
							 setColor(7);   // ���̐F�ɖ߂�
							 map[i][j] = 1; break;					/* �l�F8�@��(�Ԃ�\����A����X�V���ɓ���\���������1����) */
				case STORE: setColor(14),  // ���F
							printf("�X"),
							setColor(7);   // ���̐F�ɖ߂�
							break;									/* �l�F9�@���邨�X */
				case CHECK: printf("��"); break;					/* �l�F10 �p���𑫂������X���u�ρv�ɂ��� */
			}
		}
		printf("\n");
	}
	draw_gas_station();
}

void startCar(int start) {
	/* �Ԃ̏����J�n�ʒu�������_���� */
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
	car.distance = 0.0;				/* ���s�����̏����� */
	car.fuel_max = (rand() % 8 + 3) * 10.00;	/* �K�\�����^���N�̗e�ʃ����_����(30�`100L) */
	car.fuel_now = (rand() % 8 + 3) * 0.10;		/* ���݂̃K�\�����^���N�c�ʂ������_����(0.3�`1.0L) */
	gp.total_price = 0;				/* ���݂̔R���w���� */
	car.car_models = (rand() % 3);	/* �Ԏ�������_���őI�� */
	star.flag = 0;					/* ��邨�X�̐��̏����l��ݒ� */
	t.startMinutes = 1;			/* �c�莞��1���ɐݒ� */
	t.startSeconds = 0;			/* �c�莞��0�b�ɐݒ� */
	memcpy(map, tmpMap, sizeof(int) * MAP_HEIGHT * MAP_WIDTH);	/* ������Ԃ̃}�b�v�f�[�^���R�s�[ */
	random_price();					/* �R�����i�������_���Őݒ� */
	startCar(rand() % 10);			/* �X�^�[�g�ʒu�������_���Őݒ� */
}

/* ���Ԃ����Z����֐� */
void add_time(int* minutes, int* seconds, int add_sec) {
	*seconds += add_sec;
	while (*seconds >= 60) {
		*seconds -= 60;
		(*minutes)++;
	}
}

int main(void) {
	srand((unsigned int)time(NULL));	/* �V�[�h�ݒ�(����قȂ闐���𐶐�) */

	/* �}�b�v�f�[�^(0 �R�[�X�O�A1 ���A2 �ƁA3�`7 �K�\�����X�^���h�A 8 �ԁA 9 �X) */
	int map[MAP_HEIGHT][MAP_WIDTH] = {
		//	��̃J�E���g		   10							 20							   30(�l29)
		{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
		{0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0},
		{0, 1, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 1, 1, 1, 0},
		{9, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 1, 4, 1, 0},
		{0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 9, 0, 0, 0, 1, 0, 0, 0, 1, 1, 0, 0, 1, 0},
		{0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 0, 0, 1, 1, 0},
		{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 1, 0, 0, 0, 1, 0, 0},
		{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 1, 0, 0},
		{0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0},
		{0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0},	// 10�s��
		{0, 1, 1, 0, 2, 1, 0, 1, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 1, 0},
		{0, 0, 1, 0, 0, 1, 0, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 1, 0},
		{0, 0, 1, 0, 0, 1, 1, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0},
		{0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0},
		{0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0},
		{0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0},
		{0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0},
		{0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0},
		{0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 1, 1, 1, 0, 0, 0, 1, 0, 0},
		{0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 0, 0, 1, 0, 7, 1, 0, 1, 0, 0, 0, 1, 0, 0},	// 20�s��
		{0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 1, 9, 0},
		{0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 9, 0, 1, 0, 0, 0, 1, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0},
		{0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0},
		{0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0},
		{0, 1, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 1, 0, 1, 1, 0, 1, 1, 1, 1, 0, 0, 1, 0, 0},
		{0, 1, 0, 1, 0, 1, 0, 9, 1, 0, 0, 0, 0, 6, 1, 0, 1, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 1, 0},
		{0, 1, 0, 1, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 1, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 0, 1, 0},
		{0, 1, 0, 1, 0, 1, 0, 1, 1, 1, 1, 0, 0, 1, 1, 0, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 5, 0, 1, 0},
		{0, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0},
		{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}	// 30�s��(�l29)
	};
	int tmpMap[MAP_HEIGHT][MAP_WIDTH];	/* ���Z�b�g�p�}�b�v�̏����� */
	memcpy(tmpMap, map, sizeof(int) * MAP_HEIGHT * MAP_WIDTH);	/* tmpMap��map�f�[�^���R�s�[ */
	gameStart(map, tmpMap);		/* �J�n���̊e��f�[�^��ݒ� */
	draw_map(map);				/* �}�b�v��\�� */
	int rflag;					/* ���Z�b�g�pflag�̐錾 */
	t.lastTime = time(NULL);

	while (t.startMinutes > 0 || t.startSeconds >= 0) {
		system("cls");
		draw_map(map);
		/* �c�莞��00:00�̎��A�Q�[���I�[�o�[��\�����ďI�������� */
		if (t.startMinutes == 0 && t.startSeconds == 0) {
			printf("�@�@�@�@�@Game Over�@���Ԑ؂�\n");
			return 0;
		}
		/* 1�b���ƂɎ��Ԃ����炷 */
		if (time(NULL) - t.lastTime >= 1) {
			if (t.startSeconds >= 0) {
				t.startSeconds--;		/* 1�b���炷 */
				if (t.startMinutes > 0 && t.startSeconds == -1) {
					t.startMinutes--;		/* 1�����炷 */
					t.startSeconds = 59;	/* 59�b���Z�b�g */
				}
			}
			t.lastTime = time(NULL);
		}
			
		if (kbhit()) {						/* ���������L�[�{�[�h���͂��s��ꂽ�� */
			CAR lastCar = car;				/* ���݂̎Ԃ̈ʒu��ۑ� */
			switch (getch()) {				/* �L�[���͂��󂯎��A�\���������ɏ��� */
				case 'w': car.y--, rflag = 0; break;	/* ��Ɉړ� */
				case 'a': car.x--, rflag = 0; break;	/* ���Ɉړ� */
				case 'd': car.x++, rflag = 0; break;	/* �E�Ɉړ� */
				case 's': car.y++, rflag = 0; break;	/* ���Ɉړ� */
				case 27: gameStart(map, tmpMap), rflag = 1; break;		/* ESC�L�[�Ń��Z�b�g */
			}

			/* ������B��������A�ƂɋA��o������CLEAR */
			if (star.flag == 5) {
				if (map[car.y][car.x] == 2) {
					draw_map(map);
					printf("\n�@�@�@�@�@CLEAR\n");
					break;
				}
			}

			switch (map[car.y][car.x]) {		/* �ړ������悪���ȊO�������� */
				case 0: car = lastCar; break;	/* �R�[�X�O��������Ԃ̈ʒu��߂� */
				case 2: car = lastCar; break;	/* �Ƃ�������Ԃ̈ʒu��߂� */
					/* �K�\�����X�^���h��������Ԏ�^�C�v�̔R�����^���ɂ��ė������Z���A�Ԃ̈ʒu��߂� */
				case 3: car = lastCar,			/* �A�|���X�e�[�V���� */
					gp.total_price += (car.fuel_max - car.fuel_now) * gp.price[0][car.car_models],
					car.fuel_now = car.fuel_max;
					break;
				case 4: car = lastCar,			/* �z�N���� */
					gp.total_price += (car.fuel_max - car.fuel_now) * gp.price[1][car.car_models],
					car.fuel_now = car.fuel_max;
					break;
				case 5: car = lastCar,			/* �R�X���Ζ� */
					gp.total_price += (car.fuel_max - car.fuel_now) * gp.price[2][car.car_models],
					car.fuel_now = car.fuel_max;
					break;
				case 6: car = lastCar,			/* ���_�Ζ� */
					gp.total_price += (car.fuel_max - car.fuel_now) * gp.price[3][car.car_models],
					car.fuel_now = car.fuel_max;
					break;
				case 7: car = lastCar,			/* ENEOS */
					gp.total_price += (car.fuel_max - car.fuel_now) * gp.price[4][car.car_models],
					car.fuel_now = car.fuel_max;
					break;
					/* �X��������X�e�[�^�X���X�V���āu�X�v���u�ρv�ɂ��A�Ԃ̈ʒu��߂� */
				case 9: map[car.y][car.x] = 10,
					car = lastCar,
					star.flag++;
					add_time(&t.startMinutes, &t.startSeconds, 30);	/* �X�Ɋ���30�b���Z */
					break;
				case 10: car = lastCar; break;	/* �u�ρv��������Ԃ̈ʒu��߂� */
					/* ����ȊO�Ȃ瑖�s������^���N�c�ʂ𑀍�
					�@ �������A���Z�b�g(Esc�L�[)����Ă��Ȃ����̂� */
				default: if (rflag == 0) {
					car.distance += 0.1, car.fuel_now -= 0.01;
				}
			}

			draw_map(map);
			/* �R�����Ȃ��Ȃ�Ƃ����ŏI�� */
			if (car.fuel_now <= 0) {
				printf("\n�@�@�@�@�@GAME OVER�@�R���؂�ł�\n");
				break;
			}
		}
		Sleep(250);		/* 0.25�b�ҋ@�BCPU���S�y���Ɩڂ̃`�J�`�J�y���B1�b�Ԃ�3�`4�}�X�i�߂�B */
	}
	return 0;
}