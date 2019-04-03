#ifdef MACOSX
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include "bitmap.c"

/*
This is for COMP27112 Course Assignment 3.

Author: Toby Howard, 5/04/12.
 */

GLfloat xRotation, yRotation= 0.0;
GLint mouseX, mouseY;

/*GLfloat white_light[] =     { 1.0, 1.0, 1.0, 1.0 };
GLfloat light_position0[] = { 5.0, 5.0, 5.0, 0.0 };
GLfloat matSpecular[] =     { 1.0, 1.0, 1.0, 1.0 };
GLfloat matShininess[] =    { 50.0 };
GLfloat matSurface[] =      { 0.0, 1.0, 0.0, 0.0 };
*/
GLfloat white_light[] =     { 0.7, 0.2, 0.6, 0.4 };
GLfloat light_position0[] = { 0.0, 1.0, 4.0, 0.0 };
GLfloat matSpecular[] =     { 0.8, 0.7, 1.0, 1.0 };
GLfloat matShininess[] =    { 5.0 };
GLfloat matSurface[] =      { 0.0, 0.3, 0.0, 0.0 };

GLfloat other_light[] =     { 0.0, 0.7, 0.2, 1.0 };

BITMAPINFO *TexInfo; // Texture bitmap information
GLubyte    *TexBits; // Texture bitmap pixel bits
int textured = 0;

void initialise(void) {
   //TexBits = LoadDIBitmap("splodge.bmp", &TexInfo);
   TexBits = LoadDIBitmap("finerthingsflip.bmp", &TexInfo);

   glLightfv(GL_LIGHT0, GL_POSITION, light_position0);
   glLightfv(GL_LIGHT0, GL_DIFFUSE, white_light);
   glLightfv(GL_LIGHT0, GL_SPECULAR, white_light);
   glLightfv(GL_LIGHT0, GL_SPECULAR, other_light);

   glEnable(GL_LIGHTING);
   glEnable(GL_LIGHT0);
   glEnable(GL_DEPTH_TEST);
   //glShadeModel(GL_SMOOTH);
  glShadeModel(GL_SMOOTH);
}

void display(void) {
   glClearColor(1.0, 1.0, 1.0, 0.0);
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

// define material properties
   glMaterialfv(GL_FRONT, GL_SPECULAR,  matSpecular);
   glMaterialfv(GL_FRONT, GL_SHININESS, matShininess);
   glMaterialfv(GL_FRONT, GL_AMBIENT,   matSurface);

   // code to set up texturing...

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexImage2D(GL_TEXTURE_2D, 0, 3, TexInfo->bmiHeader.biWidth,
               TexInfo->bmiHeader.biHeight, 0, GL_BGR_EXT,
               GL_UNSIGNED_BYTE, TexBits);
  //glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
  glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_BLEND);
  glEnable(GL_TEXTURE_2D);


 // draw the teapot
   glPushMatrix();
      glRotatef(xRotation, 1.0, 0.0, 0.0);
      glRotatef(yRotation, 0.0, 1.0, 0.0);
      glutSolidTeapot(0.7);
   glPopMatrix();
   glutSwapBuffers();
}

void reshape(int w, int h) {
   glViewport(0, 0,(GLsizei) w,(GLsizei) h);
   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();
   gluPerspective(17.0, (GLfloat)w/(GLfloat)h, 1.0, 20.0);
   glMatrixMode(GL_MODELVIEW);
   glLoadIdentity();
   gluLookAt(5.0,5.0,5.0, 0.0,0.0,0.0, 0.0,1.0,0.0);
}

void keyboard(unsigned char key, int x, int y) {
   switch(key) {
	  case 27:  exit(0);
                break;
    case 115:
              printf("pressed s\n");
              if(!textured){
                textured = 1;
                glShadeModel(GL_FLAT);
              }
              else{
                textured = 0;
                glShadeModel(GL_SMOOTH);
              }
               glutPostRedisplay();
   }
}

void mouseMotion(int x, int y) {
// Called when mouse moves
	xRotation+=(y-mouseY);	mouseY= y;
	yRotation+=(x-mouseX);	mouseX= x;
	// keep all rotations between 0 and 360.
	if ( xRotation > 360.0) xRotation-= 360.0;
	if ( xRotation < 0.0)   xRotation+= 360.0;
	if ( yRotation > 360.0) yRotation-= 360.0;
	if ( yRotation < 0.0)   yRotation+= 360.0;
	// ask for redisplay
	glutPostRedisplay();
}


void mousePress(int button, int state, int x, int y) {
// When left mouse button is pressed, save the mouse(x,y)
	if((button == GLUT_LEFT_BUTTON) &&(state == GLUT_DOWN)) {
		mouseX= x;
		mouseY= y;
	}
}

int main(int argc, char** argv) {
   glutInit(&argc, argv);
   glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
   glutInitWindowSize(800, 800);
   glutCreateWindow("COMP27112 Coursework Assignment 3: The Teapot");
   glutDisplayFunc(display);
   glutReshapeFunc(reshape);
   glutKeyboardFunc(keyboard);
   glutMouseFunc(mousePress);
   glutMotionFunc(mouseMotion);
   initialise();
   glutMainLoop();
   return 0;
}
