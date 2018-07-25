#include<stdio.h>
#include<stdlib.h>       /* for rand(), srand()  */
#include<time.h>         /* for time()           */
#include<conio.h>        /* for getch()          */

/************************ �萔�錾 ***************************/
#define IN         255   /* ���͂̎��(8�r�b�g)              */
#define OUT          4   /* �o�͂̎��(4�ʂ�)                */
#define POP        100   /* �̑���                         */
#define CROSSOVER  0.7   /* �������i�����̔����m��(=70%)�j   */
#define MUTATION  0.02   /* �ˑR�ψٗ��i=2%)                 */

/******************* �}�b�v�̏��̓ǂݍ��� ******************/
#include"map1.c"         /* int mapdata[WX][WY] �Ȃǂ������� */
int map[WY][WX];         /* ��ƃG���A */

/* �̂̏�� */
int genotype[POP][IN];    /* ��`�q�^ [No.][�Z���T���͒l]    */
    /* ��jgenotype[5][0]=1 --> No.5�̌̂̎��͂��u�����N   */
    /*     (=0)�̂Ƃ��s��1��I�ԁD�E�ӂ� 0�`OUT-1(=3)        */
/* �s���o�͂̈ړ������iOUT�ʂ�j */
int move_x[OUT] = { 0, 1, 0, -1 };
int move_y[OUT] = { -1, 0, 1, 0 };
int fitness[POP];           /* �̂̓K���x�i�]���l�j        */

/***************** �֐��̃v���g�^�C�v�錾 ********************/
void disp_map(void);                   /* �}�b�v��\������   */
void init_population(void);            /* �̌Q�̏�����     */
void disp_action( int n, int steps );  /* No.n�̌̂�steps  */
                                       /* �񂾂��ړ�������   */
int  obtain_input( int x, int y );     /* ���͂̏󋵂𐔒l�� */
void generation( int n );              /* n �񐢑��コ���� */
/*************************************************************/
/* �����̃v���O�����͕s���S�ŁC���L�̊֐��̒��g������܂���D*/
/* �����L�̂Q�̊֐�����邱�Ƃ��C�����ł̉ۑ�ł��D        */
/*************************************************************/
void calc_fitness( int steps );    /* �e�̂̓K���x�����߂� */
void determine_next_generation( ); /* ������̌̌Q�����߂� */
/*************************************************************/


int main(void)
{
    int num,c;

    printf("\n*** GA(��`�I�A���S���Y��)�ɂ�鑦���v�����̍œK���f�� ***\n");
    /* �����̌Q�̐��� */
    init_population( );
    /* �����̂̓����̗Ꭶ */
    printf("�����ɏ�����No.0�̓��������Ă݂܂��傤\n");
    disp_action( 0, 100 );    /* No.0�̌̂�100step������   */
    printf("�����_���ɍ�����̂őS�R�_���ł��ˁD�D�D\n");
    printf("===== ����ł̓��[����GA�Ői�������܂��傤 =====\n");
    /* �ő吢�㐔�̓��� */
    printf("�ő吢�㐔(��F30000) = ");
    do{
        scanf("%d",&num);
    }while( num <=0 );
    /* num ��̐�����ŉ������߂� */
    generation( num );
    /* �I��� */
    printf("\n�ǂ��ł����H GA�̐����������������������ł���H ");
    printf("�����܂��D\n");
    printf("hit any key to finish:");
    c = getch();
    printf("\n");
    
    return 0;
}


void disp_map(void)
/* ��ʂɃL�����N�^�i�S�p�j�Ń}�b�v��\������                        */
/* 0:��(�@), 1:��(��)�C2:�G�[�W�F���g(��), 3:Goal(��) �C4:�O��(�@) */
{
    int x,y;  /* ����ϐ� */

    for(y=0;y<WY;y++){
        for(x=0;x<WX;x++){
            switch ( map[y][x] ){    /* �� y,x �̏� */
                case 0: printf("�@"); break;  /* �u�����N       */
                case 1: printf("��"); break;  /* ��             */
                case 2: printf("��"); break;  /* �G�[�W�F���g   */
                case 3: printf("��"); break;  /* �S�[��         */
                case 4: printf("�@"); break;  /* �O��(�����Ȃ�) */
            }
        }
        printf("\n");
    }
}


