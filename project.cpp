#include <stdio.h>
#include <array>
#include <iostream>
#include <vector>

#include "FreeGLUT/freeglut.h"
#include "FreeGLUT/glui.h"
#include "FreeGLUT/tinyfiledialogs.h"
#include "FreeGLUT/tinyfiledialogs.c"

#define WORLD_AXES 1
#define COLORCUBE 2
#define DATAMODEL 3
#define DATAMODEL_LINE 4
#define DATAMODEL_MATERIAL 5

typedef struct {
    int v1_id, v2_id, v3_id, fr, fg, fb, br, bg, bb;
} Triangle;
typedef struct {
    float x, y, z, nx, ny, nz;
} Vertex;

std::vector<Triangle> mT;
std::vector<Vertex> mV;

int window;
int width = 1400;
int height = 700;
int dragStart = -1;
int mainMenu;
int submenu;
int teapotMode = 0;
int otherState[] = { 0, 0, 0, 0, 1, 2, 0, 0, 0 }; // scale, animation, material, emission, viewing, shadingType, object, materialType, shininess
int lightState[][3] = { {0, 0, 0}, {0, 0, 0}, {0, 0, 0} };
int lightType[] = { 0, 0, 0 };

GLfloat aspect;
GLfloat bgColor[3] = { 0.0, 0.0, 0.0 };
GLfloat teapotColor[3] = { 1.0, 0.0, 0.0 };
GLfloat teapotRotation = 0.0f;
GLfloat rotationSpeed = 1.0f;
GLfloat tempSpeed = 1.0f;
GLfloat teapotXYZPosition[] = { 0.0f, 0.0f, 0.0f };
GLfloat teapotScaleXYZ[] = { 1.0f, 1.0f, 1.0f };
GLfloat teapotSize = 1.0f;
GLfloat lastScaleXYZ[] = { 1.0f, 1.0f, 1.0f };
GLfloat lightV[3][6] = { {1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f}, {1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f}, {1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f} };
GLfloat materialShininess;
GLfloat teapotShininess = 50.0f;
float modelSizeX = 1.0f;
float modelSizeY = 1.0f;
float modelSizeZ = 1.0f;
float modelScale = 1.0f;
std::array<GLfloat, 3> cubeDiffuse;
std::array<GLfloat, 3> cubeAmbient;
std::array<GLfloat, 3> modelFrontDiffuse;
std::array<GLfloat, 3> modelBackDiffuse;

GLUI* glui;
GLUI_Translation* translationXYZ[3];
GLUI_Translation* scaleXYZ[3];
GLUI_Rotation* teapotR;
GLUI_Rotation* lightR[3];
GLUI_Spinner* lightSP[3][6];
GLUI_Rollout* rollout[3];
GLUI_Checkbox* innerCheckbox[3][2];
GLUI_Checkbox* emissionCheckbox;
GLUI_Checkbox* shadingCheckbox;
GLUI_Checkbox* materialCheckbox;
GLUI_Button* lightButton[3];
GLUI_RadioGroup* shadingRadio1;
GLUI_Panel* shadingInner4;

float teapotRM[16] = {
    1.0, 0.0, 0.0, 0.0,
    0.0, 1.0, 0.0, 0.0,
    0.0, 0.0, 1.0, 0.0,
    0.0, 0.0, 0.0, 1.0 };

float lightRM[][16] = {
    {1.0, 0.0, 0.0, 0.0,
     0.0, 1.0, 0.0, 0.0,
     0.0, 0.0, 1.0, 0.0,
     0.0, 0.0, 0.0, 1.0},
    {1.0, 0.0, 0.0, 0.0,
     0.0, 1.0, 0.0, 0.0,
     0.0, 0.0, 1.0, 0.0,
     0.0, 0.0, 0.0, 1.0},
    {1.0, 0.0, 0.0, 0.0,
     0.0, 1.0, 0.0, 0.0,
     0.0, 0.0, 1.0, 0.0,
     0.0, 0.0, 0.0, 1.0} };

GLfloat CubeVertices[][3] = {
    {-1.0, -1.0, -1.0},
    {1.0, -1.0, -1.0},
    {1.0, 1.0, -1.0},
    {-1.0, 1.0, -1.0},
    {-1.0, -1.0, 1.0},
    {1.0, -1.0, 1.0},
    {1.0, 1.0, 1.0},
    {-1.0, 1.0, 1.0},
};

GLfloat colors[][3] = {
    {0.0, 0.0, 0.0},
    {1.0, 0.0, 0.0},
    {1.0, 1.0, 0.0},
    {0.0, 1.0, 0.0},
    {0.0, 0.0, 1.0},
    {1.0, 0.0, 1.0},
    {1.0, 1.0, 1.0},
    {0.0, 1.0, 1.0},
};

void init(void);
void polygon(int a, int b, int c, int d);
void display(void);
void reshape(int width, int height);
void mouse(int button, int state, int x, int y);
void motion(int x, int y);
void keyboard(unsigned char key, int x, int y);
void specialKeys(int key, int x, int y);
void autoRotateTeapot(int v);
void processMenu(int value);
void processSubmenu(int value);
void idle();
void teapotUI();
void translationCallback(int id);
void buttonCallback(int id);
void checkBoxCallback(int id);
void spinnerCallback(int id);
void radioCallback(int id);
GLUI_Spinner* createSpinner(GLUI_Panel* panel, const char* name, float* value);
GLUI_Translation* createTranslation(GLUI_Panel* panel, const char* name, int type, int id);
void lightControl();
void materialDecision(int id);
void openFile();
void readSimpleFormat(FILE* file);
void createDisplayList(const std::vector<Triangle>& triangles, const std::vector<Vertex>& vertices);

int main(int argc, char* argv[])
{
#ifdef _MSC_VER
    glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE);
#else
    glutInitDisplayMode(GLUT_3_2_CORE_PROFILE | GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE);
#endif
    glutInitWindowSize(width, height);
    glutInitWindowPosition(20, 20);
    glutInit(&argc, argv);
    window = glutCreateWindow("U11116015");
    teapotUI();
    init();
    GLUI_Master.set_glutDisplayFunc(display);
    GLUI_Master.set_glutReshapeFunc(reshape);
    GLUI_Master.set_glutMouseFunc(mouse);
    glutMotionFunc(motion);
    GLUI_Master.set_glutKeyboardFunc(keyboard);
    GLUI_Master.set_glutSpecialFunc(specialKeys);
    GLUI_Master.set_glutTimerFunc(10, autoRotateTeapot, 0);
    GLUI_Master.set_glutIdleFunc(idle);
    mainMenu = glutCreateMenu(processMenu);
    submenu = glutCreateMenu(processSubmenu);
    glutAddMenuEntry("1.0", 1);
    glutAddMenuEntry("1.5", 2);
    glutAddMenuEntry("2.0", 3);
    glutAddMenuEntry("Solid", 4);
    glutAddMenuEntry("Wire", 5);
    glutSetMenu(mainMenu);
    glutAddSubMenu("Teapot size", submenu);
    glutAddMenuEntry("Exit", 1);
    submenu = glutCreateMenu(processSubmenu);
    glutSetMenu(mainMenu);
    glutAttachMenu(GLUT_RIGHT_BUTTON);
    display();
    glutMainLoop();
    return 0;
}

