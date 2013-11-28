//.-----------------.
//|                 |
//| GLCubeSaver.cpp |
//|                 |
//'-----------------'

#include <math.h>
#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <CheckBox.h>
#include <MenuField.h>
#include <MenuItem.h>
#include <MenuBar.h>
#include <Slider.h>
#include <StringView.h>
#include <GL/glu.h>
#include <GLView.h>
#include <Beep.h>

#include "GLCubeSaver.h"

//*********************************************************************************************

GLCubes::GLCubes(BMessage *prefs, image_id image)
	: BScreenSaver(prefs, image),
	numcubes(25),
	cubesize(1.0),
	cubespin(1),
	nobounds(0),
	wireframe(0),
	fountain(0),
	lights(0),
	opaque(0),
	solidcolor(0),
	pulsate(0),
	collisions(0),
	shape(0)
{
	//Check for preferences
	if (!prefs->IsEmpty())
		restore_state(prefs);
	srandom(system_time());
}

//*********************************************************************************************

status_t GLCubes::SaveState(BMessage *prefs) const
{
	// Save preferences: cubes, rotation and other attributes
	prefs->AddInt32("nofcubes", numcubes);
	prefs->AddFloat("sofcubes", cubesize);
	prefs->AddInt32("rofspin", cubespin);
	prefs->AddBool("nobounds", nobounds);
	prefs->AddBool("wireframe", wireframe);
	prefs->AddBool("fountain", fountain);
	prefs->AddBool("lights", lights);
	prefs->AddBool("opaque", opaque);
	prefs->AddBool("solidcolor", solidcolor);
	prefs->AddBool("pulsate", pulsate);
	prefs->AddBool("collisions", collisions);
	prefs->AddInt32("shape", shape);
	return B_OK;
}

//*********************************************************************************************

void GLCubes::restore_state(BMessage *prefs)
{
	// Retrieve preferences and compare for accuracy
	int32	numcubes_temp, cubespin_temp, shape_temp; 
	float	cubesize_temp;
	bool	nobounds_temp, wireframe_temp, fountain_temp, 
			lights_temp, opaque_temp, solidcolor_temp,
			pulsate_temp, collisions_temp;
	
	if (prefs->FindInt32("nofcubes", &numcubes_temp) == B_OK)
		numcubes = numcubes_temp;
	if (prefs->FindFloat("sofcubes", &cubesize_temp) == B_OK)
		cubesize = cubesize_temp;
	if (prefs->FindInt32("rofspin", &cubespin_temp) == B_OK)
		cubespin = cubespin_temp;
	if (prefs->FindBool("nobounds", &nobounds_temp) == B_OK)
		nobounds = nobounds_temp;
	if (prefs->FindBool("wireframe", &wireframe_temp) == B_OK)
		wireframe = wireframe_temp;
	if (prefs->FindBool("fountain", &fountain_temp) == B_OK)
		fountain = fountain_temp;
	if (prefs->FindBool("lights", &lights_temp) == B_OK)
		lights = lights_temp;
	if (prefs->FindBool("opaque", &opaque_temp) == B_OK)
		opaque = opaque_temp;
	if (prefs->FindBool("solidcolor", &solidcolor_temp) == B_OK)
		solidcolor = solidcolor_temp;
	if (prefs->FindBool("pulsate", &pulsate_temp) == B_OK)
		pulsate = pulsate_temp;
	if (prefs->FindBool("collisions", &collisions_temp) == B_OK)
		collisions = collisions_temp;
	if (prefs->FindInt32("shape", &shape_temp) == B_OK)
		shape = shape_temp;
}

//*********************************************************************************************

void GLCubes::StartConfig(BView *view)
{
	BRect bounds = view->Bounds();
	GLCubeConfig *setup = new GLCubeConfig(bounds, this);
	view->AddChild(setup);
}

//*********************************************************************************************

status_t GLCubes::StartSaver(BView *view, bool preview)
{
	if(preview)
	{
		viewport = 0;
		return B_ERROR;
	}
	else
	{
		SetTickSize(50000);
	
		viewport = new GLCubeView(view->Bounds(), "objectView", B_FOLLOW_NONE, BGL_RGB | BGL_DEPTH | BGL_DOUBLE, this);
		view->AddChild(viewport);

		return B_OK;
	}
}

//*********************************************************************************************

void GLCubes::StopSaver()
{
	if(viewport)
		viewport->EnableDirectMode(false);
}

//*********************************************************************************************

