
#include <stdio.h>
#include <stdlib.h>
#include <time.h>


// 部屋の情報
struct room
{
	int hasPlayer;	
	int playerVisited;
	int doorInfo[4];
	int hasBox;
	int hasKey;
	int hasSword;
  int hasBow;
  int hasHammer;
	int healthPotion;
	int hasExit;
};
// ダンジョンの部屋は5x5のグリッドなので2次配列
// (0,0)からではなく，(1,1)からスタートするので配列は6対6にする
struct room dungeon[6][6];

// 主人公の情報
struct gamePlayer
{
	char name;
	int roomX;
	int roomY;
	int keyNumber;		// キーの数
	int hp;
	int hasSword; // 刀を持っているか
  int hasBow;
  int hasHammer;
  int wkind;
  int walk;
  int encount;
  int score;
  int money;
  int mkind;
  
  
};

// プレイヤーの宣言
struct gamePlayer player;

// プレイヤー、モンスターの最大ＨＰ
#define playerStartHP			40
#define m1StartHP			15
#define m2StartHP			25

//モンスターの耐性
//ひとくいコウモリ
#define m1rb		1.0
#define m1rp		1.5
#define m1rs		0.5

//スライム
#define m2rb		0.5
#define m2rp		1.0
#define m2rs		1.4

//マンドラゴラ
#define m3rb		0.6
#define m3rp		0.6
#define m3rs		1.5

//スケルトン
#define m4rb		1.5
#define m4rp		0.6
#define m4rs		0.7


// 攻撃のダメージを決める値

#define playerAttackRange		5

#define weaponDamageRatio		1.8

#define MonsterAttackRange		5


// 戦闘結果
#define playerWins				1
#define playerLoses				2

// ドアの状況
#define noDoor		0
#define openDoor	1
#define lockedDoor	2

// 方向
#define North		0
#define East		1
#define South		2
#define West		3

// モンスターのＨＰを格納する変数
int m1HP;
int m2HP;
int tmpHP;

int mper;
int per;

double mrb;
double mrp;
double mrs;


// 勝利と敗北のための変数
int successEscape;
int playerDead;


// 関数のプロトタイプ
void initialiseDungeon();
void initialisePlayer();
void initialiseMonster();
void initialiseGameEndingConditions();
void printGameExplanation();
void showDungeon();
void showRoomInfo();
int getUserMove();
void movePlayer(int);
void checkMonsterFight(void);
void tryPickUpSword(void);
void tryPickUpHammer(void);
void tryPickUpBow(void);
void tryPickUpKey(void);
void tryDrinkHealthPotion(void);
void tryEscape(void);
void encount(void);

int MonsterFight(int , int, int);
int playerAttack(int);
int MonsterAttack(void);
int mEscape(int, int, int);
int eTalk(int,int);



int main(void)
{
	char c;
	int moveDirection;				// 移動方向

	// 初期化
	initialiseDungeon();					// ダンジョンの初期化
	initialisePlayer();						// プレイヤーの情報を初期化
	initialiseMonster();					// モンスターの情報を初期化
	initialiseGameEndingConditions();		// ゲームの終了条件を初期化
	printGameExplanation();					// ゲームのルールを表示

	while (1)
	{
	  tryEscape();
	  if (playerDead)
		{
			break;
		}
		if (successEscape)
		{
			break;
		}
		
		showDungeon();							// ダンジョンの情報を表示
		showRoomInfo();// 部屋の情報を表示
		srand( (int)time(NULL) );
		per=rand()%8;
		mper=rand()%100;
		
		
		


		
		
		tryPickUpSword();// 刀があるなら獲得
		tryPickUpBow();
		tryPickUpHammer();
		tryDrinkHealthPotion();							// 回復アイテムがあるか、使うかのチェック
		tryPickUpKey();
		moveDirection = getUserMove();			// ユーザーから移動方向を入力してもらう
		
		movePlayer(moveDirection);			// プレイヤーを移動させる

		checkMonsterFight();

	}

	// 勝利か敗北のメッセージでゲーム終了する
	if (successEscape)
	{
	printf("\n\n**************\n");
	printf("*ゲームクリア*\n");
	printf("**************\n\n");
	printf("倒したモンスター: %d体", player.score);
	}
	else
	{
	printf("\n\n****************\n");
	printf("*ゲームオーバー*\n");
	printf("****************\n\n");
	}
	printf("\nキーを押して終了する.\n");
	c = getchar();

	return 0;
}