void init(void)
{
    aspect = (GLfloat)width / (GLfloat)height;
    glEnable(GL_DEPTH_TEST);
    glClearColor(0.2, 0.2, 0.2, 0.0);
    openFile();
    glNewList(WORLD_AXES, GL_COMPILE);
    glBegin(GL_LINES);
    glColor3f(1.0, 0.0, 0.0);
    glVertex3f(0.0, 0.0, 0.0);
    glVertex3f(aspect * 4.0, 0.0, 0.0);
    glColor3f(0.0, 1.0, 0.0);
    glVertex3f(0.0, 0.0, 0.0);
    glVertex3f(0.0, aspect * 4.0, 0.0);
    glColor3f(0.0, 0.0, 1.0);
    glVertex3f(0.0, 0.0, 0.0);
    glVertex3f(0.0, 0.0, aspect * 4.0);
    glEnd();
    glEndList();
    glNewList(COLORCUBE, GL_COMPILE);
    polygon(0, 3, 2, 1);
    polygon(2, 3, 7, 6);
    polygon(0, 4, 7, 3);
    polygon(1, 2, 6, 5);
    polygon(4, 5, 6, 7);
    polygon(0, 1, 5, 4);
    glEndList();
}

void polygon(int a, int b, int c, int d)
{
    glBegin(GL_POLYGON);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, colors[a]);
    glMaterialfv(GL_FRONT, GL_AMBIENT, colors[a]);
    glVertex3fv(CubeVertices[a]);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, colors[b]);
    glMaterialfv(GL_FRONT, GL_AMBIENT, colors[b]);
    glVertex3fv(CubeVertices[b]);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, colors[c]);
    glMaterialfv(GL_FRONT, GL_AMBIENT, colors[c]);
    glVertex3fv(CubeVertices[c]);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, colors[d]);
    glMaterialfv(GL_FRONT, GL_AMBIENT, colors[d]);
    glVertex3fv(CubeVertices[d]);
    glEnd();
}

void display(void)
{
    width = glutGet(GLUT_WINDOW_WIDTH);
    height = glutGet(GLUT_WINDOW_HEIGHT);
    aspect = (GLfloat)width / (GLfloat)height;
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(bgColor[0], bgColor[1], bgColor[2], 0.0);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    if (otherState[4])
    {
        gluPerspective(60.0f, aspect, 0.1f, 20.0f);
    }
    else
    {
        float left = -10.0f;
        float right = 10.0f;
        float bottom = -10.0f;
        float top = 10.0f;
        float nearVal = -20.0f;
        float farVal = 20.0f;
        if (aspect > 1.0f) {
            top /= aspect;
            bottom /= aspect;
        }
        else {
            left *= aspect;
            right *= aspect;
        }

        glOrtho(left, right, bottom, top, nearVal, farVal);
    }
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(5.0, 5.0, 5.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
    lightControl();
    glDisable(GL_LIGHTING);
    glCallList(WORLD_AXES);
    glEnable(GL_LIGHTING);
    glTranslatef(teapotXYZPosition[0], teapotXYZPosition[1], teapotXYZPosition[2]);
    glRotatef(teapotRotation, 0.0, 1.0, 0.0);
    glMultMatrixf(teapotRM);
    glDisable(GL_LIGHTING);
    glCallList(WORLD_AXES);
    glEnable(GL_LIGHTING);
    glColor3fv(teapotColor);
    materialDecision(otherState[7]);
    switch (otherState[6])
    {
    case 0:
        glScalef(teapotScaleXYZ[0], teapotScaleXYZ[1], teapotScaleXYZ[2]);
        shadingRadio1->enable();
        if (otherState[5] == 0)
        {
            glutWireTeapot(teapotSize);
        }
        else if (otherState[5] == 1)
        {
            glShadeModel(GL_FLAT);
            glutSolidTeapot(teapotSize);
        }
        else if (otherState[5] == 2)
        {
            glShadeModel(GL_SMOOTH);
            glutSolidTeapot(teapotSize);
        }
        break;
    case 1:
        glScalef(teapotScaleXYZ[0], teapotScaleXYZ[1], teapotScaleXYZ[2]);
        shadingRadio1->disable();
        glShadeModel(GL_SMOOTH);
        if (!otherState[2])
        {
            GLfloat currentDiffuse[4];
            GLfloat currentAmbient[4];
            glGetMaterialfv(GL_FRONT, GL_DIFFUSE, currentDiffuse);
            glGetMaterialfv(GL_FRONT, GL_AMBIENT, currentAmbient);
            glCallList(COLORCUBE);
            glMaterialfv(GL_FRONT, GL_DIFFUSE, currentDiffuse);
            glMaterialfv(GL_FRONT, GL_AMBIENT, currentAmbient);
        }
        else
        {
            
            glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, cubeDiffuse.data());
            glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, cubeAmbient.data());
            glutSolidCube(2.0);
            
        }
        break;
    case 2:
        glScalef(teapotScaleXYZ[0]* modelScale, teapotScaleXYZ[1]* modelScale, teapotScaleXYZ[2]* modelScale);
        shadingRadio1->enable();
        if (otherState[5] == 0)
        {
            glCallList(DATAMODEL_LINE);
        }
        else if (otherState[5] == 1)
        {
            glShadeModel(GL_FLAT);
            otherState[2] ? glCallList(DATAMODEL_MATERIAL) : glCallList(DATAMODEL);
        }
        else if (otherState[5] == 2)
        {
            glShadeModel(GL_SMOOTH);
            otherState[2] ? glCallList(DATAMODEL_MATERIAL) : glCallList(DATAMODEL);
        }
        break;
    }

    glutSwapBuffers();
}

void reshape(int width, int height)
{
    int x = 0, y = 0;
    aspect = width * 1.0f / height;
    GLUI_Master.get_viewport_area(&x, &y, &width, &height);
    glui->refresh();
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glViewport(x, y, width, height);
    gluPerspective(60.0f, aspect, 0.1f, 10.0f);
    glutSwapBuffers();
}

void mouse(int button, int state, int x, int y)
{
    if (button == GLUT_LEFT_BUTTON)
    {
        if (state == GLUT_DOWN)
        {
            dragStart = x;
        }
        else if (state == GLUT_UP)
        {
            dragStart = -1;
        }
    }
}