void GLCubes::DirectConnected(direct_buffer_info *info)
{
	viewport->DirectConnected(info);
	viewport->EnableDirectMode(true);
}

//*********************************************************************************************

void GLCubes::DirectDraw(int32)
{
	viewport->Advance();
}

//.------------------------------.
//| MAIN INSTANTIATION FUNCTION  |
//'------------------------------'

extern "C" _EXPORT BScreenSaver *instantiate_screen_saver(BMessage *message, image_id image)
{
	return new GLCubes(message, image);
}

//.------------------.
//|                  |
//| GLCubeConfig.cpp |
//|                  |
//'------------------'

//************************************************************************

GLCubeConfig::GLCubeConfig(BRect frame, GLCubes *saver_in)
	:	BView(frame, "", B_FOLLOW_NONE, B_WILL_DRAW),
		saver(saver_in)
{
	AddChild(new BStringView(BRect(10, 0, 250, 12), B_EMPTY_STRING, "OpenGL Cubes+ v0.9g"));
	AddChild(new BStringView(BRect(14, 0, 250, 26), B_EMPTY_STRING, "©1999-2000 Brad Leffler"));
	
	slider = new BSlider(BRect(10,35,240,80), "cubes", "Number of Objects : ", new BMessage(GLC_NUMBER_CUBES), 0, 50, B_BLOCK_THUMB);
	slider->SetHashMarks(B_HASH_MARKS_BOTTOM);
	slider->SetLimitLabels("0", "50");
	slider->SetValue(saver->numcubes);
	slider->SetHashMarkCount(10);
	AddChild(slider);
	
	rslider = new BSlider(BRect(10,83,240,128), "rotate", "Rotation Speed : ", new BMessage(GLC_CUBE_SPIN), 0, 10, B_BLOCK_THUMB);
	rslider->SetHashMarks(B_HASH_MARKS_BOTTOM);
	rslider->SetLimitLabels("0", "10");
	rslider->SetValue(saver->cubespin);
	rslider->SetHashMarkCount(11);
	AddChild(rslider);

	nobounds = new BCheckBox(BRect(10, 160, 100, 175), "nobounds", "No Bounds", new BMessage(GLC_NOBOUNDS));
	AddChild(nobounds);
	if(saver->nobounds)
		nobounds->SetValue(B_CONTROL_ON);	
	if(saver->fountain)
		nobounds->SetEnabled(false);
	
	wireframe = new BCheckBox(BRect(10, 177, 100, 192), "wireframe", "Wireframe", new BMessage(GLC_WIREFRAME));
	AddChild(wireframe);
	if(saver->wireframe)
		wireframe->SetValue(B_CONTROL_ON);
		
	fountain = new BCheckBox(BRect(10, 194, 100, 209), "fountain", "Fountain", new BMessage(GLC_FOUNTAIN));
	AddChild(fountain);
	if(saver->fountain)
		fountain->SetValue(B_CONTROL_ON);
			
	lights = new BCheckBox(BRect(10, 211, 100, 226), "lights", "Lights", new BMessage(GLC_LIGHTS));
	AddChild(lights);
	if(saver->opaque)
	{
		lights->SetValue(B_CONTROL_ON);
		lights->SetEnabled(false);
	}
	if(saver->lights)
		lights->SetValue(B_CONTROL_ON);
		
	opaque = new BCheckBox(BRect(10, 228, 100, 243), "opaque", "Transparent", new BMessage(GLC_OPAQUE));
	AddChild(opaque);
	if(saver->opaque)
		opaque->SetValue(B_CONTROL_ON);
		
	solidcolor = new BCheckBox(BRect(120, 160, 220, 175), "solidcolor", "Solid Colors", new BMessage(GLC_SOLID_COLOR));
	AddChild(solidcolor);
	solidcolor->SetEnabled(false);
	if(saver->solidcolor)
		solidcolor->SetValue(B_CONTROL_ON);
			
	pulsate = new BCheckBox(BRect(120, 177, 220, 192), "pulsate", "Pulsate", new BMessage(GLC_PULSATE));
	AddChild(pulsate);
	if(saver->pulsate)
		pulsate->SetValue(B_CONTROL_ON);
		
	collisions = new BCheckBox(BRect(120, 194, 220, 209), "collisions", "Collisions", new BMessage(GLC_COLLISIONS));
	AddChild(collisions);
	if(saver->collisions)
		collisions->SetValue(B_CONTROL_ON);
	if(saver->fountain)
		collisions->SetEnabled(false);
	
	menu = new BMenu (" Shapes ");
	BMenuField *shapes = new BMenuField(BRect(10,135,120,155), "SelectPopUp", "Shape", menu);
	shapes->SetDivider (35);
	AddChild(shapes);
	menu->SetRadioMode(true);
	menu->SetLabelFromMarked(true);
	cube = new BMenuItem("Cube", new BMessage(GLC_CUBE));
	menu->AddItem(cube);
	pyramid = new BMenuItem("Pyramid", new BMessage(GLC_PYRAMID));
	menu->AddItem(pyramid);
	gem = new BMenuItem("Gem", new BMessage(GLC_GEM));
	menu->AddItem(gem);
	diamond = new BMenuItem("Diamond", new BMessage(GLC_DIAMOND));
	menu->AddItem(diamond);
	if(saver->shape == 0)
		cube->SetMarked(true);
	if(saver->shape == 1)
		pyramid->SetMarked(true);
	if(saver->shape == 2)
		gem->SetMarked(true);
	if(saver->shape == 3)
		diamond->SetMarked(true);
}

