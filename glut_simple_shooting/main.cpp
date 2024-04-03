#include <stdio.h>
#include <GL/glut.h>
#include <math.h>
#include <time.h>
#include <string.h>

#define ENEMY_MAX 20
#define MY_BULLET_MAX 20
#define ENEMY_BULLET_MAX 100

struct Player {
	GLdouble position[2];
	int hp;
	double lastShoot;
};

struct Enemy {
	GLdouble position[2];
	int hp;
	double born;
	double lastShoot;
	double speedYoko;
};

struct Bullet {
	GLdouble position[2];
	double born;
};

int tate = 500;
int yoko = 500;
int mode = 0;	//プレイ中は0、ゲームオーバーで1、ゲームオーバーアニメーションで2、リスタート時に-1
double startTime;
double gameOverTime;
GLdouble enemySize = 10;
GLdouble playerSize = 10;
GLdouble bulletSize = 4;
double shootInterval = 0.2;
double enemyShootInterval = 1.0;
double spornInterval = 2.0;
double lastSporn = 0.0;
double mySpeed = 500;
double enemySpeed = 30;
double enemySpeedYoko = 20;
double enemyBulletSpeed = 100;
int startEnemy = 3;

int enemyCount = 0;
int myBulletCount = 0;
int enemyBulletCount = 0;
int score = 0;


struct Player player;
struct Enemy enemy[ENEMY_MAX];
struct Bullet myBullet[MY_BULLET_MAX];
struct Bullet enemyBullet[ENEMY_BULLET_MAX];



void printScore(int score) {
	int i;
	char str[10] = "score:";
	char string[15];
	snprintf(string, 10, "%d", score);
	strcat_s(str, sizeof(str), string);
	glColor3d(0.0, 0.0, 0.0); /* 黒 */
	for (i = 0; str[i] != '\0'; i++) {
		glRasterPos3d(10.0 * (GLdouble)i, 20.0, 0);
		glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, str[i]);
	}
}

void drawSquare(GLdouble centerX, GLdouble centerY, GLdouble size) {
	glVertex2d(centerX + size, centerY + size);
	glVertex2d(centerX + size, centerY - size);
	glVertex2d(centerX - size, centerY - size);
	glVertex2d(centerX - size, centerY + size);
}

void drawField() {
	glBegin(GL_LINES);
	glColor3d(0.5, 0.5, 0.5); /* 青 */
	glVertex2d(0, player.position[1] + playerSize);
	glVertex2d(yoko, player.position[1] + playerSize);
	glEnd();
}

void drawPlayer() {
	glBegin(GL_POLYGON);
	glColor3d(0.0, 0.0, 1.0); /* 青 */
	drawSquare(player.position[0], player.position[1], playerSize);
	glEnd();

}

void drawEnemy() {
	glColor3d(1.0, 0.0, 0.0); /* 赤 */
	for (int i = 0; i < enemyCount; i++) {
		glBegin(GL_POLYGON);
		drawSquare(enemy[i].position[0], enemy[i].position[1], enemySize);
		glEnd();
	}
}

void drawBullet() {
	double time = (double)clock() / CLOCKS_PER_SEC;
	glColor3d(0.3, 0.3, 1.0); /* 青 */
	for (int i = 0; i < myBulletCount; i++) {
		glBegin(GL_POLYGON);
		drawSquare(myBullet[i].position[0], myBullet[i].position[1], bulletSize);
		glEnd();
	}
	glColor3d(1.0, 0.3, 0.3); /* 赤 */
	for (int i = 0; i < enemyBulletCount; i++) {
		glBegin(GL_POLYGON);
		drawSquare(enemyBullet[i].position[0], enemyBullet[i].position[1], bulletSize);
		glEnd();
	}
}

int emitMyBullet() {
	if (myBulletCount == MY_BULLET_MAX)
		return -1;
	double time = (double)clock() / CLOCKS_PER_SEC;
	if (time - player.lastShoot > shootInterval) {	//最後の射撃からshootInterval秒経っていれば、発射処理
		player.lastShoot = time;	//lastShootを更新
		myBullet[myBulletCount].born = time;
		myBullet[myBulletCount].position[0] = player.position[0];
		myBullet[myBulletCount].position[1] = player.position[1];
		myBulletCount++;
	}
	printf("%f\n", myBullet[0].position[1]);
	return 0;
}

//引数nは、何番のenemyが発射したか
int emitEnemyBullet(int n) {
	if (enemyBulletCount == ENEMY_BULLET_MAX)
		return -1;
	double time = (double)clock() / CLOCKS_PER_SEC;
	enemyBullet[enemyBulletCount].born = time;
	enemyBullet[enemyBulletCount].position[0] = enemy[n].position[0];
	enemyBullet[enemyBulletCount].position[1] = enemy[n].position[1];
	enemyBulletCount++;
	return 0;
}

