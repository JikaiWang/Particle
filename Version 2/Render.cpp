//
//  Render.cpp
//  Particle2Dv2
//
//  Created by Jikai Wang on 7/18/18.
//  Copyright © 2018 Jikai Wang. All rights reserved.
//

#include "Render.h"

int frame_time;
int base_time;
int frame_count = 0;
int CurrentWidth;
int CurrentHeight = 1360;
int WindowHandle = 0;
int num_Vertex = 6;
int num;
int i;

GLuint VertexShaderId;
GLuint FragmentShaderId;
GLuint ProgramId;
GLuint VaoId;
GLuint VboId;
GLuint PosiBufferId;
GLuint ColorBufferId;

GLfloat *Circle = NULL;
GLfloat *Positions = NULL;
GLfloat *Colors = NULL;
vector<par_info> *particle = NULL;
bool *pauseforRender = NULL;
bool *pauseforShear = NULL;
bool *pause = NULL;
bool *next_frame = NULL;

double sys_w;
double sys_h;


const GLchar* VertexShader =
{
    "#version 410\n"\
    
    "layout(location=0) in vec4 in_Particle;\n"\
    "layout(location=1) in vec4 in_Position;\n"\
    "layout(location=2) in vec4 in_Color;\n"\
    "out vec4 ex_Color;\n"\
    
    "void main(void)\n"\
    "{\n"\
    "  gl_Position = in_Position + in_Particle;\n"\
    "  ex_Color = in_Color;\n"\
    "}\n"
};

const GLchar* FragmentShader =
{
    "#version 410\n"\
    
    "in vec4 ex_Color;\n"\
    "out vec4 out_Color;\n"\
    
    "void main(void)\n"\
    "{\n"\
    "  out_Color = ex_Color;\n"\
    "}\n"
};



void processNormalKeys(unsigned char key, int x, int y)
{
    switch (key) {
        case 27: glutDestroyWindow(WindowHandle); *pause = false; return;
        case 32: *pause = !(*pause); return;
        case 13: *next_frame = !(*next_frame); return;
    }
}


void GenCircle()
{
    GLfloat diameter_x = 1 / sys_w;
    GLfloat diameter_y = 1 / sys_h;
    
    vector<GLfloat> vertex_circle;
    for (int i = 0; i < num_Vertex; i++)
    {
        vertex_circle.push_back(2 * diameter_x * cos(2 * M_PI * i / float(num_Vertex)));
        vertex_circle.push_back(2 * diameter_y * sin(2 * M_PI * i / float(num_Vertex)));
        vertex_circle.push_back(0.0f);
        vertex_circle.push_back(1.0f);
    }
    
    for (int i = 0; i < 4 * num_Vertex; i++)
    {
        Circle[i] = vertex_circle[i];
    }
}



void RenderFunction()
{
    frame_count++;
    
    i = 0;
    *pauseforRender = true;
    while (*pauseforShear) this_thread::sleep_for(chrono::nanoseconds(1));
    for (auto par = particle->begin(); par != particle->end(); ++par) {
        Positions[i++] = 4 * par->x / sys_w - 2;
        Positions[i++] = 4 * par->y / sys_h - 2;
        Positions[i++] = 0.0f;
        Positions[i++] = 1.0f;
        i -= 4;
        Colors[i++] = par->pretag;
        Colors[i++] = 0.0f;
        Colors[i++] = 1 - par->pretag;
        Colors[i++] = 1.0f;
    }
    *pauseforRender = false;
    
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    
    glBindBuffer(GL_ARRAY_BUFFER, PosiBufferId);
    glBufferData(GL_ARRAY_BUFFER, 4 * sizeof(Positions) * num, NULL, GL_STREAM_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, 4 * sizeof(Positions) * num, Positions);
    
    glBindBuffer(GL_ARRAY_BUFFER, ColorBufferId);
    glBufferData(GL_ARRAY_BUFFER, 4 * sizeof(Colors) * num, NULL, GL_STREAM_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, 4 * sizeof(Colors) * num, Colors);
    
    glDrawArraysInstanced(GL_POINTS, 0, num_Vertex, num);
    
    glutSwapBuffers();
}


void CreateVBO(void)
{
    GLenum ErrorCheckValue = glGetError();
    
    glGenVertexArrays(1, &VaoId);
    glBindVertexArray(VaoId);
    
    glGenBuffers(1, &VboId);
    glBindBuffer(GL_ARRAY_BUFFER, VboId);
    glBufferData(GL_ARRAY_BUFFER, 4 * sizeof(Circle) * num_Vertex, Circle, GL_STATIC_DRAW);
    
    glGenBuffers(1, &PosiBufferId);
    glBindBuffer(GL_ARRAY_BUFFER, PosiBufferId);
    glBufferData(GL_ARRAY_BUFFER, 4 * sizeof(Positions) * num, NULL, GL_STREAM_DRAW);
    
    glGenBuffers(1, &ColorBufferId);
    glBindBuffer(GL_ARRAY_BUFFER, ColorBufferId);
    glBufferData(GL_ARRAY_BUFFER, 4 * sizeof(Colors) * num, NULL, GL_STREAM_DRAW);
    
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, VboId);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, (void*)0);
    
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, PosiBufferId);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, (void*)0);
    
    glEnableVertexAttribArray(2);
    glBindBuffer(GL_ARRAY_BUFFER, ColorBufferId);
    glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, 0, (void*)0);
    
    glVertexAttribDivisor(0, 0);
    glVertexAttribDivisor(1, 1);
    glVertexAttribDivisor(2, 1);
    
    ErrorCheckValue = glGetError();
    if (ErrorCheckValue != GL_NO_ERROR)
    {
        fprintf(
                stderr,
                "ERROR: Could not create a VBO: %s \n",
                gluErrorString(ErrorCheckValue)
                );
        
        exit(-1);
    }
}