//************************************************************************

void GLCubeConfig::AttachedToWindow()
{
	SetViewColor(ui_color(B_PANEL_BACKGROUND_COLOR));
	
	slider->SetTarget (this);
	slider->Invoke(new BMessage(GLC_NUMBER_CUBES));
	rslider->SetTarget (this);
	rslider->Invoke(new BMessage(GLC_CUBE_SPIN));
	nobounds->SetTarget (this);
	wireframe->SetTarget (this);
	fountain->SetTarget (this);
	lights->SetTarget (this);
	opaque->SetTarget (this);
	solidcolor->SetTarget (this);
	pulsate->SetTarget (this);
	collisions->SetTarget (this);
	menu->SetTargetForItems (this);
}

//************************************************************************

void GLCubeConfig::MessageReceived(BMessage *msg)
{	
	char	label[30];
	int32	value;
	
	switch(msg->what)
	{
		case GLC_NUMBER_CUBES:
			value = slider->Value();
			sprintf(label,"Number of Objects : %li",value);
			slider->SetLabel(label);
			saver->numcubes = slider->Value();
			break;			
		case GLC_CUBE_SPIN:
			value = rslider->Value();
			sprintf(label,"Rotation Speed : %li",value);
			rslider->SetLabel(label);
			saver->cubespin = rslider->Value();
			break;
		case GLC_NOBOUNDS:
			if (nobounds->Value() == B_CONTROL_ON)
				saver->nobounds = true;
			else
				saver->nobounds = false;
			break;
		case GLC_WIREFRAME:
			if (wireframe->Value() == B_CONTROL_ON)
				saver->wireframe = true;
			else
				saver->wireframe = false;
			break;
		case GLC_FOUNTAIN:
			if (fountain->Value() == B_CONTROL_ON)
			{
				nobounds->SetEnabled(false);
				collisions->SetEnabled(false);				
				saver->fountain = true;
			}
			else
			{
				saver->fountain = false;
				collisions->SetEnabled(true);
				nobounds->SetEnabled(true);
			}
			break;
		case GLC_LIGHTS:
			if (lights->Value() == B_CONTROL_ON)
				saver->lights = true;
			else
				saver->lights = false;
			break;
		case GLC_OPAQUE:
			if (opaque->Value() == B_CONTROL_ON)
			{
				saver->opaque = true;
				saver->lights = true;
				lights->SetValue(B_CONTROL_ON);
				lights->SetEnabled(false);
			}
			else
			{
				saver->opaque = false;
				lights->SetEnabled(true);
			}
			break;
		case GLC_SOLID_COLOR:
			if (solidcolor->Value() == B_CONTROL_ON)
				saver->solidcolor = true;
			else 
				saver->solidcolor = false;
			break;
		case GLC_PULSATE:
			if (pulsate->Value() == B_CONTROL_ON)
				saver->pulsate = true;
			else 
				saver->pulsate = false;
			break;
		case GLC_COLLISIONS:
			if (collisions->Value() == B_CONTROL_ON)
				saver->collisions = true;
			else 
				saver->collisions = false;
			break;
		case GLC_CUBE:
			saver->shape = 0;
			break;
		case GLC_PYRAMID:
			saver->shape = 1;
			break;
		case GLC_GEM:
			saver->shape = 2;
			break;
		case GLC_DIAMOND:
			saver->shape = 3;
			break;
		default :
			BView::MessageReceived(msg);
			break;
	}
}