void moveBullet() {
	double time = (double)clock() / CLOCKS_PER_SEC;
	int i;
	for (i = 0; i < myBulletCount; i++) {
		myBullet[i].position[1] -= (time - myBullet[i].born) * mySpeed;
		myBullet[i].born = time;
	}
	for (i = 0; i < enemyBulletCount; i++) {
		enemyBullet[i].position[1] += (time - enemyBullet[i].born) * enemyBulletSpeed;
		enemyBullet[i].born = time;
	}
}

void moveEnemy() {
	double time = (double)clock() / CLOCKS_PER_SEC;
	int i;
	for (i = 0; i < enemyCount; i++) {
		if (enemy[i].position[0] <= enemySize || enemy[i].position[0] > yoko - enemySize) {
			enemy[i].speedYoko *= -1;
			if (enemy[i].speedYoko < 0) {
				enemy[i].position[0] = yoko - enemySize - 1;
			}
			else {
				enemy[i].position[0] = enemySize + 1;
			}
		}
		enemy[i].position[1] += (time - enemy[i].born) * enemySpeed;	//敵のy軸移動
		enemy[i].position[0] += (time - enemy[i].born) * enemy[i].speedYoko;//敵のx軸移動
		enemy[i].born = time;
	}
}

int emitEnemy(int x, int y, int hp) {
	if (enemyCount == ENEMY_MAX)
		return -1;
	enemy[enemyCount].hp = hp;
	enemy[enemyCount].position[0] = x;
	enemy[enemyCount].position[1] = y;
	enemy[enemyCount].lastShoot = enemyShootInterval * (rand() / RAND_MAX);
	enemy[enemyCount].born = (double)clock() / CLOCKS_PER_SEC;
	if (rand() < RAND_MAX / 2.0) {
		enemy[enemyCount].speedYoko = enemySpeedYoko;
	}
	else {
		enemy[enemyCount].speedYoko = -enemySpeedYoko;
	}
	enemyCount++;
	return 0;
}

void deleteMyBullet(int n) {
	int i;
	for (i = n; i < myBulletCount; i++) {
		myBullet[i].position[0] = myBullet[i + 1].position[0];
		myBullet[i].position[1] = myBullet[i + 1].position[1];
		myBullet[i].born = myBullet[i + 1].born;
	}
	myBulletCount--;
}

void deleteEnemyBullet(int n) {
	int i;
	for (i = n; i < enemyBulletCount && i < ENEMY_BULLET_MAX; i++) {
		enemyBullet[i].position[0] = enemyBullet[i + 1].position[0];
		enemyBullet[i].position[1] = enemyBullet[i + 1].position[1];
		enemyBullet[i].born = enemyBullet[i + 1].born;
	}
	enemyBulletCount--;
}

void deleteBullet() {
	int i, j;
	//自分の弾丸が画面外に出たら削除
	for (i = 0; i < myBulletCount && i < MY_BULLET_MAX; i++) {
		if (myBullet[i].position[1] < 0 || myBullet[i].position[0] < 0 || myBullet[i].position[0] > yoko) {
			deleteMyBullet(i);
		}
	}
	//敵の弾丸が画面外に出たら削除
	for (i = 0; i < enemyBulletCount && i < ENEMY_BULLET_MAX; i++) {
		if (enemyBullet[i].position[1] > tate || enemyBullet[i].position[0] < 0 || enemyBullet[i].position[0] > yoko) {
			deleteEnemyBullet(i);
		}
	}
	//弾丸同士が接触したら削除
	for (i = 0; i < myBulletCount && i < MY_BULLET_MAX; i++) {
		for (j = 0; j < enemyBulletCount && j < ENEMY_BULLET_MAX; j++) {
			if (fabs(myBullet[i].position[0] - enemyBullet[j].position[0]) <= bulletSize * 2 && myBullet[i].position[1] - enemyBullet[j].position[1] <= bulletSize * 2) {
				deleteMyBullet(i);
				deleteEnemyBullet(j);
				score++;
			}
		}
	}
}

void deleteEnemy(int n) {
	int i;
	for (i = n; i < enemyCount && i < ENEMY_MAX; i++) {
		enemy[i].position[0] = enemy[i + 1].position[0];
		enemy[i].position[1] = enemy[i + 1].position[1];
		enemy[i].born = enemy[i + 1].born;
		enemy[i].hp = enemy[i + 1].hp;
		enemy[i].speedYoko = enemy[i + 1].speedYoko;
	}
	enemyCount--;
	score += 10;
}
void detectEnemyShoot() {
	int i;
	double time = (double)clock() / CLOCKS_PER_SEC;
	for (i = 0; i < enemyCount; i++) {
		if (time - enemy[i].lastShoot > enemyShootInterval) {
			emitEnemyBullet(i);
			enemy[i].lastShoot = time;
		}
	}
}


void detectDeleteEnemy() {
	int i, j;
	for (i = 0; i < enemyCount || i >= ENEMY_MAX; i++) {
		for (j = 0; j < myBulletCount || j >= MY_BULLET_MAX; j++) {
			if (fabs(enemy[i].position[0] - myBullet[j].position[0]) < (bulletSize + enemySize) && fabs(enemy[i].position[1] - myBullet[j].position[1]) < (bulletSize + enemySize)) {
				enemy[i].hp--;
				if (enemy[i].hp <= 0) {
					deleteEnemy(i);
					printf("X");
					deleteMyBullet(j);
				}
			}
		}
	}
}

