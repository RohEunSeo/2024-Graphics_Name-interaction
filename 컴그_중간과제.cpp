#include <windows.h>
#include <gl/gl.h>
#include <gl/glu.h>
#include <math.h> 
#include <glut.h>
#include <string>
using namespace std;

#define PI 3.141592
bool isSmiling = true; // �ʱⰪ�� ���� ǥ��
bool isDisplayingText = false; // �ؽ�Ʈ "��" ǥ�� ����
float textScale = 1.0f;   // "��" ������ ũ��
float textPositionX = 0.08f; // "��" ������ X ��ġ
float textPositionY = 0.08f; // "��" ������ Y ��ġ
bool textMovingOutward = true; // "��" ������ �̵� ���� (�ȿ��� ������)

bool isRotating = false;          // ��ü�� ȸ�� ������ ��Ÿ���� ����
bool isRotatingBack = false;      // ��ü�� ���� ��ġ�� ���ư��� ������ Ȯ���ϴ� ����
float rotationAngle = 0.0f;       // ���� ȸ�� ����
float targetRotation = 0.0f;      // ����ġ�� ���ư� �� ��ǥ ����

bool isWalking = false; // �ȱ� �ִϸ��̼� Ȱ��ȭ ����

float characterPosX = 0.0f;
float characterSpeed = 0.0002f; // �̵� �ӵ� ����
float movementRange = 0.6f;      // �̵� ���� ����
bool movingRight = true;         // ĳ���� �̵� ����
float characterRotation = 0.0f;  // ĳ���� ���� ���� (ȸ�� ����)

float shoulderAngle = 0.0f;
float legAngle = 0.0f;
bool armDirection = true;
bool legDirection = true;

bool isHandWaving = false; // �� �λ� ���� Ȱ��ȭ ����
float handWaveAngle = 0.0f; // �� �λ� ���� �ʱⰪ
bool handWaveDirection = true; // ���� �ö󰡴� ����
int handWaveCycles = 0; // �λ� Ƚ��
const int maxHandWaveCycles = 5; // �λ� �ݺ� �ִ� Ƚ��

bool isPowerOn = false; // ������ ���� ���� ����
bool isPowerButtonRotating = false; // �Ķ��� �ﰢ�� ��ư ȸ�� ����

// �߰��� ����
bool isTextRotating = false; // "��" ������ ȸ�� ����
float textRotationAngle = 0.0f; // "��" ������ ȸ�� ����

// "��" ����� ���� �� ���� ���� Y ��ġ�� �����ϴ� ���� �߰�
float lShapeVerticalPositionY = 0.0f; // ���� ���� Y ��ġ
float lShapeHorizontalPositionY = 0.0f; // ���� ���� Y ��ġ

// �ٿ �ִϸ��̼� ���¿� ��ġ ����
bool isBouncing = false;             // �ٿ �ִϸ��̼� ����
float baseLineBouncePosition = 0.0f;  // ����� Y ��ġ
bool bounceDirectionDown = true;      // �ٿ ���� (�Ʒ��θ� �̵�)
const float bounceRange = -0.05f;     // �ٿ �ִ� �ϰ� ����

// �� �κ��� scale ���� �߰�
float verticalLineScale = 1.0f;
float horizontalLineScale = 1.0f;
float intersectionScale = 1.0f;
float baseLineScale = 1.0f;

bool isButtonRotating = false;      // ��ư ȸ�� �ִϸ��̼� ����
float buttonRotationAngle = 0.0f;   // ��ư ȸ�� ����
bool buttonRotationComplete = false; // ��ư ȸ�� �Ϸ� ����




// �� �׸��� 
void drawCircle(float x, float y, float z, float radius) {
    int numSegments = 100;  // ���׸�Ʈ ���� �������� ���� �������
    glBegin(GL_TRIANGLE_FAN);
    glVertex3f(x, y, z);  // ���� �߽�
    for (int i = 0; i <= numSegments; i++) {
        float angle = 2.0f * PI * i / numSegments;
        float dx = cos(angle) * radius;
        float dy = sin(angle) * radius;
        glVertex3f(x + dx, y + dy, z);
    }
    glEnd();
}




