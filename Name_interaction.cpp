#include <windows.h>
#include <gl/gl.h>
#include <gl/glu.h>
#include <math.h> 
#include <glut.h>
#include <string>
using namespace std;

#define PI 3.141592
bool isSmiling = true; // 초기값은 웃는 표정
bool isDisplayingText = false; // 텍스트 "노" 표시 여부
float textScale = 1.0f;   // "노" 글자의 크기
float textPositionX = 0.08f; // "노" 글자의 X 위치
float textPositionY = 0.08f; // "노" 글자의 Y 위치
bool textMovingOutward = true; // "노" 글자의 이동 방향 (안에서 밖으로)

bool isRotating = false;          // 객체가 회전 중인지 나타내는 변수
bool isRotatingBack = false;      // 객체가 원래 위치로 돌아가는 중인지 확인하는 변수
float rotationAngle = 0.0f;       // 현재 회전 각도
float targetRotation = 0.0f;      // 원위치로 돌아갈 때 목표 각도

bool isWalking = false; // 걷기 애니메이션 활성화 여부

float characterPosX = 0.0f;
float characterSpeed = 0.0002f; // 이동 속도 조절
float movementRange = 0.6f;      // 이동 범위 설정
bool movingRight = true;         // 캐릭터 이동 방향
float characterRotation = 0.0f;  // 캐릭터 몸의 방향 (회전 각도)

float shoulderAngle = 0.0f;
float legAngle = 0.0f;
bool armDirection = true;
bool legDirection = true;

bool isHandWaving = false; // 손 인사 동작 활성화 여부
float handWaveAngle = 0.0f; // 팔 인사 각도 초기값
bool handWaveDirection = true; // 팔이 올라가는 방향
int handWaveCycles = 0; // 인사 횟수
const int maxHandWaveCycles = 5; // 인사 반복 최대 횟수

bool isPowerOn = false; // 전원이 켜진 상태 여부
bool isPowerButtonRotating = false; // 파란색 삼각형 버튼 회전 여부

// 추가된 변수
bool isTextRotating = false; // "노" 글자의 회전 여부
float textRotationAngle = 0.0f; // "노" 글자의 회전 각도

// "ㄴ" 모양의 수직 및 수평 선의 Y 위치를 조절하는 변수 추가
float lShapeVerticalPositionY = 0.0f; // 수직 선의 Y 위치
float lShapeHorizontalPositionY = 0.0f; // 수평 선의 Y 위치

// 바운스 애니메이션 상태와 위치 변수
bool isBouncing = false;             // 바운스 애니메이션 상태
float baseLineBouncePosition = 0.0f;  // 원기둥 Y 위치
bool bounceDirectionDown = true;      // 바운스 방향 (아래로만 이동)
const float bounceRange = -0.05f;     // 바운스 최대 하강 범위

// 각 부분의 scale 변수 추가
float verticalLineScale = 1.0f;
float horizontalLineScale = 1.0f;
float intersectionScale = 1.0f;
float baseLineScale = 1.0f;

bool isButtonRotating = false;      // 버튼 회전 애니메이션 여부
float buttonRotationAngle = 0.0f;   // 버튼 회전 각도
bool buttonRotationComplete = false; // 버튼 회전 완료 여부




// 원 그리기 
void drawCircle(float x, float y, float z, float radius) {
    int numSegments = 100;  // 세그먼트 수가 많을수록 원에 가까워짐
    glBegin(GL_TRIANGLE_FAN);
    glVertex3f(x, y, z);  // 원의 중심
    for (int i = 0; i <= numSegments; i++) {
        float angle = 2.0f * PI * i / numSegments;
        float dx = cos(angle) * radius;
        float dy = sin(angle) * radius;
        glVertex3f(x + dx, y + dy, z);
    }
    glEnd();
}