// この関数でダンジョンを初期化する
// ダンジョン情報は紙プロトタイプでテストプレイしたものと同じ
// 出力の都合によって，(1,1)は画面の右上にする
void initialiseDungeon()
{
	int y;

	// プレイヤーは最初(1,1)の部屋にいる
	dungeon[1][1].hasPlayer = 1;
	// プレイヤーは(1,1)の部屋を訪問している
	dungeon[1][1].playerVisited = 1;

	/*
	for(int x=1;x<6;x++){
		for(int y=1;y<6;y++){
		dungeon[x][y].playerVisited = 1;
		}
	}
	*/
	

	// 刀は(1,4)の部屋にある
	dungeon[3][2].hasSword = 1;
	dungeon[5][1].hasHammer = 1;
	dungeon[2][4].hasBow = 1;
	// 回復アイテムは(5,3)と(3,4)の部屋にある
	dungeon[1][5].healthPotion = 1;
	dungeon[3][3].healthPotion = 1;
	dungeon[4][5].healthPotion = 1;

	dungeon[4][4].hasKey =1;
	dungeon[3][5].hasExit=1;



	// ドアの情報

	// 1列目のドア情報
	dungeon[1][1].doorInfo[East] = openDoor;
	dungeon[1][1].doorInfo[South] = openDoor;

	dungeon[2][1].doorInfo[East] = openDoor;
	dungeon[2][1].doorInfo[South] = openDoor;
	dungeon[2][1].doorInfo[West] = openDoor;
	dungeon[3][1].doorInfo[East] = openDoor;
	dungeon[3][1].doorInfo[South] = openDoor;
	dungeon[3][1].doorInfo[West] = openDoor;
	dungeon[4][1].doorInfo[East] = openDoor;
	dungeon[4][1].doorInfo[South] = openDoor;
	dungeon[4][1].doorInfo[West] = openDoor;

	dungeon[5][1].doorInfo[South] = openDoor;
	dungeon[5][1].doorInfo[West] = openDoor;

	// 2列目から4列目までの情報は同じのでforループで初期化
	for (y = 2; y < 5; y++)
	{
		dungeon[1][y].doorInfo[North] = openDoor;
		dungeon[1][y].doorInfo[East] = openDoor;
		dungeon[1][y].doorInfo[South] = openDoor;

		dungeon[2][y].doorInfo[North] = openDoor;
		dungeon[2][y].doorInfo[East] = openDoor;
		dungeon[2][y].doorInfo[South] = openDoor;
		dungeon[2][y].doorInfo[West] = openDoor;
		dungeon[3][y].doorInfo[North] = openDoor;
		dungeon[3][y].doorInfo[East] = openDoor;
		dungeon[3][y].doorInfo[South] = openDoor;
		dungeon[3][y].doorInfo[West] = openDoor;
		dungeon[4][y].doorInfo[North] = openDoor;
		dungeon[4][y].doorInfo[East] = openDoor;
		dungeon[4][y].doorInfo[South] = openDoor;
		dungeon[4][y].doorInfo[West] = openDoor;

		dungeon[5][y].doorInfo[North] = openDoor;
		dungeon[5][y].doorInfo[South] = openDoor;
		dungeon[5][y].doorInfo[West] = openDoor;
	}

	// 5列目のドア情報
	dungeon[1][5].doorInfo[North] = openDoor;
	dungeon[1][5].doorInfo[East] = openDoor;

	dungeon[2][5].doorInfo[North] = openDoor;
	dungeon[2][5].doorInfo[East] = openDoor;
	dungeon[2][5].doorInfo[West] = openDoor;
	dungeon[3][5].doorInfo[North] = openDoor;
	dungeon[3][5].doorInfo[East] = openDoor;
	dungeon[3][5].doorInfo[West] = openDoor;
	dungeon[4][5].doorInfo[North] = openDoor;
	dungeon[4][5].doorInfo[East] = openDoor;
	dungeon[4][5].doorInfo[West] = openDoor;

	dungeon[5][5].doorInfo[North] = openDoor;
	dungeon[5][5].doorInfo[West] = openDoor;

	// ドアがないところの情報を修正
	
	dungeon[4][4].doorInfo[North] = noDoor;
	dungeon[4][4].doorInfo[South] = noDoor;
	dungeon[4][4].doorInfo[West] = noDoor;
	dungeon[4][3].doorInfo[South] = noDoor;
	dungeon[4][5].doorInfo[North] = noDoor;
	dungeon[3][4].doorInfo[East] = noDoor;
	dungeon[1][4].doorInfo[East] = noDoor;
	dungeon[2][4].doorInfo[West] = noDoor;
	dungeon[2][4].doorInfo[South] = noDoor;
	dungeon[2][5].doorInfo[North] = noDoor;
	dungeon[1][2].doorInfo[South] = noDoor;
	dungeon[1][3].doorInfo[North] = noDoor;
	dungeon[2][1].doorInfo[South] = noDoor;
	dungeon[2][2].doorInfo[North] = noDoor;
	dungeon[5][1].doorInfo[South] = noDoor;
	dungeon[5][2].doorInfo[North] = noDoor;
	dungeon[3][2].doorInfo[South] = noDoor;
	dungeon[3][3].doorInfo[North] = noDoor;
	dungeon[3][3].doorInfo[South] = noDoor;
	dungeon[3][4].doorInfo[North] = noDoor;
	dungeon[4][3].doorInfo[South] = noDoor;
	dungeon[4][4].doorInfo[North] = noDoor;
	dungeon[4][4].doorInfo[South] = noDoor;
	dungeon[4][5].doorInfo[North] = noDoor;

	dungeon[2][2].doorInfo[East] = noDoor;
	dungeon[3][2].doorInfo[West] = noDoor;
	dungeon[3][1].doorInfo[East] = noDoor;
	dungeon[4][1].doorInfo[West] = noDoor;
	dungeon[3][2].doorInfo[East] = noDoor;
	dungeon[4][2].doorInfo[West] = noDoor;
	dungeon[3][4].doorInfo[East] = noDoor;
	dungeon[4][4].doorInfo[West] = noDoor;
	dungeon[3][5].doorInfo[East] = noDoor;
	dungeon[4][5].doorInfo[West] = noDoor;
	dungeon[4][3].doorInfo[East] = noDoor;
	dungeon[5][3].doorInfo[West] = noDoor;
	
/*
	// ロックされているドアの情報を修正
	dungeon[4][4].doorInfo[East] = lockedDoor;
	dungeon[5][4].doorInfo[West] = lockedDoor;
*/
}