void motion(int x, int y)
{
    if (dragStart != -1)
    {
        float varX = (float)(x - dragStart) / width;
        bgColor[0] += varX;
        bgColor[1] += varX;
        bgColor[2] += varX;
        for (int i = 0; i < 3; i++)
        {
            if (bgColor[i] > 1.0)
                bgColor[i] = 1.0;
            else if (bgColor[i] < 0.0)
                bgColor[i] = 0.0;
        }
        dragStart = x;
    }
}

void keyboard(unsigned char key, int x, int y)
{
    if (key == 'q' || key == 'Q')
    {
        tempSpeed -= 0.1f;
    }
    else if (key == 'e' || key == 'E')
    {
        tempSpeed += 0.1f;
    }
    else if (key == 'w' || key == 'W')
    {
        teapotXYZPosition[1] += 0.1;
        translationXYZ[1]->set_y(teapotXYZPosition[1]);
    }
    else if (key == 's' || key == 'S')
    {
        teapotXYZPosition[1] -= 0.1;
        translationXYZ[1]->set_y(teapotXYZPosition[1]);
    }
    else if (key == 'a' || key == 'A')
    {
        teapotXYZPosition[0] -= 0.1;
        translationXYZ[0]->set_x(teapotXYZPosition[0]);
    }
    else if (key == 'd' || key == 'D')
    {
        teapotXYZPosition[0] += 0.1;
        translationXYZ[0]->set_x(teapotXYZPosition[0]);
    }

    glutPostRedisplay();
}

void specialKeys(int key, int x, int y)
{
    switch (key)
    {
    case GLUT_KEY_F1:
        teapotColor[0] = 1.0;
        teapotColor[1] = 0.0;
        teapotColor[2] = 0.0;
        break;
    case GLUT_KEY_F2:
        teapotColor[0] = 0.0;
        teapotColor[1] = 1.0;
        teapotColor[2] = 0.0;
        break;
    case GLUT_KEY_F3:
        teapotColor[0] = 0.0;
        teapotColor[1] = 0.0;
        teapotColor[2] = 1.0;
        break;
    }

    glutPostRedisplay();
}

void autoRotateTeapot(int v)
{
    rotationSpeed = otherState[1] ? tempSpeed : 0.0f;
    teapotRotation += rotationSpeed;
    glutPostRedisplay();
    glutTimerFunc(10, autoRotateTeapot, 0);
}

void processMenu(int value)
{
    switch (value)
    {
    case 1:
        exit(0);
        break;
    }
}

void processSubmenu(int value)
{
    switch (value)
    {
    case 1:
        teapotSize = 1.0f;
        break;
    case 2:
        teapotSize = 1.5f;
        break;
    case 3:
        teapotSize = 2.0f;
        break;
    case 4:
        otherState[5] = 2;
        shadingRadio1->set_int_val(2);
        break;
    case 5:
        otherState[5] = 0;
        shadingRadio1->set_int_val(0);
        break;
    }
}

void idle()
{
    glutSetWindow(window);
    glutPostRedisplay();
}

