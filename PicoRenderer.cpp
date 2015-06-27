#include "PicoRenderer.h"

//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////

void hueColor(float& R, float& G, float& B, double p, double sat)
{
	if(p <= 1.0/11.0){R = 1; G = 11.0/2.0*(p); B = 0;}
	else if(p <= 2.0/11.0){R = 1; G = 11.0/2.0*(p); B = 0;}
	else if(p <= 3.0/11.0){R = -11.0/2.0*(p-4.0/11.0); G = 1; B = 0;}
	else if(p <= 4.0/11.0){R = -11.0/2.0*(p-4.0/11.0); G = 1; B = 0;}
	else if(p <= 5.0/11.0){R = 0; G = 1; B = 11.0/2.0*(p-4.0/11.0);}
	else if(p <= 6.0/11.0){R = 0; G = 1; B = 11.0/2.0*(p-4.0/11.0);}
	else if(p <= 7.0/11.0){R = 0; G = -11.0/2.0*(p-8.0/11.0); B = 1;}
	else if(p <= 8.0/11.0){R = 0; G = -11.0/2.0*(p-8.0/11.0); B = 1;}
	else if(p <= 9.0/11.0){R = 11.0/2.0*(p-8.0/11.0); G = 0; B = 1;}
	else if(p <= 10.0/11.0){R = 11.0/2.0*(p-8.0/11.0); G = 0; B = 1;}
	else{R = 1; G = 0; B = 1;}
	R = sat*R+(1-sat); G = sat*G+(1-sat); B = sat*B+(1-sat);
}

void IndexColord(float& R, float& G, float& B, int idx, float sat)
{
	//find what RGB values correspond to idx
    R = 0.5; G = 0.5; B = 0.5;
    if((idx+1) == 1){R = 1.0; G = 0.0; B = 0.0;}
    if((idx+1) == 2){R = 1.0; G = 1.0; B = 0.0;}
    if((idx+1) == 3){R = 0.0; G = 1.0; B = 0.0;}
    if((idx+1) == 4){R = 0.0; G = 1.0; B = 1.0;}
    if((idx+1) == 5){R = 0.0; G = 0.0; B = 1.0;}
    if((idx+1) == 6){R = 1.0; G = 0.0; B = 1.0;}
    if((idx+1) == 7){R = 1.0; G = 1.0; B = 1.0;}
    
	//set saturation and RGB levels
	R = sat*R+(1-sat); G = sat*G+(1-sat); B = sat*B+(1-sat);
}

void drawSphere()
{
	int n = 16;
	float pi =  3.14159265358979323846264338327950288419716939937510;

	//draw body
	glBegin(GL_QUADS);
	for(int i = 0; i < 2*n; ++i)
	{
		for(int j = 1; j < n-1; ++j)
		{
			glNormal3f(sinf(j*pi/n)*sinf(i*pi/n),
					   sinf(j*pi/n)*cosf(i*pi/n),
					   cosf(j*pi/n));
			glVertex3f(sinf(j*pi/n)*sinf(i*pi/n),
					   sinf(j*pi/n)*cosf(i*pi/n),
					   cosf(j*pi/n));
			glVertex3f(sinf(j*pi/n)*sinf((i+1)*pi/n),
					   sinf(j*pi/n)*cosf((i+1)*pi/n),
					   cosf(j*pi/n));
			glVertex3f(sinf((j+1)*pi/n)*sinf((i+1)*pi/n),
					   sinf((j+1)*pi/n)*cosf((i+1)*pi/n),
					   cosf((j+1)*pi/n));
			glVertex3f(sinf((j+1)*pi/n)*sinf(i*pi/n),
					   sinf((j+1)*pi/n)*cosf(i*pi/n),
					   cosf((j+1)*pi/n));
		}
	}
	glEnd();

	//draw caps
	
	glBegin(GL_TRIANGLES);
	glColor3f(1.0,1.0,1.0);
	for(int i = 0; i < 2*n; ++i)
	{
		glNormal3f(0.0,0.0,1.0);
		glVertex3f(0.0,0.0,1.0);
		glVertex3f(sinf((1)*pi/n)*sinf((i+1)*pi/n),
					   sinf((1)*pi/n)*cosf((i+1)*pi/n),
					   cosf((1)*pi/n));
		glVertex3f(sinf((1)*pi/n)*sinf(i*pi/n),
				   sinf((1)*pi/n)*cosf(i*pi/n),
				   cosf((1)*pi/n));
	}
	glEnd();

	glBegin(GL_TRIANGLES);
	glColor3f(1.0,1.0,1.0);
	for(int i = 0; i < 2*n; ++i)
	{
		glNormal3f(0.0,0.0,-1.0);
		glVertex3f(0.0,0.0,-1.0);
		glVertex3f(-sinf((1)*pi/n)*sinf((i+1)*pi/n),
				   -sinf((1)*pi/n)*cosf((i+1)*pi/n),
				   -cosf((1)*pi/n));
		glVertex3f(-sinf((1)*pi/n)*sinf(i*pi/n),
				   -sinf((1)*pi/n)*cosf(i*pi/n),
				   -cosf((1)*pi/n));
	}
	glEnd();
}