//.----------------.
//|                |
//| GLCubeView.cpp |
//|                |
//'----------------'

//*********************************************************************************************

GLCubeView::GLCubeView(BRect frame, char *name, ulong resizingMode, ulong options, GLCubes *saver_in)
 : BGLView(frame, name, resizingMode, 0, options),
 	saver (saver_in)
{
	GLfloat light_ambient[] = {0.5, 0.5, 0.5, 1.0};
		
	GLfloat	light_diffuse[] = {1.0, 1.0, 1.0, 1.0};
	GLfloat light_position[] = { 0.0, 0.0, 5.0 };

	GLfloat light1_diffuse[]  = {  1.0,  1.0, 1.0, 1.0 };
	GLfloat light1_position[] = { 0.0, 0.0, -2.0 };
		
	GLfloat material_specular[] = {1.0, 1.0, 1.0, 1.0};
	GLfloat material_shininess[] = {60.0};
	
	int i;
	
	for (i=0; i < saver->numcubes; i ++)
	{
		if (!saver->fountain)
		{
			xpos[i] = (rand()%50)-25;
			ypos[i] = (rand()%38)-19;
		}
		else
		{
			xpos[i] = 30.0;
			ypos[i] = 30.0;
		}
		xdir[i] = ((rand()%20)-10)*0.1;
		ydir[i] = ((rand()%20)-10)*0.1;
		xrot[i] = (rand()%359);
		yrot[i] = (rand()%359);
		scale[i] = ((rand()%19)+1)*0.01;
		if (saver->pulsate)
			size[i] = (((rand()%20)+10)/2)*0.1;
		else
			size[i] = 1.0;	
	}
		
	LockGL();
	if (saver->lights)
	{
		glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
	
		glMaterialfv(GL_FRONT, GL_SPECULAR, material_specular);
		glMaterialfv(GL_FRONT, GL_SHININESS, material_shininess);

		glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
		glLightfv(GL_LIGHT0, GL_POSITION, light_position);
		glEnable(GL_LIGHT0);

		glLightfv(GL_LIGHT1, GL_DIFFUSE, light1_diffuse);
		glLightfv(GL_LIGHT1, GL_POSITION, light1_position);
		glEnable(GL_LIGHT1);

		glEnable(GL_LIGHTING);
	}
	
	glClearDepth( 1.0 );
	glDepthFunc( GL_LESS );
	glEnable( GL_DEPTH_TEST );
	
	if (saver->opaque)
	{
		glEnable( GL_BLEND );
		glBlendFunc( GL_SRC_ALPHA, GL_ONE );
	}

	glShadeModel(GL_SMOOTH);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective( 45.0, frame.Width() / frame.Height(), 2.0, 20000.0 );
	glTranslatef (0.0, 0.0, -50.0);
	glMatrixMode(GL_MODELVIEW);
	
	if (saver->wireframe)
		glPolygonMode (GL_FRONT_AND_BACK, GL_LINE);
	else
    	glPolygonMode (GL_FRONT_AND_BACK, GL_FILL); 
	
	UnlockGL();
}

//*********************************************************************************************

void GLCubeView::AttachedToWindow()
{
	LockGL();
	BGLView::AttachedToWindow();
	UnlockGL();
}

//*********************************************************************************************

void GLCubeView::Draw()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	
	int i;
		
	for (i=0; i < saver->numcubes; i++)
	{
		glPushMatrix();

			glTranslatef( xpos[i], ypos[i], 0.0);
			glRotatef( xrot[i]*2, 1.0, 0.0, 0.0 );
			glRotatef( yrot[i]*5, 0.0, 0.0, 1.0 );
			glRotatef( xrot[i], 0.0, 1.0, 0.0 );	

			if (saver->pulsate)
			{	
				size[i] += scale[i];
				if (size[i] >= 1.5)
					scale[i] *= -1;
				if (size[i] <= 0.5)
					scale[i] *= -1;
			}	

			glScalef(size[i], size[i], size[i]);
		
			if (saver->shape == 0)
				DrawCube();
			else if (saver->shape == 1)
				DrawPyramid();
			else if (saver->shape == 2)
				DrawJem();
			else if (saver->shape == 3)
				DrawDiamond();
			
		glPopMatrix();
	}
}

//*********************************************************************************************

