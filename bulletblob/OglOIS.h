#ifndef OIS_DF654H564D65H46F5E465456F
#define OIS_DF654H564D65H46F5E465456F

#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include <fstream>
#include <GL/glew.h>

#include <gl/gl.h>
#include <gl/glu.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

//OIS
#include "OISInputManager.h"
#include "OISException.h"
#include "OISMouse.h"
#include "OISEvents.h"
#include "OISKeyboard.h"

#ifdef OIS_WIN32_PLATFORM
#include <windows.h>
#endif

using namespace std;
using namespace OIS;

struct MouseInfo {
	int x, y, z;
	bool l, m, r;
	bool lp, mp, rp;
	bool lr, mr, rr;
};

class OglOIS {
public:
	void init();
	MouseInfo getmouse( );
	Keyboard *keyb;
private:
	Mouse *mouse;
	MouseInfo mi;
};

#endif