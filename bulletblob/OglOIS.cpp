#include "OglOIS.h"

#if defined OIS_LINUX_PLATFORM
#include <X11/Xlib.h>
#endif

void OglOIS::init()
{
	OIS::ParamList pl;
	OIS::InputManager *mInputSystem;

	/*SDL_SysWMinfo info;
	SDL_VERSION( &info.version );
	SDL_GetWindowWMInfo( window, &info );*/
	size_t m_windowHandle;
#if defined OIS_WIN32_PLATFORM
	m_windowHandle = (int) GetActiveWindow();//*(reinterpret_cast<std::size_t*>(&(/*info.info.x11.window*/info.info.win.window)));
#elif defined OIS_LINUX_PLATFORM
	Display *display;
	Window focus;
	int revert;

	display = XOpenDisplay( NULL );
	XGetInputFocus( display, &focus, &revert );
	m_windowHandle = (int) focus;
#endif

	ostringstream windowHndStr;
	windowHndStr << m_windowHandle;
	pl.insert( std::make_pair( std::string( "WINDOW" ), windowHndStr.str( ) ) );

#if defined OIS_WIN32_PLATFORM
	pl.insert( std::make_pair( std::string( "w32_mouse" ), std::string( "DISCL_FOREGROUND" ) ) );
	pl.insert( std::make_pair( std::string( "w32_mouse" ), std::string( "DISCL_NONEXCLUSIVE" ) ) );
#elif defined OIS_LINUX_PLATFORM
	pl.insert( std::make_pair( std::string( "x11_mouse_grab" ), std::string( "false" ) ) );
	pl.insert( std::make_pair( std::string( "x11_mouse_hide" ), std::string( "false" ) ) );;
#endif

	mInputSystem = OIS::InputManager::createInputSystem( pl );
	mouse = static_cast<OIS::Mouse*>(mInputSystem->createInputObject( OIS::OISMouse, false ));
	keyb = static_cast<OIS::Keyboard*>(mInputSystem->createInputObject( OIS::OISKeyboard, false ));
}

MouseInfo OglOIS::getmouse( )
{
	mouse->capture( );
	const MouseState &ms = mouse->getMouseState( );

	mi.x = ms.X.rel;
	mi.y = ms.Y.rel;
	mi.z = ms.Z.rel;

	mi.lp = (ms.buttonDown( OIS::MB_Left ) && !mi.l);
	mi.mp = (ms.buttonDown( OIS::MB_Middle ) && !mi.m);
	mi.rp = (ms.buttonDown( OIS::MB_Right ) && !mi.r);

	mi.lr = (!ms.buttonDown( OIS::MB_Left ) && mi.l);
	mi.mr = (!ms.buttonDown( OIS::MB_Middle ) && mi.m);
	mi.rr = (!ms.buttonDown( OIS::MB_Right ) && mi.r);

	mi.l = ms.buttonDown( OIS::MB_Left );
	mi.m = ms.buttonDown( OIS::MB_Middle );
	mi.r = ms.buttonDown( OIS::MB_Right );

	return mi;
}