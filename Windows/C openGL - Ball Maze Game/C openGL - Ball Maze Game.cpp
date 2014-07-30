/*
 *  maze_luke_testa.c
 *
 *	Luke Testa
 *	6119412
 *
 *
 Copyright (c) 2014 Luke Marcus Biagio Testa
All rights reserved.

Redistribution and use in source and binary forms are permitted
provided that the above copyright notice and this paragraph are
duplicated in all such forms and that any documentation,
advertising materials, and other materials related to such
distribution and use acknowledge that the software was developed
by the Luke Marcus Biagio Testa. The name of the
Luke Marcus Biagio Testa may not be used to endorse or promote products derived
from this software without specific prior written permission.
THIS SOFTWARE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR
IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 *
 *
 *  Copyright 2012 University of Surrey. All rights reserved.
 *
 *
 *
 *	Making File
 *
 *	Compile the file by navigating to the code's director and enterring the command
 *  'make maze_luke_testa.c' in the command line. The make file will compile the 
 *  file using the openGL libraries and created an executable called 'maze'.
 *  To execute the maze on a linux machine, type the command './maze'.
 *
 *
 *
 *
 * 
 *	Controls:
 *
 *	w, d, a, d ball movement
 *	f = fog on
 *  F = fog off
 *
 * 
 *
 *	Game Menu Add-ons
 *
 *
 *	Lose level/start game => Menu screen ( only working with open game resolution)
 *  Finish three consecutive levels => Menu screen
 *
 * 
 *  
 *  Game Play
 *
 *  Ball off maze => Falling animation - 3 second time penalty  
 *  Two simultaneous ball movements
 *	90 Seconds to complete each maze
 *	Decceleration
 *
 *
 *
 *  Game Levels  Add-ons
 *
 *
 *	3 Levels, must collect all tokens to progress to next level
 *  New level indicated by Red level text
 *
 * 
 *
 *	Tokens and Powerups   Add-ons
 *
 *
 *  Bonus (green) tokens highlighted by green cylinder above green block
 *
 *  Tokens = +2s and +1 spotlight apachure.
 *  Collected bonus acceleration + 2
 *
 *	Rendered text on token/bonus collection indicates powerup achieved
 * 
 *
 *
 *  Lighting  Add-ons
 *
 *
 *  Moving Spotlight
 *  Ball torch light apachure increases with bonuses
 *
 *  cylinder and cube tesselations used to enhance polygon lighting
 *
 * 
 *
 */


#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <time.h>

#ifdef __APPLE__
#include <OpenGL/OpenGL.h>
#include <GLUT/glut.h>
#else
#include <GL/freeglut.h>
#include <GL/gl.h>
#include <GL/glu.h>
#endif

#define MAZE_LENGTH 10																			// Sets size of MxN matrix representing the game environment
#define FURTHEST_X_FLOAT  -5
#define FURTHEST_Z_FLOAT  -5
#define CUBE_LENGTH 1
#define SCREEN_HEIGHT 1280
#define SCREEN_WIDTH 800
#define BALL_RADIUS 0.2
#define CandleMaxHeight 1;
#define CandleRadius 0.5;
#define CandleMinimumHeight 0;
#define PI 3.141592

enum Token { Empty, Blocks, Coin, Powerup };

struct  environment_midpoints															
{
	float x;
	float z;
	enum Token status;
};

typedef struct environment_midpoints midpoints;

midpoints arr[MAZE_LENGTH][MAZE_LENGTH];											// Environment matrix is 2D array where each element points to a point on the screen buffer in the x-z plane. Creates pointer to midpoint structure with environment matrix dimensions

//Lighting array
static GLfloat spot_position[4]; 
static GLfloat spot_direction[4];
static GLfloat moving_spot_direction[4];
static GLfloat ambient_position[] = {0.0, 2.0, 0.0, 0.0};
static GLfloat ambient_color[4] = {0.2,0.2,0.2,1};
static GLfloat mat_specular[] = {0.5, 0.5, 0.5, 0.5};
static GLfloat mat_shininess[] = {50.0};
//static GLfloat position_swinging_light[] = { 0.0, 2.0, 0.0, 0.0};
static GLfloat swinging_color[] = {1,1,1,1};
static float t, Vnew, Vold, a, Pold, Pnew;	

 
// Vertices defining each block's vertices
GLfloat environment_vertices[]= {	-0.5,-0.5,0.5,	-0.5,0.5,0.5,	0.5,0.5,0.5,	0.5,-0.5,0.5,	// front face
									-0.5,-0.5,-0.5,	-0.5,0.5,-0.5,	0.5,0.5,-0.5,	0.5,-0.5,-0.5,	// back face
	
									-0.5,-0.5,0.5,	-0.5,0.5,0.5,	-0.5,0.5,-0.5,	-0.5,-0.5,-0.5,	// Left Face
									0.5,-0.5,0.5,	0.5,0.5,0.5,	0.5,0.5,-0.5,	0.5,-0.5,-0.5,	// Right Face
	//								-0.5,0.5,0.5,	0.5,0.5,0.5,	0.5,0.5,-0.5,	-0.5,0.5,-0.5	// Top Face

								};