// �Բ��� ���� 
void drawMouth() {
    glColor3f(0.0f, 0.0f, 0.0f);  // ���������� ����
    glLineWidth(2.5f);

    glBegin(GL_LINE_STRIP);

    // ���� ǥ�� �Ǵ� ���� ǥ���� �׸��� ���� ����
    if (isSmiling) {
        // ���� �� ���
        for (int i = 0; i <= 20; i++) {
            float angle = PI * (i / 20.0f);  // 0���� PI������ ���� (�ݿ� ����)
            float x = cos(angle) * 0.14f;    // ���� �ʺ� ����
            float y = sin(angle) * -0.07f;   // ��� ���� ����
            glVertex3f(x, y + 0.4f, 0.701f); // �߾� ���ʿ� ��ġ
        }
    }
    else {
        // ���� �� ���
        for (int i = 0; i <= 20; i++) {
            float angle = PI * (i / 20.0f);  // 0���� PI������ ���� (�ݿ� ����)
            float x = cos(angle) * 0.14f;
            float y = sin(angle) * 0.07f;    // ���� �Ʒ��� ���������� ����
            glVertex3f(x, y + 0.3f, 0.701f); // �߾� �Ʒ��ʿ� ��ġ
        }
    }

    glEnd();

    glLineWidth(1.0f); // �⺻ �β��� ����
}

// �� �׸���
void drawFace() {
    if (isPowerOn && !isDisplayingText) { // ������ ���� ��쿡�� �� ǥ��
        glColor3f(0.0f, 0.0f, 0.0f);
        drawCircle(-0.35f, 0.5f, 0.701f, 0.04f);  // ���� ��
        drawCircle(0.35f, 0.5f, 0.701f, 0.04f);   // ������ ��
        drawMouth(); // �� �׸���
    }
}


void drawTextWithLines() {
    if (isDisplayingText) {
        glPushMatrix();

        glTranslatef(textPositionX, textPositionY, 0.0f); // Y �� ������ ����
        glScalef(textScale, textScale, 1.0f); // ��ü ũ�� ����

        if (isTextRotating) {
            glRotatef(textRotationAngle, 0.0f, 1.0f, 0.0f); // Z�� ���� ȸ��
        }
        // 1. ���� �� (���� ���� ���� �κ�) - �Ʒ��� ����, ���θ� Ȯ��
        glPushMatrix();
        glColor3f(1.0f, 0.6f, 0.6f); // ���� ������ ����
        glTranslatef(-0.15f, 0.45f + lShapeVerticalPositionY + (0.125f * (verticalLineScale - 1.0f)), 0.701f); // ���� ��ġ ����
        glScalef(0.05f, 0.25f * verticalLineScale, 0.5f); // y�� �������θ� ũ�� Ȯ��
        glutSolidCube(1.0f); // ä���� ť��
        glColor3f(0.0f, 0.0f, 0.0f); // �׵θ� ���� (����)
        glutWireCube(1.0f); // �׵θ� ť��
        glPopMatrix();

        // 2. ���� �� (���� ���κ�) - ���� ����, ���������θ� Ȯ��
        glPushMatrix();
        glColor3f(1.0f, 1.0f, 0.6f); // ���� ����� ����
        glTranslatef(0.0f + (0.15f * (horizontalLineScale - 1.0f)), 0.35f + lShapeHorizontalPositionY, 0.701f); // ���� ��ġ ����
        glScalef(0.3f * horizontalLineScale, 0.05f, 0.5f); // x�� �������θ� ũ�� Ȯ��
        glutSolidCube(1.0f); // ä���� ť��
        glColor3f(0.0f, 0.0f, 0.0f); // �׵θ� ���� (����)
        glutWireCube(1.0f); // �׵θ� ť��
        glPopMatrix();


        // 3. ������ (���� ���� �κ�)
        glPushMatrix();
        glColor3f(0.6f, 1.0f, 0.6f); // ���� �ʷϻ� ����
        glTranslatef(0.0f, 0.23f, 0.701f); // ��ġ ����
        glScalef(intersectionScale, intersectionScale, intersectionScale * 2.0f); // �� ���� ���� (z�� ������ ũ�� ����)
        glutSolidSphere(0.06f, 8, 8); // ä���� ��
        glColor3f(0.0f, 0.0f, 0.0f); // �׵θ� ���� (����)
        glutWireSphere(0.06f, 8, 8); // �׵θ� ��
        glPopMatrix();

        // 4. ��ħ �� (���� ��ħ �κ� - ����� �׵θ���)
        glPushMatrix();
        glColor3f(0.0f, 0.5f, 1.0f); // �Ķ��� ���� (�׵θ�)
        glTranslatef(-0.2f, 0.1f + baseLineBouncePosition, 0.5f); // �ٿ ��ġ ����
        glRotatef(60.0f, 0.0f, 1.0f, 0.0f); // y�� �������� ȸ���Ͽ� ���� �������� ����
        glRotatef(15.0f, 0.0f, 0.0f, 1.0f); // z�� �������� �ణ ����̱�

        // ũ�� ���� �߰�
        glScalef(baseLineScale, baseLineScale, baseLineScale); // ��ü ũ�� ����

        GLUquadric* quadric = gluNewQuadric();
        gluQuadricDrawStyle(quadric, GLU_LINE); // �׵θ��� ���̵��� ����

        // ����� ��ü
        gluCylinder(quadric, 0.05f, 0.05f, 0.5f, 20, 20); // ����� ���� (�⺻ ũ��)

        gluDeleteQuadric(quadric); // quadric ��ü ����
        glPopMatrix();

        glPopMatrix();
    }
}