void init_population(void)
/* �̌Q�̈�`�q�^�������_���ɏ��������� */
{
    int i,j;  /* ��ƕϐ� */

    /* �����̏����� */
    srand((unsigned)time(NULL));    /* �����ŏ�����     */
    /* ��`�q�^�̏����� */
    for(i=0;i<POP;i++){             /* i:�̔ԍ�       */
        for(j=0;j<IN;j++){          /* ���͂ɑ΂���o�� */
            genotype[i][j] = rand() % 4;   /* [0,3] */
        }
    }
}


void disp_action( int n, int steps )
/* No.n �̌̂� steps�񂾂��������D                        */
/* �r���ŕǂɂԂ�������C�P�񋏂��ꏊ�ɖ߂�����C�S�[���� */
/* ���B������ steps ��ɓ��B���Ă��Ȃ��Ă��\�����I������D  */
{
    int x, y, i;         /* ��ƕϐ�                   */
    int dx, dy;          /* �ړ���̍��W               */
    int input, output;   /* ���́E�o�͂𐔒l���������� */
    int fit,finish,kbd;

    /* �}�b�v�̏����� */
    for(y=0;y<WY;y++){
        for(x=0;x<WX;x++){
            map[y][x] = mapdata[y][x];
        }
    }
    map[START_Y][START_X] = 2;    /* �X�^�[�g�n�_�̐ݒ�    */
    map[GOAL_Y][GOAL_X]   = 3;    /* �S�[���n�_�̐ݒ�      */
    x = START_X;                  /* ���ݒn��X���W�̏����� */
    y = START_Y;                  /* ���ݒn��Y���W�̏����� */
    /* ��No.n �� steps �񂾂������� */
    i=1;        /* i:�X�e�b�v����\���ϐ�           */
    finish=0;   /* �������̌����ŏI��������P�ɂ��� */
    do{
        /* ��ʂ� step�C���͐M���C�o�͐M�� ��\������ */
        printf("step:%d  ",i);
        input = obtain_input( x, y );  /* ���̓p�^�[���̒��� */
        printf("����(���͂̏�):%d, ",input);
        output = genotype[n][input];  /* �s���o�� */
        printf("�o��(���̈ړ�����):");
        switch( output ){
            case 0: printf("��\n");  break;
            case 1: printf("��\n");  break;
            case 2: printf("��\n");  break;
            case 3: printf("��\n");
        }
        /* �}�b�v��\������ */
        disp_map();
        /* ���̏ꏊ�����߂� */
        dx = x + move_x[output];    /* ���̏ꏊ�i���j */
        dy = y + move_y[output];    /* ���̏ꏊ�i���j */
        if ( dx>0 && dx<WX-1 && dy>0 && dy<WY-1 && 
             ( map[dy][dx] != 1 && map[dy][dx] != 4 ) ){
            /* ���̏ꏊ����(=1)����ђʉߋO��(=4)�ȊO�Ȃ瓮���� */
            map[y][x] = 4;  /* ������ꏊ��ʉߋO��(=4)�ɂ��� */
            x = dx;         /* ���ݒn��X���W���X�V���� */
            y = dy;         /* ���ݒn��Y���W���X�V���� */
            map[y][x] = 2;  /* �ړ���̐��l�𒼂�      */
            /* �S�[���ɓ��B������\������ */
            if ( x == GOAL_X && y == GOAL_Y ){
                printf("GOAL!!! ������R(*�L���M*)Ʉ���!!\n");
            }
        } else {
            finish=1;  /* ���I�� */
            if ( map[dy][dx] == 1 ){    /* �ǂɂԂ�����         */
                printf("�ǂɂԂ���܂����I ��ߥ(*ɄD`*)�ߥ�B\n");
            } else {                    /* �P�x�ʂ����}�X�ɖ߂��� */
                printf("�P�x�ʂ����}�X�ɖ߂��Ă��܂��܂����I ��ߥ(*ɄD`*)�ߥ�B\n");
            }
        }
        /* �L�[�{�[�h���͑҂��ɂ��� */
        printf("hit any key (�����I���������Ƃ��̓X�y�[�X�L�[):");
        kbd = getch();
		if ( kbd == 32 ) exit(1);
        printf("\n\n");
        /* �X�e�b�v�𑝂₷ */
        i++;
    }while( ( x != GOAL_X || y != GOAL_Y ) && i<=steps && finish==0 );
    /* �ŏI�I�ȓK���x�i�]���l�C�S�[���Ƃ̌��݈ʒu�Ƃ̍��Ōv�Z�j*/
    fit = 1 + abs(GOAL_X - START_X) + abs(GOAL_Y - START_Y ) - abs(GOAL_X - x) - abs(GOAL_Y - y);
    printf("���̌̂̓K���x = %d\n",fit);
}