// First Level environment. First number indicates the number of X consecutive values (N+1 element)
GLint level_1[] = {	0, 0, 0, 0, 0, 0, 0, 1, 1, 1,
					2, 3, 3, 2, 0, 0, 0, 1, 0, 0,
					1, 0, 0, 1, 0, 0, 0, 3, 0, 0,
					1, 0, 0, 1, 0, 0, 0, 1, 0, 1,
					1, 3, 1, 1, 0, 0, 0, 1, 0, 1,
					0, 0, 0, 1, 1, 1, 1, 3, 0, 1,
					0, 0, 0, 1, 0, 0, 0, 0, 0, 1,
					0, 0, 0, 1, 1, 3, 2, 2, 3, 2,
					0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
					0, 0, 0, 0, 0, 0, 0, 0, 0, 0
					};

GLint level_2[] = {	1, 1, 1, 1, 1, 1, 1, 1, 1, 3,
					3, 1, 1, 1, 0, 0, 0, 1, 0, 0,
					1, 1, 0, 1, 0, 1, 0, 3, 0, 0,
					1, 0, 2, 1, 0, 1, 0, 1, 0, 3,
					1, 3, 1, 1, 0, 0, 0, 1, 0, 1,
					1, 0, 2, 1, 1, 2, 1, 2, 0, 1,
					1, 1, 1, 1, 1, 0, 0, 0, 0, 1,
					0, 0, 0, 1, 1, 2, 1, 1, 1, 3,
					0, 1, 0, 0, 1, 0, 0, 0, 0, 0,
					0, 2, 1, 1, 1, 0, 0, 0, 0, 0
};

GLint level_3[] = {	1, 0, 0, 0, 0, 0, 0, 0, 0, 0,
					1, 0, 0, 3, 3, 3, 0, 0, 0, 0,
					3, 0, 0, 3, 0, 0, 0, 1, 2, 2,
					3, 0, 0, 3, 0, 0, 0, 1, 0, 2,
					1, 0, 0, 3, 0, 0, 0, 2, 0, 2,
					1, 0, 0, 1, 0, 0, 0, 2, 0, 2,
					1, 0, 0, 1, 0, 0, 0, 2, 0, 2,
					2, 2, 1, 1, 1, 1, 1, 1, 0, 2,
					0, 0, 0, 0, 0, 2, 0, 1, 0, 2,
					0, 0, 0, 2, 2, 2, 0, 1, 0, 2
};

// Cylinder (candle) and cube array tesselations
float cylinder[1080];																			// total elements = (Co-ordinates per vertex) x (No Vertices per quad) x (No quads per level) x (No level in cylinder) -> 10 tesselations = 1200
GLfloat cube_top[256];

GLubyte blocks[21];	
GLubyte cylinder_vertex_pointer[360];									// Change with no tesselations
GLubyte top_face[85];


float rotation_amount_z, boolean, rotation_multiplier, rotation_amount_x, token_rot;									// keyboard interaction variables; ball rotation & motion
float ds, dy, movement_x, movement_z, rectified=0; 
clock_t starttime, endtime; 
int lives=3, tokens = 0, spot_angle = 10, start = 0;
float window_X, window_Y;		// Window co-ordinates
double game_time=0;
int Instructions=0, Difficulty;
int level = 1, acceleration_bonus = 0;
int bonus = 0, count=0, pressed=0;
float collected_tokens = 0, collected_bonus = 0;
float Ball_Y,fall_Y=0;
double fall_time=0, fall_start;

/* predefine function names */
void initgl();
void display(void);
void Initialize_Maze_Midpoints(void);
void Level_environment(void);
void timer(int value);
int mechanics(int environment_check);
void keyboard_interaction(unsigned char key, int x, int y);
void rotation(int velocity);
void keyboard_up(unsigned char key, int x, int y);
void ball_movement();
int environment_detection();
void render2dText(char string[], float r, float g, float b, float x, float y);
// Max height = height of cylinder body. Tesselations = Number of Quad tesselations. Radius = radius of single quad face. 
int CandleTesselation(float MaxHeight, int Tesselations, float Minimum, float radius, float angle,  GLfloat *arr);
void initialise_lighting();
void cylinder_tesselation(int tesselations, float Total_Height, float radius);
void cube_tesselation(int tesselations, float length);
void SpecialKeyStrokes(int button, int state, int x, int y);
void display_menu();
void Level_environment_2();
void Level_environment_3();



/* main program - setup window using GLUT */ 
int main(int argc, char **argv)
{
	
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);						// Hidden surface removal renders the scene such that the nearest surface rendered to the viewer is seen as the closest visible surface. By using a depth buffer, each rendered pixel in the depth buffer stores the distance to the previously rendered surface. Depending on the depth mode used, i.e. LESS for instance, if the new surface is closer to the view then the value of thedepth pixel is replaced otherwise is discarded and not rendered.
    glutInitWindowSize(1000, 500);
    glutCreateWindow("3D Maze Game");
    glutDisplayFunc(display);
    initgl();
	glutKeyboardFunc(keyboard_interaction);	
	glutKeyboardUpFunc(keyboard_up);
	glutMouseFunc(SpecialKeyStrokes);
	glutTimerFunc(17, timer, 0); //Add a timer
	glutMainLoop();
    return  0;
	
}

	