// ���ӱ� ��ư �׸���
void drawButtons() {
    glColor3f(0.0f, 0.0f, 0.0f);
    glBegin(GL_POLYGON);
    glVertex3f(-0.7f, -0.1f, 0.702f);
    glVertex3f(0.2f, -0.1f, 0.702f);
    glVertex3f(0.2f, -0.15f, 0.702f);
    glVertex3f(-0.7f, -0.15f, 0.702f);
    glEnd();

    // ��� ���� ���� ��ư
    glColor3f(0.0f, 0.0f, 0.0f);
    drawCircle(0.55f, -0.13f, 0.702f, 0.045f);

    // ����� ���ڰ� ��ư ȸ�� �ִϸ��̼�
    glPushMatrix();
    if (isButtonRotating) {
        glTranslatef(-0.4f, -0.45f, 0.0f); // ���ڰ� ��ư �߽����� �̵�
        glRotatef(buttonRotationAngle, 0.0f, 0.0f, 1.0f); // Z���� �������� ȸ��
        glTranslatef(0.4f, 0.45f, 0.0f); // ���� ��ġ�� ����
    }

    // ����� ���ڰ� ��ư
    glColor3f(1.0f, 0.9f, 0.2f); // ����� ����
    glBegin(GL_POLYGON);

    // ���� �κ�
    glVertex3f(-0.45f, -0.5f, 0.702f);
    glVertex3f(-0.55f, -0.5f, 0.702f);
    glVertex3f(-0.55f, -0.4f, 0.702f);
    glVertex3f(-0.45f, -0.4f, 0.702f);

    // ���κ�
    glVertex3f(-0.45f, -0.4f, 0.702f);
    glVertex3f(-0.45f, -0.3f, 0.702f);
    glVertex3f(-0.35f, -0.3f, 0.702f);
    glVertex3f(-0.35f, -0.4f, 0.702f);

    // ������ �κ�
    glVertex3f(-0.35f, -0.4f, 0.702f);
    glVertex3f(-0.25f, -0.4f, 0.702f);
    glVertex3f(-0.25f, -0.5f, 0.702f);
    glVertex3f(-0.35f, -0.5f, 0.702f);

    // �Ʒ��κ�
    glVertex3f(-0.35f, -0.5f, 0.702f);
    glVertex3f(-0.35f, -0.6f, 0.702f);
    glVertex3f(-0.45f, -0.6f, 0.702f);
    glVertex3f(-0.45f, -0.5f, 0.702f);

    glEnd();
    glPopMatrix(); // ���ڰ� ��ư ȸ�� �Ϸ� �� ��ȯ �ʱ�ȭ


    // �Ķ��� �ﰢ�� ��ư ȸ�� ó��
    glPushMatrix();
    if (isPowerButtonRotating) {
        glTranslatef(0.25f, -0.4f, 0.0f); // �ﰢ�� �߽����� �̵�
        glRotatef(buttonRotationAngle, 0.0f, 0.0f, 1.0f); // ȸ��
        glTranslatef(-0.25f, 0.4f, 0.0f); // ���� ��ġ�� ����
    }
    glColor3f(0.0f, 0.7f, 1.0f); // �Ķ��� ����
    glBegin(GL_TRIANGLES);
    glVertex3f(0.1f, -0.5f, 0.702f);
    glVertex3f(0.4f, -0.5f, 0.702f);
    glVertex3f(0.25f, -0.3f, 0.702f);
    glEnd();
    glPopMatrix(); // �Ķ��� �ﰢ�� ��ư ȸ�� ��

    // ������ ���� ��ư
    glColor3f(1.0f, 0.2f, 0.2f);
    drawCircle(0.4f, -0.75f, 0.702f, 0.14f);

    // �ʷϻ� ���� ��ư
    glColor3f(0.5f, 1.0f, 0.5f);
    drawCircle(0.6f, -0.45f, 0.702f, 0.06f);

    // ���� �Ʒ� �Ķ��� ���� ��
    glColor3f(0.0f, 0.0f, 1.0f);
    glBegin(GL_POLYGON);
    glVertex3f(-0.6f, -0.85f, 0.702f);
    glVertex3f(-0.4f, -0.85f, 0.702f);
    glVertex3f(-0.4f, -0.89f, 0.702f);
    glVertex3f(-0.6f, -0.89f, 0.702f);
    glEnd();

    // ������ �Ʒ� �Ķ��� ���� ��
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

    // �� ������ ����
    if (armDirection) {
        shoulderAngle += speed;
        if (shoulderAngle >= 30.0f) armDirection = false;
    }
    else {
        shoulderAngle -= speed;
        if (shoulderAngle <= -30.0f) armDirection = true;
    }

    // �ٸ� ������ ����
    if (legDirection) {
        legAngle += speed;
        if (legAngle >= 30.0f) legDirection = false;
    }
    else {
        legAngle -= speed;
        if (legAngle <= -30.0f) legDirection = true;
    }

    // ĳ���� �̵� ����
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
    glTranslatef(x, y + 0.425f, z); // ���� ��ġ�� �̵�

    if (isLeft && isHandWaving) {
        // ���� Z�� �������� ȸ������ ������ �����̵��� ����
        glRotatef(handWaveAngle, 0.5f, 0.1f, -1.0f); // (������ ȸ��)
    }
    else {
        glRotatef(isLeft ? -shoulderAngle : shoulderAngle, 1.0f, 0.0f, 0.0f); // ���� ��� ȸ�� ����
    }

    glTranslatef(0.0f, -0.425f, 0.0f); // ���� ���������� �̵�
    glScalef(0.2f, 0.85f, 0.2f);       // ���� ũ�� ����
    glColor3f(0.4f, 0.85f, 0.8f);      // ���� ���� ����
    glutSolidCube(1.0f);               // ä���� �� �׸���
    glColor3f(0.0f, 0.0f, 0.0f);       // �׵θ� ���� ����
    glutWireCube(1.0f);                // ���� �׵θ� �׸���
    glPopMatrix();
}


