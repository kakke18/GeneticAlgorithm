/************************ �C���N���[�h ***************************/
#include<stdio.h>
#include<stdlib.h>      // for rand(), srand()
#include<time.h>        // for time()
#include<conio.h>       // for getch()
#include<stdbool.h>     // for boolean


/************************ �萔�錾 ***************************/
#define IN         255   // ���͂̎��(8�r�b�g)
#define OUT          4   // �o�͂̎��(4�ʂ�)
#define POP        100   // �̑���
#define CROSSOVER  0.7   // ������
#define MUTATION  0.02   // �ˑR�ψٗ�


/******************* �}�b�v�̏��̓ǂݍ��� ******************/
#include"map1.c"         // int mapdata[WX][WY] �Ȃǂ�������


/************************ �ϐ��錾 ***************************/
int map[WY][WX];                    // ��ƃG���A
int genotype[POP][IN];              // ��`�q�^ [No.][�Z���T���͒l]
/* �s���o�͂̈ړ������iOUT�ʂ�j */
int move_x[OUT] = { 0, 1, 0, -1 };  //x����
int move_y[OUT] = { -1, 0, 1, 0 };  //y����
int fitness[POP];                   // �̂̓K���x


/***************** �֐��̃v���g�^�C�v�錾 ********************/
void disp_map();                    // �}�b�v�\��
void init_population();             // �̌Q�̏�����     
void disp_action(int n, int steps); // No.n �̌̂� steps �񂾂��ړ�   
int  obtain_input(int x, int y);    // ���͂̏󋵂𐔒l�� 
void generation(int n);             // n �񐢑���
void calc_fitness(int steps);       // �e�̂̓K���x���Z�o 
void determine_next_generation();   // ������̌̌Q���Z�o 


/* ���C���֐� */
int main() {
    int num, c; // ��Ɨp�ϐ�

    /* BEGIN */
    printf("\n*** GA(��`�I�A���S���Y��)�ɂ�鑦���v�����̍œK���f�� ***\n");

    /* �����̌Q�̐��� */
    init_population();
    
    /* �����̂̓����̗Ꭶ */
    printf("�����ɏ�����No.0�̓��������Ă݂܂��傤\n");
    disp_action(0, 100);                                        // No.0�̌̂�100step������
    printf("�����_���ɍ�����̂őS�R�_���ł��ˁD�D�D\n");
    printf("===== ����ł̓��[����GA�Ői�������܂��傤 =====\n");
    
    /* �ő吢�㐔�̓��� */
    printf("�ő吢�㐔(��F30000) = ");
    do{
        scanf("%d", &num);
    } while (num <= 0);

    /* num ��̐�����ŉ������߂� */
    generation(num);
    
    /* END */
    printf("\n�ǂ��ł����H GA�̐����������������������ł���H ");
    printf("�����܂��D\n");
    printf("hit any key to finish:");
    c = getch();
    printf("\n");
    
    return 0;
}

/* ��ʂɃL�����N�^�Ń}�b�v��\��                                   */
/* 0:��(�@), 1:��(��)�C2:�G�[�W�F���g(��), 3:Goal(��) �C4:�O��(�@)   */
void disp_map() {
    for (int y = 0; y < WY; y++) {
        for (int x = 0; x < WX; x++) {
            switch( map[y][x]) {
                // �u�����N
                case 0: 
                    printf("�@");
                    break;  
                // ��
                case 1:
                    printf("��");
                    break;  
                // �G�[�W�F���g
                case 2:
                    printf("��");
                    break;
                // �S�[��  
                case 3:
                    printf("��");
                    break;  
                // �O��
                case 4:
                    printf("�@");
                    break;  
            }
        }
        printf("\n");
    }
}

/* �̌Q�̈�`�q�^�������_���ɏ����� */
void init_population() {
    /* �����̏����� */
    srand((unsigned)time(NULL));    // �����ŏ�����

    /* ��`�q�^�̏����� */
    for (int i = 0; i < POP; i++) {
        for (int j= 0; j < IN; j++) { 
            genotype[i][j] = rand() % 4;
        }
    }
}