// 입꼬리 설정 
void drawMouth() {
    glColor3f(0.0f, 0.0f, 0.0f);  // 검정색으로 설정
    glLineWidth(2.5f);

    glBegin(GL_LINE_STRIP);

    // 웃는 표정 또는 슬픈 표정을 그리기 위한 조건
    if (isSmiling) {
        // 웃는 입 모양
        for (int i = 0; i <= 20; i++) {
            float angle = PI * (i / 20.0f);  // 0부터 PI까지의 각도 (반원 형태)
            float x = cos(angle) * 0.14f;    // 입의 너비 설정
            float y = sin(angle) * -0.07f;   // 곡선의 깊이 설정
            glVertex3f(x, y + 0.4f, 0.701f); // 중앙 위쪽에 위치
        }
    }
    else {
        // 슬픈 입 모양
        for (int i = 0; i <= 20; i++) {
            float angle = PI * (i / 20.0f);  // 0부터 PI까지의 각도 (반원 형태)
            float x = cos(angle) * 0.14f;
            float y = sin(angle) * 0.07f;    // 입이 아래로 내려가도록 설정
            glVertex3f(x, y + 0.3f, 0.701f); // 중앙 아래쪽에 위치
        }
    }

    glEnd();

    glLineWidth(1.0f); // 기본 두께로 복원
}

// 얼굴 그리기
void drawFace() {
    if (isPowerOn && !isDisplayingText) { // 전원이 켜진 경우에만 얼굴 표시
        glColor3f(0.0f, 0.0f, 0.0f);
        drawCircle(-0.35f, 0.5f, 0.701f, 0.04f);  // 왼쪽 눈
        drawCircle(0.35f, 0.5f, 0.701f, 0.04f);   // 오른쪽 눈
        drawMouth(); // 입 그리기
    }
}


void drawTextWithLines() {
    if (isDisplayingText) {
        glPushMatrix();

        glTranslatef(textPositionX, textPositionY, 0.0f); // Y 축 오프셋 제거
        glScalef(textScale, textScale, 1.0f); // 전체 크기 조절

        if (isTextRotating) {
            glRotatef(textRotationAngle, 0.0f, 1.0f, 0.0f); // Z축 기준 회전
        }
        // 1. 수직 선 (ㄴ의 왼쪽 세로 부분) - 아래쪽 고정, 위로만 확대
        glPushMatrix();
        glColor3f(1.0f, 0.6f, 0.6f); // 연한 빨강색 설정
        glTranslatef(-0.15f, 0.45f + lShapeVerticalPositionY + (0.125f * (verticalLineScale - 1.0f)), 0.701f); // 수직 위치 조절
        glScalef(0.05f, 0.25f * verticalLineScale, 0.5f); // y축 방향으로만 크기 확대
        glutSolidCube(1.0f); // 채워진 큐브
        glColor3f(0.0f, 0.0f, 0.0f); // 테두리 색상 (검정)
        glutWireCube(1.0f); // 테두리 큐브
        glPopMatrix();

        // 2. 수평 선 (ㄴ의 윗부분) - 왼쪽 고정, 오른쪽으로만 확대
        glPushMatrix();
        glColor3f(1.0f, 1.0f, 0.6f); // 연한 노랑색 설정
        glTranslatef(0.0f + (0.15f * (horizontalLineScale - 1.0f)), 0.35f + lShapeHorizontalPositionY, 0.701f); // 수평 위치 조절
        glScalef(0.3f * horizontalLineScale, 0.05f, 0.5f); // x축 방향으로만 크기 확대
        glutSolidCube(1.0f); // 채워진 큐브
        glColor3f(0.0f, 0.0f, 0.0f); // 테두리 색상 (검정)
        glutWireCube(1.0f); // 테두리 큐브
        glPopMatrix();


        // 3. 교차점 (ㅗ의 교차 부분)
        glPushMatrix();
        glColor3f(0.6f, 1.0f, 0.6f); // 연한 초록색 설정
        glTranslatef(0.0f, 0.23f, 0.701f); // 위치 조정
        glScalef(intersectionScale, intersectionScale, intersectionScale * 2.0f); // 구 깊이 조절 (z축 스케일 크게 증가)
        glutSolidSphere(0.06f, 8, 8); // 채워진 구
        glColor3f(0.0f, 0.0f, 0.0f); // 테두리 색상 (검정)
        glutWireSphere(0.06f, 8, 8); // 테두리 구
        glPopMatrix();

        // 4. 받침 선 (ㅗ의 받침 부분 - 원기둥 테두리만)
        glPushMatrix();
        glColor3f(0.0f, 0.5f, 1.0f); // 파란색 설정 (테두리)
        glTranslatef(-0.2f, 0.1f + baseLineBouncePosition, 0.5f); // 바운스 위치 적용
        glRotatef(60.0f, 0.0f, 1.0f, 0.0f); // y축 기준으로 회전하여 가로 방향으로 설정
        glRotatef(15.0f, 0.0f, 0.0f, 1.0f); // z축 기준으로 약간 기울이기

        // 크기 조절 추가
        glScalef(baseLineScale, baseLineScale, baseLineScale); // 전체 크기 조절

        GLUquadric* quadric = gluNewQuadric();
        gluQuadricDrawStyle(quadric, GLU_LINE); // 테두리만 보이도록 설정

        // 원기둥 본체
        gluCylinder(quadric, 0.05f, 0.05f, 0.5f, 20, 20); // 원기둥 생성 (기본 크기)

        gluDeleteQuadric(quadric); // quadric 객체 삭제
        glPopMatrix();

        glPopMatrix();
    }
}