// プレイヤーの情報を初期化
void initialisePlayer()
{
	// プレイヤーが最初にいる部屋は(1,1)
	player.roomX = 1;
	player.roomY = 1;
	// プレイヤーは最初刀を持っていない
	player.hasSword = 0;
	player.hasHammer = 0;
	player.hasBow = 0;
	player.walk=0;
	player.encount=0;
	player.score=0;
	player.money=0;
	
	player.keyNumber = 0;
	// 最初は最大のＨＰに設定
	player.hp = playerStartHP;
}

//モンスターの情報を初期化
void initialiseMonster()
{
	m1HP = m1StartHP;
	m2HP = m2StartHP;
}

// ゲームの終了条件を初期化
void initialiseGameEndingConditions()
{
	successEscape= 0;//脱出に成功したら1にする
	playerDead = 0;	 //死んだら1にする
}

// ゲームのルールと目的を説明する
void printGameExplanation()
{
	char c, userSelection;



	/*printf("■■■■■■■■■■■■■■■\n");
	printf("■■■■■■■■■■■■■■■\n");
	printf("■■■■■■■■■■■■■■■\n");
	printf("■■■■■■■■■■■■■■■\n");
	printf("■■■■■■■■■■■■■■■\n");
	printf("■■■■■■■■■■■■■■■\n\n");*/
	
	/*printf("■　　　　■　　　■　　　　■\n");
	printf("■■　　■■■■■■■　　■■\n");
	printf("■■■■■　■■■　■■■■■\n");
	printf("■■■■■■■■■■■■■■■\n");
	printf("■■■　　■　　　■　　■■■\n");
	printf("■　　　　■　　　■　　　　■\n\n");
	*/

        /*printf("■    ■   ■    ■\n");
	printf("■■  ■■■■■■■  ■■\n");
	printf("■■■■  ■■■  ■■■■\n");
	printf("■■■■■■■■■■■■■■■\n");
	printf("■■■  ■   ■  ■■■\n");
	printf("■    ■   ■    ■\n\n");*/

	/*printf("　　　　　　　　　　　　　　■　\n");
	printf("■　　■■■■■■　　■　　　　\n");
	printf("　　■　　■■　　■　　　■　　\n");
	printf("　　■■■■■■■■■　　　　　\n");
	printf("　■■■■■　■■■■■■■　　\n");
	printf("　■■■■■■■■■■■■■■　\n\n");
	*/
	/*printf("             ■ \n");
	printf("■  ■■■■■■  ■    \n");
	printf("  ■  ■■  ■   ■  \n");
	printf("  ■■■ ■■■■■     \n");
	printf(" ■■■■■■■■■■■■■  \n");
	printf(" ■■■■■■■■■■■■■■ \n\n");*/

	/*printf("■■■■■■■■■■■■■■■\n");
	printf("■■■■■■■■■■■■■■■\n");
	printf("■■■■■■■■■■■■■■■\n");
	printf("■■■■■■■■■■■■■■■\n");
	printf("■■■■■■■■■■■■■■■\n");
	printf("■■■■■■■■■■■■■■■\n");
	printf("■■■■■■■■■■■■■■■\n");
	printf("■■■■■■■■■■■■■■■\n");
	printf("■■■■■■■■■■■■■■■\n");
	printf("■■■■■■■■■■■■■■■\n\n");
	*/

	/*printf("■■■　■■■　■■■　■■■\n");
	printf("　　■■■■■■■■■■■　　\n");
	printf("　　　　■■■■■■■　　　　\n");
	printf("　　　　　　■■■　　　　　　\n");
	printf("　　　■■■■■■■■■■　　\n");
	printf("　■■■　　■■■■　　■■　\n");
	printf("　　　　　■■■■■■■　　　\n");
	printf("　　　　■　　■■　　■■■　\n");
	printf("　　　■■■■　■■■■■■　\n");
	printf("　　　■■■■■■■■■■■　\n\n");
	*/

	/*printf("■■■  ■■  ■■  ■■\n");
	printf("  ■■■■■■■■■■■  \n");
	printf("    ■■■■■■■    \n");
	printf("      ■■■      \n");
	printf("   ■■■■■■■■■■  \n");
	printf(" ■■■  ■■■■  ■■ \n");
	printf("     ■■■■■■■   \n");
	printf("    ■  ■■  ■■  \n");
	printf("   ■■■■ ■■■■■  \n");
	printf("   ■■■■■■■■■■  \n\n");*/

	/*printf("　　　　　■■■■■　　　■　\n");
	printf("　　　　■■　■　■■　■■■\n");
	printf("　　　　■■■■■■■　■■■\n");
	printf("　　　　　■　■　■　　　■　\n");
	printf("　　　　　　　　　　　　　■　\n");
	printf("　　　　　　■■■　　　　■　\n");
	printf("　　　　　　　■　　　　　■　\n");
	printf("　　　■■■■■■■■■　■　\n");
	printf("　　■　　■■■■■　　■■　\n");
	printf("■■　　　　　■　　　　　■■\n");
	printf("　　　　　■■■■■　　　■　\n");
	printf("　　　　　　■■■　　　　■　\n");
	printf("　　　　■■　　　■■　　■　\n");
        printf("　　　　　■　　　■　　　■　\n");
	printf("　　　　　■　　　■　　　■　\n");
	printf("　　　　　■　　　■　　　■　\n");
	printf("　　　　■■　　　■■　　■　\n\n");
	*/
	

	/*	printf("     ■■■■■    ■ \n");
	printf("   ■■  ■  ■■ ■■■\n");
	printf("    ■■■■■■■  ■■■\n");
	printf("     ■ ■ ■    ■ \n");
	printf("              ■ \n");
	printf("      ■■■     ■ \n");
	printf("       ■      ■ \n");
	printf("   ■■■■■■■■■  ■ \n");
	printf("  ■  ■■■■■   ■■ \n");
	printf("■■     ■      ■■\n");
	printf("     ■■■■■    ■ \n");
	printf("      ■■■     ■ \n");
	printf("    ■■   ■■   ■ \n");
        printf("     ■   ■    ■ \n");
	printf("     ■   ■    ■ \n");
	printf("     ■   ■    ■ \n");
	printf("    ■■   ■■   ■ \n\n");
	*/



	

	printf("****************************************\n");
	printf("*                                      *\n");
	printf("*             迷いの迷宮               *\n");
	printf("*                                      *\n");
	printf("****************************************\n");

	printf("\n");
	printf("1)ルールの説明\n");
	printf("2)ゲームを始める\n");
	printf("1か2を入力してください");

	// ユーザの選択を獲得
	c = getchar();
	userSelection = c;
	while ((c = getchar()) != '\n')
		;
	if (userSelection == '1')
	{
		printf("\nルールの説明:\n");
		printf("==========================\n");
		printf("1)説明文\n");
		printf("2)あなたはモンスターの潜む迷宮に迷いこんでしまった\n");
		printf("3)迷宮のどこかにある鍵を見つけて脱出しよう\n");
		printf("\n4)迷宮では武器やポーションが落ちていることがある\n");
		printf("5)ポーションを使用すると自分のHPを最大値まで回復することができる\n");
		printf("6)武器には属性があり、素手・槌は打撃、弓は貫通、剣は斬撃属性を持つ\n");
		printf("7)槌・弓・剣の威力は同じなので、モンスターの耐性に合わせて使い分けよう\n");
		printf("8)迷宮を進んでいるとモンスターと遭遇する場合がある\n");
		printf("9)戦闘では攻撃の他に、逃走や見逃してもらうといった行動ができる\n");
		printf("10)逃走は成功率が低いため、危なくなったら金で解決するのも良いかもしれない\n");
		printf("11)モンスターを倒していくたびに、より強いモンスターが出現する\n");
		printf("12)強いモンスターは見逃してもらう時に支払う金額も多くなる\n");
		printf("\nボタンを押してゲームを始めましょう\n");
		c = getchar();
	}

}