void teapotUI()
{
    const char* sName[] = { "Diffuse: Red", "Diffuse: Green", "Diffuse: Blue", "Specular: Red", "Specular: Green", "Specular: Blue" };
    glui = GLUI_Master.create_glui_subwindow(window, GLUI_SUBWINDOW_RIGHT);
    GLUI_Panel* lightPanel = glui->add_panel("Lighting");
    GLUI_Panel* lightC = glui->add_panel_to_panel(lightPanel, "", GLUI_PANEL_RAISED);
    GLUI_Checkbox* lightCheckbox0 = glui->add_checkbox_to_panel(lightC, "Light 0", &lightState[0][2], 3, checkBoxCallback);
    glui->add_column_to_panel(lightC, false);
    rollout[0] = glui->add_rollout_to_panel(lightPanel, "Light 0", 0);
    GLUI_Panel* innerPanel2 = glui->add_panel_to_panel(rollout[0], "", GLUI_PANEL_NONE);
    innerCheckbox[0][0] = glui->add_checkbox_to_panel(innerPanel2, "Directional", &lightState[0][0], 1, checkBoxCallback);
    innerCheckbox[0][1] = glui->add_checkbox_to_panel(innerPanel2, "Spot", &lightState[0][1], 2, checkBoxCallback);
    lightR[0] = glui->add_rotation_to_panel(innerPanel2, "Rotation", lightRM[0]);
    lightButton[0] = glui->add_button_to_panel(innerPanel2, "Reset Rotation", 3, buttonCallback);
    glui->add_column_to_panel(rollout[0], false);
    GLUI_Panel* innerPanel3 = glui->add_panel_to_panel(rollout[0], "", GLUI_PANEL_NONE);
    for (int i = 0; i < 6; i++)
    {
        lightSP[0][i] = createSpinner(innerPanel3, sName[i], &lightV[0][i]);
    }
    rollout[0]->disable();
    GLUI_Checkbox* lightCheckbox1 = glui->add_checkbox_to_panel(lightC, "Light 1", &lightState[1][2], 6, checkBoxCallback);
    glui->add_column_to_panel(lightC, false);
    rollout[1] = glui->add_rollout_to_panel(lightPanel, "Light 1", 0);
    GLUI_Panel* innerPanel4 = glui->add_panel_to_panel(rollout[1], "", GLUI_PANEL_NONE);
    innerCheckbox[1][0] = glui->add_checkbox_to_panel(innerPanel4, "Directional", &lightState[1][0], 4, checkBoxCallback);
    innerCheckbox[1][1] = glui->add_checkbox_to_panel(innerPanel4, "Spot", &lightState[1][1], 5, checkBoxCallback);
    lightR[1] = glui->add_rotation_to_panel(innerPanel4, "Rotation", lightRM[1]);
    lightButton[1] = glui->add_button_to_panel(innerPanel4, "Reset Rotation", 4, buttonCallback);
    glui->add_column_to_panel(rollout[1], false);
    GLUI_Panel* innerPanel5 = glui->add_panel_to_panel(rollout[1], "", GLUI_PANEL_NONE);
    for (int i = 0; i < 6; i++)
    {
        lightSP[1][i] = createSpinner(innerPanel5, sName[i], &lightV[1][i]);
    }
    rollout[1]->disable();
    GLUI_Checkbox* lightCheckbox2 = glui->add_checkbox_to_panel(lightC, "Light 2", &lightState[2][2], 9, checkBoxCallback);
    rollout[2] = glui->add_rollout_to_panel(lightPanel, "Light 2", 0);
    GLUI_Panel* innerPanel6 = glui->add_panel_to_panel(rollout[2], "", GLUI_PANEL_NONE);
    innerCheckbox[2][0] = glui->add_checkbox_to_panel(innerPanel6, "Directional", &lightState[2][0], 7, checkBoxCallback);
    innerCheckbox[2][1] = glui->add_checkbox_to_panel(innerPanel6, "Spot", &lightState[2][1], 8, checkBoxCallback);
    lightR[2] = glui->add_rotation_to_panel(innerPanel6, "Rotation", lightRM[2]);
    lightButton[2] = glui->add_button_to_panel(innerPanel6, "Reset Rotation", 5, buttonCallback);
    glui->add_column_to_panel(rollout[2], false);
    GLUI_Panel* innerPanel7 = glui->add_panel_to_panel(rollout[2], "", GLUI_PANEL_NONE);
    for (int i = 0; i < 6; i++)
    {
        lightSP[2][i] = createSpinner(innerPanel7, sName[i], &lightV[2][i]);
    }
    rollout[2]->disable();
    GLUI_Panel* panel0 = glui->add_panel("Instance Transformation");
    GLUI_Panel* TransPanel = glui->add_panel_to_panel(panel0, "");
    GLUI_Panel* panel1 = glui->add_panel_to_panel(TransPanel, "Translation");
    GLUI_Panel* innerPanel0 = glui->add_panel_to_panel(panel1, "", GLUI_PANEL_NONE);
    translationXYZ[0] = createTranslation(innerPanel0, "X", GLUI_TRANSLATION_X, 1);
    translationXYZ[0]->set_x(teapotXYZPosition[0]);
    glui->add_column_to_panel(innerPanel0, false);
    translationXYZ[1] = createTranslation(innerPanel0, "Y", GLUI_TRANSLATION_Y, 2);
    translationXYZ[1]->set_y(teapotXYZPosition[1]);
    glui->add_column_to_panel(innerPanel0, false);
    translationXYZ[2] = createTranslation(innerPanel0, "Z", GLUI_TRANSLATION_Z, 3);
    translationXYZ[2]->set_z(teapotXYZPosition[2]);
    glui->add_separator_to_panel(panel1);
    glui->add_button_to_panel(panel1, "Reset Translation", 0, buttonCallback);
    glui->add_column_to_panel(TransPanel, true);
    GLUI_Panel* panel3 = glui->add_panel_to_panel(TransPanel, "");
    teapotR = glui->add_rotation_to_panel(panel3, "Rotation", teapotRM);
    glui->add_button_to_panel(panel3, "Reset Rotation", 2, buttonCallback);
    GLUI_Panel* scalePanel = glui->add_panel_to_panel(panel0, "");
    GLUI_Panel* scaleInner1 = glui->add_panel_to_panel(scalePanel, "Scaling");
    GLUI_Panel* scaleInner2 = glui->add_panel_to_panel(scaleInner1, "", GLUI_PANEL_NONE);
    scaleXYZ[0] = createTranslation(scaleInner2, "X", GLUI_TRANSLATION_X, 4);
    scaleXYZ[0]->set_x(teapotScaleXYZ[0]);
    glui->add_column_to_panel(scaleInner2, false);
    scaleXYZ[1] = createTranslation(scaleInner2, "Y", GLUI_TRANSLATION_Y, 5);
    scaleXYZ[1]->set_y(teapotScaleXYZ[1]);
    glui->add_column_to_panel(scaleInner2, false);
    scaleXYZ[2] = createTranslation(scaleInner2, "Z", GLUI_TRANSLATION_Z, 6);
    scaleXYZ[2]->set_z(teapotScaleXYZ[2]);
    glui->add_separator_to_panel(scaleInner1);
    GLUI_Checkbox* checkbox0 = glui->add_checkbox_to_panel(scaleInner1, "Uniform Scaling", &otherState[0], 0, checkBoxCallback);
    checkbox0->set_alignment(GLUI_ALIGN_CENTER);
    glui->add_button_to_panel(scaleInner1, "Reset Scaling", 1, buttonCallback);
    glui->add_column_to_panel(scalePanel, true);
    GLUI_Panel* scaleInner3 = glui->add_panel_to_panel(scalePanel, "Animation");
    GLUI_Checkbox* animeCheckbox = glui->add_checkbox_to_panel(scaleInner3, "Animate", &otherState[1], 4, checkBoxCallback);
    GLUI_Panel* scaleInner4 = glui->add_panel_to_panel(scalePanel, "Material");
    materialCheckbox = glui->add_checkbox_to_panel(scaleInner4, "Material", &otherState[2], 5, checkBoxCallback);
    materialCheckbox->set_int_val(1);
    emissionCheckbox = glui->add_checkbox_to_panel(scaleInner4, "Emission", &otherState[3], 6, checkBoxCallback);
    GLUI_Panel* scaleInner5 = glui->add_panel_to_panel(scalePanel, "Viewing");
    GLUI_RadioGroup* viewingRadio = glui->add_radiogroup_to_panel(scaleInner5, &otherState[4], 1, radioCallback);
    GLUI_RadioButton* ortho = glui->add_radiobutton_to_group(viewingRadio, "Ortho");
    GLUI_RadioButton* persp = glui->add_radiobutton_to_group(viewingRadio, "Perspec");
    GLUI_Panel* shadingPanel = glui->add_panel("Shading and Material");
    GLUI_Panel* shadingInner1 = glui->add_panel_to_panel(shadingPanel, "");
    GLUI_Panel* shadingInner2 = glui->add_panel_to_panel(shadingInner1, "");
    shadingRadio1 = glui->add_radiogroup_to_panel(shadingInner2, &otherState[5], 2, radioCallback);
    GLUI_RadioButton* shadingWire = glui->add_radiobutton_to_group(shadingRadio1, "Wire");
    GLUI_RadioButton* shadingFlat = glui->add_radiobutton_to_group(shadingRadio1, "Flat");
    GLUI_RadioButton* shadingSmooth = glui->add_radiobutton_to_group(shadingRadio1, "Smooth");
    glui->add_column_to_panel(shadingInner1, true);
    GLUI_Panel* shadingInner3 = glui->add_panel_to_panel(shadingInner1, "");
    GLUI_RadioGroup* shadingRadio2 = glui->add_radiogroup_to_panel(shadingInner3, &otherState[6], 3, radioCallback);
    GLUI_RadioButton* shadingTeapot = glui->add_radiobutton_to_group(shadingRadio2, "Teapot");
    GLUI_RadioButton* shadingCube = glui->add_radiobutton_to_group(shadingRadio2, "Color Cube");
    GLUI_RadioButton* shadingModel = glui->add_radiobutton_to_group(shadingRadio2, "Data Model");
    glui->add_button_to_panel(shadingInner3, "Choose file", 6, buttonCallback);
    shadingCheckbox = glui->add_checkbox_to_panel(shadingPanel, "Material Shininess", &otherState[8], 6, checkBoxCallback);
    GLUI_Spinner* shadingSpinner = glui->add_spinner_to_panel(shadingPanel, "Shininess", GLUI_SPINNER_FLOAT, &teapotShininess, 0, spinnerCallback);
    shadingSpinner->set_float_limits(0.0f, 128.0f);
    shadingSpinner->set_speed(0.1f);
    glui->add_column_to_panel(shadingPanel, true);
    shadingInner4 = glui->add_panel_to_panel(shadingPanel, "Material Type");
    GLUI_RadioGroup* shadingRadio3 = glui->add_radiogroup_to_panel(shadingInner4, &otherState[7], 4, radioCallback);
    GLUI_RadioButton* goldT = glui->add_radiobutton_to_group(shadingRadio3, "Gold");
    GLUI_RadioButton* pewterT = glui->add_radiobutton_to_group(shadingRadio3, "Pewter");
    GLUI_RadioButton* SilverT = glui->add_radiobutton_to_group(shadingRadio3, "Silver");
    GLUI_RadioButton* CopperT = glui->add_radiobutton_to_group(shadingRadio3, "Copper");
    GLUI_RadioButton* ChromeT = glui->add_radiobutton_to_group(shadingRadio3, "Chrome");
    glui->set_main_gfx_window(window);
}