// 게임기 버튼 그리기
void drawButtons() {
    glColor3f(0.0f, 0.0f, 0.0f);
    glBegin(GL_POLYGON);
    glVertex3f(-0.7f, -0.1f, 0.702f);
    glVertex3f(0.2f, -0.1f, 0.702f);
    glVertex3f(0.2f, -0.15f, 0.702f);
    glVertex3f(-0.7f, -0.15f, 0.702f);
    glEnd();

    // 상단 검정 원형 버튼
    glColor3f(0.0f, 0.0f, 0.0f);
    drawCircle(0.55f, -0.13f, 0.702f, 0.045f);

    // 노란색 십자가 버튼 회전 애니메이션
    glPushMatrix();
    if (isButtonRotating) {
        glTranslatef(-0.4f, -0.45f, 0.0f); // 십자가 버튼 중심으로 이동
        glRotatef(buttonRotationAngle, 0.0f, 0.0f, 1.0f); // Z축을 기준으로 회전
        glTranslatef(0.4f, 0.45f, 0.0f); // 원래 위치로 복귀
    }

    // 노란색 십자가 버튼
    glColor3f(1.0f, 0.9f, 0.2f); // 노란색 설정
    glBegin(GL_POLYGON);

    // 왼쪽 부분
    glVertex3f(-0.45f, -0.5f, 0.702f);
    glVertex3f(-0.55f, -0.5f, 0.702f);
    glVertex3f(-0.55f, -0.4f, 0.702f);
    glVertex3f(-0.45f, -0.4f, 0.702f);

    // 윗부분
    glVertex3f(-0.45f, -0.4f, 0.702f);
    glVertex3f(-0.45f, -0.3f, 0.702f);
    glVertex3f(-0.35f, -0.3f, 0.702f);
    glVertex3f(-0.35f, -0.4f, 0.702f);

    // 오른쪽 부분
    glVertex3f(-0.35f, -0.4f, 0.702f);
    glVertex3f(-0.25f, -0.4f, 0.702f);
    glVertex3f(-0.25f, -0.5f, 0.702f);
    glVertex3f(-0.35f, -0.5f, 0.702f);

    // 아랫부분
    glVertex3f(-0.35f, -0.5f, 0.702f);
    glVertex3f(-0.35f, -0.6f, 0.702f);
    glVertex3f(-0.45f, -0.6f, 0.702f);
    glVertex3f(-0.45f, -0.5f, 0.702f);

    glEnd();
    glPopMatrix(); // 십자가 버튼 회전 완료 후 변환 초기화


    // 파란색 삼각형 버튼 회전 처리
    glPushMatrix();
    if (isPowerButtonRotating) {
        glTranslatef(0.25f, -0.4f, 0.0f); // 삼각형 중심으로 이동
        glRotatef(buttonRotationAngle, 0.0f, 0.0f, 1.0f); // 회전
        glTranslatef(-0.25f, 0.4f, 0.0f); // 원래 위치로 복귀
    }
    glColor3f(0.0f, 0.7f, 1.0f); // 파란색 설정
    glBegin(GL_TRIANGLES);
    glVertex3f(0.1f, -0.5f, 0.702f);
    glVertex3f(0.4f, -0.5f, 0.702f);
    glVertex3f(0.25f, -0.3f, 0.702f);
    glEnd();
    glPopMatrix(); // 파란색 삼각형 버튼 회전 끝

    // 빨간색 원형 버튼
    glColor3f(1.0f, 0.2f, 0.2f);
    drawCircle(0.4f, -0.75f, 0.702f, 0.14f);

    // 초록색 원형 버튼
    glColor3f(0.5f, 1.0f, 0.5f);
    drawCircle(0.6f, -0.45f, 0.702f, 0.06f);

    // 왼쪽 아래 파란색 가로 바
    glColor3f(0.0f, 0.0f, 1.0f);
    glBegin(GL_POLYGON);
    glVertex3f(-0.6f, -0.85f, 0.702f);
    glVertex3f(-0.4f, -0.85f, 0.702f);
    glVertex3f(-0.4f, -0.89f, 0.702f);
    glVertex3f(-0.6f, -0.89f, 0.702f);
    glEnd();

    // 오른쪽 아래 파란색 가로 바
    glColor3f(0.0f, 0.0f, 1.0f);
    glBegin(GL_POLYGON);
    glVertex3f(-0.33f, -0.85f, 0.702f);
    glVertex3f(-0.13f, -0.85f, 0.702f);
    glVertex3f(-0.13f, -0.89f, 0.702f);
    glVertex3f(-0.33f, -0.89f, 0.702f);
    glEnd();
}


