/***************************************************************\
*																*
*					Zombie City for Glut v0.1					*
*																*
*	Port by Michael Cook (www.foobarsoft.com)					*
*		of 3D version by Alan Gordon (unknown contact info)		*
*		of Java applet by Kevan Davis (www.kevan.org)			*
*																*
*		See readme.txt for more information and license			*
*																*
\***************************************************************/

#include <math.h>			// Header File for Math
#include <stdio.h>			// Header FIle for Input/Output
#include <iostream.h>
#include <OpenGL/gl.h>			// Header File For The OpenGL32 Library
#include <OpenGL/glu.h>			// Header File For The GLu32 Library
#include <GLUT/glut.h>
#include <fstream.h>

bool	keys[256];			// Array Used For The Keyboard Routine
bool	keypressed[256];	// Array Used For The Keyboard Routine
bool	active=true;		// Window Active Flag Set To TRUE By Default
bool	fullscreen=false;	// Fullscreen Flag Set To Fullscreen Mode By Default
bool	end=false;
bool	mouseclick;

GLfloat lightposition[]={ 1.0f, 1.0f, 1.0f, 1.0f};
GLfloat LightDiffuse[]=	{ 1.0f, 1.0f, 1.0f, 1.0f};

float i_tick;   
float framelength;
float GAMESPEED = 0.09f;
float i_frame;
float i_tick_2;
float i_frame_rate;
int leveltime;

GLfloat camerapos[2] = {125.0f, 125.0f};
int centersquare[2] = {0,0};

int population = 2000;		//Amount of total beings
int zombies = 1;			//Amount of Zombies
bool hud;					//Is the HUD online?

struct {
	int state;		//0=human, 1=panic, 2=zombie
	int xPos, yPos;	//Coordinates
	int dir;		//Facing
	int active;		//How long it's gonna be running
	GLfloat move;	//how far it is towards the next square
	GLfloat flash;	//Flash of light when they convert to Zombie-hood
	bool moving;
} being[4000];

int city[250][250];	//the City Plan
int tracker[250][250]; //Who's where?

int list[3];

void glutSpecialCallback(int key, int x, int y) {
	if (key == GLUT_KEY_LEFT)
		keys[GLUT_KEY_LEFT] = true;
	else if (key == GLUT_KEY_RIGHT)
		keys[GLUT_KEY_RIGHT] = true;
	else if (key == GLUT_KEY_DOWN)
		keys[GLUT_KEY_DOWN] = true;
	else if (key == GLUT_KEY_UP)
		keys[GLUT_KEY_UP] = true;
}

void glutKeyboardCallback(unsigned char key, int x, int y) {
	keys[key] = true;
}