/* Initialisation of OpenGL */
void initgl()
{	
	int i, fogMode; 	
		
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glMatrixMode(GL_MODELVIEW);	
	glLoadIdentity();
	
	glClearColor (0,0,0, 0);												// Initialize color matrix

	glEnable(GL_FOG);
	{
		GLfloat fogColor[4] = {0, 0, 0, 1.0};				// BLACK OR GREY???
		
		fogMode = GL_EXP2;										// fog has exponential desntiy
		glFogi (GL_FOG_MODE, fogMode);							// rate of fade mode
		glFogfv (GL_FOG_COLOR, fogColor);						// RGBA colour for fog to fade to
		glFogf (GL_FOG_DENSITY, 0.4);							// density of fog
		glHint (GL_FOG_HINT, GL_DONT_CARE);						//start depth of fog
		glFogf (GL_FOG_START, 10.0);								// end depth of fog (fades to color)
		glFogf (GL_FOG_END, 30.0);
	}
	
    glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);							// glPolygonMode controls the interpretation of polygons for rasterization.	face describes which polygons mode applies to:	front-facing polygons (GL_FRONT),	back-facing polygons (GL_BACK),	or both (GL_FRONT_AND_BACK).	The polygon mode affects only the final rasterization of polygons.	In particular,	a polygon's vertices are lit and	the polygon is clipped and possibly culled before these modes are applied. 	Specifies the polygons that mode applies to.	Must be	GL_FRONT for front-facing polygons,		GL_BACK for back-facing polygons,			or GL_FRONT_AND_BACK for front- and back-facing polygons.
    glShadeModel(GL_SMOOTH);										// Interpolates polygon colours with ratio of each vertex's colours for shading. Minimises pixel variations at boundaries between polygons. // Lighting or colours vary depending on the distances between the polygon's vertex/colour co-ordinates and the light source if lighting is used. AS a result, flat shading colours thewhole polygon according to the first vertex rendered. This has high pixel variations between polygons that is noticable. The smooth shading interpolates each vertex's colour based on the colour ratio at each vertex, splitting anad colouring the whole polygon based on equal areas between each vertex. The result is a much more smoothed colouring/shading

	
	glEnable(GL_DEPTH_TEST);								
	glDepthFunc(GL_LEQUAL);															// renders scene. If a face being rendered in front of the camera is less than or equal to the nearest rendered object, it is not drawn
	glClearDepth(1.0);																// sets value to initialize depth buffer to
	
	
	glEnableClientState(GL_VERTEX_ARRAY);											// Enables openGL vertex pointer for list drawing
	Initialize_Maze_Midpoints();													// Function intializes position of empty blocks/tokens in the environment	
	Level_environment();
	cylinder_tesselation(10,1,0.1);
	cube_tesselation(3,1);

	
	for(i =0; i<21;i++)															// Sets each element in deference list of array elements to equal number of vertices in cube array (environment_vertices)
		blocks[i]=i;															// blocks is essentially an array of structures where each struct contains each x,y,z co-ordinate of cube vertices
																				// Set correct midpoints in environment to have a block
	for(i=0; i<300; i++)								// Cylinder openGL Vertex Pointer - Change with No Tesselations
		cylinder_vertex_pointer[i] = i;
	
	for(i=0; i<85; i++)
		top_face[i]=i;
	
}
	 