// ダンジョンの情報を画面に表示する
// 一つの部屋は5x5の大きさにする
// 1列目：北ドア、2列目：見やすくするための空きスペース、3列目：出口、
// 4列目：西ドア、プレイヤー、東ドア、5列目：武器。回復アイテム、6列目：南ドア
void showDungeon()
{
	int x, y;

	// 部屋を一つずつ表示する
	for (y = 1; y <= 5; y++)
	{

		// 1列目：北ドア
		for (x = 1; x <= 5; x++)
		{
			if (dungeon[x][y].playerVisited)
			{
				if (dungeon[x][y].doorInfo[North] == noDoor)
				{
					printf("-------");
				}
				else if (dungeon[x][y].doorInfo[North] == openDoor)
				{
					printf("---D---");
				}
				else if (dungeon[x][y].doorInfo[North] == lockedDoor)
				{
					printf("---L---");
				}
			}
			else
			{
				printf("       ");
			}
		}
		printf("\n");

		// 2列目：見やすくするための空きスペース
		for (x = 1; x <= 5; x++)
		{
			if (dungeon[x][y].playerVisited)
			{
				printf("|     |");
			}
			else
			{
				printf("       ");
			}
		}
		printf("\n");

		// 3列目：出口
		for (x = 1; x <= 5; x++)
		{
			if (dungeon[x][y].playerVisited)
			{

				if (dungeon[x][y].hasExit)
				{
					printf("|  E  |");
				}		
				else
				{

					printf("|     |");
				}
			}
			else
			{
				printf("       ");
			}
		}

		printf("\n");

		// 4列目：西ドア、プレイヤー、東ドア
		for (x = 1; x <= 5; x++)
		{
			if (dungeon[x][y].playerVisited)
			{
				if (dungeon[x][y].doorInfo[West] == noDoor)
				{
					printf("| ");
				}
				else if (dungeon[x][y].doorInfo[West] == openDoor)
				{
					printf("D ");
				}
				else if (dungeon[x][y].doorInfo[West] == lockedDoor)
				{
					printf("L ");
				}
				if (dungeon[x][y].hasPlayer)
				{
					printf(" P  ");
				}
				else
				{
					printf("    ");
				}




				if (dungeon[x][y].doorInfo[East] == noDoor)
				{
					printf("|");
				}
				else if (dungeon[x][y].doorInfo[East] == openDoor)
				{
					printf("D");
				}
				else if (dungeon[x][y].doorInfo[East] == lockedDoor)
				{
					printf("L");
				}
			}
			else {
				printf("       ");
			}
		}
		printf("\n");

		// 5列目：武器・ポーション
		for (x = 1; x <= 5; x++)
		{
			if (dungeon[x][y].playerVisited)
			{
				if (dungeon[x][y].hasSword||dungeon[x][y].hasBow||dungeon[x][y].hasHammer)
				{
					printf("| W ");
				}
				else if(dungeon[x][y].hasKey)
				{
					printf("| K ");
				}
				else
				{
					printf("|   ");
				}
				if (dungeon[x][y].healthPotion)
				{
					printf("H |");
				}
				else
				{
					printf("  |");
				}
			}
			else
			{
				printf("       ");
			}
		}
		printf("\n");

		// 6列目：南ドア
		for (x = 1; x <= 5; x++)
		{
			if (dungeon[x][y].playerVisited)
			{
				if (dungeon[x][y].doorInfo[South] == noDoor)
				{
					printf("-------");
				}
				else if (dungeon[x][y].doorInfo[South] == openDoor)
				{
					printf("---D---");
				}
				else if (dungeon[x][y].doorInfo[South] == lockedDoor)
				{
					printf("---L---");
				}
			}
			else
			{
				printf("       ");
			}
		}
		printf("\n");

	}

	// 記号の説明
	printf("P = プレイヤー\n");
	printf("K = 鍵 , W = 武器 H = 回復アイテム\n");
	printf("D = 扉 , E = 出口 ,| - = 壁 \n");

	// プレイヤーのHPを表示
	printf("Player HP: %d", player.hp);
	printf("  ,Money : %d M\n", player.money);
	printf("持ち物: ");
	if(player.hasSword==1){
	  printf("剣,");
	}
	if(player.hasBow==1){
	  printf("弓,");
	}if(player.hasHammer==1){
	  printf("槌,");
	}
	if(player.keyNumber==1){
	  printf("     鍵,");
	}
	printf("\n");
	  
}