void GLCubeView::Advance()
{
	int i;
	double dx, dy, distance;
	double r = 1.3;

	if (!saver->fountain)
	{
		//Normal cube updates	
		for (i=0; i < saver->numcubes; i++)
		{
			if (saver->nobounds)
			{
				if ((xpos[i] > 29.0) || (xpos[i] < -29.0))
				{
					xpos[i] *= -1.0;
				}
				xpos[i] += xdir[i];
			
				if ((ypos[i] > 22.0) || (ypos[i] < -22.0))
				{
					ypos[i] *= -1.0;
				}
				ypos[i] += ydir[i];
			}
			else
			{
				if ((xpos[i] > 25.0) || (xpos[i] < -25.0))
				{
					xdir[i] *= -1.0;
				}
				xpos[i] += xdir[i];
			
				if ((ypos[i] > 19.0) || (ypos[i] < -19.0))
				{
					ydir[i] *= -1.0;
				}
				ypos[i] += ydir[i];
			}	
			
			// **** Begin Collisions ****
			if (saver->collisions)
			{
				for (j=0; j < saver->numcubes; j++)
				{
					if (i != j)
					{
						dx = xpos[i] - xpos[j];
						dy = ypos[i] - ypos[j];
						dx = fabs(dx);
						dy = fabs(dy);
						if (dx < 2.0 && dy < 2.0)
						{
							dx *= dx;
							dy *= dx;
							distance = sqrt(dx+dy);
							if (r > (distance/2))
							{
								if (dx > dy)
								{
									xdir[i] *= -1.0;
									xdir[j] *= -1.0;
								}
								else 
								{
									ydir[i] *= -1.0;
									ydir[j] *= -1.0;
								}
							}	
						}
					}
				}
			}			
			// **** End Collisions ****
				
			xrot[i] += saver->cubespin;
			if (xrot[i] > 360.0)
			{
				xrot[i] -= 360.0;
			}
			
			yrot[i] += saver->cubespin;
			if (yrot[i] > 360.0)
			{
				yrot[i] -= 360.0;
			}
		}
	}
	else
	{
		//Fountain cube updates	
		for (i = 0; i < saver->numcubes ; i++)
		{
		if (i < j)
		{
			if ((xpos[i] > 29.0) || (xpos[i] < -29.0) || (ypos[i] > 22.0) || (ypos[i] < -22.0))
			{
				xpos[i] = 0.0;
				ypos[i] = 0.0;
				xdir[i] = (((rand()%18)-9)+0.5)*0.1;
				ydir[i] = (((rand()%18)-9)+0.5)*0.1;
			}
		
			xpos[i] += xdir[i];
			ypos[i] += ydir[i];
						
			xrot[i] += saver->cubespin;
		
			if (xrot[i] > 360.0)
			{
				xrot[i] -= 360.0;
			}
			
			yrot[i] += saver->cubespin;
			
			if (yrot[i] > 360.0)
			{
				yrot[i] -= 360.0;
			}
		}
		}
			if (j < saver->numcubes)
			j++;			
	}
	
	LockGL();
	Draw();
	SwapBuffers();
	UnlockGL();
}

//*********************************************************************************************