/* display function called by OpenGL */
void display(void)
{
	int i, j, Ball_Height;
	float camerax, cameraz, lookatx, lookatz;
	static float current_level = 1;

	
	char Time[] = "Time: ", Tokens[] = "Tokens";	
	Ball_Height = BALL_RADIUS + (CUBE_LENGTH/2);
	Ball_Y = Ball_Height - fall_Y;
	
	// to access raster pixel co-ordinates must reset modelview and projection matrices. Thus calls the identity matrix for each then resets the modelview matrix with the perspective camera 
	glLoadIdentity();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glDisable(GL_LIGHTING);

	
	// Game menus in Orthographic mode
	if(start == 0)
	{
		display_menu();
		return;
	}
	

	
	// Render +4 seconds when token is collected
	if( (collected_tokens < tokens) )
	{
		render2dText("t+4s", 1,1,1, 0, 0);
		collected_tokens += 0.02;
	}
	
	// Render +4 second and plus +2 acceleration if ball collects bonus token
	if(collected_bonus < bonus)
	{
		render2dText("a+2", 0,1,0, -0.15, 0);
		collected_bonus += 0.02;
	}
	
	
	// Flash level text to indicate new level
	if( current_level < level)
	{
		render2dText("Level", 1,0,0, -0.2, 0.9);
		render2dText("NEW LEVEL", 1,0,0, -0.1, 0);
		current_level += 0.005;
	}
	else
		render2dText("Level", 1,1,1, -0.2, 0.9);
		
	if(fall_time>0.5)
		render2dText("Lives-1", 1,0,0, 0, 0);


	
	

	// Render text on raster
	render2dText(Time, 1,1,1, 0.55, 0.9);										// Time Information	
	render2dText(Tokens, 1,1,1, -0.6, 0.9);										// Collected token information
	render2dText("Tokens:", 1,1,1, -0.76, 0.9);
	render2dText("Levels", 1,1,1, -0.05, 0.9);
	render2dText("Live", 1,1,1, -0.8, 0.9);

	
	//Render Lives Icon. Enable Orthographic2D for ball icon.
	glPushMatrix();
	glColor3f(1,1,1);
			gluOrtho2D( -1, 1, -1, 1);
			glTranslatef(-0.9,0.93,0);
						glRotatef(90, 1, 0, 0);
								glutWireSphere(0.06, 8, 8);
	glPopMatrix();
	
	
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT1);
	glEnable(GL_LIGHT2);
	glEnable(GL_LIGHT0);										// enables modifying the coefficients for the first light source matrix
	
	camerax = (1.5*sin( (rotation_amount_x*PI)/180 )) + movement_x;										// calculate camera movement with ball. Minimum z set as 2
	cameraz = (1.5*cos( (rotation_amount_x*PI)/180 )) + movement_z;	
	lookatz = (-1*cos( (rotation_amount_x*PI)/180 )) + movement_z;
	lookatx = (-1*sin( (rotation_amount_x*PI)/180 )) + movement_x;	
	
	// Set up lighting to move with ball
	spot_direction[0] = ((-1)*lookatx);
	spot_direction[1] = 1;
	spot_direction[2] = (((-1)*lookatz));
	//spot_direction[3] = 0;
	spot_position[0] = lookatx;
	spot_position[1] = 1;
	spot_position[2] = lookatz;
	spot_position[3] = 0;
	moving_spot_direction[0] = rectified;
	moving_spot_direction[1] = 1;
	moving_spot_direction[2] = 0;
	moving_spot_direction[4] = 0;
	
	
	
	//Disable Ortho for scene. Enable perspective for modelview matrix to follow ball
	gluPerspective(60, (SCREEN_HEIGHT/SCREEN_WIDTH), 1, 30);											// (ORTHO) defines bottom left/top right co-ordinates of the camera frustum. z depth is determined for rendering in the other inputs (near and far). Depth and height is inifinite in orthographic so model is stretched so much to be perceived as 2D. Ortho = 3D orthographic 2D modelling thus ortho already has a cube frustum. L,R,UL,UR co-ordinates only needed as it creates a square frustum in all planes. PErspective is a pyramid frustum without infinite depth and height/width planes (thus not 2D as everything is not scaled infinitely in all planes). Thus as the pyramid is scaled to a cube, the model's depth causes variations in it's H and W values creating a 3D perspective
	
	gluLookAt(camerax, (3.8-fall_Y), cameraz,  	lookatx, 0, lookatz,	0, 1, 0);
	//gluLookAt(camerax, 20, cameraz,   lookatx, 0, lookatz,    0,1,0);
	
	initialise_lighting();

	
	// Draw Maze Environment blocks. Includes token and powerup drawing	
	for(i=0;i<MAZE_LENGTH;i++)
	{
		for(j=0;j<MAZE_LENGTH;j++)
		{
			// Draw Environment blocks																// As LEQUAL depth buffer is on, draws environment cube from top -> front -> back. As front<top, the front and top is rendered. The remaining faces do not satisfy the depth condition thus are not drawn.
			if(arr[i][j].status != Empty)
			{
				if(arr[i][j].status == Powerup)
					glColor3f(0,1,0);
				else
					glColor3f(0.5,0.5,0.5);
				glPushMatrix();
				glTranslatef(arr[i][j].x,0,arr[i][j].z);											// Translate origin to square midpoint for drawing
						glTranslatef(0, 0.5,0);
								glVertexPointer(3,GL_FLOAT,0,cube_top);							// Stores environment_vertices' storage in openG Vertex pointer (co-ordinates per vertices, type, and array pointer name)
								glDrawElements(GL_QUADS,85,GL_UNSIGNED_BYTE,top_face);						// Deferences list of aarray elements. Second array used to draw primitives (mode = primitive type, count = No elements to render, indice type, second array name)
				glPopMatrix();
				
					
				glPushMatrix();
						glTranslatef(arr[i][j].x,0,arr[i][j].z);											// Translate origin to square midpoint for drawing
										glVertexPointer(3,GL_FLOAT,0,environment_vertices);							// Stores environment_vertices' storage in openG Vertex pointer (co-ordinates per vertices, type, and array pointer name)
										glDrawElements(GL_QUADS,21,GL_UNSIGNED_BYTE,blocks);						// Deferences list of aarray elements. Second array used to draw primitives (mode = primitive type, count = No elements to render, indice type, second array name)
				
										// draw token
										if(arr[i][j].status == Coin)										// If coin, draw block, translate to coin origin and draw coin polygon
										{
											glColor3f(1,1,1);
											glTranslatef(0, 0.4, 0);
													glRotatef(token_rot,0,1,0);
															glVertexPointer(3,GL_FLOAT,0,cylinder);							// Stores environment_vertices' storage in openG Vertex pointer (co-ordinates per vertices, type, and array pointer name)
															glDrawElements(GL_QUADS,200,GL_UNSIGNED_BYTE,cylinder_vertex_pointer);
										}
				// draw Powerup
				if(arr[i][j].status == Powerup)										// If coin, draw block, translate to coin origin and draw coin polygon
				{
					glColor3f(0,1,0);
					glTranslatef(0, 0.4, 0);
					glRotatef(token_rot,0,1,0);
					glVertexPointer(3,GL_FLOAT,0,cylinder);							// Stores environment_vertices' storage in openG Vertex pointer (co-ordinates per vertices, type, and array pointer name)
					glDrawElements(GL_QUADS,200,GL_UNSIGNED_BYTE,cylinder_vertex_pointer);
				}
							
				glPopMatrix();																		// Restore original modelmatrix
			}
		}
	}
	
	
	// Draw and animate ball	
	glColor3f(1,1,1);
	Ball_Y = Ball_Height - fall_Y;

	glDisable(GL_DEPTH_TEST);																// As scene is first drawn, LEQUAL is enabled. The scene is drawn before the sphere's co-ordinates. Thusthe depth buffer is disabled to render the cube and then re-enabled for the environment
	glPushMatrix();
			glTranslatef(movement_x,	Ball_Y,  movement_z);							// move from mid-point (origin) to ball origin
					glRotatef(rotation_amount_x, 0, 1, 0);
					if(boolean == 1)
						glRotatef( rotation_amount_z, 1, 0, 0);	// Ball Rotation
					glutWireSphere(BALL_RADIUS, 8, 8);
	glPopMatrix();
	glEnable(GL_DEPTH_TEST);
	
	glutSwapBuffers();	
}