void BuildLists() {
	list[0]=glGenLists(3);
	glNewList(list[0], GL_COMPILE);
	glBegin(GL_QUADS);				//Ground
		glColor3f( 0.5f, 0.5f, 0.5f);
		glNormal3f( 0.0f, 0.0f, 1.0f);
		glVertex3f(-0.5f,-0.5f, 0.0f);
		glVertex3f( 0.5f,-0.5f, 0.0f);
		glVertex3f( 0.5f, 0.5f, 0.0f);
		glVertex3f(-0.5f, 0.5f, 0.0f);
	glEnd();
	glEndList();
	list[1] = list[0] + 1;
	glNewList(list[1], GL_COMPILE);
	glBegin(GL_QUADS);				//Wall
		glColor3f( 0.7f, 0.7f, 0.7f);
		glNormal3f( 0.0f, 0.0f, 1.0f);
		glVertex3f(-0.5f,-0.5f, 2.0f);
		glVertex3f( 0.5f,-0.5f, 2.0f);
		glVertex3f( 0.5f, 0.5f, 2.0f);
		glVertex3f(-0.5f, 0.5f, 2.0f);

		glNormal3f(-1.0f, 0.0f, 0.0f);
		glVertex3f(-0.5f,-0.5f, 2.0f);
		glVertex3f(-0.5f, 0.5f, 2.0f);
		glVertex3f(-0.5f, 0.5f, 0.0f);
		glVertex3f(-0.5f,-0.5f, 0.0f);

		glNormal3f( 0.0f,-1.0f, 0.0f);
		glVertex3f(-0.5f,-0.5f, 2.0f);
		glVertex3f( 0.5f,-0.5f, 2.0f);
		glVertex3f( 0.5f,-0.5f, 0.0f);
		glVertex3f(-0.5f,-0.5f, 0.0f);

		glNormal3f( 1.0f, 0.0f, 0.0f);
		glVertex3f( 0.5f,-0.5f, 2.0f);
		glVertex3f( 0.5f, 0.5f, 2.0f);
		glVertex3f( 0.5f, 0.5f, 0.0f);
		glVertex3f( 0.5f,-0.5f, 0.0f);

		glNormal3f( 0.0f, 1.0f, 0.0f);
		glVertex3f(-0.5f, 0.5f, 2.0f);
		glVertex3f( 0.5f, 0.5f, 2.0f);
		glVertex3f( 0.5f, 0.5f, 0.0f);
		glVertex3f(-0.5f, 0.5f, 0.0f);
	glEnd();
	glEndList();
	list[2] = list[1] + 1;
	glNewList(list[2], GL_COMPILE);
	glBegin(GL_TRIANGLES);				//Being
		glNormal3f( 0.577f, 0.577f, 0.577f);
		glVertex3f( 0.0f, 0.0f, 1.2f);
		glVertex3f( 0.3f, 0.0f, 0.9f);
		glVertex3f( 0.0f, 0.3f, 0.9f);

		glNormal3f(-0.577f, 0.577f, 0.577f);
		glVertex3f( 0.0f, 0.0f, 1.2f);
		glVertex3f(-0.3f, 0.0f, 0.9f);
		glVertex3f( 0.0f, 0.3f, 0.9f);

		glNormal3f( 0.577f, 0.577f,-0.577f);
		glVertex3f( 0.0f, 0.0f, 0.6f);
		glVertex3f( 0.3f, 0.0f, 0.9f);
		glVertex3f( 0.0f, 0.3f, 0.9f);

		glNormal3f(-0.577f, 0.577f,-0.577f);
		glVertex3f( 0.0f, 0.0f, 0.6f);
		glVertex3f(-0.3f, 0.0f, 0.9f);
		glVertex3f( 0.0f, 0.3f, 0.9f);

		glNormal3f( 0.0f,-1.0f, 0.0f);
		glVertex3f( 0.0f, 0.0f, 1.2f);
		glVertex3f( 0.3f, 0.0f, 0.9f);
		glVertex3f( 0.0f, 0.0f, 0.6f);

		glNormal3f( 0.0f,-1.0f, 0.0f);
		glVertex3f( 0.0f, 0.0f, 1.2f);
		glVertex3f(-0.3f, 0.0f, 0.9f);
		glVertex3f( 0.0f, 0.0f, 0.6f);

		glNormal3f( 0.667f, 0.667f, 0.333f);
		glVertex3f( 0.0f, 0.0f, 0.9f);
		glVertex3f( 0.3f, 0.0f, 0.3f);
		glVertex3f( 0.0f, 0.3f, 0.3f);

		glNormal3f( 0.667f,-0.667f, 0.333f);
		glVertex3f( 0.0f, 0.0f, 0.9f);
		glVertex3f( 0.3f, 0.0f, 0.3f);
		glVertex3f( 0.0f,-0.3f, 0.3f);

		glNormal3f(-0.667f, 0.667f, 0.333f);
		glVertex3f( 0.0f, 0.0f, 0.9f);
		glVertex3f(-0.3f, 0.0f, 0.3f);
		glVertex3f( 0.0f, 0.3f, 0.3f);

		glNormal3f(-0.667f,-0.667f, 0.333f);
		glVertex3f( 0.0f, 0.0f, 0.9f);
		glVertex3f(-0.3f, 0.0f, 0.3f);
		glVertex3f( 0.0f,-0.3f, 0.3f);
	glEnd();
	glEndList();
}

