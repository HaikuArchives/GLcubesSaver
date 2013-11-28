//.---------------.
//|               |
//| GLCubeSaver.h |
//|               |
//'---------------'

#include <ScreenSaver.h>
#include <View.h>

GLfloat red[] = {1.0, 0.0, 0.0, 1.0};
GLfloat blue[] = {0.0, 0.0, 1.0, 1.0};
GLfloat green[] = {0.0, 0.5, 0.0, 1.0};
GLfloat purple[] = {1.0, 0.0, 1.0, 1.0};
GLfloat orange[] = {1.0, 0.6, 0.0, 1.0};
GLfloat yellow[] = {1.0, 1.0, 0.0, 1.0};
GLfloat cyan[] = {0.0, 0.75, 1.0, 1.0};
GLfloat white[] = {1.0, 1.0, 1.0, 1.0};

//*********************************************************************************************

class GLCubes : public BScreenSaver
{
	public:
		GLCubes(BMessage *message, image_id image);

		status_t	SaveState(BMessage *prefs) const;
		void		restore_state(BMessage *prefs);
		
		void		StartConfig(BView *view);

		status_t	StartSaver(BView *view, bool preview);
		void		StopSaver();
		void		DirectConnected(direct_buffer_info *info);
		void		DirectDraw(int32 frame);
	
	private:
		friend class GLCubeConfig;
		friend class GLCubeView;	
		
		GLCubeView	*viewport;

		int32		numcubes;
		float		cubesize;
		int32		cubespin;
		bool		nobounds;
		bool		wireframe;
		bool		fountain;
		bool		lights;
		bool		opaque;
		bool		solidcolor;
		bool		pulsate;
		bool		collisions;
		int32		shape;
};

//.----------------.
//|                |
//|     Config     |
//|                |
//'----------------'

//*********************************************************************************************

enum {
		GLC_NUMBER_CUBES	= 'numc',
		GLC_CUBE_SIZE		= 'size',
		GLC_CUBE_SPIN		= 'spin',
		GLC_NOBOUNDS		= 'bnds',
		GLC_WIREFRAME		= 'wire',
		GLC_FOUNTAIN		= 'foun',
		GLC_LIGHTS			= 'lite',
		GLC_COLLISIONS		= 'bang',
		GLC_OPAQUE			= 'glas',
		GLC_SOLID_COLOR		= 'sold',
		GLC_PULSATE			= 'puls',
		GLC_CUBE			= 'cube',
		GLC_PYRAMID			= 'pyra',
		GLC_DIAMOND			= 'diam',
		GLC_GEM				= 'jems',
		GLC_SHAPES			= 'shap'
};

//******************************************

class GLCubes;

class GLCubeConfig : public BView
{
	public:
		GLCubeConfig(BRect frame, GLCubes *saver);
		
		void AttachedToWindow();
		void MessageReceived(BMessage *msg);
		
	private:
		GLCubes		*saver;
		
		uint32		window_flags;
		
		BSlider		*slider;
		BSlider		*sslider;
		BSlider		*rslider;
		BCheckBox	*nobounds;
		BCheckBox	*wireframe;
		BCheckBox	*fountain;
		BCheckBox	*lights;
		BCheckBox	*opaque;
		BCheckBox	*solidcolor;
		BCheckBox	*pulsate;
		BCheckBox	*collisions;
		BMenuField	*shapes;
		BMenu		*menu;
		BMenuItem	*cube;
		BMenuItem	*pyramid;
		BMenuItem	*gem;
		BMenuItem	*diamond;
};

//.--------------.
//|              |
//|     View     |
//|              |
//'--------------'

//*********************************************************************************************

class GLCubeView : public BGLView
{
	public:
			GLCubeView(BRect r, char *name, ulong resizingMode, ulong options, GLCubes *saver);
	void	AttachedToWindow();
	void	Draw();
	void	Advance();
			
	GLfloat xpos[51];
	GLfloat ypos[51];
	GLfloat xdir[51];
	GLfloat ydir[51];
	GLfloat xrot[51];
	GLfloat yrot[51];
	GLfloat size[51];
	
	private:

	GLCubes	*saver;
		
	int		j;
	float	scale[51];
	GLint	numcubes;
	float	cubesize;
	GLint	cubespin;	
	bool	nobounds;
	bool	wireframe;
		
	BBitmap *texture_bits;
};

void DrawCube (void);
void DrawPyramid (void);
void DrawJem (void);
void DrawDiamond (void);