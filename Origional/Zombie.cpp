#include <windows.h>		// Header File For Windows
#include <scrnsave.h>		// Header File for Screensaving
#include <math.h>			// Header File for Math
#include <stdio.h>			// Header FIle for Input/Output
#include <stdarg.h>
#include <gl\gl.h>			// Header File For The OpenGL32 Library
#include <gl\glu.h>			// Header File For The GLu32 Library
#include <gl\glaux.h>		// Header File For The Glaux Library
#include <fstream.h>

HDC			hDC=NULL;		// Private GDI Device Context
HGLRC		hRC=NULL;		// Permanent Rendering Context
HWND		hWnd=NULL;		// Holds Our Window Handle
HINSTANCE	hInstance;		// Holds The Instance Of The Application

bool	keys[256];			// Array Used For The Keyboard Routine
bool	keypressed[256];	// Array Used For The Keyboard Routine
bool	active=true;		// Window Active Flag Set To TRUE By Default
bool	fullscreen=true;	// Fullscreen Flag Set To Fullscreen Mode By Default
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

struct being{
	int state;		//0=human, 1=panic, 2=zombie
	int xPos, yPos;	//Coordinates
	int dir;		//Facing
	int active;		//How long it's gonna be running
	GLfloat move;	//how far it is towards the next square
	GLfloat flash;	//Flash of light when they convert to Zombie-hood
	bool moving;
}being[4000];

int city[250][250];	//the City Plan
int tracker[250][250]; //Who's where?

int list[3];

GLvoid BuildLists() {
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

LRESULT	CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);	// Declaration For WndProc