void ReSizeGLScene(int width, int height)		// Resize And Initialize The GL Window
{
	if (height==0)										// Prevent A Divide By Zero By
	{
		height=1;										// Making Height Equal One
	}

	glViewport(0,0,width,height);						// Reset The Current Viewport

	glMatrixMode(GL_PROJECTION);						// Select The Projection Matrix
	glLoadIdentity();									// Reset The Projection Matrix

	// Calculate The Aspect Ratio Of The Window
	gluPerspective(45.0f,(GLfloat)width/(GLfloat)height,0.05f,100.0f);

	glMatrixMode(GL_MODELVIEW);							// Select The Modelview Matrix
	glLoadIdentity();									// Reset The Modelview Matrix
}

void Populate_City(int pop) {
	int x, y, i;
	for(i = 0; i < pop; ++i) {
		do {
			x = rand() % 249 + 1;
			y = rand() % 249 + 1;
		} while (city[x][y] == 1);
		being[i].dir = rand() % 4;
		being[i].move = 0.0f;
		being[i].state = 0;
		being[i].xPos = x;
		being[i].yPos = y;
		being[i].active = rand() % 10;
		being[i].moving = false;
		tracker[being[i].xPos][being[i].yPos] = 0;
	}
	x = rand() % pop;
	being[x].state = 2;
	being[x].move = 0.8f;
	tracker[being[i].xPos][being[i].yPos] = 2;
	camerapos[0] = GLfloat(being[x].xPos);
	camerapos[1] = GLfloat(being[x].yPos);
	zombies = 1;
}

void Build_City() {
	int i;
	for(i = 0; i < 250; ++i) {
		for(int d = 0; d < 250; ++d) {
			city[i][d] = 0;
			tracker[i][d] = -1;
		}
	}
	int x,y,x_length, y_length;
	for(i = 0; i < 100; ++i) {
		x = rand() % 250;
		y = rand() % 250;
		x_length = rand() % 60 + 10;
		y_length = rand() % 60 + 10;
		for(int d = 0; d < x_length; ++d) {
			for(int e = 0; e < y_length; ++e) {
				if(d < 250 && e < 250) {
					if(d == 0 || e == 0 || d == x_length - 1 || e == y_length - 1) {
						city[d+x][e+y] = 0;
					} else {
						city[d+x][e+y] = 1;
					}
				}
			}
		}
	}
	for(i = 0; i < 30; ++i) {
		x = rand() % 250;
		y = rand() % 250;
		x_length = rand() % 20 + 20;
		y_length = rand() % 20 + 20;
		for(int d = 0; d < x_length; ++d) {
			for(int e = 0; e < y_length; ++e) {
				if(d < 250 && e < 250) {
					city[d+x][e+y] = 0;
				}
			}
		}
	}
	Populate_City(population);
};

int InitGL()										// All Setup For OpenGL Goes Here
{
	srand(time(NULL));
	glShadeModel(GL_SMOOTH);							// Enable Smooth Shading
	glClearColor(0.0f, 0.0f, 0.0f, 0.5f);				// Black Background
	glClearDepth(1.0f);									// Depth Buffer Setup
	glDepthFunc(GL_LESS);								// The Type Of Depth Testing To Do
	glEnable(GL_DEPTH_TEST);							// Enables Depth Testing
	glLightfv(GL_LIGHT1, GL_DIFFUSE, LightDiffuse);
	glLightfv(GL_LIGHT1, GL_POSITION, lightposition);
	glEnable(GL_LIGHT1);
	glEnable(GL_LIGHTING);
	glShadeModel(GL_SMOOTH);							// Enables Smooth Color Shading
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);	// Really Nice Perspective Calculations
	glEnable(GL_COLOR_MATERIAL);
 	BuildLists();
	Build_City();
	return true;										// Initialization Went OK
}

int look(int x, int y, int dir, int dist) {
	int mod[5] = {0,1,0,-1,0};
	for(int i=0; i<dist; i++)
	{
		y += mod[dir+1];
		x += mod[dir];
		if (x>249 || x<1 || y>249 || y<1) {
			return 3;
		} else { 
			if (city[x][y] == 1)
				return 3;
		}
		if(tracker[x][y] != -1) {
			return tracker[x][y];
		}
	}
	return -1;
}