void DrawCube (void)
{
	glBegin( GL_QUAD_STRIP );
		glColor3f( 0.0, 1.0, 0.0 );
		glMaterialfv(GL_FRONT, GL_DIFFUSE, green);
		glNormal3f(  0.0,  1.0,  0.0 );
		glVertex3f(  1.0,  1.0, -1.0 );
		glVertex3f( -1.0,  1.0, -1.0 );
		glVertex3f(  1.0,  1.0,  1.0 );
		glVertex3f( -1.0,  1.0,  1.0 );	

		glColor3f( 1.0, 0.6, 0.0 );
		glMaterialfv(GL_FRONT, GL_DIFFUSE, orange);
		glNormal3f(  0.0,  0.0,  1.0 );
		glVertex3f( -1.0,  1.0,  1.0 );
		glVertex3f(  1.0,  1.0,  1.0 );
		glVertex3f( -1.0, -1.0,  1.0 );
		glVertex3f(  1.0, -1.0,  1.0 );
		
		glColor3f( 1.0, 0.0, 0.0 );
		glMaterialfv(GL_FRONT, GL_DIFFUSE, red);
		glNormal3f(  0.0, -1.0,  0.0 );
		glVertex3f( -1.0, -1.0,  1.0 );
		glVertex3f(  1.0, -1.0,  1.0 );
		glVertex3f( -1.0, -1.0, -1.0 );
		glVertex3f(  1.0, -1.0, -1.0 );
	glEnd();
		
	glBegin( GL_QUAD_STRIP);
		glColor3f( 1.0, 1.0, 0.0 );
		glMaterialfv(GL_FRONT, GL_DIFFUSE, yellow);
		glNormal3f( -1.0,  0.0,  0.0 );
		glVertex3f( -1.0,  1.0,  1.0 );
		glVertex3f( -1.0, -1.0,  1.0 );
		glVertex3f( -1.0,  1.0, -1.0 );
		glVertex3f( -1.0, -1.0, -1.0 );
			
		glColor3f( 0.0, 0.0, 1.0 );
		glMaterialfv(GL_FRONT, GL_DIFFUSE, blue);
		glNormal3f(  0.0,  0.0, -1.0 );
		glVertex3f( -1.0,  1.0, -1.0 );
		glVertex3f( -1.0, -1.0, -1.0 );
		glVertex3f(  1.0,  1.0, -1.0 );
		glVertex3f(  1.0, -1.0, -1.0 );

		glColor3f( 1.0, 0.0, 1.0 );
		glMaterialfv(GL_FRONT, GL_DIFFUSE, purple);
		glNormal3f(  1.0,  0.0,  0.0 );
		glVertex3f(  1.0,  1.0, -1.0 );
		glVertex3f(  1.0, -1.0, -1.0 );
		glVertex3f(  1.0,  1.0,  1.0 );
		glVertex3f(  1.0, -1.0,  1.0 );
	glEnd();
}

void DrawPyramid (void)
{
	glBegin( GL_QUADS );
		glColor3f( 0.0, 1.0, 0.0 );
		glMaterialfv(GL_FRONT, GL_DIFFUSE, green);
		glNormal3f(  0.0,  -1.0,  0.0 );
		glVertex3f(  1.0,  -1.0,  1.0 );
		glVertex3f(  1.0,  -1.0, -1.0 );
		glVertex3f( -1.0,  -1.0, -1.0 );
		glVertex3f( -1.0,  -1.0,  1.0 );	
	glEnd();
	
	glBegin( GL_TRIANGLES);
		glColor3f( 1.0, 0.6, 0.0 );
		glMaterialfv(GL_FRONT, GL_DIFFUSE, orange);
		glNormal3f(  0.0,  0.0,  1.0 );
		glVertex3f(  1.0, -1.0,  1.0 );
		glVertex3f(  0.0,  1.0,  0.0 );
		glVertex3f( -1.0, -1.0,  1.0 );
		
		glColor3f( 1.0, 0.0, 0.0 );
		glMaterialfv(GL_FRONT, GL_DIFFUSE, red);
		glNormal3f(  0.0,  0.0, -1.0 );
		glVertex3f(  1.0, -1.0, -1.0 );
		glVertex3f(  0.0,  1.0,  0.0 );
		glVertex3f( -1.0, -1.0, -1.0 );
		
		glColor3f( 1.0, 1.0, 0.0 );
		glMaterialfv(GL_FRONT, GL_DIFFUSE, yellow);
		glNormal3f( -1.0,  0.0,  0.0 );
		glVertex3f( -1.0, -1.0,  1.0 );
		glVertex3f(  0.0,  1.0,  0.0 );
		glVertex3f( -1.0, -1.0, -1.0 );
			
		glColor3f( 0.0, 0.0, 1.0 );
		glMaterialfv(GL_FRONT, GL_DIFFUSE, blue);
		glNormal3f(  1.0,  0.0,  0.0 );
		glVertex3f(  1.0, -1.0, -1.0 );
		glVertex3f(  0.0,  1.0,  0.0 );
		glVertex3f(  1.0, -1.0,  1.0 );
	glEnd();
}