// Render Menu Screen
void display_menu()
{
	if(Instructions == 0 && Difficulty == 0)												// While Instructions == 0, Not in Instructions SubWindow
	{
		render2dText("Luke Testa's Amazing 3D Maze Game - Escape", 1,0,0, -0.4, 0.5);
		render2dText("Start Game", 1,1,1, -0.1, 0);
		render2dText("Instructions", 1,1,1, -0.1, -0.2);
		render2dText("Exit Game", 1,1,1, -0.1, -0.4);
	}
	
	if(Instructions == 1 && Difficulty == 0)
	{
		render2dText("a = Turn Left, d = Turn Right", 0,0,1, -0.2, 0);
		render2dText("w = Move Forward, s = Move Backwards", 0,0,1, -0.3, -0.2);
		render2dText("Esc = Exit Game", 0,0,1, -0.1, -0.4);
		render2dText("Back", 1,1,1, 0, -0.7);
	}
	
	
	//		gluOrtho2D(-1, 1, -1, 1);	// width of menu, height of menu;
	glutSwapBuffers();
}


// Initialise and enable lighting
void initialise_lighting()
{	
	
	// Moving overhead lamp
	glLightfv(GL_LIGHT1, GL_SPOT_DIRECTION, moving_spot_direction);
	glLighti(GL_LIGHT1, GL_SPOT_CUTOFF, 35);					// Sets up a light source n, the typ of light used; diffuse for reflections, ambient is monopole distribution, position and spot is directional. Final input sets the position of the light in the x,y,z plane with attenuation factor alpha.
	glLightfv(GL_LIGHT1, GL_AMBIENT, swinging_color);
	glLightfv(GL_LIGHT1, GL_POSITION, ambient_position);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, swinging_color);
	
	// Torch Light (Spotlight)
	
	glLightfv(GL_LIGHT0, GL_SPOT_DIRECTION, spot_direction);
	glLightfv(GL_LIGHT0, GL_POSITION, spot_position);
	glLighti(GL_LIGHT0, GL_SPOT_CUTOFF, 25);		// change to spot_angle
	glLightf(GL_LIGHT0, GL_LINEAR_ATTENUATION, 0.1);
	glLightfv(GL_LIGHT0, GL_AMBIENT, swinging_color);															// directional light shading does not take into account the ordering of surfaces relative to the viewer.


	
	// Material Reflective material
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, mat_specular);			// Material affects the surface properties of polygons with lighting. The first input is the polygon back or front faces to the camera being affected. The second input is the parameter of each light source modified, ie. the specular or shininess (reflectivenesS) of a material under lighting. The final input is the co-ordinates of the lighting material.
	glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, mat_shininess);	
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ambient_color);
    glColorMaterial(GL_FRONT_AND_BACK,GL_AMBIENT_AND_DIFFUSE);
	
	
	glEnable(GL_COLOR_MATERIAL);

}


void cylinder_tesselation(const int tesselations, float Total_Height, float radius)
{
	int i=0, j=0;	float angle = 0, height;
	int* x, *y, *z;

	x = (int*) malloc( sizeof(int)*tesselations );
	y = (int*) malloc( sizeof(int)*tesselations );
	z = (int*)malloc( sizeof(int)*tesselations );

	height = Total_Height/tesselations;
	
	// find x,z of each level's point
	while(angle<360)
	{
		angle += 360/tesselations;
		z[i] = radius*sin((angle*PI)/180);
		x[i] = radius*cos((angle*PI)/180);
		i++;
	}
	
	// For 5 tesselations, total vertices = 100 and co-ordinates = 300
	
	// find height of each level. First level = 0, top = max_height
	for(i=0;i<tesselations;i++)
	{
		y[i] = height*i;
	}
	y[i+i] = height;
	
	
	// Store all found x,y,z in co-ordinate array	
	int element = 0;
	for(i=0;i<tesselations;i++)
	{
		for(j=0;j<(tesselations-1);j++)
		{
			cylinder[element++] = x[j];
			cylinder[element++] = y[i];
			cylinder[element++] = z[j];
			
			cylinder[element++] = x[j];
			cylinder[element++] = y[i+1];
			cylinder[element++] = z[j];
			
			cylinder[element++] = x[j+1];
			cylinder[element++] = y[i+1];
			cylinder[element++] = z[j+1];
			
			cylinder[element++] = x[j+1];
			cylinder[element++] = y[i];
			cylinder[element++] = z[j+1];
		}
		
		cylinder[element++] = x[j];
		cylinder[element++] = y[i];
		cylinder[element++] = z[j];
		
		cylinder[element++] = x[j];
		cylinder[element++] = y[i+1];
		cylinder[element++] = z[j];
		
		cylinder[element++] = x[0];
		cylinder[element++] = y[i+1];
		cylinder[element++] = z[0];
		
		cylinder[element++] = x[0];
		cylinder[element++] = y[i];
		cylinder[element++] = z[0];
			
	}
	
	free(x);
	free(y);
	free(z);
}	