int Count_Zombies() {
	int z = 0;
	for(int i = 0; i < population; ++i) {
		if(being[i].state == 2) ++z;
	}
	return z;
}

void Move_Beings() {
	int mod[5] = {0,1,0,-1,0};
	for(int i = 0; i < population; ++i) {
		switch(being[i].state) {
		case 0:
			being[i].move += 0.05f * framelength;
			break;
		case 1:
			being[i].move += 0.1f * framelength;
			break;
		case 2:
			being[i].move += 0.01f * framelength;
		}
		if(being[i].move > 1.0f) {	//AI Time!
			being[i].move = 0.0f;
			if(being[i].moving) {
				being[i].xPos += mod[being[i].dir];
				being[i].yPos += mod[being[i].dir+1];
			}
			tracker[being[i].xPos][being[i].yPos] = -1;
			being[i].moving = false;
			int r = rand() % 10;
			int target[2] = {look(being[i].xPos, being[i].yPos, being[i].dir, 1), 
				look(being[i].xPos, being[i].yPos, being[i].dir, 10)};
			switch(being[i].state) {
			case 0:
				if(being[i].active == 0 && r < 5) {
					being[i].dir = rand() % 4;
					being[i].active = 10;
					if(look(being[i].xPos, being[i].yPos, being[i].dir, 1) != -1)
						being[i].moving = false;
				}
				being[i].moving = true;
				if(look(being[i].xPos, being[i].yPos, being[i].dir, 1) != -1) {
					being[i].dir = rand() % 4;
					being[i].moving = false;
				}
				if(being[i].active > 0) --being[i].active;
				if(target[1] == 2 || target[1] == 1) {
					being[i].state = 1;
					being[i].active = 10;
				}
				break;
			case 1:
				if(being[i].active == 0 && r < 5) {
					being[i].dir = rand() % 4;
					being[i].active = 10;
					if(look(being[i].xPos, being[i].yPos, being[i].dir, 1) != -1) {
						being[i].moving = false;
						if(look(being[i].xPos, being[i].yPos, being[i].dir, 1) == 2) {
							being[i].state = 2;
							zombies = Count_Zombies();
							being[i].flash = 1.0f;
						}
					}
				}
				being[i].moving = true;
				if(look(being[i].xPos, being[i].yPos, being[i].dir, 1) != -1) {
					being[i].dir = rand() % 4;
					being[i].moving = false;
					if(target[0] == 2) {
						being[i].state = 2;
						zombies = Count_Zombies();
						being[i].flash = 1.0f;
					}
				}
				if(being[i].active > 0) --being[i].active;
				if(target[1] == 2) {
					being[i].dir += 2;
					if(being[i].dir > 3) {
						being[i].dir -= 4;
						if(look(being[i].xPos, being[i].yPos, being[i].dir, 1) != -1) {
							being[i].dir = rand() % 4;
							being[i].moving = false;
							if(look(being[i].xPos, being[i].yPos, being[i].dir, 1) == 2) {
								being[i].state = 2;
								zombies = Count_Zombies();
								being[i].flash = 1.0f;
							}
						}
					}
				}
				break;
			case 2:
				if(target[1] == 0 || target[1] == 1)
					being[i].active = 10;
				if(being[i].active == 0) {
					being[i].dir = rand() % 4;
					if(look(being[i].xPos, being[i].yPos, being[i].dir, 1) != -1)
						being[i].moving = false;
				}
				being[i].moving = true;
				if(look(being[i].xPos, being[i].yPos, being[i].dir, 1) != -1) {
					if(target[0] == 1 || target[0] == 0) {
						for(int d = 0; d < population; ++d) {
							if(being[d].xPos == being[i].xPos + mod[being[i].dir] &&
								being[d].yPos == being[i].yPos + mod[being[i].dir+1]) {
								being[d].state = 2;
								zombies = Count_Zombies();
								being[d].moving = false;
								being[i].flash = 1.0f;
								break;
							}
						}
					}
					being[i].dir = rand() % 4;
					being[i].moving = false;
				}
				if(being[i].active > 0) --being[i].active;
				break;
			}
			if(look(being[i].xPos, being[i].yPos, being[i].dir, 1) == 0 ||
				look(being[i].xPos, being[i].yPos, being[i].dir, 1) == 1 ||
				look(being[i].xPos, being[i].yPos, being[i].dir, 1) == 2 ||
				look(being[i].xPos, being[i].yPos, being[i].dir, 1) == 3)
				being[i].moving = false;
			if(being[i].moving) {
				tracker[being[i].xPos + mod[being[i].dir]][being[i].yPos + mod[being[i].dir+1]] 
					= being[i].state;
			} else {
				tracker[being[i].xPos][being[i].yPos] = being[i].state;
			}
			if(being[i].yPos == 0 || being[i].xPos == 0) {
				int x,y;
				do {
					x = rand() % 249 + 1;
					y = rand() % 249 + 1;
				} while(city[x][y] != 0 || tracker[x][y] != -1 || (x > centersquare[0] - 25 &&
					x < centersquare[0] + 25 && y > centersquare[1] - 25 && 
					y < centersquare[1] + 25));
				being[i].xPos = x;
				being[i].yPos = y;
			}
		}
	}
}