void handleWalkAnimation() {
    float speed = 0.3f;

    // 팔 움직임 설정
    if (armDirection) {
        shoulderAngle += speed;
        if (shoulderAngle >= 30.0f) armDirection = false;
    }
    else {
        shoulderAngle -= speed;
        if (shoulderAngle <= -30.0f) armDirection = true;
    }

    // 다리 움직임 설정
    if (legDirection) {
        legAngle += speed;
        if (legAngle >= 30.0f) legDirection = false;
    }
    else {
        legAngle -= speed;
        if (legAngle <= -30.0f) legDirection = true;
    }

    // 캐릭터 이동 설정
    if (movingRight) {
        characterPosX += characterSpeed;
        characterRotation = 0.0f;
    }
    else {
        characterPosX -= characterSpeed;
        characterRotation = 180.0f;
    }

    glutPostRedisplay();
}



void drawArm(float x, float y, float z, bool isLeft) {
    glPushMatrix();
    glTranslatef(x, y + 0.425f, z); // 팔의 위치로 이동

    if (isLeft && isHandWaving) {
        // 팔을 Z축 기준으로 회전시켜 옆으로 움직이도록 설정
        glRotatef(handWaveAngle, 0.5f, 0.1f, -1.0f); // (옆으로 회전)
    }
    else {
        glRotatef(isLeft ? -shoulderAngle : shoulderAngle, 1.0f, 0.0f, 0.0f); // 기존 어깨 회전 적용
    }

    glTranslatef(0.0f, -0.425f, 0.0f); // 팔의 기준점으로 이동
    glScalef(0.2f, 0.85f, 0.2f);       // 팔의 크기 조정
    glColor3f(0.4f, 0.85f, 0.8f);      // 팔의 색상 설정
    glutSolidCube(1.0f);               // 채워진 팔 그리기
    glColor3f(0.0f, 0.0f, 0.0f);       // 테두리 색상 설정
    glutWireCube(1.0f);                // 팔의 테두리 그리기
    glPopMatrix();
}


// 다리 그리기 함수 수정 - 위쪽 끝을 기준으로 회전
void drawThickLeg(float x, float y, float z, bool isLeft) {
    glPushMatrix();

    // 다리의 위쪽 끝으로 이동하여 회전 축을 설정
    glTranslatef(x, y + 0.3f, z); // 다리의 위쪽 끝으로 이동
    glRotatef(isLeft ? legAngle : -legAngle, 1.0f, 0.0f, 0.0f); // 회전 적용
    glTranslatef(0.0f, -0.3f, 0.0f); // 다리의 나머지 부분으로 이동

    // 다리 그리기
    glScalef(0.2f, 0.6f, 0.2f);
    glColor3f(0.4f, 0.85f, 0.8f);
    glutSolidCube(1.0f);
    glColor3f(0.0f, 0.0f, 0.0f);
    glutWireCube(1.0f);
    glPopMatrix();
}