void DestroyVBO(void)
{
    GLenum ErrorCheckValue = glGetError();
    
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(0);
    
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    
    glDeleteBuffers(1, &PosiBufferId);
    glDeleteBuffers(1, &VboId);
    
    glBindVertexArray(0);
    glDeleteVertexArrays(1, &VaoId);
    
    ErrorCheckValue = glGetError();
    if (ErrorCheckValue != GL_NO_ERROR)
    {
        fprintf(
                stderr,
                "ERROR: Could not destroy the VBO: %s \n",
                gluErrorString(ErrorCheckValue)
                );
        
        exit(-1);
    }
}

void CreateShaders(void)
{
    GLenum ErrorCheckValue = glGetError();
    
    VertexShaderId = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(VertexShaderId, 1, &VertexShader, NULL);
    glCompileShader(VertexShaderId);
    
    FragmentShaderId = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(FragmentShaderId, 1, &FragmentShader, NULL);
    glCompileShader(FragmentShaderId);
    
    ProgramId = glCreateProgram();
    glAttachShader(ProgramId, VertexShaderId);
    glAttachShader(ProgramId, FragmentShaderId);
    glLinkProgram(ProgramId);
    glUseProgram(ProgramId);
    
    ErrorCheckValue = glGetError();
    if (ErrorCheckValue != GL_NO_ERROR)
    {
        fprintf(
                stderr,
                "ERROR: Could not create the shaders: %s \n",
                gluErrorString(ErrorCheckValue)
                );
        
        exit(-1);
    }
}

void DestroyShader(void)
{
    GLenum ErrorCheckValue = glGetError();
    
    glUseProgram(0);
    
    glDetachShader(ProgramId, VertexShaderId);
    glDetachShader(ProgramId, FragmentShaderId);
    
    glDeleteShader(FragmentShaderId);
    glDeleteShader(VertexShaderId);
    
    glDeleteProgram(ProgramId);
    
    ErrorCheckValue = glGetError();
    if (ErrorCheckValue != GL_NO_ERROR)
    {
        fprintf(
                stderr,
                "ERROR: Could not destroy the shaders: %s \n",
                gluErrorString(ErrorCheckValue)
                );
        
        exit(-1);
    }
}

void ResizeFunction(int Width, int Height)
{
    CurrentWidth = Width;
    CurrentHeight = Height;
    glViewport(0, 0, CurrentWidth, CurrentHeight);
}

void IdleFunction(void)
{
    glutPostRedisplay();
}

void TimerFunction(int Value)
{
    frame_time = glutGet(GLUT_ELAPSED_TIME);
    
    if (0 != Value) {
        char* TempString = (char*)
        malloc(512 + strlen(WINDOW_TITLE_PREFIX));
        
        sprintf(
                TempString,
                "%s: %2.0f Frames Per Second @ %d x %d",
                WINDOW_TITLE_PREFIX,
                frame_count * 1000.0 / float(frame_time - base_time),
                CurrentWidth,
                CurrentHeight
                );
        
        glutSetWindowTitle(TempString);
        free(TempString);
    }
    
    frame_count = 0;
    base_time = frame_time;
    glutTimerFunc(100, TimerFunction, 1);
}

void Cleanup(void)
{
    DestroyShader();
    DestroyVBO();
    delete[] Circle;
    delete[] Positions;
    delete[] Colors;
}


void initRenderer(render_info renderInfo)
{
    pauseforRender = renderInfo.p_pauseforRender;
    pauseforShear = renderInfo.p_pauseforShear;
    pause = renderInfo.p_pause;
    next_frame = renderInfo.p_next_frame;
    particle = renderInfo.p_particle;
    num = particle->size();
    sys_w = renderInfo.sys_w;
    sys_h = renderInfo.sys_h;
    CurrentWidth = CurrentHeight * sys_w / sys_h;
    
    Circle = new GLfloat[4 * num_Vertex];
    GenCircle();
    Positions = new GLfloat[particle->size() * 4];
    Colors = new GLfloat[particle->size() * 4];
    
    char *myargv[1];
    int myargc = 1;
    myargv[0] = strdup("Myappname");
    
    glutInit(&myargc, myargv);
    
    glutInitWindowPosition(-1, -1);
    glutInitWindowSize(CurrentWidth, CurrentHeight);
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_RGBA | GLUT_3_2_CORE_PROFILE);
    
    WindowHandle = glutCreateWindow(WINDOW_TITLE_PREFIX);
    
    if (WindowHandle < 1) {
        fprintf(
                stderr,
                "ERROR: Could not create a new rendering window.\n"
                );
        exit(EXIT_FAILURE);
    }
    
    glutReshapeFunc(ResizeFunction);
    glutDisplayFunc(RenderFunction);
    glutIdleFunc(IdleFunction);
    glutTimerFunc(0, TimerFunction, 0);
    glutWMCloseFunc(Cleanup);
    glutKeyboardFunc(processNormalKeys);
    
    CreateShaders();
    CreateVBO();
    
//    cout << glGetString(GL_VERSION) << endl;
}

void render()
{
    glutMainLoop();
}

