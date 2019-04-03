/* The following ratios are not to scale: */
/* Moon orbit : planet orbit */
/* Orbit radius : body radius */
/* Sun radius : planet radius */

#ifdef MACOS
  #include <GLUT/glut.h>
#else
  #include <GL/glut.h>
#endif

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define MAX_BODIES 25
#define TOP_VIEW 1
#define ECLIPTIC_VIEW 2
#define SHIP_VIEW 3
#define EARTH_VIEW 4
#define PI 3.141593
#define DEG_TO_RAD 0.01745329
#define ORBIT_POLY_SIDES 50
#define TIME_STEP 0.1   /* days per frame */
#define RUN_SPEED  10000
#define TURN_ANGLE 30.0

bool coordinates_on = false;

typedef struct {
  char    name[25];       /* name */
  GLfloat r,g,b;          /* colour */
  GLfloat orbital_radius; /* distance to parent body (km) */
  GLfloat orbital_tilt;   /* angle of orbit wrt ecliptic (deg) */
  GLfloat orbital_period; /* time taken to orbit (days) */
  GLfloat radius;         /* radius of body (km) */
  GLfloat axis_tilt;      /* tilt of axis wrt body's orbital plane (deg) */
  GLfloat rot_period;     /* body's period of rotation (days) */
  GLint   orbits_body;    /* identifier of parent body */
  GLfloat spin;           /* current spin value (deg) */
  GLfloat orbit;          /* current orbit value (deg) */
 } body;

body  bodies[MAX_BODIES];
int   numBodies, current_view, draw_orbits, draw_labels, draw_starfield;
float  date;

/*****************************/

float myRand (void)
{
  /* return a random float in the range [0,1] */
  return (float) rand() / RAND_MAX;
}


/*****************************/

double rand_stars[3000] = {0};

void drawStarfield (void)
{
  int star;
  for(star = 1; star <= 1000; star++)
  {
    glColor3f(1.0,1.0,1.0);
    glBegin(GL_POINTS); //starts drawing of points
    GLfloat x = rand_stars[star-1] * 600000000;
    GLfloat y = rand_stars[star] * 600000000;
    GLfloat z = rand_stars[star + 1] * 600000000;
    glVertex3f(x,y,z);
    glEnd();//end drawing of points
      //glFlush();
  }
}

void drawCoordinates()
{
  if(coordinates_on)
  {
    float LEN = 1e25f;
    glLineWidth(5.0);

    glBegin(GL_LINES);
    glColor3f(1.0,0.0,0.0); // red
        glVertex3f(0.0, 0.0, 0.0);
        glVertex3f(LEN, 0.0, 0.0);

    glColor3f(0.0,1.0,0.0); // green
        glVertex3f(0.0, 0.0, 0.0);
        glVertex3f(0.0, LEN, 0.0);

    glColor3f(0.0,0.0,1.0); // blue
        glVertex3f(0.0, 0.0, 0.0);
        glVertex3f(0.0, 0.0, LEN);
    glEnd();

    glLineWidth(1.0);
  }
}

/*****************************/

void readSystem(void)
{
  /* reads in the description of the solar system */

  FILE *f;
  int i;

  f= fopen("sys", "r");
  if (f == NULL) {
     printf("ex2.c: Couldn't open 'sys'\n");
     printf("To get this file, use the following command:\n");
     printf("  cp /opt/info/courses/COMP27112/ex2/sys .\n");
     exit(0);
  }
  fscanf(f, "%d", &numBodies);
  for (i= 0; i < numBodies; i++)
  {
    fscanf(f, "%s %f %f %f %f %f %f %f %f %f %d",
      bodies[i].name,
      &bodies[i].r, &bodies[i].g, &bodies[i].b,
      &bodies[i].orbital_radius,
      &bodies[i].orbital_tilt,
      &bodies[i].orbital_period,
      &bodies[i].radius,
      &bodies[i].axis_tilt,
      &bodies[i].rot_period,
      &bodies[i].orbits_body);

    /* Initialise the body's state */
    bodies[i].spin= 0.0;
    bodies[i].orbit= myRand() * 360.0; /* Start each body's orbit at a
                                          random angle */
    bodies[i].radius*= 1000.0; /* Magnify the radii to make them visible */
  }
  fclose(f);
}