// 게임기 몸통 그리기 
void drawBox()
{
    // 진한 민트색 앞면 설정
    glColor3f(0.4f, 0.8f, 0.7f);

    // 앞면 (진한 민트색)
    glBegin(GL_POLYGON);
    glVertex3f(-0.9, -1.0, 0.7);
    glVertex3f(0.9, -1.0, 0.7);
    glVertex3f(0.9, 1.0, 0.7);
    glVertex3f(-0.9, 1.0, 0.7);
    glEnd();

    // 뒷면
    glBegin(GL_POLYGON);
    glVertex3f(-0.9, -1.0, -0.7);
    glVertex3f(0.9, -1.0, -0.7);
    glVertex3f(0.9, 1.0, -0.7);
    glVertex3f(-0.9, 1.0, -0.7);
    glEnd();

    // 왼쪽 면
    glBegin(GL_POLYGON);
    glVertex3f(-0.9, -1.0, -0.7);
    glVertex3f(-0.9, -1.0, 0.7);
    glVertex3f(-0.9, 1.0, 0.7);
    glVertex3f(-0.9, 1.0, -0.7);
    glEnd();

    // 오른쪽 면
    glBegin(GL_POLYGON);
    glVertex3f(0.9, -1.0, -0.7);
    glVertex3f(0.9, -1.0, 0.7);
    glVertex3f(0.9, 1.0, 0.7);
    glVertex3f(0.9, 1.0, -0.7);
    glEnd();

    // 윗면
    glBegin(GL_POLYGON);
    glVertex3f(-0.9, 1.0, -0.7);
    glVertex3f(0.9, 1.0, -0.7);
    glVertex3f(0.9, 1.0, 0.7);
    glVertex3f(-0.9, 1.0, 0.7);
    glEnd();

    // 아랫면
    glBegin(GL_POLYGON);
    glVertex3f(-0.9, -1.0, -0.7);
    glVertex3f(0.9, -1.0, -0.7);
    glVertex3f(0.9, -1.0, 0.7);
    glVertex3f(-0.9, -1.0, 0.7);
    glEnd();

    // 검정색 테두리 그리기 (각 면의 외곽선만)
    glColor3f(0.0f, 0.0f, 0.0f);

    // 앞면 테두리
    glBegin(GL_LINE_LOOP);
    glVertex3f(-0.9, -1.0, 0.7);
    glVertex3f(0.9, -1.0, 0.7);
    glVertex3f(0.9, 1.0, 0.7);
    glVertex3f(-0.9, 1.0, 0.7);
    glEnd();

    // 아랫면
    glBegin(GL_LINE_LOOP);
    glVertex3f(-0.9, -1.0, -0.7);
    glVertex3f(0.9, -1.0, -0.7);
    glVertex3f(0.9, -1.0, 0.7);
    glVertex3f(-0.9, -1.0, 0.7);
    glEnd();

    // 오른쪽 면
    glBegin(GL_LINE_STRIP);
    glVertex3f(0.9, -1.0, -0.7);
    glVertex3f(0.9, -1.0, 0.7);
    glVertex3f(0.9, 1.0, 0.7);
    glVertex3f(0.9, 1.0, -0.7);
    glEnd();

    // 왼쪽 면 테두리
    glBegin(GL_LINE_LOOP);
    glVertex3f(-0.9, -1.0, -0.7);
    glVertex3f(-0.9, -1.0, 0.7);
    glVertex3f(-0.9, 1.0, 0.7);
    glVertex3f(-0.9, 1.0, -0.7);
    glEnd();

    // 윗면 테두리
    glBegin(GL_LINE_LOOP);
    glVertex3f(-0.9, 1.0, -0.7);
    glVertex3f(0.9, 1.0, -0.7);
    glVertex3f(0.9, 1.0, 0.7);
    glVertex3f(-0.9, 1.0, 0.7);
    glEnd();

    if (!isPowerOn) {
        glColor3f(0.3f, 0.3f, 0.3f); // 전원이 꺼진 상태에서는 검정색
    }
    else {
        glColor3f(0.8f, 0.95f, 0.9f); // 전원이 켜지면 원래 얼굴 색상
    }

    glBegin(GL_POLYGON);
    glVertex3f(-0.75, 0.0, 0.701);
    glVertex3f(0.75, 0.0, 0.701);
    glVertex3f(0.75, 0.8, 0.701);
    glVertex3f(-0.75, 0.8, 0.701);
    glEnd();

    glColor3f(0.0f, 0.0f, 0.0f);
    glBegin(GL_LINE_LOOP);
    glVertex3f(-0.75, 0.0, 0.701);
    glVertex3f(0.75, 0.0, 0.701);
    glVertex3f(0.75, 0.8, 0.701);
    glVertex3f(-0.75, 0.8, 0.701);
    glEnd();

    if (isPowerOn) {
        drawFace();
    }

    drawTextWithLines(); // 텍스트 그리기
    drawButtons();       // 버튼 그리기
    drawArm(-1.0f, -0.8f, 0.0f, true);  // 왼쪽 팔
    drawArm(1.0f, -0.8f, 0.0f, false);  // 오른쪽 팔
    drawThickLeg(-0.3f, -1.3f, 0.0f, true);  // 왼쪽 다리
    drawThickLeg(0.3f, -1.3f, 0.0f, false);  // 오른쪽 다리
}