void translationCallback(int id)
{
    teapotXYZPosition[0] = translationXYZ[0]->get_x();
    teapotXYZPosition[1] = translationXYZ[1]->get_y();
    teapotXYZPosition[2] = translationXYZ[2]->get_z();
    if (!otherState[0])
    {
        teapotScaleXYZ[0] = scaleXYZ[0]->get_x();
        teapotScaleXYZ[1] = scaleXYZ[1]->get_y();
        teapotScaleXYZ[2] = scaleXYZ[2]->get_z();
        if (teapotScaleXYZ[0] == 0.0f)
            teapotScaleXYZ[0] = 0.001f;
        if (teapotScaleXYZ[1] == 0.0f)
            teapotScaleXYZ[1] = 0.001f;
        if (teapotScaleXYZ[2] == 0.0f)
            teapotScaleXYZ[2] = 0.001f;
        lastScaleXYZ[0] = teapotScaleXYZ[0];
        lastScaleXYZ[1] = teapotScaleXYZ[1];
        lastScaleXYZ[2] = teapotScaleXYZ[2];
    }
    else
    {
        teapotScaleXYZ[0] = scaleXYZ[0]->get_x() * lastScaleXYZ[0];
        teapotScaleXYZ[1] = scaleXYZ[0]->get_x() * lastScaleXYZ[1];
        teapotScaleXYZ[2] = scaleXYZ[0]->get_x() * lastScaleXYZ[2];
        if (teapotScaleXYZ[0] == 0.0f)
            teapotScaleXYZ[0] = 0.001f * lastScaleXYZ[0];
        if (teapotScaleXYZ[1] == 0.0f)
            teapotScaleXYZ[1] = 0.001f * lastScaleXYZ[1];
        if (teapotScaleXYZ[2] == 0.0f)
            teapotScaleXYZ[2] = 0.001f * lastScaleXYZ[2];
    }

    glutPostRedisplay();
}

void buttonCallback(int id)
{
    if (id == 0)
    {
        translationXYZ[0]->set_x(0.0f);
        teapotXYZPosition[0] = 0.0f;
        translationXYZ[1]->set_y(0.0f);
        teapotXYZPosition[1] = 0.0f;
        translationXYZ[2]->set_z(0.0f);
        teapotXYZPosition[2] = 0.0f;
    }
    else if (id == 1)
    {
        scaleXYZ[0]->set_x(1.0f);
        teapotScaleXYZ[0] = 1.0f;
        scaleXYZ[1]->set_y(1.0f);
        teapotScaleXYZ[1] = 1.0f;
        scaleXYZ[2]->set_z(1.0f);
        teapotScaleXYZ[2] = 1.0f;
        lastScaleXYZ[0] = 1.0f;
        lastScaleXYZ[1] = 1.0f;
        lastScaleXYZ[2] = 1.0f;
    }
    else if (id == 2)
    {
        teapotR->reset();
    }
    else if (id == 3)
    {
        lightR[0]->reset();
    }
    else if (id == 4)
    {
        lightR[1]->reset();
    }
    else if (id == 5)
    {
        lightR[2]->reset();
    }
    else if (id == 6)
    {
        openFile();
    }
}

void checkBoxCallback(int id)
{
    if (otherState[0])
    {
        scaleXYZ[0]->set_x(1.0f);
        scaleXYZ[1]->disable();
        scaleXYZ[2]->disable();
    }
    else
    {
        scaleXYZ[0]->set_x(teapotScaleXYZ[0]);
        scaleXYZ[1]->enable();
        scaleXYZ[1]->set_y(teapotScaleXYZ[1]);
        scaleXYZ[2]->enable();
        scaleXYZ[2]->set_z(teapotScaleXYZ[2]);
    }
    if (id == 1 && lightState[0][0])
    {
        innerCheckbox[0][1]->set_int_val(0);
    }
    else if (id == 2 && lightState[0][1])
    {
        innerCheckbox[0][0]->set_int_val(0);
    }
    if (id == 4 && lightState[1][0])
    {
        innerCheckbox[1][1]->set_int_val(0);
    }
    else if (id == 5 && lightState[1][1])
    {
        innerCheckbox[1][0]->set_int_val(0);
    }
    if (id == 7 && lightState[2][0])
    {
        innerCheckbox[2][1]->set_int_val(0);
    }
    else if (id == 8 && lightState[2][1])
    {
        innerCheckbox[2][0]->set_int_val(0);
    }
    lightR[0]->reset();
    lightType[0] = (lightState[0][0]) ? 1 : ((lightState[0][1]) ? 2 : 0);
    lightR[1]->reset();
    lightType[1] = (lightState[1][0]) ? 1 : ((lightState[1][1]) ? 2 : 0);
    lightR[2]->reset();
    lightType[2] = (lightState[2][0]) ? 1 : ((lightState[2][1]) ? 2 : 0);
    if (lightState[0][2])
    {
        rollout[0]->enable();
        glEnable(GL_LIGHT0);
        lightType[0] = 0;
    }
    else
    {
        rollout[0]->disable();
        glDisable(GL_LIGHT0);
    }
    if (lightState[1][2])
    {
        rollout[1]->enable();
        glEnable(GL_LIGHT1);
        lightType[1] = 0;
    }
    else
    {
        rollout[1]->disable();
        glDisable(GL_LIGHT1);
    }
    if (lightState[2][2])
    {
        rollout[2]->enable();
        glEnable(GL_LIGHT2);
        lightType[2] = 0;
    }
    else
    {
        rollout[2]->disable();
        glDisable(GL_LIGHT2);
    }
}

void spinnerCallback(int id) {}

void radioCallback(int id) {}