/* No.n �̌̂� steps �񂾂��������D                        */
/* �r���ŕǂɂԂ�������C�P�񋏂��ꏊ�ɖ߂�����C�S�[���� */
/* ���B������ steps ��ɓ��B���Ă��Ȃ��Ă��\�����I������D  */
void disp_action(int n, int steps) {
    int dx, dy;         // �ړ���̍��W
    int x, y, i;        // ��Ɨp�ϐ�
    bool finish;        // �I���t���O
    int input, output;  // ���́E�o�͂𐔒l���������� */
    int fit, kbd;

    /* �ϐ��̏����� */
    i = 1;
    finish = false;

    /* �}�b�v�̏����� */
    for (int y = 0; y < WY; y++) {
        for (int x = 0; x < WX; x++) {
            map[y][x] = mapdata[y][x];
        }
    }
    map[START_Y][START_X] = 2;    // �X�^�[�g�n�_�̐ݒ�    */
    map[GOAL_Y][GOAL_X]   = 3;    // �S�[���n�_�̐ݒ�      */
    x = START_X;                  // ���ݒn��X���W�̏����� */
    y = START_Y;                  // ���ݒn��Y���W�̏����� */

    /* ��No.n �� steps �񂾂������� */
    do {
        /* ��ʂ� step�C���͐M���C�o�͐M�� ��\�� */
        printf("step:%d  ", i);
        input = obtain_input(x, y);             // ���̓p�^�[���̒���
        printf("����(���͂̏�):%d, ", input);
        output = genotype[n][input];            // �s���o��
        printf("�o��(���̈ړ�����):");
        switch(output) {
            case 0:
                printf("��\n");
                break;
            case 1:
                printf("��\n");
                break;
            case 2:
                printf("��\n");
                break;
            case 3:
                printf("��\n");
                break;
        }

        /* �}�b�v��\������ */
        disp_map();
        
        /* ���̏ꏊ�����߂� */
        dx = x + move_x[output];    // ���̏ꏊ(��)
        dy = y + move_y[output];    // ���̏ꏊ(��)

        /* ���̏ꏊ����(=1)�C�ʉߋO��(=4)�ȊO�Ȃ瓮���� */
        if (dx > 0 && dx < WX - 1 && dy > 0 && dy < WY - 1 && ( map[dy][dx] != 1 && map[dy][dx] != 4)) {
            map[y][x] = 4;  // ������ꏊ��ʉߋO��(=4)�ɂ���
            x = dx;         // ���ݒn��X���W���X�V����
            y = dy;         // ���ݒn��Y���W���X�V����
            map[y][x] = 2;  // �ړ���̐��l�𒼂�

            /* �S�[���ɓ��B������\������ */
            if (x == GOAL_X && y == GOAL_Y) {
                printf("GOAL!!! ������R(*�L���M*)Ʉ���!!\n");
            }
        }
        else {
            finish = true;  // ���I��
            //�ǂɂԂ�����
            if (map[dy][dx] == 1) {
                printf("�ǂɂԂ���܂����I ��ߥ(*ɄD`*)�ߥ�B\n");
            }
            //����O�ɖ߂���
            else {
                printf("�P�x�ʂ����}�X�ɖ߂��Ă��܂��܂����I ��ߥ(*ɄD`*)�ߥ�B\n");
            }
        }

        /* �L�[�{�[�h���͑҂��ɂ��� */
        printf("hit any key (�����I���������Ƃ��̓X�y�[�X�L�[):");
        kbd = getch();
		if (kbd == 32) {
            exit(1);
        }
        printf("\n\n");

        /* �X�e�b�v�𑝂₷ */
        i++;
    } while ((x != GOAL_X || y != GOAL_Y) && i <= steps && !finish);

    /* �ŏI�I�ȓK���x(�]���l�C�S�[���Ƃ̌��݈ʒu�Ƃ̍��Ōv�Z)*/
    fit = 1 + abs(GOAL_X - START_X) + abs(GOAL_Y - START_Y ) - abs(GOAL_X - x) - abs(GOAL_Y - y);
    printf("���̌̂̓K���x = %d\n", fit);
}

/* ���W(x,y)�ɋ���Ƃ��̎��͂� 0�`255 �ŕ\���ĕԂ� */
/*  0  1  2      �G�[�W�F���g(A)�̎���8�}�X�����̂悤��0�`7bit �� */
/*  7  A  3      �݂Ȃ��C�ǂ������1�C�Ȃ����0�Ƃ��� [0, 255] �� */
/*  6  5  4      ���l�����Ă���D                                 */
int obtain_input(int x, int y) {
    int shift_x[8] = {-1, 0, 1, 1, 1, 0, -1, -1};  // X�̃V�t�g��
    int shift_y[8] = {-1, -1, -1, 0, 1, 1, 1, 0};  // Y�̃V�t�g��
    int input_value;                                // ��ƕϐ�

    /* �ϐ��̏����� */
    input_value = 0;
 
    /* �G�[�W�F���g�̎��͂�8�r�b�g�Ő��l�����Ă��� */
    for (int i = 0; i < 8; i++) {
        input_value = input_value * 2;
        if (map[y + shift_y[i]][x + shift_x[i]] == 1) {
            input_value++;
        }
    }
    return input_value;
}