int obtain_input( int x, int y )
/* ���W(x,y)�ɋ���Ƃ��̎��͂� 0�`255 �ŕ\���ĕԂ� */
/*  0  1  2      �G�[�W�F���g(A)�̎���8�}�X�����̂悤��0�`7bit �� */
/*  7  A  3      �݂Ȃ��C�ǂ������1�C�Ȃ����0�Ƃ��� [0, 255] �� */
/*  6  5  4      ���l�����Ă���D                                 */
{
    int shift_x[8] = {-1, 0, 1, 1, 1, 0, -1, -1};  /* X�̃V�t�g�� */
    int shift_y[8] = {-1, -1, -1, 0, 1, 1, 1, 0};  /* Y�̃V�t�g�� */
    int i,input_value=0;    /* ��ƕϐ� */

    /* �G�[�W�F���g�̎��͂�8�r�b�g�Ő��l�����Ă��� */
    for(i=0;i<8;i++){
        input_value = input_value * 2;
        if ( map[ y+shift_y[i] ][ x+shift_x[i] ] == 1 ){
            input_value++;
        }
    }
    return input_value;
}


void generation( int n )
/* n ���ゾ�������コ���� */
{
    int i,j,max,maxnum;      /* ��ƕϐ�           */
    int maximum;             /* ���_��̍ő�K���x */
    int kbd;

    /* ���_��̍ő�K���x�i����̓K���x��`���ɂ��j  */
    /* �i���݈ʒu���S�[���ʒu�Ɉ�v�����Ƃ��ő�ɂȂ�j*/
    maximum = 1 + abs(GOAL_X - START_X) + abs(GOAL_Y - START_Y );
    /* �����̌Q�̓K���x�����߂�(�����͓������ő�step��) */
    calc_fitness( 100 );
    /* ������ */
    i=0;  /* i:���㐔 */
    do{
        /* ������� POP�̌̂����߂� */
        determine_next_generation( );
        /* �e�̂̓K���x�����߂�(�����͓������ő�step��) */
        calc_fitness( 100 );
        /* �ő�K���x�����߂� */
        max = - WX * WY;    /* ���̒l */
        maxnum = 0;         /* ���̒l */
        for(j=0;j<POP;j++){    /* �S�̂��X�L�������Ēl���X�V */
            if ( fitness[j] > max ){
                max = fitness[j];
                maxnum = j;
            }
        }
        if ( i % 100 == 0 ){    /* 100���ゲ�Ƃɉ�ʕ\�� */
            printf("���� No.%d:�ő�K���x = %d\n",i,max);
        }
        i++;
    }while( max < maximum && i < n );
    if ( max == maximum ){
        printf("���� No.%d:�ő�K���x = %d\n",i,max);
        printf("�S�[���ɓ��B���郋�[��������ꂽ�̂Ő�������I�����܂����D\n");
    }
    printf("����ꂽ���������܂��Dhit any key:");
    /* �L�[�{�[�h���͑҂��ɂ��� */
    kbd = getch();
    printf("\n\n");
    printf("�ŗD�G�� No.%d �̓K���x = %d\n",maxnum,max);
    /* �ŗD�G�̂̓����\�� */
    disp_action( maxnum, 100 );
    /* ����ꂽ���[���̉�ʂւ̕\�� */
    printf("\n����ꂽ��(���[��(��`�q�^))�͎��̒ʂ�ł�\n");
    printf("�i����0�������%d�܂ł̂��ꂼ��̍s���o�͂ł��j\n",IN);
    for(i=0;i<IN;i++){
        printf("%d",genotype[maxnum][i]);
    }
    printf("\n");
}