void DrawJem (void)
{
	glBegin( GL_TRIANGLES);
		glColor3f( 1.0, 1.0, 1.0 );
		glMaterialfv(GL_FRONT, GL_DIFFUSE, white);
		glNormal3f(  0.0,  0.0,  1.0 );
		glVertex3f(  1.0,  0.0,  1.0 );
		glVertex3f(  0.0,  1.0,  0.0 );
		glVertex3f( -1.0,  0.0,  1.0 );
		
		glColor3f( 1.0, 0.6, 0.0 );
		glMaterialfv(GL_FRONT, GL_DIFFUSE, orange);
		glNormal3f(  0.0,  0.0, -1.0 );
		glVertex3f(  1.0,  0.0, -1.0 );
		glVertex3f(  0.0,  1.0,  0.0 );
		glVertex3f( -1.0,  0.0, -1.0 );
		
		glColor3f( 0.0, 0.0, 1.0 );
		glMaterialfv(GL_FRONT, GL_DIFFUSE, blue);
		glNormal3f( -1.0,  0.0,  0.0 );
		glVertex3f( -1.0,  0.0,  1.0 );
		glVertex3f(  0.0,  1.0,  0.0 );
		glVertex3f( -1.0,  0.0, -1.0 );
			
		glColor3f( 1.0, 0.0, 0.0 );
		glMaterialfv(GL_FRONT, GL_DIFFUSE, red);
		glNormal3f(  1.0,  0.0,  0.0 );
		glVertex3f(  1.0,  0.0, -1.0 );
		glVertex3f(  0.0,  1.0,  0.0 );
		glVertex3f(  1.0,  0.0,  1.0 );
		
		glColor3f( 0.0, 1.0, 0.0 );
		glMaterialfv(GL_FRONT, GL_DIFFUSE, green);
		glNormal3f(  0.0,  0.0,  1.0 );
		glVertex3f(  1.0,  0.0,  1.0 );
		glVertex3f(  0.0, -1.0,  0.0 );
		glVertex3f( -1.0,  0.0,  1.0 );
		
		glColor3f( 0.0, 0.75, 1.0 );
		glMaterialfv(GL_FRONT, GL_DIFFUSE, cyan);
		glNormal3f(  0.0,  0.0, -1.0 );
		glVertex3f(  1.0,  0.0, -1.0 );
		glVertex3f(  0.0, -1.0,  0.0 );
		glVertex3f( -1.0,  0.0, -1.0 );
		
		glColor3f( 1.0, 1.0, 0.0 );
		glMaterialfv(GL_FRONT, GL_DIFFUSE, yellow);
		glNormal3f( -1.0,  0.0,  0.0 );
		glVertex3f( -1.0,  0.0,  1.0 );
		glVertex3f(  0.0, -1.0,  0.0 );
		glVertex3f( -1.0,  0.0, -1.0 );
			
		glColor3f( 1.0, 0.0, 1.0 );
		glMaterialfv(GL_FRONT, GL_DIFFUSE, purple);
		glNormal3f(  1.0,  0.0,  0.0 );
		glVertex3f(  1.0,  0.0, -1.0 );
		glVertex3f(  0.0, -1.0,  0.0 );
		glVertex3f(  1.0,  0.0,  1.0 );
	glEnd();
}