void handleTextAnimation(bool isExpanding) {
    if (isExpanding) {  // 확대
        textScale += 0.05f; // 크기 증가 속도 조절
        textPositionX += 0.07f; // X 방향으로 대각선 이동
        textPositionY += 0.01f; // Y 방향으로 대각선 이동
        if (textScale >= 5.0f) {  // 최대 크기에 도달 시 확대 멈춤
            textScale = 5.0f;
        }
    }
    else {  // 축소
        if (textScale > 1.0f) {  // 최소 크기에 도달할 때까지만 축소
            textScale -= 0.05f; // 크기 감소 속도 조절
            textPositionX -= 0.07f; // X 방향으로 대각선 복귀
            textPositionY -= 0.01f; // Y 방향으로 대각선 복귀
            if (textScale <= 1.0f) {  // 원위치 도달 시 멈춤
                textScale = 1.0f;
                textPositionX = 0.08f;  // 초기 위치로 설정
                textPositionY = 0.08f;
            }
        }
    }
    glutPostRedisplay(); // 화면 갱신
}


void display(void) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glPushMatrix();

    // 기본 3D 각도로 캐릭터를 회전시켜 화면에서 기울어진 각도로 보이게 함
    glRotatef(10.0, 1.0, 0.0, 0.0);
    glRotatef(20.0, 0.0, 1.0, 0.0);

    // 캐릭터의 이동 위치 적용
    glTranslatef(characterPosX, 0.0, 0.0);

    // 이동 방향에 따라 회전 적용
    glRotatef(characterRotation, 0.0, 1.0, 0.0);

    // r 키로 인한 제자리 회전 적용
    glRotatef(rotationAngle, 0.0, 1.0, 0.0);

    glScalef(0.2, 0.4, 0.2); // 캐릭터 크기 설정
    drawBox(); // 캐릭터 그리기

    glPopMatrix();
    glFlush();

    // 화면을 갱신하여 애니메이션 적용
    glutPostRedisplay();
}