void detectEnemySporn() {
	double time = (double)clock() / CLOCKS_PER_SEC;
	if (time - lastSporn > spornInterval) {
		emitEnemy(rand() * yoko / RAND_MAX, 50, 1);
		lastSporn = time;
	}
}

void detectGameOver() {
	int i;
	for (i = 0; i < enemyCount && i < ENEMY_MAX; i++) {
		//敵が自分を通り越したら1を返す
		if (enemy[i].position[1] > player.position[1])
			mode = 1;
		//敵の弾丸がプレイヤーに当たったらhpを減らし、hpが0以下になったら1を返す
		if (fabs(player.position[0] - enemyBullet[i].position[0]) < (bulletSize + playerSize) && fabs(player.position[1] - enemyBullet[i].position[1]) < (bulletSize + playerSize)) {
			player.hp--;
			if (player.hp <= 0) {
				mode = 1;
			}
		}
	}
	if (mode == 1) {
		printf("GameOver!だよ");
		gameOverTime = (double)clock() / CLOCKS_PER_SEC;
	}
}


void gameOverDisplay() {
	int i;
	char str[] = "GameOver";
	glColor3d(0.0, 0.0, 0.0); /* 黒 */
	for (i = 0; str[i] != '\0'; i++) {
		glRasterPos3d(yoko / 2.0 + (GLdouble)i * 10.0, tate / 2.0, 0);
		glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, str[i]);
	}
}

void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT);
	drawField();
	drawBullet();
	drawPlayer();
	drawEnemy();
	printScore(score);
	if (mode == 2) {
		gameOverDisplay();
	}
	glutSwapBuffers();
}

void initPlayer() {
	player.hp = 3;
	player.position[0] = 250;
	player.position[1] = (GLdouble)tate - 100;
	player.lastShoot = 0;
}

void initEnemy() {
	int i;
	for (i = 0; i < startEnemy; i++) {
		emitEnemy((enemySize + 50) + ((yoko - 2 * (enemySize - 50)) * i) / ((GLdouble)startEnemy), 50, 1);
	}
}

void detectHard() {
	double time = (double)clock() / CLOCKS_PER_SEC;
	if (time - startTime < 20.0) {

	}
	else if (time - startTime < 40.0) {
		spornInterval = 1.5;
	}
	else if (time - startTime < 80.0) {
		spornInterval = 1.0;
	}
	else if (time - startTime < 120.0) {
		spornInterval = 0.5;
	}
}


void init(void)
{
	glClearColor(1.0, 1.0, 1.0, 1.0);
	initPlayer();
	initEnemy();

	startTime;
	enemyShootInterval = 1.0;
	spornInterval = 2.0;
	lastSporn = 0.0;
	enemySpeedYoko = 20;
	enemyBulletSpeed = 100;

	enemyCount = 0;
	myBulletCount = 0;
	enemyBulletCount = 0;
	score = 0;

	mode = 0;
	startTime = (double)clock() / CLOCKS_PER_SEC;
}



void resize(int w, int h)
{
	glViewport(0, 0, w, h);
	glLoadIdentity();
	glOrtho(-0.5, (GLdouble)w - 0.5,
		(GLdouble)h - 0.5, -0.5, -1.0, 1.0);
}

void keyboard(unsigned char key, int x, int y)
{
	switch (key) {
	case 'r':
		init();
		break;
	}
	display();
}


void mouse(int button, int state, int x, int y)
{
	if (button == GLUT_LEFT_BUTTON /* && state == GLUT_DOWN*/) {
		emitMyBullet();
		display();
	}
	else if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN) {
	}
}

//クリックしていないときのマウス位置を、常にnowpos配列に代入
void motion(int x, int y) {
	if (x >= playerSize && x <= yoko - playerSize) {
		player.position[0] = x;
	}
	display();
}

void timer(int value) {
	if (mode == 0) {
		moveBullet();
		moveEnemy();
		deleteBullet();
		detectEnemySporn();
		detectDeleteEnemy();
		detectEnemyShoot();
		detectGameOver();
		detectHard();
		display();
		// 100ミリ秒後にtimer関数を引数0で自分自身を呼び出す
	}
	else if (mode == 1) {
		display();
	}
	else if (mode == -1) {
		init();
	}
	glutTimerFunc(10, timer, 0);
}

int main(int argc, char* argv[])
{
	srand((unsigned int)time(NULL));	//randの初期化
	enemyCount = 0;
	printf("SSS");
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(yoko, tate);
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE);
	glutCreateWindow(argv[0]);
	glutDisplayFunc(display);
	glutPassiveMotionFunc(motion);
	glutMotionFunc(motion);
	glutKeyboardFunc(keyboard);
	glutMouseFunc(mouse);       //マウス操作に対するイベントハンドラ
	glutReshapeFunc(resize);
	init();
	glutTimerFunc(10, timer, 0);
	glutMainLoop();
	return 0;
}