// 部屋の情報を表示する
void showRoomInfo()
{
	int x = player.roomX;
	int y = player.roomY;


	// この部屋に刀があることを表示
	if (dungeon[x][y].hasSword)
	{
		printf("この部屋には武器がある\n");
	}
	if (dungeon[x][y].hasBow)
	{
		printf("この部屋には武器がある\n");
	}
	if (dungeon[x][y].hasHammer)
	{
		printf("この部屋には武器がある\n");
	}
	if (dungeon[x][y].hasKey)
	{
		printf("この部屋には鍵がある\n");
	}
	// この部屋に回復アイテムがあることを表示
	if (dungeon[x][y].healthPotion)
	{
		printf("この部屋にはポーションがある\n");
	}

	// この部屋に出口があることを表示
	if (dungeon[x][y].hasExit)
	{
		printf("この部屋には出口がある\n");
	}


	// この部屋のドア情報を表示
	if (dungeon[x][y].doorInfo[North] == openDoor)
	{
		printf("北の方向にドアがある\n");
	}
	else if (dungeon[x][y].doorInfo[North] == lockedDoor)
	{
		printf("There is a locked door to the North\n");
	}
	if (dungeon[x][y].doorInfo[East] == openDoor)
	{
		printf("東の方向にドアがある\n");
	}
	else if (dungeon[x][y].doorInfo[East] == lockedDoor)
	{
		printf("There is a locked door to the East\n");
	}
	if (dungeon[x][y].doorInfo[South] == openDoor)
	{
		printf("南の方向にドアがある\n");
	}
	else if (dungeon[x][y].doorInfo[South] == lockedDoor)
	{
		printf("There is a locked door to the South\n");
	}
	if (dungeon[x][y].doorInfo[West] == openDoor)
	{
		printf("西の方向にドアがある\n");
	}
	else if (dungeon[x][y].doorInfo[West] == lockedDoor)
	{
		printf("There is a locked door to the West\n");
	}
}

// ユーザーから移動方向を入力してもらう
int getUserMove()
{
	int direction = -1;
	int legalDirection = 0;
	char c;
	int x = player.roomX;
	int y = player.roomY;

	// 正しい方向が入力されてないまで繰り返す
	while (!legalDirection)
	{
		// ユーザーから方向を入力
		printf("移動したい方向を入力してください (N,E,S,W): ");
		c = getchar();
		// 改行文字をバッファーから削除
		scanf("%*c");

		// 入力は正しいかどうかを確認
		switch (c)
		{
		case 'N':
			// 北方向にドアがあれば方向決定
			if (dungeon[x][y].doorInfo[North] == openDoor)
			{
				direction = North;
				legalDirection = 1;
				player.walk++;
			}
			else if (dungeon[x][y].doorInfo[North] == lockedDoor)
			{
				if (player.keyNumber == 2)
				{
					direction = North;
					legalDirection = 1;
				}
				else
				{
					printf("This door is locked. You need two keys to enter\n");
				}
			}
			else
			{
				printf("その方向には進めません\n");
			}
			break;
		case 'E':
			// 東方向にドアがあれば方向決定
			if (dungeon[x][y].doorInfo[East] == openDoor)
			{
				direction = East;
				legalDirection = 1;
				player.walk++;
			}
			else if (dungeon[x][y].doorInfo[East] == lockedDoor)
			{
				if (player.keyNumber == 2)
				{
					direction = East;
					legalDirection = 1;
				}
				else
				{
					printf("This door is locked. You need two keys to enter\n");
				}
			}
			else
			{
				printf("その方向には進めません\n");
			}
			break;
		case 'S':
			// 南方向にドアがあれば方向決定
			if (dungeon[x][y].doorInfo[South] == openDoor)
			{
				direction = South;
				legalDirection = 1;
				player.walk++;
			}
			else if (dungeon[x][y].doorInfo[South] == lockedDoor)
			{
				if (player.keyNumber == 2)
				{
					direction = South;
					legalDirection = 1;
				}
				else
				{
					printf("This door is locked. You need two keys to enter\n");
				}
			}
			else
			{
				printf("その方向には進めません\n");
			}
			break;
		case 'W':
			// 西方向にドアがあれば方向決定
			if (dungeon[x][y].doorInfo[West] == openDoor)
			{
				direction = West;
				legalDirection = 1;
				player.walk++;
			}
			else if (dungeon[x][y].doorInfo[West] == lockedDoor)
			{
				if (player.keyNumber == 2)
				{
					direction = West;
					legalDirection = 1;
				}
				else
				{
					printf("This door is locked. You need two keys to enter\n");
				}
			}
			else
			{
				printf("その方向には進めません\n");
			}
			break;
		default:
			// 不正入力
			printf("不正な入力です\n");
			break;
		}
	}

	return direction;
}