/* n ���ゾ�������コ���� */
void generation(int n) {
    int i, max, maxnum, kbd;  // ��ƕϐ� 
    int maximum;              // ���_��̍ő�K���x

    /* �ϐ������� */
    i = 0;

    /* ���_��̍ő�K���x�i����̓K���x��`���ɂ��j  */
    /* �i���݈ʒu���S�[���ʒu�Ɉ�v�����Ƃ��ő�ɂȂ�j*/
    maximum = 1 + abs(GOAL_X - START_X) + abs(GOAL_Y - START_Y );

    /* �����̌Q�̓K���x�����߂�(�����͓������ő�step��) */
    calc_fitness(100);

    /* ������ */
    do {
        /* ������� POP�̌̂����߂� */
        determine_next_generation( );
        
        /* �e�̂̓K���x�����߂�(�����͓������ő�step��) */
        calc_fitness(100);
        /* �ő�K���x�����߂� */
        max = - WX * WY;    // ���̒l
        maxnum = 0;         // ���̒l 
        for (int j = 0; j < POP; j++) {    // �S�̂��X�L�������Ēl���X�V
            if (fitness[j] > max) {
                max = fitness[j];
                maxnum = j;
            }
        }
        if (i % 100 == 0) {    // 100���ゲ�Ƃɉ�ʕ\�� */
            printf("���� No.%d:�ő�K���x = %d\n", i, max);
        }
        i++;
    } while (max < maximum && i < n);

    if (max == maximum) {
        printf("���� No.%d:�ő�K���x = %d\n", i, max);
        printf("�S�[���ɓ��B���郋�[��������ꂽ�̂Ő�������I�����܂����D\n");
    }
    printf("����ꂽ���������܂��Dhit any key:");

    /* �L�[�{�[�h���͑҂��ɂ��� */
    kbd = getch();
    printf("\n\n");
    printf("�ŗD�G�� No.%d �̓K���x = %d\n", maxnum, max);

    /* �ŗD�G�̂̓����\�� */
    disp_action(maxnum, 100);

    /* ����ꂽ���[���̉�ʂւ̕\�� */
    printf("\n����ꂽ��(���[��(��`�q�^))�͎��̒ʂ�ł�\n");
    printf("�i����0�������%d�܂ł̂��ꂼ��̍s���o�͂ł��j\n", IN);
    for (i = 0; i < IN; i++) {
        printf("%d", genotype[maxnum][i]);
    }
    printf("\n");
}

/* �e�̂� steps �������������Ƃ��̓K���x fitness[0�`POP-1] �����߂� */
void calc_fitness(int steps) {
    int x, y, i, fit, kbd;  // ��ƕϐ�
    bool finish;            // �I���t���O
    int dx, dy;             // �ړ���̍��W 
    int input, output;      // ���́E�o�͂𐔒l����������

    /* �ϐ������� */
    i = 1;
    finish = false;

    for (int n = 0; n < POP; n++) {
        /* �}�b�v�̏����� */
        for (y = 0; y < WY; y++) {
            for (x = 0; x < WX; x++) {
                map[y][x] = mapdata[y][x];
            }
        }
        map[START_Y][START_X] = 2;    // �X�^�[�g�n�_�̐ݒ�
        map[GOAL_Y][GOAL_X]   = 3;    // �S�[���n�_�̐ݒ�
        x = START_X;                  // ���ݒn��X���W�̏�����
        y = START_Y;                  // ���ݒn��Y���W�̏�����

        /* ��No.n �� steps �񂾂������� */
        do {
            input = obtain_input(x, y);     // ���̓p�^�[���̒���
            output = genotype[n][input];    // �s���o��
            
            /* ���̏ꏊ�����߂� */
            dx = x + move_x[output]; 
            dy = y + move_y[output];

            /* ���̏ꏊ����(=1)�C�S�[��(=3)�C�ʉߋO��(=4)�ȊO�Ȃ瓮���� */
            if (dx > 0 && dx < WX - 1 && dy > 0 && dy < WY - 1 && ( map[dy][dx] != 1 && map[dy][dx] != 3 && map[dy][dx] != 4)) {
                map[y][x] = 4;  /* ������ꏊ��ʉߋO��(=4)�ɂ��� */
                x = dx;         /* ���ݒn��X���W���X�V���� */
                y = dy;         /* ���ݒn��Y���W���X�V���� */
                map[y][x] = 2;  /* �ړ���̐��l�𒼂�      */
            }
            else {
                finish = true;
            }

            /* �X�e�b�v�𑝂₷ */
            i++;
        } while ((x != GOAL_X || y != GOAL_Y) && i <= steps && !finish);

        /* �ŏI�I�ȓK���x�i�]���l�C�S�[���Ƃ̌��݈ʒu�Ƃ̍��Ōv�Z�j*/
        fitness[n] = 1 + abs(GOAL_X - START_X) + abs(GOAL_Y - START_Y ) - abs(GOAL_X - x) - abs(GOAL_Y - y);

        /* fitnness < 0 �Ȃ� fitness = 0 �Ƃ��� */
        if (fitness[n] < 0) {
            fitness[n] = 0;
        }
    }
}