void calc_fitness( int steps )
/* �e�̂� steps �������������Ƃ��̓K���x�����߂� */
/* �\���I�ɂ� disp_actoin() �ƂقƂ�Ǔ����ł���D */
{
    /* �S�̂𓮂����ēK���x fitness[0�`POP-1] �����߂܂��傤�D    */
    /* ����C�ŏ��Ƀ}�b�v�����������Ă��瓮�����܂��Ddisp_action �� */
    /* �R�s�y���Ă��璼���Ɗy�ł��D                                 */

    /* �X�^�[�g�n�_����C���ꂼ��̈ʒu�ł̓��͒l�����ɁC��`�q�^�� */
    /* �����ꂽ�s���K���ɏ]���ē������܂��D���̏ꏊ���C��(=1)���C�� */
    /* ���O��(=4)�C���邢�̓S�[��(=3)�ɂȂ�����C���̍��W(x,y)����C*/
    /* �������g���ċ��߂܂��D                                       */
    /* fitness = 1 + abs(GOAL_X - START_X) + abs(GOAL_Y - START_Y ) */
    /*           - abs(GOAL_X - x) - abs(GOAL_Y - y);               */
    /* �������Cfitness < 0 �ƂȂ����� fitness = 0 �ɂ��邱�ƁD      */
}


void determine_next_generation( )
/* �V���v���f�`�����i���[���b�g�I���j�{�G���[�g�ۑ��Ŏ�����̌̂����߂�    */
/* �����ł͕��ʂɃ��[���b�g�I�����Ă���C0�Ԃ̌̂ɃG���[�g��߂����ƂƂ��� */
{
    /* �܂��̓G���[�g�i�ŗD�G�j�̂̈�`�q�^�� elitest[IN] �ɕۑ����悤�D   */
    
    /* ���[���b�g�I���́C�܂��͑S�Ă̌̂̓K���x�̘a�� sum �Ƃ��ċ��߁C     */
    /* ���� random() % sum �Ƃ��� [0,sum-1]�̗����Ń��[���b�g�̈ʒu�����߁C  */
    /* ���ꂪ���Ԗڂ̌̂ɊY�����邩�𒲂ׂ܂��傤�D����� POP�񂨂��Ȃ��āC*/
    /* �V�����̂̈�`�q�^ new_genotype[][] �ɕۑ����܂��傤�D              */

    /* ���ɁC�̂��Q���g�ݍ��킹�ĂP�_���������܂��D                    */
    /* ���̍ہC�������̊m���Ō����������邱�ƂƂ��܂��D                      */
    /* �����_�̈ʒu�̓����_���ɐݒ肵�C���̈ʒu�Őe�P�C�e�Q�̈�`�q�^�𕪊�  */
    /* ���āC�q�P�F�e�P�̑O���{�e�Q�̌㔼�C�q�Q�F�e�Q�̑O���{�e�P�̌㔼  ��  */
    /* ���ĐV������`�q�^�����܂��D                                        */

    /* ���ɁC�ˑR�ψقł́C�S�̂̑S��`�q�i0�`3�j��ΏۂƂ��āC�ˑR�ψٗ�  */
    /* �̐��N�m���ŁC0�`3�Ƀ����_���ɕύX���܂��傤�D                        */

    /* �Ō�ɁC�ŏ��Ɏ���Ă�����������̃G���[�g�̂̈�`�q�����CNo.0 ��   */
    /* ��`�q���ɋ����I�ɃR�s�[���܂��傤�D�܂� No.0����ɍő�K���x�̌� */
    /* �ƂȂ�܂��D                                                          */

    /* ��Ƃ͈ȏ�ŏI���ł��D��̍�Ƃ����邽�߂ɁC���̊֐����œK�X�C�ϐ���  */
    /* �錾���ė��p���ĉ������D                                              */
}