// ユーザが選んだ方向にプレイヤーを移動させる
void movePlayer(int direction)
{
	int currentX = player.roomX;
	int currentY = player.roomY;
	int newX, newY;

	//　プレイヤーを元の部屋から消す
	dungeon[currentX][currentY].hasPlayer = 0;

	//　新しい部屋のX軸とY軸を方向から獲得
	if (direction == North)
	{
		newX = currentX;
		newY = currentY - 1;
	}
	else if (direction == East)
	{
		newX = currentX + 1;
		newY = currentY;
	}
	else if (direction == South)
	{
		newX = currentX;
		newY = currentY + 1;
	}
	else if (direction == West)
	{
		newX = currentX - 1;
		newY = currentY;
	}

	// 新しい場所の情報を追加
	player.roomX = newX;
	player.roomY = newY;
	dungeon[newX][newY].hasPlayer = 1;
	dungeon[newX][newY].playerVisited = 1;

}

// 刀があるなら獲得
void tryPickUpSword(void)
{
	int x = player.roomX;
	int y = player.roomY;

	// 部屋に刀があれば，刀を獲得
	if (dungeon[x][y].hasSword)
	{
		player.hasSword = 1;
		dungeon[x][y].hasSword = 0;
		printf("剣を装備した!\n");
	}
}


void tryPickUpHammer(void)
{
	int x = player.roomX;
	int y = player.roomY;

	// 部屋に刀があれば，刀を獲得
	if (dungeon[x][y].hasHammer)
	{
		player.hasHammer = 1;
		dungeon[x][y].hasHammer = 0;
		printf("槌を装備した！\n");
	}
}

void tryPickUpBow(void)
{
	int x = player.roomX;
	int y = player.roomY;
	// 部屋に刀があれば，刀を獲得
	if (dungeon[x][y].hasBow)
	{
		player.hasBow = 1;
		dungeon[x][y].hasBow = 0;
		printf("弓を装備した！\n");
	}
}

void tryPickUpKey(void)
{
	int x = player.roomX;
	int y = player.roomY;

	if (dungeon[x][y].hasKey)
	{
		player.keyNumber++;
		dungeon[x][y].hasKey = 0;
		printf("鍵を拾った！\n");
	}
}

// ポーションがあるか、使うかのチェック
void tryDrinkHealthPotion(void)
{
  char c, userSelection;
	int x = player.roomX;
	int y = player.roomY;

	int legalDirection = 0;

		  	// 正しい方向が入力されてないまで繰り返す
	

	if (dungeon[x][y].healthPotion && player.hp< playerStartHP)
	{
	  while (!legalDirection)
	{
	  
		printf("ポーションを飲みますか？\n");
	        
		printf("Y:はい　N:いいえ\n");
		

		// ユーザの選択を獲得
		c = getchar();
		//scanf("%*c");
		userSelection = c;
		while ((c = getchar()) != '\n')
			;
		switch (userSelection)
		{
		case 'Y':
		  player.hp = playerStartHP;
		dungeon[x][y].healthPotion = 0;
		printf("HPを回復した！\n");
		legalDirection = 1;
			break;
			
		case 'N':
	        printf("ポーションはそのままにしておいた\n");
		legalDirection = 1;
			break;
		

		default:
			printf("不正な入力です\n");
			break;
				}

	}
		
	}

}

void tryEscape(void)
{
	int x = player.roomX;
	int y = player.roomY;
	char c, userSelection;
	int legalDirection=0;
	

	// 鍵を持っている場合は脱出
	if (dungeon[x][y].hasExit)
	{
		if(player.keyNumber>0){
		  while (!legalDirection)
	{
		printf("出口がある！鍵を使って脱出しますか？\n");
	        
		printf("Y:はい　N:いいえ\n");

		// ユーザの選択を獲得
		c = getchar();
		userSelection = c;
		while ((c = getchar()) != '\n')
			;
		switch (userSelection)
		{
		case 'Y':
		  
		printf("鍵を使って脱出した！\n");
		successEscape=1;
		player.encount=0;
		player.keyNumber=0;
		legalDirection=1;
			break;
			
		case 'N':
	        printf("もうしばらくここに残ることにした\n");
		legalDirection=1;
			break;
		

		default:
			printf("不正な入力です\n");
			break;
		}
	}
		}
		else
		{
		printf("鍵を持っていない\n");
		}
	}

}