// �ٸ� �׸��� �Լ� ���� - ���� ���� �������� ȸ��
void drawThickLeg(float x, float y, float z, bool isLeft) {
    glPushMatrix();

    // �ٸ��� ���� ������ �̵��Ͽ� ȸ�� ���� ����
    glTranslatef(x, y + 0.3f, z); // �ٸ��� ���� ������ �̵�
    glRotatef(isLeft ? legAngle : -legAngle, 1.0f, 0.0f, 0.0f); // ȸ�� ����
    glTranslatef(0.0f, -0.3f, 0.0f); // �ٸ��� ������ �κ����� �̵�

    // �ٸ� �׸���
    glScalef(0.2f, 0.6f, 0.2f);
    glColor3f(0.4f, 0.85f, 0.8f);
    glutSolidCube(1.0f);
    glColor3f(0.0f, 0.0f, 0.0f);
    glutWireCube(1.0f);
    glPopMatrix();
}


// ���ӱ� ���� �׸��� 
void drawBox()
{
    // ���� ��Ʈ�� �ո� ����
    glColor3f(0.4f, 0.8f, 0.7f);

    // �ո� (���� ��Ʈ��)
    glBegin(GL_POLYGON);
    glVertex3f(-0.9, -1.0, 0.7);
    glVertex3f(0.9, -1.0, 0.7);
    glVertex3f(0.9, 1.0, 0.7);
    glVertex3f(-0.9, 1.0, 0.7);
    glEnd();

    // �޸�
    glBegin(GL_POLYGON);
    glVertex3f(-0.9, -1.0, -0.7);
    glVertex3f(0.9, -1.0, -0.7);
    glVertex3f(0.9, 1.0, -0.7);
    glVertex3f(-0.9, 1.0, -0.7);
    glEnd();

    // ���� ��
    glBegin(GL_POLYGON);
    glVertex3f(-0.9, -1.0, -0.7);
    glVertex3f(-0.9, -1.0, 0.7);
    glVertex3f(-0.9, 1.0, 0.7);
    glVertex3f(-0.9, 1.0, -0.7);
    glEnd();

    // ������ ��
    glBegin(GL_POLYGON);
    glVertex3f(0.9, -1.0, -0.7);
    glVertex3f(0.9, -1.0, 0.7);
    glVertex3f(0.9, 1.0, 0.7);
    glVertex3f(0.9, 1.0, -0.7);
    glEnd();

    // ����
    glBegin(GL_POLYGON);
    glVertex3f(-0.9, 1.0, -0.7);
    glVertex3f(0.9, 1.0, -0.7);
    glVertex3f(0.9, 1.0, 0.7);
    glVertex3f(-0.9, 1.0, 0.7);
    glEnd();

    // �Ʒ���
    glBegin(GL_POLYGON);
    glVertex3f(-0.9, -1.0, -0.7);
    glVertex3f(0.9, -1.0, -0.7);
    glVertex3f(0.9, -1.0, 0.7);
    glVertex3f(-0.9, -1.0, 0.7);
    glEnd();

    // ������ �׵θ� �׸��� (�� ���� �ܰ�����)
    glColor3f(0.0f, 0.0f, 0.0f);

    // �ո� �׵θ�
    glBegin(GL_LINE_LOOP);
    glVertex3f(-0.9, -1.0, 0.7);
    glVertex3f(0.9, -1.0, 0.7);
    glVertex3f(0.9, 1.0, 0.7);
    glVertex3f(-0.9, 1.0, 0.7);
    glEnd();

    // �Ʒ���
    glBegin(GL_LINE_LOOP);
    glVertex3f(-0.9, -1.0, -0.7);
    glVertex3f(0.9, -1.0, -0.7);
    glVertex3f(0.9, -1.0, 0.7);
    glVertex3f(-0.9, -1.0, 0.7);
    glEnd();

    // ������ ��
    glBegin(GL_LINE_STRIP);
    glVertex3f(0.9, -1.0, -0.7);
    glVertex3f(0.9, -1.0, 0.7);
    glVertex3f(0.9, 1.0, 0.7);
    glVertex3f(0.9, 1.0, -0.7);
    glEnd();

    // ���� �� �׵θ�
    glBegin(GL_LINE_LOOP);
    glVertex3f(-0.9, -1.0, -0.7);
    glVertex3f(-0.9, -1.0, 0.7);
    glVertex3f(-0.9, 1.0, 0.7);
    glVertex3f(-0.9, 1.0, -0.7);
    glEnd();

    // ���� �׵θ�
    glBegin(GL_LINE_LOOP);
    glVertex3f(-0.9, 1.0, -0.7);
    glVertex3f(0.9, 1.0, -0.7);
    glVertex3f(0.9, 1.0, 0.7);
    glVertex3f(-0.9, 1.0, 0.7);
    glEnd();

    if (!isPowerOn) {
        glColor3f(0.3f, 0.3f, 0.3f); // ������ ���� ���¿����� ������
    }
    else {
        glColor3f(0.8f, 0.95f, 0.9f); // ������ ������ ���� �� ����
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

    drawTextWithLines(); // �ؽ�Ʈ �׸���
    drawButtons();       // ��ư �׸���
    drawArm(-1.0f, -0.8f, 0.0f, true);  // ���� ��
    drawArm(1.0f, -0.8f, 0.0f, false);  // ������ ��
    drawThickLeg(-0.3f, -1.3f, 0.0f, true);  // ���� �ٸ�
    drawThickLeg(0.3f, -1.3f, 0.0f, false);  // ������ �ٸ�
}

void handleTextAnimation(bool isExpanding) {
    if (isExpanding) {  // Ȯ��
        textScale += 0.05f; // ũ�� ���� �ӵ� ����
        textPositionX += 0.07f; // X �������� �밢�� �̵�
        textPositionY += 0.01f; // Y �������� �밢�� �̵�
        if (textScale >= 5.0f) {  // �ִ� ũ�⿡ ���� �� Ȯ�� ����
            textScale = 5.0f;
        }
    }
    else {  // ���
        if (textScale > 1.0f) {  // �ּ� ũ�⿡ ������ �������� ���
            textScale -= 0.05f; // ũ�� ���� �ӵ� ����
            textPositionX -= 0.07f; // X �������� �밢�� ����
            textPositionY -= 0.01f; // Y �������� �밢�� ����
            if (textScale <= 1.0f) {  // ����ġ ���� �� ����
                textScale = 1.0f;
                textPositionX = 0.08f;  // �ʱ� ��ġ�� ����
                textPositionY = 0.08f;
            }
        }
    }
    glutPostRedisplay(); // ȭ�� ����
}


void display(void) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glPushMatrix();

    // �⺻ 3D ������ ĳ���͸� ȸ������ ȭ�鿡�� ������ ������ ���̰� ��
    glRotatef(10.0, 1.0, 0.0, 0.0);
    glRotatef(20.0, 0.0, 1.0, 0.0);

    // ĳ������ �̵� ��ġ ����
    glTranslatef(characterPosX, 0.0, 0.0);

    // �̵� ���⿡ ���� ȸ�� ����
    glRotatef(characterRotation, 0.0, 1.0, 0.0);

    // r Ű�� ���� ���ڸ� ȸ�� ����
    glRotatef(rotationAngle, 0.0, 1.0, 0.0);

    glScalef(0.2, 0.4, 0.2); // ĳ���� ũ�� ����
    drawBox(); // ĳ���� �׸���

    glPopMatrix();
    glFlush();

    // ȭ���� �����Ͽ� �ִϸ��̼� ����
    glutPostRedisplay();
}