// Tesselates one face. The face is then translated and rotated to create a whole cube
void cube_tesselation(int tesselations, float length)
{
	
	int counter=1, i, j,	t_per_length=1;	float t_length, new_z_length, new_x_length;
	
	t_length = length;
	
	if(tesselations<3)
		tesselations = 3;
	
	do
	{		
		t_per_length *= 2;
		t_length /= 2;		
		counter++;
	}
	while(counter!=tesselations);
		
	new_z_length = new_x_length = t_length;
	
	int elements = 0;
	for(i=0; i<t_per_length; i++)
	{		
		for(j=0; j<t_per_length; j++)
		{			
			cube_top[elements++] = (-length/2) + new_x_length - t_length;
			cube_top[elements++] = 0;
			cube_top[elements++] = (-length/2) + new_z_length - t_length;
			
			cube_top[elements++] = (-length/2) + new_x_length - t_length;
			cube_top[elements++] = 0;
			cube_top[elements++] = (-length/2) + (new_z_length + t_length) - t_length;
			
			cube_top[elements++] = (-length/2) + (new_x_length + t_length) - t_length;
			cube_top[elements++] = 0;
			cube_top[elements++] = (-length/2) + (new_z_length + t_length) - t_length;
			
			cube_top[elements++] = (-length/2) + (new_x_length + t_length) - t_length;
			cube_top[elements++] = 0;
			cube_top[elements++] = (-length/2) + new_z_length - t_length;
			
			new_x_length += t_length;
				
		}
		
		new_x_length = t_length;
		new_z_length += t_length;
	}
	
}
	
	
	

void render2dText(char* string, float r, float g, float b, float x, float y)
{
	
	unsigned int i;
	char time[7], live[2], Coin[3], levels[2];
																		
	glColor3f(r, g, b);
	glRasterPos2f(x, y);														// check reading list on safari. I think its 2D co=ordinates of window with depth buffer set to 0. Varies the pixel colours manually before enabling the model and projection matrices
	
			   
	if( strcmp("Live",string) == 0)
	{
		_snprintf_s(live, sizeof(live), "%i", lives);
		glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, live[0]);
	}
	
	else if (strcmp("Levels",string) == 0)
	{
		_snprintf_s(levels, sizeof(levels), "%i", level);
		glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, levels[0]);
	}
	
	else if( strcmp("Tokens",string) == 0)
	{
		_snprintf_s(Coin, sizeof(Coin), "%i", tokens);
		
		for(i=0; i < strlen(Coin); i++)
			glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, Coin[i]);
	}
	
	else if( strcmp("Time: ",string) == 0)
	{
		endtime = clock();
		game_time = (( (double)(endtime - starttime) )/(CLOCKS_PER_SEC*60))*100;	

		printf("%f",(90 - game_time + (4*tokens)));
		_snprintf_s(time, sizeof(time), "%.1f", (90 - game_time + (4*tokens)));
		
//		if(game_time >= (60 + tokens))														// 1 collected token = +1 seconds
//			exit(1);
		
		for(i = 0; i < strlen(string); i++)
			glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, string[i]);
		for(i=0; i < strlen(time); i++)
			glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, time[i]);
	}
	else 
	{
		for(i = 0; i < strlen(string); i++)
			glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, string[i]);
	}
}


// Initialize environment matrix midpoints
void Initialize_Maze_Midpoints()
{	
	int i, j;
	
	for(i=0;i<MAZE_LENGTH;i++)
	{		
		for(j=0;j<MAZE_LENGTH;j++)
		{
			arr[i][j].z = FURTHEST_Z_FLOAT + (i*CUBE_LENGTH);
			arr[i][j].x = FURTHEST_X_FLOAT + (j*CUBE_LENGTH);
			
			arr[i][j].status = Empty;
		}
	}
}


// Create level environment for drawing. Set correct blocks to a value
void Level_environment()
{
	int i, j, n=0;
	level = 1;
	
	for(i=0;i<MAZE_LENGTH;i++)
	{
		for(j=0;j<MAZE_LENGTH;j++)
		{
			if(level_1[n] == 1)
				arr[i][j].status = Blocks;
			if(level_1[n] == 2)
				arr[i][j].status = Coin;
			if(level_1[n] == 3)
				arr[i][j].status = Powerup;
			n++;
		}
	}
}