void encount(){
  int per=0;
  int flag=0;
  
  
  if(player.walk>3){
    if(player.walk>per){
      //printf("%d ",mper);
      player.encount=1;
      player.mkind=mper%4;
      player.walk=0;
      printf("\n");
      if(player.mkind==0){
	printf("■    ■   ■    ■\n");
	printf("■■  ■■■■■■■  ■■\n");
	printf("■■■■  ■■■  ■■■■\n");
	printf("■■■■■■■■■■■■■■■\n");
	printf("■■■  ■   ■  ■■■\n");
	printf("■    ■   ■    ■\n\n");
	printf("ひとくいコウモリが現れた!\n");

	mrs=m1rs;
	mrp=m1rp;
	mrb=m1rb;
      }else if(player.mkind==1){

	printf("             ■ \n");
	printf("■  ■■■■■■  ■    \n");
	printf("  ■  ■■  ■   ■  \n");
	printf("  ■■■ ■■■■■     \n");
	printf(" ■■■■■■■■■■■■■  \n");
	printf(" ■■■■■■■■■■■■■■ \n\n");
	printf("スライムが現れた!\n");
	mrs=m2rs;
	mrp=m2rp;
	mrb=m2rb;
      }else if(player.mkind==2){

	printf("■■■  ■■  ■■  ■■\n");
	printf("  ■■■■■■■■■■■  \n");
	printf("    ■■■■■■■    \n");
	printf("      ■■■      \n");
	printf("   ■■■■■■■■■■  \n");
	printf(" ■■■  ■■■■  ■■ \n");
	printf("     ■■■■■■■   \n");
	printf("    ■  ■■  ■■  \n");
	printf("   ■■■■ ■■■■■  \n");
	printf("   ■■■■■■■■■■  \n\n");
	printf("マンドラゴラが現れた!\n");

	mrs=m3rs;
	mrp=m3rp;
	mrb=m3rb;
      }else{

	printf("     ■■■■■    ■ \n");
	printf("   ■■  ■  ■■ ■■■\n");
	printf("    ■■■■■■■  ■■■\n");
	printf("     ■ ■ ■    ■ \n");
	printf("              ■ \n");
	printf("      ■■■     ■ \n");
	printf("       ■      ■ \n");
	printf("   ■■■■■■■■■  ■ \n");
	printf("  ■  ■■■■■   ■■ \n");
	printf("■■     ■      ■■\n");
	printf("     ■■■■■    ■ \n");
	printf("      ■■■     ■ \n");
	printf("    ■■   ■■   ■ \n");
        printf("     ■   ■    ■ \n");
	printf("     ■   ■    ■ \n");
	printf("     ■   ■    ■ \n");
	printf("    ■■   ■■   ■ \n\n");
	printf("スケルトンが現れた!\n");
	mrs=m4rs;
	mrp=m4rp;
	mrb=m4rb;
      }
    }
  }    
}

// モンスターとエンカウントしたら戦闘
void checkMonsterFight(void)
{
	char c, userSelection;
	//	int endTestSession = 0;
	encount();
	tmpHP=m1StartHP+player.score*2;
	  while (player.encount)
	{
	  printf("\n------------------------------------------------------------\n");
	  //printf("テスト　モンスターの種類: %d\n", player.mkind);
	  //printf("0コウモリ　1スライム　2マンドラゴラ　3スケルトン\n");
	  printf("持っている武器: ");
	  if(player.hasSword==1){
	  printf("剣,");
	  }
	  if(player.hasBow==1){
	    printf("弓,");
	  }if(player.hasHammer==1){
	    printf("槌");
	  }
	  printf("\n");
		printf("1) 素手で戦う(打撃)\n");
		printf("2) 剣で戦う(斬撃)\n");
		printf("3) 弓で戦う(貫通)\n");
		printf("4) 槌で戦う(打撃)\n");
		printf("5) モンスターから逃げる\n");
		printf("6) お金を払って見逃してもらう\n");
		printf("Player HP: %d", player.hp);
		printf("  ,Monster HP: %d", tmpHP);
		printf("  ,Money : %d M\n", player.money);
		
	        
		printf("\n1〜6のうちどれかを入力:\n ");
		

		// ユーザの選択を獲得
		c = getchar();
		userSelection = c;
		while ((c = getchar()) != '\n')
			;
		switch (userSelection)
		{
		case '1':
			// 素手
			MonsterFight(player.hp, tmpHP, 0);
			break;
			
		case '2':
			// 剣
		  if(player.hasSword==1){
			MonsterFight(player.hp, tmpHP, 1);
		  }
		  else{
		    printf("剣を持っていない\n");
		  }
		    
			break;
			
		case '3':
		  // 弓
	        if(player.hasBow==1){
			MonsterFight(player.hp, tmpHP, 2);
		  }
		  else{
		    printf("弓を持っていない\n");
		  }
		break;

		case '4':
		  // 槌
	        if(player.hasHammer==1){
			MonsterFight(player.hp, tmpHP, 3);
		  }
		else{
		    printf("槌を持っていない\n");
		  }
		break;

		case '5':
		  // 逃げる
			mEscape(player.hp, tmpHP, player.score);
		break;

		case '6':
		  // 見逃してもらう
			eTalk(player.hp,player.score);
		break;

		default:
			printf("不正な入力です\n");
			break;
		}
	}
}