GLUI_Spinner* createSpinner(GLUI_Panel* panel, const char* name, float* value)
{
    GLUI_Spinner* spinner = glui->add_spinner_to_panel(panel, name, GLUI_SPINNER_FLOAT, value);
    spinner->set_float_limits(0.0f, 1.0f);
    spinner->set_speed(0.1f);
    return spinner;
}

GLUI_Translation* createTranslation(GLUI_Panel* panel, const char* name, int type, int id)
{
    GLUI_Translation* translation = glui->add_translation_to_panel(panel, name, type, 0, id, translationCallback);
    translation->set_speed(0.005f);
    return translation;
}

void lightControl()
{
    GLfloat lightPPosition[][4] = { {2.0f, 0.0f, 0.0f, 1.0f}, {0.0f, 2.0f, 0.0f, 1.0f}, {0.0f, 0.0f, 2.0f, 1.0f} };
    GLfloat lightDPosition[][4] = { {1.0f, 0.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 1.0f, 0.0f} };
    GLfloat lightSPosition[][4] = { {3.0f, 0.0f, 0.0f, 1.0f}, {0.0f, 3.0f, 0.0f, 1.0f}, {0.0f, 0.0f, 3.0f, 1.0f} };
    GLfloat lightDiffuse[][4] = { {lightV[0][0], lightV[0][1], lightV[0][2], 1.0}, {lightV[1][0], lightV[1][1], lightV[1][2], 1.0}, {lightV[2][0], lightV[2][1], lightV[2][2], 1.0} };
    GLfloat lightSpecular[][4] = { {lightV[0][3], lightV[0][4], lightV[0][5], 1.0}, {lightV[1][3], lightV[1][4], lightV[1][5], 1.0}, {lightV[2][3], lightV[2][4], lightV[2][5], 1.0} };
    GLfloat lightDirection[][3] = { {-1.0f, 0.0f, 0.0f}, {0.0f, -1.0f, 0.0f}, {0.0f, 0.0f, -1.0f} };
    GLfloat lightCutoff[] = { 180.0f, 180.0f, 180.0f };
    GLfloat lightExponent[] = { 0.0f, 0.0f, 0.0f };
    lightType[0] = lightState[0][0] ? (lightR[0]->disable(), lightButton[0]->disable(), 1) : (lightState[0][1] ? (lightR[0]->enable(), lightButton[0]->enable(), 2) : (lightR[0]->disable(), lightButton[0]->disable(), 0));
    lightType[1] = lightState[1][0] ? (lightR[1]->disable(), lightButton[1]->disable(), 1) : (lightState[1][1] ? (lightR[1]->enable(), lightButton[1]->enable(), 2) : (lightR[1]->disable(), lightButton[1]->disable(), 0));
    lightType[2] = lightState[2][0] ? (lightR[2]->disable(), lightButton[2]->disable(), 1) : (lightState[2][1] ? (lightR[2]->enable(), lightButton[2]->enable(), 2) : (lightR[2]->disable(), lightButton[2]->disable(), 0));
    glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDiffuse[0]);
    glLightfv(GL_LIGHT0, GL_SPECULAR, lightSpecular[0]);
    glLightfv(GL_LIGHT1, GL_DIFFUSE, lightDiffuse[1]);
    glLightfv(GL_LIGHT1, GL_SPECULAR, lightSpecular[1]);
    glLightfv(GL_LIGHT2, GL_DIFFUSE, lightDiffuse[2]);
    glLightfv(GL_LIGHT2, GL_SPECULAR, lightSpecular[2]);
    for (int i = 0; i < 3; i++)
    {
        lightCutoff[i] = lightType[i] == 2 ? 10.0f : 180.0f;
    }
    for (int i = 0; i < 3; i++)
    {
        lightExponent[i] = lightType[i] == 2 ? 10.0f : 0.0f;
    }
    glLightf(GL_LIGHT0, GL_SPOT_CUTOFF, lightCutoff[0]);
    glLightf(GL_LIGHT0, GL_SPOT_EXPONENT, lightExponent[0]);
    switch (lightType[0])
    {
    case 0:
        glLightfv(GL_LIGHT0, GL_POSITION, lightPPosition[0]);
        break;
    case 1:
        glLightfv(GL_LIGHT0, GL_POSITION, lightDPosition[0]);
        break;
    case 2:
        if (!lightState[0][2]) {
            lightR[0]->disable();
            lightButton[0]->disable();
        }
        glLightfv(GL_LIGHT0, GL_POSITION, lightSPosition[0]);
        lightDirection[0][1] = -lightRM[0][6];
        lightDirection[0][2] = -lightRM[0][8];
        glLightfv(GL_LIGHT0, GL_SPOT_DIRECTION, lightDirection[0]);
        break;
    }
    glLightf(GL_LIGHT1, GL_SPOT_CUTOFF, lightCutoff[1]);
    glLightf(GL_LIGHT1, GL_SPOT_EXPONENT, lightExponent[1]);
    switch (lightType[1])
    {
    case 0:
        glLightfv(GL_LIGHT1, GL_POSITION, lightPPosition[1]);
        break;
    case 1:
        glLightfv(GL_LIGHT1, GL_POSITION, lightDPosition[1]);
        break;
    case 2:
        if (!lightState[1][2]) {
            lightR[1]->disable();
            lightButton[1]->disable();
        }
        glLightfv(GL_LIGHT1, GL_POSITION, lightSPosition[1]);
        lightDirection[1][0] = -lightRM[1][6];
        lightDirection[1][2] = -lightRM[1][8];
        glLightfv(GL_LIGHT1, GL_SPOT_DIRECTION, lightDirection[1]);
        break;
    }
    glLightf(GL_LIGHT2, GL_SPOT_CUTOFF, lightCutoff[2]);
    glLightf(GL_LIGHT2, GL_SPOT_EXPONENT, lightExponent[2]);
    switch (lightType[2])
    {
    case 0:
        glLightfv(GL_LIGHT2, GL_POSITION, lightPPosition[2]);
        break;
    case 1:
        glLightfv(GL_LIGHT2, GL_POSITION, lightDPosition[2]);
        break;
    case 2:
        if (!lightState[2][2]) {
            lightR[2]->disable();
            lightButton[2]->disable();
        }
        glLightfv(GL_LIGHT2, GL_POSITION, lightSPosition[2]);
        lightDirection[2][0] = -lightRM[2][8];
        lightDirection[2][1] = -lightRM[2][6];
        glLightfv(GL_LIGHT2, GL_SPOT_DIRECTION, lightDirection[2]);
        break;
    }
    glEnable(GL_NORMALIZE);
}