// idle 함수
void idle() {
 

    // 텍스트 회전 애니메이션
    if (isTextRotating) {
        textRotationAngle += 0.9f; // 회전 속도 조절
        if (textRotationAngle >= 360.0f) {
            textRotationAngle -= 360.0f; // 0-360도 사이로 유지
        }
    }
    // 전원을 키거나 끄는 함수 
    if (isPowerButtonRotating) {
        buttonRotationAngle += 0.7f; // 회전 속도 조정
        if (buttonRotationAngle >= 1080.0f) {
            buttonRotationAngle = 0.0f;
            isPowerButtonRotating = false; // 버튼 회전 종료
            isPowerOn = !isPowerOn; // 전원 상태 토글
        }
    }
    if (isRotating) {
        // r 키로 인해 계속 회전하는 애니메이션
        rotationAngle += 1.0f;
        if (rotationAngle >= 360.0f) {
            rotationAngle -= 360.0f;  // 0-360도 사이로 유지
        }
    }
    else if (isRotatingBack) {
        // 원위치로 돌아가는 애니메이션 (0도까지)
        if (rotationAngle > 0.0f) {
            rotationAngle -= 1.0;
            if (rotationAngle <= 0.0f) {
                rotationAngle = 0.0f;
                isRotatingBack = false;  // 원위치 도달 시 멈춤
            }
        }
        else if (rotationAngle < 0.0f) {
            rotationAngle += 1.0f;
            if (rotationAngle >= 0.0f) {
                rotationAngle = 0.0f;
                isRotatingBack = false;
            }
        }
    }
    // 바운스 애니메이션
    if (isBouncing) {
        if (bounceDirectionDown) {
            baseLineBouncePosition -= 0.01f; // 아래로 이동
            if (baseLineBouncePosition <= bounceRange) { // 최대 하강 범위 도달 시 방향 전환
                bounceDirectionDown = false;
            }
        }
        else {
            baseLineBouncePosition += 0.01f; // 위로 이동
            if (baseLineBouncePosition >= 0.0f) { // 원래 위치 도달 시 방향 전환
                bounceDirectionDown = true;
            }
        }
    }
    else {
        // 원위치로 돌아가기
        if (baseLineBouncePosition < 0.0f) {
            baseLineBouncePosition += 0.01f;
            if (baseLineBouncePosition > 0.0f) {
                baseLineBouncePosition = 0.0f; // 정확히 원위치로 고정
            }
        }
    }

    // 손 인사 애니메이션
    if (isHandWaving) {
        if (handWaveDirection) {
            handWaveAngle += 0.3f; // 팔을 올리는 속도
            if (handWaveAngle >= 180.0f) { // 180도에 도달하면 방향 전환
                handWaveDirection = false;
            }
        }
        else {
            handWaveAngle -= 0.3f; // 팔을 내리는 속도
            if (handWaveAngle <= 110.0f) { // 0도에 도달하면 방향 전환
                handWaveDirection = true;
                handWaveCycles++;
            }
        }
        if (handWaveCycles >= maxHandWaveCycles) {
            isHandWaving = false;
            handWaveAngle = 0.0f;
            handWaveCycles = 0;
        }
    }

    // 걷기 애니메이션
    if (isWalking) {
        handleWalkAnimation();
    }

    if (isButtonRotating) {
        buttonRotationAngle += 1.0f;
        if (buttonRotationAngle >= 720.0f) {
            buttonRotationAngle = 0.0f;
            isButtonRotating = false;
            buttonRotationComplete = true;
        }
    }

    if (buttonRotationComplete) {
        isDisplayingText = !isDisplayingText;
        buttonRotationComplete = false;
    }


    glutPostRedisplay();
}



void scale(char mode)
{
	switch (mode) {
	case 'e': glScalef(1.2, 1.2, 1.2); break; // 확대
	case 'q': glScalef(0.8, 0.8, 0.8); break; // 축소
	default: break;
	}
}

void translate(char direction)
{
	switch (direction) {
	case 'd': glTranslatef(0.05, 0.0, 0.0); break;
	case 'a': glTranslatef(-0.05, 0.0, 0.0); break;
    case 'w': glTranslatef(0.0, 0.05, 0.0); break;
    case 's': glTranslatef(0.0, -0.05, 0.0); break;
	default: break;
	}
}