GLfloat eyez_new;
GLfloat eyey_new;
GLfloat eyex_new;
GLfloat orb_tilt;
GLfloat orb_angle;
void setView (void) {
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  switch (current_view) {
  case TOP_VIEW:
    gluLookAt(0, 359305000, 0, 0, 0, 0, 1, 0, 0);
    break;
  case ECLIPTIC_VIEW:
    gluLookAt(0, 0, 359305000, 0, 0, 0, 0, 1, 0);
    break;
  case SHIP_VIEW:
    gluLookAt(359305000, 159305000, 359305000, 0, 0, 0, 0, 1, 0);
    break;
  case EARTH_VIEW:
    orb_tilt = bodies[3].orbital_tilt;
    orb_angle = bodies[3].orbit;
    eyex_new = bodies[3].orbital_radius*cos(DEG_TO_RAD*(bodies[3].orbit-1));
    eyey_new = bodies[3].orbital_radius*sin(DEG_TO_RAD*bodies[3].orbital_tilt) + bodies[3].radius;
    eyez_new = bodies[3].orbital_radius*sin(DEG_TO_RAD*(bodies[3].orbit-1))*-1;
    gluLookAt(eyex_new, eyey_new, eyez_new, 0, 0, 0, 0, 1, 0);
    break;
  }
}

/*****************************/

void menu (int menuentry) {
  switch (menuentry) {
  case 1: current_view= TOP_VIEW;
          break;
  case 2: current_view= ECLIPTIC_VIEW;
          break;
  case 3: current_view= SHIP_VIEW;
          break;
  case 4: current_view= EARTH_VIEW;
          break;
  case 5: draw_labels= !draw_labels;
          break;
  case 6: draw_orbits= !draw_orbits;
          break;
  case 7: draw_starfield= !draw_starfield;
          break;
  case 8: exit(0);
  }
}

/*****************************/

void init(void)
{
  /* Define background colour */
  glClearColor(0.0, 0.0, 0.0, 0.0);
  /* Set initial view parameters */

  glutCreateMenu (menu);
  glutAddMenuEntry ("Top view", 1);
  glutAddMenuEntry ("Ecliptic view", 2);
  glutAddMenuEntry ("Spaceship view", 3);
  glutAddMenuEntry ("Earth view", 4);
  glutAddMenuEntry ("", 999);
  glutAddMenuEntry ("Toggle labels", 5);
  glutAddMenuEntry ("Toggle orbits", 6);
  glutAddMenuEntry ("Toggle starfield", 7);
  glutAddMenuEntry ("", 999);
  glutAddMenuEntry ("Quit", 8);
  glutAttachMenu (GLUT_RIGHT_BUTTON);

  current_view= TOP_VIEW;
  draw_labels= 1;
  draw_orbits= 1;
  draw_starfield= 1;
}

/*****************************/

void animate(void)
{
  int i;

    date+= TIME_STEP;

    for (i= 0; i < numBodies; i++)  {
      bodies[i].spin += 360.0 * TIME_STEP / bodies[i].rot_period;
      bodies[i].orbit += 360.0 * TIME_STEP / bodies[i].orbital_period;
      glutPostRedisplay();
    }
}

/*****************************/

void drawOrbit (int n)
{ /* Draws a polygon to approximate the circular
     orbit of body "n" */
     glColor4f(1.0,1.0,1.0, 0.3);
     glBegin(GL_LINE_LOOP);
     int edge;
     for(edge = 0; edge < ORBIT_POLY_SIDES; edge++)
     {
       float theta = 2.0f * 3.1415926f * (float)edge/(float)ORBIT_POLY_SIDES;//get the current angle
       float x = bodies[n].orbital_radius * cosf(theta);//calculate the x component
       float y = bodies[n].orbital_radius * sinf(theta);//calculate the y component
       glVertex2f(x, y);//output vertex
     }
     glEnd();
}