GLvoid ReSizeGLScene(GLsizei width, GLsizei height)		// Resize And Initialize The GL Window
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
	int x,y;
	for(int i = 0; i < pop; ++i) {
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
	for(int i = 0; i < 250; ++i) {
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

int InitGL(GLvoid)										// All Setup For OpenGL Goes Here
{
	srand(GetTickCount());
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
	return TRUE;										// Initialization Went OK
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

int DrawGLScene(GLvoid)									// Here's Where We Do All The Drawing
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	// Clear Screen And Depth Buffer
	Move_Beings();
	for(int i = centersquare[0] - 25; i < centersquare[0] + 25; ++i) {
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
	return true;
}

GLvoid KillGLWindow(GLvoid)								// Properly Kill The Window
{
	if (fullscreen)										// Are We In Fullscreen Mode?
	{
		ChangeDisplaySettings(NULL,0);					// If So Switch Back To The Desktop
		ShowCursor(TRUE);								// Show Mouse Pointer
	}

	if (hRC)											// Do We Have A Rendering Context?
	{
		if (!wglMakeCurrent(NULL,NULL))					// Are We Able To Release The DC And RC Contexts?
		{
			MessageBox(NULL,"Release Of DC And RC Failed.","SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION);
		}

		if (!wglDeleteContext(hRC))						// Are We Able To Delete The RC?
		{
			MessageBox(NULL,"Release Rendering Context Failed.","SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION);
		}
		hRC=NULL;										// Set RC To NULL
	}

	if (hDC && !ReleaseDC(hWnd,hDC))					// Are We Able To Release The DC
	{
		MessageBox(NULL,"Release Device Context Failed.","SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION);
		hDC=NULL;										// Set DC To NULL
	}

	if (hWnd && !DestroyWindow(hWnd))					// Are We Able To Destroy The Window?
	{
		MessageBox(NULL,"Could Not Release hWnd.","SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION);
		hWnd=NULL;										// Set hWnd To NULL
	}

	if (!UnregisterClass("OpenGL",hInstance))			// Are We Able To Unregister Class
	{
		MessageBox(NULL,"Could Not Unregister Class.","SHUTDOWN ERROR",MB_OK | MB_ICONINFORMATION);
		hInstance=NULL;									// Set hInstance To NULL
	}
}

/*	This Code Creates Our OpenGL Window.  Parameters Are:					*
 *	title			- Title To Appear At The Top Of The Window				*
 *	width			- Width Of The GL Window Or Fullscreen Mode				*
 *	height			- Height Of The GL Window Or Fullscreen Mode			*
 *	bits			- Number Of Bits To Use For Color (8/16/24/32)			*
 *	fullscreenflag	- Use Fullscreen Mode (TRUE) Or Windowed Mode (FALSE)	*/
 
BOOL CreateGLWindow(char* title, int width, int height, int bits, bool fullscreenflag)
{
	GLuint		PixelFormat;			// Holds The Results After Searching For A Match
	WNDCLASS	wc;						// Windows Class Structure
	DWORD		dwExStyle;				// Window Extended Style
	DWORD		dwStyle;				// Window Style
	RECT		WindowRect;				// Grabs Rectangle Upper Left / Lower Right Values
	WindowRect.left=(long)0;			// Set Left Value To 0
	WindowRect.right=(long)width;		// Set Right Value To Requested Width
	WindowRect.top=(long)0;				// Set Top Value To 0
	WindowRect.bottom=(long)height;		// Set Bottom Value To Requested Height

	fullscreen=fullscreenflag;			// Set The Global Fullscreen Flag

	hInstance			= GetModuleHandle(NULL);				// Grab An Instance For Our Window
	wc.style			= CS_HREDRAW | CS_VREDRAW | CS_OWNDC;	// Redraw On Size, And Own DC For Window.
	wc.lpfnWndProc		= (WNDPROC) WndProc;					// WndProc Handles Messages
	wc.cbClsExtra		= 0;									// No Extra Window Data
	wc.cbWndExtra		= 0;									// No Extra Window Data
	wc.hInstance		= hInstance;							// Set The Instance
	wc.hIcon			= LoadIcon(NULL, IDI_WINLOGO);			// Load The Default Icon
	wc.hCursor			= LoadCursor(NULL, IDC_ARROW);			// Load The Arrow Pointer
	wc.hbrBackground	= NULL;									// No Background Required For GL
	wc.lpszMenuName		= NULL;									// We Don't Want A Menu
	wc.lpszClassName	= "OpenGL";								// Set The Class Name

	if (!RegisterClass(&wc))									// Attempt To Register The Window Class
	{
		MessageBox(NULL,"Failed To Register The Window Class.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;											// Return FALSE
	}
	
	if (fullscreen)												// Attempt Fullscreen Mode?
	{
		DEVMODE dmScreenSettings;								// Device Mode
		memset(&dmScreenSettings,0,sizeof(dmScreenSettings));	// Makes Sure Memory's Cleared
		dmScreenSettings.dmSize=sizeof(dmScreenSettings);		// Size Of The Devmode Structure
		dmScreenSettings.dmPelsWidth	= width;				// Selected Screen Width
		dmScreenSettings.dmPelsHeight	= height;				// Selected Screen Height
		dmScreenSettings.dmBitsPerPel	= bits;					// Selected Bits Per Pixel
		dmScreenSettings.dmFields=DM_BITSPERPEL|DM_PELSWIDTH|DM_PELSHEIGHT;

		// Try To Set Selected Mode And Get Results.  NOTE: CDS_FULLSCREEN Gets Rid Of Start Bar.
		if (ChangeDisplaySettings(&dmScreenSettings,CDS_FULLSCREEN)!=DISP_CHANGE_SUCCESSFUL)
		{
			// If The Mode Fails, Offer Two Options.  Quit Or Use Windowed Mode.
			if (MessageBox(NULL,"The Requested Fullscreen Mode Is Not Supported By\nYour Video Card. Use Windowed Mode Instead?","NeHe GL",MB_YESNO|MB_ICONEXCLAMATION)==IDYES)
			{
				fullscreen=FALSE;		// Windowed Mode Selected.  Fullscreen = FALSE
			}
			else
			{
				// Pop Up A Message Box Letting User Know The Program Is Closing.
				MessageBox(NULL,"Program Will Now Close.","ERROR",MB_OK|MB_ICONSTOP);
				return FALSE;									// Return FALSE
			}
		}
	}

	if (fullscreen)												// Are We Still In Fullscreen Mode?
	{
		dwExStyle=WS_EX_APPWINDOW;								// Window Extended Style
		dwStyle=WS_POPUP;										// Windows Style
	}
	else
	{
		dwExStyle=WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;			// Window Extended Style
		dwStyle=WS_OVERLAPPEDWINDOW;							// Windows Style
	}
	ShowCursor(!fullscreen);									// Hide/Show Mouse Pointer

	AdjustWindowRectEx(&WindowRect, dwStyle, FALSE, dwExStyle);		// Adjust Window To True Requested Size

	// Create The Window
	if (!(hWnd=CreateWindowEx(	dwExStyle,							// Extended Style For The Window
								"OpenGL",							// Class Name
								title,								// Window Title
								dwStyle |							// Defined Window Style
								WS_CLIPSIBLINGS |					// Required Window Style
								WS_CLIPCHILDREN,					// Required Window Style
								0, 0,								// Window Position
								WindowRect.right-WindowRect.left,	// Calculate Window Width
								WindowRect.bottom-WindowRect.top,	// Calculate Window Height
								NULL,								// No Parent Window
								NULL,								// No Menu
								hInstance,							// Instance
								NULL)))								// Dont Pass Anything To WM_CREATE
	{
		KillGLWindow();								// Reset The Display
		MessageBox(NULL,"Window Creation Error.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;								// Return FALSE
	}

	static	PIXELFORMATDESCRIPTOR pfd=				// pfd Tells Windows How We Want Things To Be
	{
		sizeof(PIXELFORMATDESCRIPTOR),				// Size Of This Pixel Format Descriptor
		1,											// Version Number
		PFD_DRAW_TO_WINDOW |						// Format Must Support Window
		PFD_SUPPORT_OPENGL |						// Format Must Support OpenGL
		PFD_DOUBLEBUFFER,							// Must Support Double Buffering
		PFD_TYPE_RGBA,								// Request An RGBA Format
		bits,										// Select Our Color Depth
		0, 0, 0, 0, 0, 0,							// Color Bits Ignored
		0,											// No Alpha Buffer
		0,											// Shift Bit Ignored
		0,											// No Accumulation Buffer
		0, 0, 0, 0,									// Accumulation Bits Ignored
		16,											// 16Bit Z-Buffer (Depth Buffer)  
		0,											// No Stencil Buffer
		0,											// No Auxiliary Buffer
		PFD_MAIN_PLANE,								// Main Drawing Layer
		0,											// Reserved
		0, 0, 0										// Layer Masks Ignored
	};
	
	if (!(hDC=GetDC(hWnd)))							// Did We Get A Device Context?
	{
		KillGLWindow();								// Reset The Display
		MessageBox(NULL,"Can't Create A GL Device Context.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;								// Return FALSE
	}

	if (!(PixelFormat=ChoosePixelFormat(hDC,&pfd)))	// Did Windows Find A Matching Pixel Format?
	{
		KillGLWindow();								// Reset The Display
		MessageBox(NULL,"Can't Find A Suitable PixelFormat.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;								// Return FALSE
	}

	if(!SetPixelFormat(hDC,PixelFormat,&pfd))		// Are We Able To Set The Pixel Format?
	{
		KillGLWindow();								// Reset The Display
		MessageBox(NULL,"Can't Set The PixelFormat.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;								// Return FALSE
	}

	if (!(hRC=wglCreateContext(hDC)))				// Are We Able To Get A Rendering Context?
	{
		KillGLWindow();								// Reset The Display
		MessageBox(NULL,"Can't Create A GL Rendering Context.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;								// Return FALSE
	}

	if(!wglMakeCurrent(hDC,hRC))					// Try To Activate The Rendering Context
	{
		KillGLWindow();								// Reset The Display
		MessageBox(NULL,"Can't Activate The GL Rendering Context.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;								// Return FALSE
	}

	ShowWindow(hWnd,SW_SHOW);						// Show The Window
	SetForegroundWindow(hWnd);						// Slightly Higher Priority
	SetFocus(hWnd);									// Sets Keyboard Focus To The Window
	ReSizeGLScene(width, height);					// Set Up Our Perspective GL Screen

	if (!InitGL())									// Initialize Our Newly Created GL Window
	{
		KillGLWindow();								// Reset The Display
		MessageBox(NULL,"Initialization Failed.","ERROR",MB_OK|MB_ICONEXCLAMATION);
		return FALSE;								// Return FALSE
	}

	return TRUE;									// Success
}

LRESULT CALLBACK WndProc(	HWND	hWnd,			// Handle For This Window
							UINT	uMsg,			// Message For This Window
							WPARAM	wParam,			// Additional Message Information
							LPARAM	lParam)			// Additional Message Information
{
	switch (uMsg)									// Check For Windows Messages
	{
		case WM_ACTIVATE:							// Watch For Window Activate Message
		{
			if (!HIWORD(wParam))					// Check Minimization State
			{
				active=TRUE;						// Program Is Active
			}
			else
			{
				active=FALSE;						// Program Is No Longer Active
			}

			return 0;								// Return To The Message Loop
		}

		case WM_SYSCOMMAND:							// Intercept System Commands
		{
			switch (wParam)							// Check System Calls
			{
				case SC_SCREENSAVE:					// Screensaver Trying To Start?
				case SC_MONITORPOWER:				// Monitor Trying To Enter Powersave?
				return 0;							// Prevent From Happening
			}
			break;									// Exit
		}

		case WM_CLOSE:								// Did We Receive A Close Message?
		{
			PostQuitMessage(0);						// Send A Quit Message
			return 0;								// Jump Back
		}

		case WM_KEYDOWN:							// Is A Key Being Held Down?
		{
			keys[wParam] = true;					// If So, Mark It As TRUE
			return 0;								// Jump Back
		}

		case WM_KEYUP:								// Has A Key Been Released?
		{
			keys[wParam] = false;					// If So, Mark It As FALSE
			keypressed[wParam] = false;
			return 0;								// Jump Back
		}

		case WM_LBUTTONDOWN: {
			mouseclick = true;
			return 0;
		}
		
		case WM_LBUTTONUP: {
			mouseclick = false;
			return 0;
		}
		case WM_MOUSEMOVE: {
			return 0;
		}
		case WM_SIZE:								// Resize The OpenGL Window
		{
			ReSizeGLScene(LOWORD(lParam),HIWORD(lParam));  // LoWord=Width, HiWord=Height
			return 0;								// Jump Back
		}
	}

	// Pass All Unhandled Messages To DefWindowProc
	return DefWindowProc(hWnd,uMsg,wParam,lParam);
}

update() {
	if(keys[VK_LEFT]) {
		camerapos[0] -= 0.0707f * 4 * framelength;
		camerapos[1] += 0.0707f * 4 * framelength;
	}
	if(keys[VK_RIGHT]) {
		camerapos[0] += 0.0707f * 4 * framelength;
		camerapos[1] -= 0.0707f * 4 * framelength;
	}
	if(keys[VK_UP]) {
		camerapos[0] += 0.0707f * 4 * framelength;
		camerapos[1] += 0.0707f * 4 * framelength;
	}
	if(keys[VK_DOWN]) {
		camerapos[0] -= 0.0707f * 4 * framelength;
		camerapos[1] -= 0.0707f * 4 * framelength;
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
	if(keys[VK_SUBTRACT]) {
		keys[VK_SUBTRACT] = false;
		population -= 100;
		if(population < 100) population = 100;
		Build_City();
	}
	if(keys[VK_ADD]) {
		keys[VK_ADD] = false;
		population += 100;
		if(population > 4000) population = 4000;
		Build_City();
	}
	if(keys[VK_TAB]) {
		keys[VK_TAB] = false;
		hud = !hud;
	}
};

int WINAPI WinMain(	HINSTANCE	hInstance,			// Instance
					HINSTANCE	hPrevInstance,		// Previous Instance
					LPSTR		lpCmdLine,			// Command Line Parameters
					int			nCmdShow)			// Window Show State
{
	MSG		msg;									// Windows Message Structure
	BOOL	done=FALSE;								// Bool Variable To Exit Loop

	// Ask The User Which Screen Mode They Prefer
 	fullscreen=true;	// Fullscreen Mode
	if (MessageBox(NULL,"Would You Like To Run In Fullscreen Mode?", "Start FullScreen?",MB_YESNO|MB_ICONQUESTION)==IDNO)
	{
		fullscreen=false;
	}
	// Create Our OpenGL Window
	if (!CreateGLWindow("Zombie City!",640,480,16,fullscreen))
	{
		return 0;									// Quit If Window Was Not Created
	}
	for(int i=0; i<256; ++i) {
		keys[i] = 0;
	}
	i_tick = float(GetTickCount());                 //Set i_tick to the new tick count - so the difference does not exponentially increase
	while(!done)									// Loop That Runs While done=FALSE
	{
		if (PeekMessage(&msg,NULL,0,0,PM_REMOVE))	// Is There A Message Waiting?
		{
			if (msg.message==WM_QUIT)				// Have We Received A Quit Message?
			{
				done=TRUE;							// If So done=TRUE
			}
			else									// If Not, Deal With Window Messages
			{
				TranslateMessage(&msg);				// Translate The Message
				DispatchMessage(&msg);				// Dispatch The Message
			}
		}
		else										// If There Are No Messages
		{
			do
			{
			} while (( GetTickCount() - i_tick ) < 10 );
			framelength = (GetTickCount() - i_tick) * GAMESPEED;    //When the frame starts, find out how many 'ticks' have occurred since the last frame
			if(framelength > 10.0f) framelength = 10.0f;
			leveltime += int(GetTickCount() - i_tick);
			i_tick = float(GetTickCount());                                         //Set i_tick to the new tick count - so the difference does not exponentially increase
			i_frame+=1;
			if ((i_tick_2 + 1000) < GetTickCount())
			{
			    i_frame_rate = i_frame;
			    i_frame=0;
				i_tick_2 = float(GetTickCount());
			};
			update();
			// Draw The Scene.  Watch For ESC Key And Quit Messages From DrawGLScene()
			if ((active && !DrawGLScene()) || keys[VK_ESCAPE])	// Active?  Was There A Quit Received?
			{
				done=TRUE;							// ESC or DrawGLScene Signalled A Quit
			}
			else									// Not Time To Quit, Update Screen
			{
				SwapBuffers(hDC);					// Swap Buffers (Double Buffering)
			}

			if (keys[VK_F1])						// Is F1 Being Pressed?
			{
				keys[VK_F1]=FALSE;					// If So Make Key FALSE
				KillGLWindow();						// Kill Our Current Window
				fullscreen=!fullscreen;				// Toggle Fullscreen / Windowed Mode
				// Recreate Our OpenGL Window
				if (!CreateGLWindow("Zombie City",640,480,16,fullscreen))
				{
					return 0;						// Quit If Window Was Not Created
				}
			}
		}
		if(end) done=true;
	}

	// Shutdown
	KillGLWindow();									// Kill The Window
	return (msg.wParam);							// Exit The Program
}