void keyboard(unsigned char key, int x, int y)
{
	switch (key)
	{
    case 'p': // 전원 버튼 토글
        if (!isPowerButtonRotating) {
            isPowerButtonRotating = true; // 삼각형 버튼 회전 시작
            glutIdleFunc(idle); // idle 함수 활성화
        }
        break;
   
   // "ㄴ" 수직 선 위로만 확대/축소
    case '1':
        verticalLineScale += 0.5f;
        glutPostRedisplay();
        break;
    case '2':
        if (verticalLineScale > 0.1f) // 최소 스케일 제한
            verticalLineScale -= 0.5f;
        glutPostRedisplay();
        break;

        // "ㄴ" 수평 선 오른쪽으로만 확대/축소
    case '3':
        horizontalLineScale += 0.5f;
        glutPostRedisplay();
        break;
    case '4':
        if (horizontalLineScale > 0.1f) // 최소 스케일 제한
            horizontalLineScale -= 0.5f;
        glutPostRedisplay();
        break;


    // 교차점 확대/축소 (구)
    case '5': intersectionScale += 0.5f; glutPostRedisplay(); break;
    case '6': intersectionScale -= 0.5f; glutPostRedisplay(); break;

    // 받침 선 확대/축소
    case '7': baseLineScale += 0.5f; glutPostRedisplay(); break;
    case '8': baseLineScale -= 0.5f; glutPostRedisplay(); break;

	case 'q': scale('q'); glutPostRedisplay(); break; 
	case 'e': scale('e'); glutPostRedisplay(); break; 
	case 'd': translate('d'); glutPostRedisplay(); break; 
	case 'a': translate('a'); glutPostRedisplay(); break;
    case 'w': translate('w'); glutPostRedisplay(); break;
    case 's': translate('s'); glutPostRedisplay(); break;
    case 'r':
        if (isRotating) {
            // 회전 중인 경우 원위치로 돌아가도록 설정
            isRotating = false;
            isRotatingBack = true;
        }
        else if (!isRotatingBack) {
            // 멈춘 상태라면 회전 시작
            isRotating = true;
        }
        glutIdleFunc(idle);  // idle 함수 호출로 애니메이션 처리
        break;
    case 'b':
        isSmiling = !isSmiling; // 웃는 상태 반전
        glutPostRedisplay();
        break;
        // keyboard 함수 내 'n' 키 입력 처리
    case 'n':
        if (!isButtonRotating) { // 버튼 회전 중이 아닐 때만
            isButtonRotating = true;
            buttonRotationComplete = false;
            glutIdleFunc(idle); // idle 함수 실행
        }
        break;
    case 't':
        isTextRotating = !isTextRotating; // t 키를 눌러 회전 여부 토글
        glutIdleFunc(idle); // 애니메이션 활성화
        break;
  
        // c 키로 "ㄴ" 모양을 위로 이동
    case 'c':
        isBouncing = !isBouncing; // z 키로 바운스 애니메이션 토글
        glutIdleFunc(idle);       // idle 함수 활성화
        break;
    case 'z':
        lShapeVerticalPositionY += 0.06f; // 수직 선의 Y 위치 이동
        lShapeHorizontalPositionY += 0.06f; // 수평 선의 Y 위치 이동
        glutPostRedisplay();
        break;

        // x 키로 "ㄴ" 모양을 아래로 이동
    case 'x':
        lShapeVerticalPositionY -= 0.06f; // 수직 선의 Y 위치 이동
        lShapeHorizontalPositionY -= 0.06f; // 수평 선의 Y 위치 이동
        glutPostRedisplay();
        break;
        // "ㄴ" 수직 선 위로만 확대/축소
    case 'k': // 확대
        handleTextAnimation(true); // 확대 모드
        break;

    case 'm': // 축소
        handleTextAnimation(false); // 축소 모드
        break;

    case 'l': // 걷기 애니메이션 시작/멈춤
        isWalking = !isWalking;  // 걷기 애니메이션 상태 토글
        glutPostRedisplay(); // 화면 갱신 요청
        break;

    case 'o': // 걷는 방향 전환
        movingRight = !movingRight;  // 이동 방향을 반대로 전환
        break;

    case 'h':
        isHandWaving = true; // H 키를 눌러 인사 애니메이션 시작
        handWaveDirection = true; // 처음엔 팔을 들어 올리도록 설정
        break;
        // keyboard 함수 내에 'z' 키에 대한 처리 추가
   
    case 'g': // "노" 위로 이동
        textPositionY += 0.3f; // 원하는 만큼 위로 이동 (값 조절 가능)
        glutPostRedisplay();
        break;

    case 'v': // "노" 아래로 이동
        textPositionY -= 0.3f; // 원하는 만큼 아래로 이동 (값 조절 가능)
        glutPostRedisplay();
        break;
  
        
    default: break;
    }
	
}

int main(int argc, char** argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH | GLUT_SINGLE);
    glutCreateWindow("중간 과제");

    glClearColor(1.0, 1.0, 1.0, 0.0);
    glutDisplayFunc(display);
    glutKeyboardFunc(keyboard);

    
    glutIdleFunc(idle);  // 애니메이션을 위한 idle 함수 설정

    glutMainLoop();
    return 0;
}