// idle �Լ�
void idle() {
 

    // �ؽ�Ʈ ȸ�� �ִϸ��̼�
    if (isTextRotating) {
        textRotationAngle += 0.9f; // ȸ�� �ӵ� ����
        if (textRotationAngle >= 360.0f) {
            textRotationAngle -= 360.0f; // 0-360�� ���̷� ����
        }
    }
    // ������ Ű�ų� ���� �Լ� 
    if (isPowerButtonRotating) {
        buttonRotationAngle += 0.7f; // ȸ�� �ӵ� ����
        if (buttonRotationAngle >= 1080.0f) {
            buttonRotationAngle = 0.0f;
            isPowerButtonRotating = false; // ��ư ȸ�� ����
            isPowerOn = !isPowerOn; // ���� ���� ���
        }
    }
    if (isRotating) {
        // r Ű�� ���� ��� ȸ���ϴ� �ִϸ��̼�
        rotationAngle += 1.0f;
        if (rotationAngle >= 360.0f) {
            rotationAngle -= 360.0f;  // 0-360�� ���̷� ����
        }
    }
    else if (isRotatingBack) {
        // ����ġ�� ���ư��� �ִϸ��̼� (0������)
        if (rotationAngle > 0.0f) {
            rotationAngle -= 1.0;
            if (rotationAngle <= 0.0f) {
                rotationAngle = 0.0f;
                isRotatingBack = false;  // ����ġ ���� �� ����
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
    // �ٿ �ִϸ��̼�
    if (isBouncing) {
        if (bounceDirectionDown) {
            baseLineBouncePosition -= 0.01f; // �Ʒ��� �̵�
            if (baseLineBouncePosition <= bounceRange) { // �ִ� �ϰ� ���� ���� �� ���� ��ȯ
                bounceDirectionDown = false;
            }
        }
        else {
            baseLineBouncePosition += 0.01f; // ���� �̵�
            if (baseLineBouncePosition >= 0.0f) { // ���� ��ġ ���� �� ���� ��ȯ
                bounceDirectionDown = true;
            }
        }
    }
    else {
        // ����ġ�� ���ư���
        if (baseLineBouncePosition < 0.0f) {
            baseLineBouncePosition += 0.01f;
            if (baseLineBouncePosition > 0.0f) {
                baseLineBouncePosition = 0.0f; // ��Ȯ�� ����ġ�� ����
            }
        }
    }

    // �� �λ� �ִϸ��̼�
    if (isHandWaving) {
        if (handWaveDirection) {
            handWaveAngle += 0.3f; // ���� �ø��� �ӵ�
            if (handWaveAngle >= 180.0f) { // 180���� �����ϸ� ���� ��ȯ
                handWaveDirection = false;
            }
        }
        else {
            handWaveAngle -= 0.3f; // ���� ������ �ӵ�
            if (handWaveAngle <= 110.0f) { // 0���� �����ϸ� ���� ��ȯ
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

    // �ȱ� �ִϸ��̼�
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
	case 'e': glScalef(1.2, 1.2, 1.2); break; // Ȯ��
	case 'q': glScalef(0.8, 0.8, 0.8); break; // ���
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
    case 'p': // ���� ��ư ���
        if (!isPowerButtonRotating) {
            isPowerButtonRotating = true; // �ﰢ�� ��ư ȸ�� ����
            glutIdleFunc(idle); // idle �Լ� Ȱ��ȭ
        }
        break;
   
   // "��" ���� �� ���θ� Ȯ��/���
    case '1':
        verticalLineScale += 0.5f;
        glutPostRedisplay();
        break;
    case '2':
        if (verticalLineScale > 0.1f) // �ּ� ������ ����
            verticalLineScale -= 0.5f;
        glutPostRedisplay();
        break;

        // "��" ���� �� ���������θ� Ȯ��/���
    case '3':
        horizontalLineScale += 0.5f;
        glutPostRedisplay();
        break;
    case '4':
        if (horizontalLineScale > 0.1f) // �ּ� ������ ����
            horizontalLineScale -= 0.5f;
        glutPostRedisplay();
        break;


    // ������ Ȯ��/��� (��)
    case '5': intersectionScale += 0.5f; glutPostRedisplay(); break;
    case '6': intersectionScale -= 0.5f; glutPostRedisplay(); break;

    // ��ħ �� Ȯ��/���
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
            // ȸ�� ���� ��� ����ġ�� ���ư����� ����
            isRotating = false;
            isRotatingBack = true;
        }
        else if (!isRotatingBack) {
            // ���� ���¶�� ȸ�� ����
            isRotating = true;
        }
        glutIdleFunc(idle);  // idle �Լ� ȣ��� �ִϸ��̼� ó��
        break;
    case 'b':
        isSmiling = !isSmiling; // ���� ���� ����
        glutPostRedisplay();
        break;
        // keyboard �Լ� �� 'n' Ű �Է� ó��
    case 'n':
        if (!isButtonRotating) { // ��ư ȸ�� ���� �ƴ� ����
            isButtonRotating = true;
            buttonRotationComplete = false;
            glutIdleFunc(idle); // idle �Լ� ����
        }
        break;
    case 't':
        isTextRotating = !isTextRotating; // t Ű�� ���� ȸ�� ���� ���
        glutIdleFunc(idle); // �ִϸ��̼� Ȱ��ȭ
        break;
  
        // c Ű�� "��" ����� ���� �̵�
    case 'c':
        isBouncing = !isBouncing; // z Ű�� �ٿ �ִϸ��̼� ���
        glutIdleFunc(idle);       // idle �Լ� Ȱ��ȭ
        break;
    case 'z':
        lShapeVerticalPositionY += 0.06f; // ���� ���� Y ��ġ �̵�
        lShapeHorizontalPositionY += 0.06f; // ���� ���� Y ��ġ �̵�
        glutPostRedisplay();
        break;

        // x Ű�� "��" ����� �Ʒ��� �̵�
    case 'x':
        lShapeVerticalPositionY -= 0.06f; // ���� ���� Y ��ġ �̵�
        lShapeHorizontalPositionY -= 0.06f; // ���� ���� Y ��ġ �̵�
        glutPostRedisplay();
        break;
        // "��" ���� �� ���θ� Ȯ��/���
    case 'k': // Ȯ��
        handleTextAnimation(true); // Ȯ�� ���
        break;

    case 'm': // ���
        handleTextAnimation(false); // ��� ���
        break;

    case 'l': // �ȱ� �ִϸ��̼� ����/����
        isWalking = !isWalking;  // �ȱ� �ִϸ��̼� ���� ���
        glutPostRedisplay(); // ȭ�� ���� ��û
        break;

    case 'o': // �ȴ� ���� ��ȯ
        movingRight = !movingRight;  // �̵� ������ �ݴ�� ��ȯ
        break;

    case 'h':
        isHandWaving = true; // H Ű�� ���� �λ� �ִϸ��̼� ����
        handWaveDirection = true; // ó���� ���� ��� �ø����� ����
        break;
        // keyboard �Լ� ���� 'z' Ű�� ���� ó�� �߰�
   
    case 'g': // "��" ���� �̵�
        textPositionY += 0.3f; // ���ϴ� ��ŭ ���� �̵� (�� ���� ����)
        glutPostRedisplay();
        break;

    case 'v': // "��" �Ʒ��� �̵�
        textPositionY -= 0.3f; // ���ϴ� ��ŭ �Ʒ��� �̵� (�� ���� ����)
        glutPostRedisplay();
        break;
  
        
    default: break;
    }
	
}

int main(int argc, char** argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH | GLUT_SINGLE);
    glutCreateWindow("�߰� ����");

    glClearColor(1.0, 1.0, 1.0, 0.0);
    glutDisplayFunc(display);
    glutKeyboardFunc(keyboard);

    
    glutIdleFunc(idle);  // �ִϸ��̼��� ���� idle �Լ� ����

    glutMainLoop();
    return 0;
}