void drawBox()
{
	glBegin(GL_QUADS);
	glNormal3f(0.0,-1.0,0.0);
	glVertex3f(-1.0,-1.0,-1.0);
	glVertex3f(1.0,-1.0,-1.0);
	glVertex3f(1.0,-1.0,1.0);
	glVertex3f(-1.0,-1.0,1.0);

	glNormal3f(0.0,1.0,0.0);
	glVertex3f(-1.0,1.0,-1.0);
	glVertex3f(1.0,1.0,-1.0);
	glVertex3f(1.0,1.0,1.0);
	glVertex3f(-1.0,1.0,1.0);
	glEnd();
}

void drawLineCube()
{
	
	glBegin(GL_LINES);
	glColor3f(1.0,1.0,1.0);

	glVertex3f(0.0,0.0,0.0);
	glVertex3f(1.0,0.0,0.0);

	glVertex3f(1.0,0.0,0.0);
	glVertex3f(1.0,1.0,0.0);

	glVertex3f(1.0,1.0,0.0);
	glVertex3f(0.0,1.0,0.0);

	glVertex3f(0.0,1.0,0.0);
	glVertex3f(0.0,0.0,0.0);

	glVertex3f(0.0,0.0,1.0);
	glVertex3f(1.0,0.0,1.0);

	glVertex3f(1.0,0.0,1.0);
	glVertex3f(1.0,1.0,1.0);

	glVertex3f(1.0,1.0,1.0);
	glVertex3f(0.0,1.0,1.0);

	glVertex3f(0.0,1.0,1.0);
	glVertex3f(0.0,0.0,1.0);

	glVertex3f(0.0,0.0,0.0);
	glVertex3f(0.0,0.0,1.0);

	glVertex3f(1.0,0.0,0.0);
	glVertex3f(1.0,0.0,1.0);

	glVertex3f(1.0,1.0,0.0);
	glVertex3f(1.0,1.0,1.0);

	glVertex3f(0.0,1.0,0.0);
	glVertex3f(0.0,1.0,1.0);

	glColor3f(1.0,1.0,1.0);
	glVertex3f(0.0,0.0,0.0);
	glVertex3f(1.0,1.0,1.0);

	glEnd();
}

void drawLineAxes()
{
    glLineWidth(4.0);
	glBegin(GL_LINES);
	glColor3f(1.0,0.7,0.7);
	glVertex3f(0.0,0.0,0.0);
	glVertex3f(1.0,0.0,0.0);

    glColor3f(0.7,1.0,0.7);
	glVertex3f(0.0,0.0,0.0);
	glVertex3f(0.0,1.0,0.0);

    glColor3f(0.7,0.7,1.0);
	glVertex3f(0.0,0.0,0.0);
	glVertex3f(0.0,0.0,1.0);

	glEnd();
}