void Level_environment_2()
{
	int i, j, n=0;
	level = 2;
	
	for(i=0;i<MAZE_LENGTH;i++)
	{
		for(j=0;j<MAZE_LENGTH;j++)
		{
			if(level_2[n] == 1)
				arr[i][j].status = Blocks;
			if(level_2[n] == 2)
				arr[i][j].status = Coin;
			if(level_2[n] == 3)
				arr[i][j].status = Powerup;
			n++;
		}
	}
}


void Level_environment_3()
{
	int i, j, n=0;
	level = 3;
	
	for(i=0;i<MAZE_LENGTH;i++)
	{
		for(j=0;j<MAZE_LENGTH;j++)
		{
			if(level_3[n] == 1)
				arr[i][j].status = Blocks;
			if(level_3[n] == 2)
				arr[i][j].status = Coin;
			if(level_3[n] == 3)
				arr[i][j].status = Powerup;
			n++;
		}
	}
}


// timer update with ball's mechanics calculated
void timer(int value)
{
	
	float velocity; int on_environment;
	static float t = 0;
	
	// Controls swinging light's motion
	t += 1;
	rectified = sin( (t*PI)/180);
	
	if(start==0)
		starttime = clock();
		
	on_environment = environment_detection();	
	if(on_environment==0)
	{
		
		double fall_current; 

		// Render ball falling animation if ball out of maze boundaries
		if(fall_time < 3)
		{
			fall_Y += 0.05;
			fall_current = clock();
			fall_time = (( (double)(fall_current - fall_start) )/(CLOCKS_PER_SEC*60))*1000;	
		}
		else
		{
			lives--;
			count = 0;										// reset falling time to 0;
			movement_x = movement_z = ds = dy = fall_Y = 0;
			fall_time = 0;
		}
	}
	
	if( (tokens==12) && (level == 1) )
	{
		spot_angle = 10;
		lives = 3;											// Reset gameplay
		tokens = 0;
		game_time = 0;
		start = 1;
		movement_z = 0;										// reset mouse position
		movement_x = 0;
		ds = dy = 0;
		Level_environment_2();								// Initialise level
		spot_angle = 10;									// reset spot light from bonuses
		tokens = 0;
		acceleration_bonus = 0;
		collected_tokens = 0;
		collected_bonus = 0;
		bonus = 0;
		starttime = clock();
	}
	
	if( (tokens==12) && (level == 2) )
	{
		lives = 3;											// Reset gameplay
		tokens = 0;
		game_time = 0;
		collected_tokens = 0;
		collected_bonus = 0;
		start = 1;
		movement_z = -5;										// reset mouse position
		movement_x = -5;
		ds = dy = 0;
		Level_environment_3();								// Initialise level
		spot_angle = 10;									// reset spot light from bonuses
		tokens = 0;
		acceleration_bonus = 0;
		bonus = 0;
		starttime = clock();
	}
	
	
	if((tokens==26) && (level == 3))
	{
		lives = 3;											// Reset gameplay
		tokens = 0;
		game_time = 0;
		start = 0;
		collected_tokens = 0;
		collected_bonus = 0;
		movement_z = 0;										// reset mouse position
		movement_x = 0;
		ds = dy = 0;
		Level_environment();								// Initialise level
		spot_angle = 10;									// reset spot light from bonuses
		tokens = 0;
		acceleration_bonus = 0;
		bonus = 0;
		starttime = clock();

	}
			
	velocity = mechanics(on_environment);
	rotation(velocity);
	ball_movement();
	
	if(lives==0 || ( 90 - game_time + (4*tokens) ) <= 0)
		start = 0;
		
	glutPostRedisplay();
	glutTimerFunc(17, timer, 0);						//glutTimerFunc(unsigned int msecs, Timer, 0). 1/60 = 0.017ms for 60 fps

}	


// All in-game kinematics
int mechanics(int environment_check)
{	
		
	static int stop = 0;
	
	// force no key press to a = 0;
	
	if( (stop == 1) && ( ds < 0.001 && ds > (-0.001)) )
	{
		stop = Vnew = Vold = a = Pold = Pnew = 0;
	}
	
	if(rotation_multiplier>0)						// acceleration polarity 
		a = 2+acceleration_bonus;
	else if(rotation_multiplier<0)
		a = -2-acceleration_bonus;
	if (pressed!=0)
	{
		rotation_multiplier =0;
		a = a*(-2);								// if no key pressed deccelerate
		pressed = 0;
		stop=1;
	}
	
	
	// if ball is falling, no ball motion in x,z direction (count = 1 if ball falling)
	if(fall_time > 0.5)
	{
		Vnew = Vold = a = 0;
	}
			
	t = 0.0167;										// Ball mechanics
	Vnew = Vold+(t*a);
	Pnew = Pold + (t*Vnew);
	Vold = Vnew;
	ds = (Pnew/10);									// restrict ball movement
		
	return Vnew;
	
}


// Uses mechanics variables and trigonometry to calculate ball movement
void ball_movement()
{
	float ans, dx, dz;
	
	ans = sin( (rotation_amount_x*PI)/180 );
	dx = ds*ans;													// convert sin(angle) -> degrees and compute origin_ball_x after diagonal motion	
	movement_x += dx;
	
	
	ans = cos( (rotation_amount_x*PI)/180 );						// convert sin(angle) -> degrees and compute origin_ball_z after diagonal motion
	dz = ds*ans;
	movement_z += dz;
}