void drawBody(int n)
{
  //Draws body "n"
  if(n==0)
  {
    glLineWidth(1);
    glBegin(GL_LINES);
      glVertex3f(0.0, 1.5*bodies[0].radius, 0.0);
      glVertex3f(0, -1.5*bodies[0].radius, 0.0);
    glEnd();
  }
  if (n<5)
  {
    glRotatef(bodies[n].orbital_tilt, 0.0, 0.0, 1.0);
    glRotatef(bodies[n].orbit, 0.0, 1.0, 0.0);
    glRotatef(90 , 1.0, 0.0, 0.0);
    drawOrbit(n);
    glRotatef(-90 , 1.0, 0.0, 0.0);
    glTranslatef(bodies[n].orbital_radius, 0.0, 0.0);
    glRotatef(bodies[n].axis_tilt, 0.0, 0.0, 1.0);
    glRotatef(bodies[n].spin, 0.0, 1.0, 0.0);
    glRotatef(90 , 1.0, 0.0, 0.0);
    glRotatef(90 , 0.0, 0.0, 1.0);
    glColor3f(bodies[n].r,bodies[n].g,bodies[n].b);
    glutWireSphere(bodies[n].radius, 22, 22);
  }
  if(n > 4 && n < 8)
  {
    GLint parent = bodies[n].orbits_body;
    glRotatef(bodies[n].orbital_tilt, 0.0, 0.0, 1.0);
    glRotatef(bodies[parent].orbit, 0.0, 1.0, 0.0);
    glTranslatef(bodies[parent].orbital_radius, 0.0, 0.0);
    glRotatef(bodies[n].orbit, 0.0, 1.0, 0.0);
    glRotatef(bodies[parent].orbit, 0.0, 1.0, 0.0);
    glRotatef(90 , 1.0, 0.0, 0.0);
    drawOrbit(n);
    glRotatef(-90 , 1.0, 0.0, 0.0);
    glTranslatef(bodies[n].orbital_radius, 0.0, 0.0);
    glRotatef(bodies[n].axis_tilt, 0.0, 0.0, 1.0);
    glRotatef(bodies[n].spin, 0.0, 1.0, 0.0);
    glRotatef(90 , 1.0, 0.0, 0.0);
    glColor3f(bodies[n].r,bodies[n].g,bodies[n].b);
    glutWireSphere(bodies[n].radius, 22, 22);
  }
}


void display(void)
{
  int i;

  glClear(GL_COLOR_BUFFER_BIT);

  /* set the camera */
  setView();

  if (draw_starfield) drawStarfield();

  for (i= 0; i < numBodies; i++)
  {
    glPushMatrix();
      drawBody (i);
    glPopMatrix();
  }

  drawCoordinates();
  glutSwapBuffers();
}


void reshape(int w, int h)
{
  glViewport(0, 0, (GLsizei) w, (GLsizei) h);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  gluPerspective (48.0, (GLfloat)w/(GLfloat)h,  10000.0, 800000000.0);
}//reshape()

/*****************************/

void keyboard(unsigned char key, int x, int y)
{
  switch(key)
  {
    case 27:  /* Escape key */
      exit(0);
    case 97:
      if(!coordinates_on)
        coordinates_on = true;
      else
        coordinates_on = false;
      break;
  }
}

/*****************************/

int main(int argc, char** argv)
{
  int star_nr;
  for (star_nr = 0; star_nr < 3000; star_nr++)
    rand_stars[star_nr] = pow(-1, rand() & 1) * myRand();
  glutInit (&argc, argv);
  glutInitDisplayMode (GLUT_DOUBLE | GLUT_RGB);
  glutCreateWindow ("COMP27112 Exercise 2");
  glutFullScreen();
  init();
  glutDisplayFunc (display);
  glutReshapeFunc (reshape);
  glutKeyboardFunc (keyboard);
  glutIdleFunc (animate);
  readSystem();
  glutMainLoop();
  return 0;
}