void drawDataStreams(PicoSimulation* xsimulation)
{
	float R,G,B;
	DataStreams *dstreams = xsimulation->getDataStreams();

	glLineWidth(3.0);
	glPushMatrix();
	glTranslatef(0.0,0.0,0.0);
	glScalef(1.0,0.1,1.0);
	glColor3f(1.0,1.0,1.0);
	glBegin(GL_LINE_STRIP);
	for(int i = 0; i < DATA_STREAM_SIZE; ++i)
	{
		glVertex3f(1.0*i/DATA_STREAM_SIZE,(dstreams->stomachContentsStream[(dstreams->stomachContentsIdx + i)%DATA_STREAM_SIZE])/100.0,0.0);
	}
	glEnd();
	glPopMatrix();

	glPushMatrix();
	glTranslatef(0.0,0.1,0.0);
	glScalef(1.0,0.1,1.0);
	glColor3f(1.0,1.0,0.0);
	glBegin(GL_LINE_STRIP);
	for(int i = 0; i < DATA_STREAM_SIZE; ++i)
	{
		glVertex3f(1.0*i/DATA_STREAM_SIZE,(dstreams->latentInsulinStream[(dstreams->latentInsulinIdx + i)%DATA_STREAM_SIZE])/10.0,0.0);
	}
	glEnd();
	glPopMatrix();

	glPushMatrix();
	glTranslatef(0.0,0.2,0.0);
	glScalef(1.0,0.1,1.0);
	glColor3f(1.0,0.0,0.0);
	glBegin(GL_LINE_STRIP);
	for(int i = 0; i < DATA_STREAM_SIZE; ++i)
	{
		glVertex3f(1.0*i/DATA_STREAM_SIZE,(dstreams->bloodSugarStream[(dstreams->bloodSugarIdx + i)%DATA_STREAM_SIZE])/500.0,0.0);
	}
	glEnd();

	glBegin(GL_LINES);
	glColor3f(1.0,0.8,0.8);
	glVertex3f(0.0,70.0/500.0,0.0);
	glVertex3f(1.0,70.0/500.0,0.0);

	glVertex3f(0.0,120.0/500.0,0.0);
	glVertex3f(1.0,120.0/500.0,0.0);
	glEnd();
	glPopMatrix();

	glPushMatrix();
	glTranslatef(0.0,0.3,0.0);
	glScalef(1.0,0.1,1.0);
	glColor3f(0.0,1.0,0.0);
	glBegin(GL_LINE_STRIP);
	for(int i = 0; i < DATA_STREAM_SIZE; ++i)
	{
		glVertex3f(1.0*i/DATA_STREAM_SIZE,(dstreams->bloodInsulinStream[(dstreams->bloodInsulinIdx + i)%DATA_STREAM_SIZE])/10.0,0.0);
	}
	glEnd();
	glPopMatrix();

	glPushMatrix();
	glTranslatef(0.0,0.4,0.0);
	glScalef(1.0,0.1,1.0);
	glColor3f(1.0,0.5,1.0);
	glBegin(GL_LINE_STRIP);
	for(int i = 0; i < DATA_STREAM_SIZE; ++i)
	{
		glVertex3f(1.0*i/DATA_STREAM_SIZE,(dstreams->sensorSugarStream[(dstreams->sensorSugarIdx + i)%DATA_STREAM_SIZE])/20.0,0.0);
	}
	glEnd();
	glPopMatrix();
}

//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////

void PicoRenderer::initialize(HWND hwndin,int widthin,int heightin)
{
	int pixelFormat;
	
	_width = widthin;
	_height = heightin;
	_aspect = float(_width)/_height;
	_hDC = GetDC(hwndin);

	PIXELFORMATDESCRIPTOR pfd = {
        sizeof(PIXELFORMATDESCRIPTOR),  /* size */
        1,                              /* version */
        PFD_SUPPORT_OPENGL |
        PFD_DRAW_TO_WINDOW |
        PFD_DOUBLEBUFFER,               /* support double-buffering */
        PFD_TYPE_RGBA,                  /* color type */
        16,                             /* prefered color depth */
        0, 0, 0, 0, 0, 0,               /* color bits (ignored) */
        0,                              /* no alpha buffer */
        0,                              /* alpha bits (ignored) */
        0,                              /* no accumulation buffer */
        0, 0, 0, 0,                     /* accum bits (ignored) */
        16,                             /* depth buffer */
        0,                              /* no stencil buffer */
        0,                              /* no auxiliary buffers */
        PFD_MAIN_PLANE,                 /* main layer */
        0,                              /* reserved */
        0, 0, 0,                        /* no layer, visible, damage masks */
    };
    pixelFormat = ChoosePixelFormat(_hDC, &pfd);
	SetPixelFormat(_hDC, pixelFormat, &pfd);

	_hGLRC = wglCreateContext(_hDC);
	wglMakeCurrent(_hDC, _hGLRC);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_POINT_SMOOTH);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glViewport(0, 0, _width, _height);
}

void PicoRenderer::update(PicoSimulation* xsimulation)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
	glPushMatrix();
	//cameraView(xsimulation->getxCamera());	
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0.0,1.0,0.0,1.0,0.1,100.0);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glDisable(GL_LIGHTING);

	glPushMatrix();
	glTranslatef(0.0,0.0,-0.5);
	glScalef(1.0,2.0,1.0);
	drawDataStreams(xsimulation);
	glPopMatrix();

	glEnable(GL_LIGHTING);
	glPopMatrix();
	glFlush();
	SwapBuffers(_hDC);
}

void PicoRenderer::cameraView(PicoCamera* xcam)
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(xcam->_FoV, _aspect, 0.1, 1000.0);

	glTranslatef(0.0,0.0,-xcam->_distance);
	glRotatef(xcam->_elevation,1.0,0.0,0.0);
	glRotatef(-xcam->_azimuth,0.0,1.0,0.0);
	glTranslatef(-xcam->_position.x,-xcam->_position.y,-xcam->_position.z);
}

void PicoRenderer::release()
{
}