// Calculates vertical ball rotation
void rotation(int velocity)
{
	// Controls ball rotation in correct direction of motion		
	if( boolean == 1)																		// Conditions for rotation to occur; Key must be pressed
		rotation_amount_z += velocity;
	
	if(rotation_amount_z > 360)
		rotation_amount_z -= 360;
	else if(rotation_amount_z < -360)
		rotation_amount_z += 360;
	
	token_rot += 1;
}


// checks to see if block is still within the environment. Returns 0 = Not in block, 1 = on blck, 2 = on token, 3 = on powerup
int environment_detection()
{
	float max_x, min_x, min_z, max_z, width = 0.5; 
	static int foundx=5, foundz=5;
	
	min_x = arr[0][0].x - CUBE_LENGTH;
	min_z = arr[0][0].z - CUBE_LENGTH;
	max_x = (arr[MAZE_LENGTH-1][MAZE_LENGTH-1].x) + (CUBE_LENGTH);
	max_z = (arr[MAZE_LENGTH-1][MAZE_LENGTH-1].z) + (CUBE_LENGTH);
	
	
	// Check environment boundaries
	
	if( (min_x > movement_x || min_z > movement_z) || (max_x < movement_x || max_z < movement_z) )
	{	
		foundx = foundz = 5;														// reset to origin
		return 0;
	}
		
	// starting position in level is at origin, (0,0)								// (z,x)
	// check 9 squares surrounding square
	
	float centerPointOffsetX = -5.0;
	float centerPointOffsetZ = -5.0;
	foundx = (int)(movement_x - centerPointOffsetX + width);
	foundz = (int)(movement_z - centerPointOffsetZ + width);
	
	if(arr[foundz][foundx].status == Blocks)
		return 1;
	
	else if(arr[foundz][foundx].status == Coin)
	{
		arr[foundz][foundx].status = Blocks;
		//spot_angle += 5;
		tokens++;
		return 2;
	}
	else if(arr[foundz][foundx].status == Powerup)
	{
		arr[foundz][foundx].status = Blocks;
		//spot_angle += 5;
		tokens++;
		bonus++;
		acceleration_bonus += 2;
		return 3;
	}
	
	foundx = foundz = 5;															// reset to origin
	
	if(count==0)					// count is forced 1 when ball starts to fall off environment
	{
		fall_start= clock();
		count++;
	}								// count forced 0 after ball has finished falling

	
	return 0;
	
}


//Interaction with ASCII keyboard for motion
void keyboard_interaction(unsigned char key, int x, int y)
{
	
	switch(key)
	{
		case 27:															// If esc is pressed, exit game
			exit(1);
		case 'w':
			boolean = 1;
			rotation_multiplier = -1;
			break;
		case 's':
			boolean = 1;																	// boolean's control the direction of rotation (about y or x = 0). 1 = x-axis rot, 2 = y-axis rot, 0 = no rot
			rotation_multiplier = 1;														// multiplier forces rotation angle positive or negative
			break;
		case 'a':	
			rotation_amount_x += 10;
			break;
		case 'd':
			rotation_amount_x -= 10;
			break;
		case 'f':
			glEnable(GL_FOG);
			break;
		case 'F':
			glDisable(GL_FOG);
			break;
		default:
			break;
	}
	
	
	glutPostRedisplay();
	
}


void keyboard_up(unsigned char key, int x, int y)												// called when key is up
{

	switch(key)
	{
		case 'w':
			pressed = 1;
			break;
		case 's':
			pressed = 1;
			break;
		default:
			break;
	}

	glutPostRedisplay();
}


void SpecialKeyStrokes(int button, int state, int x, int y)
{
	
	if(button == GLUT_LEFT_BUTTON)
	{
		if( (x <= 563 && x> 450) && ( y>229 && y<256) && Instructions == 0)			// If start game pressed, start game
		{	
			lives = 3;											// Reset gameplay
			tokens = 0;
			fall_Y = 0;
			game_time = 0;
			start = 1;
			movement_z = 0;										// reset mouse position
			movement_x = 0;
			ds = dy = 0;
			spot_angle = 10;									// reset spot light from bonuses
			tokens = 0;
			acceleration_bonus = 0;
			bonus = 0;
			collected_bonus = 0;
			collected_tokens = 0;
			starttime = clock();
			if(level == 1)
				Level_environment();								// Initialise level
			else if(level == 2)
				Level_environment_2();
			else {
				Level_environment_3();
			}

		}
		
		if( ((x <= 569 && x> 449) && ( y>281 && y<305)) && Instructions == 0 )			// If Instructions pressed, show instructions
			Instructions = 1;
		if( (x>= 491 && x<= 582) && (y>= 407 && y<= 432) && Instructions == 1 )		// If back presse in instructions submenu, go back to main menu
			Instructions = 0;
		
		if( (x>= 448 && x<= 559) && (y>= 331 && y<= 355) && Instructions == 0 )		// If exit game pressed, exit game
			exit(1);
	}
	
	
}

// Fix spotlight bug
// Try stencil buffer if time