void DrawGLScene()									// Here's Where We Do All The Drawing
{
	int i;
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	// Clear Screen And Depth Buffer
	Move_Beings();
	for(i = centersquare[0] - 25; i < centersquare[0] + 25; ++i) {
		for(int d = centersquare[1] - 25; d < centersquare[1] + 25; ++d) {
			if(i > 0 && i < 250 && d > 0 && d < 250) {
				glLoadIdentity();
				glTranslatef(0.0f, 0.0f, -25.0f);
				glRotatef(-30.0f, 1.0f, 0.0f, 0.0f);
				glRotatef( 45.0f, 0.0f, 0.0f, 1.0f);
				glTranslatef(-camerapos[0],-camerapos[1], 0.0f);
				glTranslatef(GLfloat(i), GLfloat(d), 0.0f);
				glColor3f(0.5f, 0.5f, 0.5f);
				glCallList(list[city[i][d]]);
			}
		}
	}
	for(i = 0; i < population; ++i) {
		if(being[i].xPos > centersquare[0] - 25 && being[i].xPos < centersquare[0] + 25 && 
			being[i].yPos > centersquare[1] - 25 && being[i].yPos < centersquare[1] + 25) {
			glLoadIdentity();
			glTranslatef(0.0f, 0.0f, -25.0f);
			glRotatef(-30.0f, 1.0f, 0.0f, 0.0f);
			glRotatef( 45.0f, 0.0f, 0.0f, 1.0f);
			glTranslatef(-camerapos[0],-camerapos[1], 0.0f);
			glTranslatef(GLfloat(being[i].xPos), being[i].yPos, 0.0f);
			switch(being[i].state) {
			case 0:
				glColor3f(0.6f, 0.0f, 0.0f);
				break;
			case 1:
				glColor3f(1.0f, 0.3f, 0.3f);
				break;
			case 2:
				glColor3f(being[i].flash, 1.0f, being[i].flash);
				being[i].flash -= 0.03 * framelength;
				break;
			}
			glRotatef(-90.0f * being[i].dir, 0.0f, 0.0f, 1.0f);
			if(being[i].moving == true) {
				glTranslatef(0.0f, being[i].move, 0.0f);
			}
			glCallList(list[2]);
		}
	}
	if(hud) {
		GLfloat a,b;
		a = GLfloat(population - zombies) / GLfloat(population);	//Red Bar Length
		b = GLfloat(zombies) / GLfloat(population);					//Green Bar Length
		glLoadIdentity();
		glTranslatef(-2.7f, 2.0f,-5.0f);
		glBegin(GL_QUADS);
			glNormal3f(0.0f, 0.0f, 1.0f);
			glColor3f(1.0f, 0.0f, 0.0f);
			glVertex3f(0.0f, 0.03f, 0.0f);
			glVertex3f(0.0f, 0.0f, 0.0f);
			glVertex3f(   a, 0.0f, 0.0f);
			glVertex3f(   a, 0.03f, 0.0f);

			glColor3f(0.0f, 1.0f, 0.0f);
			glVertex3f(0.0f,-0.03f, 0.0f);
			glVertex3f(0.0f, 0.0f, 0.0f);
			glVertex3f(   b, 0.0f, 0.0f);
			glVertex3f(   b,-0.03f, 0.0f);
		glEnd();
	}

	glutSwapBuffers();
}