/* �V���v���f�`�����i���[���b�g�I���j�{�G���[�g�ۑ��Ŏ�����̌̂����߂�    */
/* �����ł͕��ʂɃ��[���b�g�I�����Ă���C0�Ԃ̌̂ɃG���[�g��߂����ƂƂ��� */
void determine_next_generation() {
    int elitest[IN];                // �G���[�g
    int max, maxnum, sum, random;   // ��Ɨp�ϐ�
    int new_genotype[POP][IN];      // �V������
    int result_roulette;            // ���[���b�g�̌���
    int tmp1[random], tmp2[random]; // ��_�����p�̍�ƕϐ�

    /* �ϐ������� */
    max = fitness[0];
    maxnum = 0;
    sum = 0;

    /*---------- �G���[�g�I�� ----------*/
    /* fitness �̍ő�l��T�� */
    for (int i = 1; i < POP; i++) {
        if (fitness[i] > max ) {
            max = fitness[i];
            maxnum = i;
        }
    }
    /* eletest �ɑ�� */
    for (int i = 0; i < IN; i++) {
        elitest[i] = genotype[maxnum][i];
    }
    /*---------------------------------*/

    /*---------- ���[���b�g�I�� ----------*/
    /* �S�Ă̌̒l�̓K���x�̘a sum ���Z�o */
    for (int i = 0; i < POP; i++) {
        sum += fitness[i];
    }
    /* �V�����̂𐶐� */
    for (int i = 0; i < POP; i++) {
        /* [0,sum-1]�̗������Z�o */
        random = rand() % sum;
        /* ���[���b�g�̈ʒu���Z�o */
        for (int j = 0; j < POP; j++) {
            random -= fitness[j];
            if (random < 0) {
                result_roulette = j;
                break;
            }
        }
        /* �Z�o�������[���b�g�̈ʒu result_roulette �Ԗڂ̌� genotype[][] �� �V������ new_genotype[][] �ɕۑ� */
        for (int j = 0; j < IN; j++){
            new_genotype[i][j] = genotype[result_roulette][j];
        }
    }
    /*-----------------------------------*/

    /*---------- ��_���� ----------*/
    for (int i = 0; i < POP; i += 2) {
        /* ������ */
        random = rand() % 10;

        /* �������� */
        if (random < CROSSOVER * 10) {
            /* �����_�̈ʒu���Z�o */
            random = rand() % IN;

            /* ��`�q�^�𕪊� */
            for(int j = random; j < IN; j++) {
                tmp1[j] = new_genotype[i][j];       // �e1�̌㔼
                tmp2[j] = new_genotype[i + 1][j];   // �e2�̌㔼
            }
            for(int j = random; j < IN; j++) {
                new_genotype[i][j] = tmp2[j];       // �e1�̌㔼��e2�̌㔼�ŏ㏑��
                new_genotype[i + 1][j] = tmp1[j];   // �e2�̌㔼��e1�̌㔼�ŏ㏑��
            }
        }
    }
    /*-----------------------------*/

    /*---------- �ˑR�ψ� ----------*/
    for (int i = 0; i < POP; i++) {
        for (int j = 0; j < IN; j++) {
            /* �ˑR�ψٗ����Z�o */
            random = rand() % 100;

            /* �ˑR�ψق��� */
            if (random < MUTATION * 100) {
                new_genotype[i][j] = rand() % 4;
            }
        }
    }
    /*-----------------------------*/

    /*---------- �G���[�g�ۑ� ----------*/
    for (int i = 0; i < IN; i++) {
        new_genotype[0][i] = elitest[i];
    }
    /*---------------------------------*/
}
