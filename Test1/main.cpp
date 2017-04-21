// texture_sphere_main.cpp
// Mapeo de Texturas a una Esfera en OpenGL

#include <GL/glew.h>
#include <GL/freeglut.h>
#include <iostream>
#include <cmath>

#include "image_loader.h"

using namespace std;

// Tamaño de la ventana
GLint width = 800;
GLint height = 600;

// Parámetros de la cámara virtual
GLint mouseButton = 0;
GLboolean mouseRotate = false;
GLboolean mouseZoom = false;
GLboolean mousePan = false;
GLint xClick = 0;
GLint yClick = 0;
GLint xAux = 0;
GLint yAux = 0;
GLfloat rotX = 0.f;
GLfloat rotY = 0.f;
GLfloat panX = 0.f;
GLfloat panY = 0.f;
GLfloat zoomZ = -10.f;

GLUquadric *quad;
GLuint textureId;
GLfloat rot = 0.;

// Función que inicializa el Contexto OpenGL y la geometría de la escena
void init()
{
	glClearColor(0.f, 0.f, 0.f, 0.f);
	glColor3f(1.f, 1.f, 1.f);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_TEXTURE_2D);

	Image *image = loadBMP("earth.bmp");
	GLuint textureId;
	glGenTextures(1, &textureId); //Make room for our texture
	glBindTexture(GL_TEXTURE_2D, textureId); //Tell OpenGL which texture to edit

	//Map the image to the texture
	glTexImage2D(GL_TEXTURE_2D,                //Always GL_TEXTURE_2D
		0,                            //0 for now
		GL_RGB,                       //Format OpenGL uses for image
		image->width, image->height,  //Width and height
		0,                            //The border of the image
		GL_RGB, //GL_RGB, because pixels are stored in RGB format
		GL_UNSIGNED_BYTE, //GL_UNSIGNED_BYTE, because pixels are stored
		//as unsigned numbers
		image->pixels);               //The actual pixel data
	delete image;
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	quad = gluNewQuadric();
	gluQuadricDrawStyle(quad, GLU_FILL);
	gluQuadricTexture(quad, TRUE);
	gluQuadricNormals(quad, GLU_SMOOTH);
}

// Función que se invoca cada vez que se redimensiona la ventana
void resize(GLint w, GLint h)
{
	if (h == 0)
	{
		h = 1;
	}
	width = w;
	height = h;
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(25.f, (float)width / height, 0.1f, 1000.f);		// Transf. de Proyección en Perspectiva
	glMatrixMode(GL_MODELVIEW);
	glViewport(0, 0, width, height);								// Transf. de Viewport (mapeo al área de dibujo)
}

// Geometría sistema coordenado
void geomCoordSys(GLfloat size)
{
	float currentColor[4];
	glGetFloatv(GL_CURRENT_COLOR, currentColor);

	// Eje X (Rojo)
	glColor3f(1., 0., 0.);
	glBegin(GL_LINES);
	glVertex3f(0., 0., 0.);
	glVertex3f(size, 0., 0.);
	glEnd();

	// Eje Y (Verde))
	glColor3f(0., 1., 0.);
	glBegin(GL_LINES);
	glVertex3f(0., 0., 0.);
	glVertex3f(0., size, 0.);
	glEnd();

	// Eje Z (Azul)
	glColor3f(0., 0., 1.);
	glBegin(GL_LINES);
	glVertex3f(0., 0., 0.);
	glVertex3f(0., 0., size);
	glEnd();

	glColor3f(currentColor[0], currentColor[1], currentColor[2]);
}


// Función que se invoca cada vez que se dibuja
void render()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	//Se limpian los buffers con el color activo definido por glClearColor

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	// Transformación de vista
	glTranslatef(panX, panY, zoomZ);
	glRotatef(rotY, 1.0, 0.0, 0.0);
	glRotatef(rotX, 0.0, 1.0, 0.0);

	// Escena
	geomCoordSys(2.f);
	//glutSolidTeapot(1.f);
	glPushMatrix();
	glRotatef(90., 0., 1., 0.);
	glRotatef(-90., 1., 0., 0.);
	gluSphere(quad, 2, 20, 20);
	glPopMatrix();
	glutSwapBuffers();			// Se intercambian buffers
}

void mouse(int button, int state, int x, int y)
{
	mouseButton = button;
	mouseRotate = false;
	mouseZoom = false;
	mousePan = false;
	xClick = x;
	yClick = y;
	if (button == GLUT_LEFT_BUTTON)
	{
		mouseRotate = true;
		xAux = rotX;
		yAux = rotY;
	}
	else if (button == GLUT_RIGHT_BUTTON)
	{
		mouseZoom = true;
		xAux = zoomZ;
	}
	else if (button == GLUT_MIDDLE_BUTTON)
	{
		mousePan = true;
		xAux = panX;
		yAux = panY;
	}
}

void mouseMotion(int x, int y)
{
	if (mouseRotate == true)
	{
		if (mouseButton == GLUT_LEFT_BUTTON)
		{
			if ((x - xClick + xAux) > 360.0)
			{
				xAux = xAux - 360.0;
			}
			if ((x - xClick + xAux) < 0.0)
			{
				xAux = xAux + 360.0;
			}
			if ((y - yClick + yAux) > 360.0)
			{
				yAux = yAux - 360.0;
			}
			if ((y - yClick + yAux) < 0.0)
			{
				yAux = yAux + 360.0;
			}
			rotX = x - xClick + xAux;
			rotY = y - yClick + yAux;
		}
	}
	else if (mouseZoom == true)
	{
		if (mouseButton == GLUT_RIGHT_BUTTON)
		{
			zoomZ = ((x - xClick) / 10.0) + xAux;
		}
	}
	else if (mousePan == true)
	{
		if (mouseButton == GLUT_MIDDLE_BUTTON)
		{
			panX = ((x - xClick) / 63.0) + xAux;
			panY = ((y - yClick) / (-63.0)) + yAux;
		}
	}
	glutPostRedisplay();
}

int main(GLint argc, GLchar **argv)
{
	// 1. Se crea una ventana y un contexto OpenGL usando GLUT
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowSize(width, height);
	glutCreateWindow("Mapping a Texture image to a Sphere");

	// 1.2 Se definen las funciones callback para el manejo de eventos
	glutReshapeFunc(resize);
	glutDisplayFunc(render);
	glutMouseFunc(mouse);
	glutMotionFunc(mouseMotion);

	// 2. Se direcciona a las funciones correctas del API de OpenGL
	GLenum err = glewInit();
	if (GLEW_OK != err)
	{
		fprintf(stderr, "GLEW Error");
		return false;
	}

	// 3. Se inicializa el contexto de OpenGL y la geometría de la escena
	init();

	// 4. Se inicia el ciclo de escucha de eventos
	glutMainLoop();
	return 0;
}