void update() {
	if(keys[GLUT_KEY_LEFT]) {
		camerapos[0] -= 0.0707f * 4 * framelength;
		camerapos[1] += 0.0707f * 4 * framelength;
		keys[GLUT_KEY_LEFT] = false;
	}
	if(keys[GLUT_KEY_RIGHT]) {
		camerapos[0] += 0.0707f * 4 * framelength;
		camerapos[1] -= 0.0707f * 4 * framelength;
		keys[GLUT_KEY_RIGHT] = false;
	}
	if(keys[GLUT_KEY_UP]) {
		camerapos[0] += 0.0707f * 4 * framelength;
		camerapos[1] += 0.0707f * 4 * framelength;
		keys[GLUT_KEY_UP] = false;
	}
	if(keys[GLUT_KEY_DOWN]) {
		camerapos[0] -= 0.0707f * 4 * framelength;
		camerapos[1] -= 0.0707f * 4 * framelength;
		keys[GLUT_KEY_DOWN] = false;
	}
	centersquare[0] = int(camerapos[0]);
	centersquare[1] = int(camerapos[1]);
	if(keys['Z']) {
		keys['Z'] = false;
		Build_City();
	}
	if(keys['S']) {
		keys['S'] = false;
		GAMESPEED += 0.01f;
		if(GAMESPEED > 0.3) GAMESPEED = 0.02f;
	}
	if(keys['-']) {
		keys['-'] = false;
		population -= 100;
		if(population < 100) population = 100;
		Build_City();
	}
	if(keys['+']) {
		keys['+'] = false;
		population += 100;
		if(population > 4000) population = 4000;
		Build_City();
	}
	if(keys['\t']) {
		keys['\t'] = false;
		hud = !hud;
	}
}

int glutWindow;

void createWindow(char *title, int width, int height, bool fullScreen) {
	glutWindow = glutCreateWindow(title);
	if (fullScreen)
		glutFullScreen();
}

void glutTimerCallback(int pointless) {

//	cout << "Got called.\n";
//	cout.flush();

	bool done = false;

	framelength = float(33) * GAMESPEED;

	update();
	DrawGLScene();

	// Draw The Scene.  Watch For ESC Key And Quit Messages From DrawGLScene()

	if (keys['\e'])	// Active?  Was There A Quit Received?
	{
		done=true;							// ESC or DrawGLScene Signalled A Quit
	}

	if (done)
		exit(0);
}

void glutIdleCallback() {
	glutTimerCallback(0);
}

int main(int argc, char **argv) {

	// Now prepare some stuff

	cout << "Preparing the keyboard array...\n";

	for(int i=0; i<256; ++i) {
		keys[i] = 0;
	}

	// Start GLUT

	cout << "Setting window size...\n";

	glutInitWindowSize(640, 480);

	cout << "Starting glut...\n";

	glutInit(&argc, argv);

	cout << "Setting up displaly mode...\n";

	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE);

	// Get a window, register callbacks

	cout << "Making a window...\n";

	createWindow("Zombie City", 640, 480, false);

	cout << "Setting up callbacks...\n";

	glutDisplayFunc(&DrawGLScene);
	glutReshapeFunc(&ReSizeGLScene);
	glutKeyboardFunc(&glutKeyboardCallback);
	glutSpecialFunc(&glutSpecialCallback);
//	glutTimerFunc(33, &glutTimerCallback, 0);
	glutIdleFunc(&glutIdleCallback);

	// Prepare OpenGL

	cout << "Preparing OpenGL...\n";

	InitGL();

	// Now run!

	cout << "Running main loop...\n";

	glutMainLoop();

	// Now we're done

	return 0;
}