void DrawDiamond (void)
{
	glBegin( GL_TRIANGLES);
		glColor3f( 1.0, 1.0, 1.0 );
		glMaterialfv(GL_FRONT, GL_DIFFUSE, white);
		glNormal3f(  1.0,  0.0,  0.0 );
		glVertex3f(  0.0, -1.0,  0.0 );
		glVertex3f(  1.0,  0.5, -0.5 );
		glVertex3f(  1.0,  0.5,  0.5 );
		
		glColor3f( 1.0, 0.6, 0.0 );
		glMaterialfv(GL_FRONT, GL_DIFFUSE, orange);
		glNormal3f(  1.0,  0.0, -1.0 );
		glVertex3f(  0.0, -1.0,  0.0 );
		glVertex3f(  1.0,  0.5, -0.5 );
		glVertex3f(  0.5,  0.5, -1.0 );
		
		glColor3f( 0.0, 0.0, 1.0 );
		glMaterialfv(GL_FRONT, GL_DIFFUSE, blue);
		glNormal3f(  0.0,  0.0, -1.0 );
		glVertex3f(  0.0, -1.0,  0.0 );
		glVertex3f(  0.5,  0.5, -1.0 );
		glVertex3f( -0.5,  0.5, -1.0 );
			
		glColor3f( 1.0, 0.0, 0.0 );
		glMaterialfv(GL_FRONT, GL_DIFFUSE, red);
		glNormal3f( -1.0,  0.0, -1.0 );
		glVertex3f(  0.0, -1.0,  0.0 );
		glVertex3f( -0.5,  0.5, -1.0 );
		glVertex3f( -1.0,  0.5, -0.5 );
		
		glColor3f( 0.0, 1.0, 0.0 );
		glMaterialfv(GL_FRONT, GL_DIFFUSE, green);
		glNormal3f( -1.0,  0.0,  0.0 );
		glVertex3f(  0.0, -1.0,  0.0 );
		glVertex3f( -1.0,  0.5, -0.5 );
		glVertex3f( -1.0,  0.5,  0.5 );
		
		glColor3f( 0.0, 0.75, 1.0 );
		glMaterialfv(GL_FRONT, GL_DIFFUSE, cyan);
		glNormal3f( -1.0,  0.0,  1.0 );
		glVertex3f(  0.0, -1.0,  0.0 );
		glVertex3f( -1.0,  0.5,  0.5 );
		glVertex3f( -0.5,  0.5,  1.0 );
		
		glColor3f( 1.0, 1.0, 0.0 );
		glMaterialfv(GL_FRONT, GL_DIFFUSE, yellow);
		glNormal3f(  0.0,  0.0,  1.0 );
		glVertex3f(  0.0, -1.0,  0.0 );
		glVertex3f( -0.5,  0.5,  1.0 );
		glVertex3f(  0.5,  0.5,  1.0 );
			
		glColor3f( 1.0, 0.0, 1.0 );
		glMaterialfv(GL_FRONT, GL_DIFFUSE, purple);
		glNormal3f(  1.0,  0.0,  1.0 );
		glVertex3f(  0.0, -1.0,  0.0 );
		glVertex3f(  0.5,  0.5,  1.0 );
		glVertex3f(  1.0,  0.5,  0.5 );
		
		glColor3f( 0.0, 1.0, 0.0 );
		glMaterialfv(GL_FRONT, GL_DIFFUSE, green);
		glNormal3f(  1.0,  0.0,  0.0 );
		glVertex3f(  0.0,  1.0,  0.0 );
		glVertex3f(  1.0,  0.5, -0.5 );
		glVertex3f(  1.0,  0.5,  0.5 );
		
		glColor3f( 0.0, 0.75, 1.0 );
		glMaterialfv(GL_FRONT, GL_DIFFUSE, cyan);
		glNormal3f(  1.0,  0.0, -1.0 );
		glVertex3f(  0.0,  1.0,  0.0 );
		glVertex3f(  1.0,  0.5, -0.5 );
		glVertex3f(  0.5,  0.5, -1.0 );
		
		glColor3f( 1.0, 1.0, 0.0 );
		glMaterialfv(GL_FRONT, GL_DIFFUSE, yellow);
		glNormal3f(  0.0,  0.0, -1.0 );
		glVertex3f(  0.0,  1.0,  0.0 );
		glVertex3f(  0.5,  0.5, -1.0 );
		glVertex3f( -0.5,  0.5, -1.0 );
			
		glColor3f( 1.0, 0.0, 1.0 );
		glMaterialfv(GL_FRONT, GL_DIFFUSE, purple);
		glNormal3f( -1.0,  0.0, -1.0 );
		glVertex3f(  0.0,  1.0,  0.0 );
		glVertex3f( -0.5,  0.5, -1.0 );
		glVertex3f( -1.0,  0.5, -0.5 );
		
		glColor3f( 1.0, 1.0, 1.0 );
		glMaterialfv(GL_FRONT, GL_DIFFUSE, white);
		glNormal3f( -1.0,  0.0,  0.0 );
		glVertex3f(  0.0,  1.0,  0.0 );
		glVertex3f( -1.0,  0.5, -0.5 );
		glVertex3f( -1.0,  0.5,  0.5 );
		
		glColor3f( 1.0, 0.6, 0.0 );
		glMaterialfv(GL_FRONT, GL_DIFFUSE, orange);
		glNormal3f( -1.0,  0.0,  1.0 );
		glVertex3f(  0.0,  1.0,  0.0 );
		glVertex3f( -1.0,  0.5,  0.5 );
		glVertex3f( -0.5,  0.5,  1.0 );
		
		glColor3f( 0.0, 0.0, 1.0 );
		glMaterialfv(GL_FRONT, GL_DIFFUSE, blue);
		glNormal3f(  0.0,  0.0,  1.0 );
		glVertex3f(  0.0,  1.0,  0.0 );
		glVertex3f( -0.5,  0.5,  1.0 );
		glVertex3f(  0.5,  0.5,  1.0 );
			
		glColor3f( 1.0, 0.0, 0.0 );
		glMaterialfv(GL_FRONT, GL_DIFFUSE, red);
		glNormal3f(  1.0,  0.0,  1.0 );
		glVertex3f(  0.0,  1.0,  0.0 );
		glVertex3f(  0.5,  0.5,  1.0 );
		glVertex3f(  1.0,  0.5,  0.5 );
	glEnd();




	
}