// １回の戦闘シミュレーション
// 戦闘の終了のＨＰを呼び出された関数に伝えるためにポインタ*playerHPを使用
int MonsterFight(int playerHP, int mHP, int wkind)
{
  
  //	while (1)
	  //{
  
  int tmpm=0;
		// プレイヤーは攻撃する
		mHP -= playerAttack(wkind);
		tmpHP=mHP;
		
		// モンスターを倒したかどうかのチェック
		if (mHP <= 0)
		{
		  player.encount=0;
		  player.hp=playerHP;
		  printf("モンスターに勝利した!\n");
		  tmpm = 50*(player.score+1)+100+rand()%50;
		  printf("%d Mを拾った!\n",tmpm );
		  player.money+=tmpm;
		  player.score++;
		  //showDungeon();
		  
			return playerWins;
		}
		// モンスターは反撃する
		playerHP -= MonsterAttack();
		player.hp=playerHP;
		// プレイヤーが死んだかどうかのチェック
		if (playerHP <= 0)
		{
		  
		  player.encount=0;
		  printf("モンスターに敗北した…\n");
		  playerDead=1;
			return playerLoses;
		}

		//	}
	return 0;
}

// プレイヤーの攻撃
int playerAttack(int wkind)
{
	// 攻撃のダメージは乱数で決める
	int damage = rand() % playerAttackRange+3;
	// 武器があると攻撃力アップ
	printf("プレイヤーの攻撃!\n\n");
	if (wkind==0)
	{
	  if(mrb>1.0){
	      printf("打撃属性が弱点のようだ！\n");
	    }else if(mrb<1.0){
	      printf("打撃属性に耐性があるようだ...\n");
	    }
		damage *= mrb;
	}else if(wkind==1){
	    damage*=weaponDamageRatio*mrs;
	    if(mrs>1.0){
	      printf("斬撃属性が弱点のようだ！\n");
	    }else if(mrs<1.0){
	      printf("斬撃属性に耐性があるようだ...\n");
	    }
	  }else if(wkind==2){
	    damage*=weaponDamageRatio*mrp;
	    if(mrp>1.0){
	      printf("貫通属性が弱点のようだ！\n");
	    }else if(mrp<1.0){
	      printf("貫通属性に耐性があるようだ...\n");
	    }
	  }else{
	    damage*=weaponDamageRatio*mrb;
	    if(mrb>1.0){
	      printf("打撃属性が弱点のようだ！\n");
	    }else if(mrb<1.0){
	      printf("打撃属性に耐性があるようだ...\n");
	    }
	    
	}
	//printf("テスト:打撃耐性 %g ,",mrb);
	//printf("貫通耐性 %g ,",mrp);
	//printf("斬撃耐性 %g \n",mrs);
	  
	
	
	printf("\nモンスターに%dのダメージを与えた!\n",damage);
	return damage;
}

// モンスターの攻撃
int MonsterAttack(void)
{
	// 攻撃のダメージは乱数で決める
  int damage = rand() % MonsterAttackRange+1+(player.score/2);
	printf("モンスターの攻撃!\n");
	printf("プレイヤーは%dのダメージを受けた!\n",damage);
	
	
	return damage;
}

int mEscape(int pHP,int mHP,int score){

  int per=rand()%100;
  int num=25;
  if(score>=20){
    num=5;
  }else{
    num-=score;
  }
  
  if(per<=num){
    printf("逃走に成功した！\n");
    player.encount=0;
    //showDungeon();
  }else{
    printf("逃げられなかった!\n");
		pHP -= MonsterAttack();
		player.hp=pHP;
		// プレイヤーが死んだかどうかのチェック
		if (pHP <= 0)
		{
		  
		  player.encount=0;
		  printf("モンスターに敗北した…\n");
		  playerDead=1;
			return playerLoses;
		}
  }
  }

int eTalk(int pHP,int s){
	int legalChose= 0;
	char c;
	int tmpm;
	
	while (!legalChose)
	{
	  tmpm=50*(s+1)+200+rand()%10;
	  printf("「見逃してほしかったら %d M寄越せ」\n",tmpm);
	  printf("%d Mを払いますか? Y:はい N:いいえ\n",tmpm);
		c = getchar();
		// 改行文字をバッファーから削除
		scanf("%*c");
		 
		// 入力は正しいかどうかを確認
		switch (c)
		{
		case 'Y':
		  if(player.money<tmpm){
		    legalChose=1;
		    printf("「金が足りないぞ！」\n");
		    pHP -= MonsterAttack();
		    player.hp=pHP;
		// プレイヤーが死んだかどうかのチェック
		if (pHP <= 0)
		{
		  
		  player.encount=0;
		  printf("モンスターに敗北した…\n");
		  playerDead=1;
			return playerLoses;
		}
		  }else{
		    printf("%d M支払った\n",tmpm);
		    printf("「話のわかる奴だ」\n");
		    printf("モンスターに見逃してもらった!\n");
		    player.money-=tmpm;
		    player.encount=0;
		    legalChose=1;
		    //showDungeon();
		  }
		  break;
		case 'N':
		    printf("「なら死ね！」\n");
		    legalChose=1;
		      pHP -= MonsterAttack();
		player.hp=pHP ;
		
		// プレイヤーが死んだかどうかのチェック
		if (pHP <= 0)
		{
		  
		  player.encount=0;
		  printf("モンスターに敗北した…\n");
		  playerDead=1;
			return playerLoses;
		}
		break;
		default:
			printf("不正な入力です\n");
			break;
		}
	}
}
		  
		  
		    
		    
		    
  