void materialDecision(int id)
{
    if (!otherState[2])
    {
        GLfloat teapotDiffuse[] = { teapotColor[0], teapotColor[1], teapotColor[2] };
        if (otherState[6] != 2)
        {
            glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, teapotDiffuse);
        }
        glMaterialf(GL_FRONT, GL_SHININESS, teapotShininess);
        emissionCheckbox->set_int_val(0);
        emissionCheckbox->disable();
        shadingCheckbox->set_int_val(0);
        shadingCheckbox->disable();
        shadingInner4->disable();
        GLfloat defaultEmission[] = { 0.0, 0.0, 0.0, 1.0 };
        glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, defaultEmission);
    }
    else
    {
        emissionCheckbox->enable();
        shadingCheckbox->enable();
        shadingInner4->enable();
        std::array<float, 3> teapotDiffuse = { 0.0f, 0.0f, 0.0f };
        std::array<float, 3> teapotAmbient = { 0.0f, 0.0f, 0.0f };
        std::array<float, 3> teapotSpecular = { 0.0f, 0.0f, 0.0f };
        std::array<float, 4> teapotEmission = { 0.0f, 0.0f, 0.0f, 1.0f };
        switch (id)
        {
        case 0:
            teapotDiffuse = { 0.75f, 0.6f, 0.22f };
            teapotAmbient = { 0.24f, 0.19f, 0.07f };
            teapotSpecular = { 0.62f, 0.55f, 0.36f };
            teapotEmission = { 0.75f, 0.6f, 0.22f, 1.0f };
            materialShininess = 32.0f;
            break;
        case 1:
            teapotDiffuse = { 0.42f, 0.47f, 0.54f };
            teapotAmbient = { 0.10f, 0.05f, 0.11f };
            teapotSpecular = { 0.33f, 0.33f, 0.52f };
            teapotEmission = { 0.75f, 0.6f, 0.22f, 1.0f };
            materialShininess = 9.84f;
            break;
        case 2:
            teapotDiffuse = { 0.50f, 0.50f, 0.50f };
            teapotAmbient = { 0.19f, 0.19f, 0.19f };
            teapotSpecular = { 0.50f, 0.50f, 0.50f };
            teapotEmission = { 0.75f, 0.6f, 0.22f, 1.0f };
            materialShininess = 51.2f;
            break;
        case 3:
            teapotDiffuse = { 0.70f, 0.27f, 0.08f };
            teapotAmbient = { 0.19f, 0.07f, 0.02f };
            teapotSpecular = { 0.25f, 0.13f, 0.08f };
            teapotEmission = { 0.75f, 0.6f, 0.22f, 1.0f };
            materialShininess = 12.8f;
            break;
        case 4:
            teapotDiffuse = { 0.4f, 0.4f, 0.4f };
            teapotAmbient = { 0.25f, 0.25f, 0.25f };
            teapotSpecular = { 0.77f, 0.77f, 0.77f };
            teapotEmission = { 0.75f, 0.6f, 0.22f, 1.0f };
            materialShininess = 76.8f;
            break;
        }
        cubeDiffuse = teapotDiffuse;
        cubeAmbient = teapotAmbient;
        glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, teapotDiffuse.data());
        glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, teapotAmbient.data());
        glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, teapotSpecular.data());
        if (otherState[3])
        {
            glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, teapotEmission.data());
        }
        else
        {
            GLfloat defaultEmission[] = { 0.0f, 0.0f, 0.0f, 1.0f };
            glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, defaultEmission);
        }
        if (otherState[8])
        {
            glMaterialf(GL_FRONT, GL_SHININESS, materialShininess);
        }
        else
        {
            glMaterialf(GL_FRONT, GL_SHININESS, teapotShininess);
        }
    }
}

void openFile() {
    const char* filters[] = { "*.tri" };
    const char* result = tinyfd_openFileDialog(
        "Open File",
        "Desktop",
        1,
        filters,
        ".tri File",
        0
    );

    if (result != NULL && strlen(result) > 0) {
        FILE* file = fopen(result, "r");
        if (file != NULL) {
            readSimpleFormat(file);
            fclose(file);
        }
    }
}

void readSimpleFormat(FILE* file) {
    int n = 0, m = 0, format = 0;
    char buffer[256];
    fgets(buffer, sizeof(buffer), file);
    if (strncmp(buffer, "Simple: ", strlen("Simple: ")) == 0) {
        format = 0;
        sscanf(buffer, "Simple: %d", &n);
    }
    else if (strncmp(buffer, "Color: ", strlen("Color: ")) == 0) {
        format = 1;
        sscanf(buffer, "Color: %d", &n);
    }
    fscanf(file, "Vertices: %d\n", &m);
    std::vector<Triangle> triangles(n);
    std::vector<Vertex> vertices(m);

    if (!format) {
        for (int i = 0; i < n; i++) {
            fscanf(file, "%*d %d %d %d\n", &triangles[i].v1_id, &triangles[i].v2_id, &triangles[i].v3_id);
        }
    }
    else {
        for (int i = 0; i < n; i++) {
            fscanf(file, "%*d %d %d %d %d %d %d %d %d %d\n",
                &triangles[i].v1_id, &triangles[i].v2_id, &triangles[i].v3_id,
                &triangles[i].fr, &triangles[i].fg, &triangles[i].fb,
                &triangles[i].br, &triangles[i].bg, &triangles[i].bb);
        }
    }

    for (int i = 0; i < m; i++) {
        fscanf(file, "%*d %f %f %f %f %f %f\n",
            &vertices[i].x, &vertices[i].y, &vertices[i].z,
            &vertices[i].nx, &vertices[i].ny, &vertices[i].nz);
    }

    float minX = FLT_MAX, minY = FLT_MAX, minZ = FLT_MAX;
    float maxX = -FLT_MAX, maxY = -FLT_MAX, maxZ = -FLT_MAX;
    for (const auto& v : vertices) {
        if (v.x < minX) minX = v.x;
        if (v.y < minY) minY = v.y;
        if (v.z < minZ) minZ = v.z;
        if (v.x > maxX) maxX = v.x;
        if (v.y > maxY) maxY = v.y;
        if (v.z > maxZ) maxZ = v.z;
    }
    modelSizeX = maxX - minX;
    modelSizeY = maxY - minY;
    modelSizeZ = maxZ - minZ;

    float maxSize = std::max({ modelSizeX, modelSizeY, modelSizeZ });

    float targetSize = 2.0f;
    modelScale = targetSize / maxSize;

    mT = triangles;
    mV = vertices;
    createDisplayList(triangles, vertices);
}

void createDisplayList(const std::vector<Triangle>& triangles, const std::vector<Vertex>& vertices) {
    float centerX = 0.0f, centerY = 0.0f, centerZ = 0.0f;
    for (const Vertex& vertex : vertices) {
        centerX += vertex.x;
        centerY += vertex.y;
        centerZ += vertex.z;
    }
    centerX /= vertices.size();
    centerY /= vertices.size();
    centerZ /= vertices.size();
    glNewList(DATAMODEL, GL_COMPILE);
    glPushMatrix();
    glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
    glTranslatef(-centerX, -centerY, -centerZ);
    glBegin(GL_TRIANGLES);
    for (const Triangle& triangle : triangles) {
        modelFrontDiffuse = { triangle.fr / 255.0f, triangle.fg / 255.0f, triangle.fb / 255.0f };
        glMaterialfv(GL_FRONT, GL_DIFFUSE, modelFrontDiffuse.data());
        glNormal3f(vertices[triangle.v1_id].nx, vertices[triangle.v1_id].ny, vertices[triangle.v1_id].nz);
        glVertex3f(vertices[triangle.v1_id].x, vertices[triangle.v1_id].y, vertices[triangle.v1_id].z);
        glNormal3f(vertices[triangle.v2_id].nx, vertices[triangle.v2_id].ny, vertices[triangle.v2_id].nz);
        glVertex3f(vertices[triangle.v2_id].x, vertices[triangle.v2_id].y, vertices[triangle.v2_id].z);
        glNormal3f(vertices[triangle.v3_id].nx, vertices[triangle.v3_id].ny, vertices[triangle.v3_id].nz);
        glVertex3f(vertices[triangle.v3_id].x, vertices[triangle.v3_id].y, vertices[triangle.v3_id].z);
        modelBackDiffuse = { triangle.br / 255.0f, triangle.bg / 255.0f, triangle.bb / 255.0f };
        glMaterialfv(GL_BACK, GL_DIFFUSE, modelBackDiffuse.data());
        glNormal3f(vertices[triangle.v3_id].nx, vertices[triangle.v3_id].ny, vertices[triangle.v3_id].nz);
        glVertex3f(vertices[triangle.v3_id].x, vertices[triangle.v3_id].y, vertices[triangle.v3_id].z);
        glNormal3f(vertices[triangle.v2_id].nx, vertices[triangle.v2_id].ny, vertices[triangle.v2_id].nz);
        glVertex3f(vertices[triangle.v2_id].x, vertices[triangle.v2_id].y, vertices[triangle.v2_id].z);
        glNormal3f(vertices[triangle.v1_id].nx, vertices[triangle.v1_id].ny, vertices[triangle.v1_id].nz);
        glVertex3f(vertices[triangle.v1_id].x, vertices[triangle.v1_id].y, vertices[triangle.v1_id].z);
    }
    glEnd();
    glPopMatrix();
    glEndList();

    glNewList(DATAMODEL_LINE, GL_COMPILE);
    glPushMatrix();
    glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
    glTranslatef(-centerX, -centerY, -centerZ);
    glBegin(GL_LINES);
    for (const Triangle& triangle : triangles) {
        modelFrontDiffuse = { triangle.fr / 255.0f, triangle.fg / 255.0f, triangle.fb / 255.0f };
        glMaterialfv(GL_FRONT, GL_DIFFUSE, modelFrontDiffuse.data());
        glNormal3f(vertices[triangle.v1_id].nx, vertices[triangle.v1_id].ny, vertices[triangle.v1_id].nz);
        glVertex3f(vertices[triangle.v1_id].x, vertices[triangle.v1_id].y, vertices[triangle.v1_id].z);
        glNormal3f(vertices[triangle.v2_id].nx, vertices[triangle.v2_id].ny, vertices[triangle.v2_id].nz);
        glVertex3f(vertices[triangle.v2_id].x, vertices[triangle.v2_id].y, vertices[triangle.v2_id].z);
        glNormal3f(vertices[triangle.v3_id].nx, vertices[triangle.v3_id].ny, vertices[triangle.v3_id].nz);
        glVertex3f(vertices[triangle.v3_id].x, vertices[triangle.v3_id].y, vertices[triangle.v3_id].z);
        modelBackDiffuse = { triangle.br / 255.0f, triangle.bg / 255.0f, triangle.bb / 255.0f };
        glMaterialfv(GL_BACK, GL_DIFFUSE, modelBackDiffuse.data());
        glNormal3f(vertices[triangle.v3_id].nx, vertices[triangle.v3_id].ny, vertices[triangle.v3_id].nz);
        glVertex3f(vertices[triangle.v3_id].x, vertices[triangle.v3_id].y, vertices[triangle.v3_id].z);
        glNormal3f(vertices[triangle.v2_id].nx, vertices[triangle.v2_id].ny, vertices[triangle.v2_id].nz);
        glVertex3f(vertices[triangle.v2_id].x, vertices[triangle.v2_id].y, vertices[triangle.v2_id].z);
        glNormal3f(vertices[triangle.v1_id].nx, vertices[triangle.v1_id].ny, vertices[triangle.v1_id].nz);
        glVertex3f(vertices[triangle.v1_id].x, vertices[triangle.v1_id].y, vertices[triangle.v1_id].z);
    }
    glEnd();
    glPopMatrix();
    glEndList();

    glNewList(DATAMODEL_MATERIAL, GL_COMPILE);
    glPushMatrix();
    glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
    glTranslatef(-centerX, -centerY, -centerZ);
    glBegin(GL_TRIANGLES);
    for (const Triangle& triangle : triangles) {
        glNormal3f(vertices[triangle.v1_id].nx, vertices[triangle.v1_id].ny, vertices[triangle.v1_id].nz);
        glVertex3f(vertices[triangle.v1_id].x, vertices[triangle.v1_id].y, vertices[triangle.v1_id].z);
        glNormal3f(vertices[triangle.v2_id].nx, vertices[triangle.v2_id].ny, vertices[triangle.v2_id].nz);
        glVertex3f(vertices[triangle.v2_id].x, vertices[triangle.v2_id].y, vertices[triangle.v2_id].z);
        glNormal3f(vertices[triangle.v3_id].nx, vertices[triangle.v3_id].ny, vertices[triangle.v3_id].nz);
        glVertex3f(vertices[triangle.v3_id].x, vertices[triangle.v3_id].y, vertices[triangle.v3_id].z);
        glNormal3f(vertices[triangle.v3_id].nx, vertices[triangle.v3_id].ny, vertices[triangle.v3_id].nz);
        glVertex3f(vertices[triangle.v3_id].x, vertices[triangle.v3_id].y, vertices[triangle.v3_id].z);
        glNormal3f(vertices[triangle.v2_id].nx, vertices[triangle.v2_id].ny, vertices[triangle.v2_id].nz);
        glVertex3f(vertices[triangle.v2_id].x, vertices[triangle.v2_id].y, vertices[triangle.v2_id].z);
        glNormal3f(vertices[triangle.v1_id].nx, vertices[triangle.v1_id].ny, vertices[triangle.v1_id].nz);
        glVertex3f(vertices[triangle.v1_id].x, vertices[triangle.v1_id].y, vertices[triangle.v1_id].z);
    }
    glEnd();
    glPopMatrix();
    glEndList();
}
