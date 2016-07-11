#include "OglCam.h"

glm::mat4 OglCam::update(frameinfo frinf)
{
	wi = frinf.rwidth;
	he = frinf.rheight;

	GLfloat xAngle = fov*(1.0*wi / he)*DEGTORAD;
	GLfloat yAngle = fov*DEGTORAD;

	look = glm::normalize( glm::vec3( xt, yt, zt ) - glm::vec3( x, y, z ) )*nearz;
	right = glm::cross( look, glm::vec3( 0, 1, 0 ) );
	right = glm::normalize( right ) * nearz * tan( xAngle / 2 );
	up_preroll = -glm::cross( look, right );
	up = glm::rotate( up_preroll, roll, look );
	up = glm::normalize( up ) * nearz * tan( yAngle / 2 );

	//if we are rotated upside down odd times, invert up vector
	float denom = floor( (roty + PI / 2.) / PI ) / 2.;
	if ( floor( denom ) != denom )
		up = -up;

	glm::mat4 Projection = glm::perspective( fov, 1.f*wi / he, nearz, farz );
	glm::mat4 View = glm::lookAt(
		glm::vec3( x, y, z ),
		glm::vec3( xt, yt, zt ),
		up
		);
	glm::mat4 ViewOrigin = glm::lookAt(
		glm::vec3( 0, 0, 0 ),
		glm::vec3( xt-x, yt-y, zt-z ),
		up
		);

	glm::mat4 MVP = Projection*View;
	glm::mat4 MVPOrigin = Projection*ViewOrigin;

	matrix = MVP;
	invMatrix = glm::inverse( MVPOrigin );
	prevMatrix = MVP;
	noTranslMatrix = MVPOrigin;
	return MVP;
}
void OglCam::lookat( float &xo, float &yo, float &zo, float rotxi, float rotyi )
{
	xo = sin( rotxi )*cos( rotyi );
	zo = cos( rotxi )*cos( rotyi );
	yo = sin( rotyi );
}
void OglCam::lookat( )
{
	xt = x + sin( rotx )*cos( roty );
	zt = z + cos( rotx )*cos( roty );
	yt = y + sin( roty );
}
void OglCam::init( const char* name )
{
	namestr = name;
	//namestr+="_pos";
}
glm::vec3 OglCam::getGeneric( )
{
	return right - look + up;
}
OglCam::OglCam( )
{
	x = 4; y = 3; z = 4;
	x = 0; y = 0; z = 0;
	xt = 0; yt = 0; zt = 0;
	/*x = 10.5114;
	y = 52.615;
	z = 1.2199;
	xt = 10.2639;
	yt = 51.6512;
	zt = 1.11835;
	x = 5.9;
	y = 9.9;
	z = 6.4;
	rotx = -2.25;
	roty = -0.9;*/
	fov = 45; nearz = 0.1f; farz = 1000; roll = 0;
	wi = 640; he = 480;
	//rotx = -2.44500089; roty = -0.475000024;
	rotx = 0; roty = 0.5;
	//rotx = -2.16; roty = -1.37;
	look = right = up = up_preroll = glm::vec3( 1, 1, 1 );
	prevMatrix = glm::mat4( 1 );
}
CamData OglCam::getData()
{
	CamData data;
	data.pos = glm::vec3( x, y, z );
	data.zClip = glm::vec2( nearz, farz );
	data.zClipRatio = 1.0f * farz / nearz;
	data.invMatrix = invMatrix;
	data.noTranslMatrix = noTranslMatrix;
	data.matrix = matrix;

	return data;
}

//////////////////////

CamWASDRF::CamWASDRF( )
{
	OglCam( );
	lpressed = false;
}
void CamWASDRF::doinput( MouseInfo &mi, Keyboard* keyb, double delta_ )
{
	//return;
	//cout << rotx << " " << roty << endl;

	delta = delta_;
	if ( mi.l )
	{
		rotx -= 1.*mi.x / sens;
		roty -= 1.*mi.y / sens;
	}
	roty = min( roty, PI/2-0.0001 );
	roty = max( roty, -PI/2+0.0001 );

	bool shift = keyb->isModifierDown( Keyboard::Shift );
	if ( keyb->isKeyDown( KC_W ) )
		move( KC_W, shift );
	if ( keyb->isKeyDown( KC_A ) )
		move( KC_A, shift );
	if ( keyb->isKeyDown( KC_S ) )
		move( KC_S, shift );
	if ( keyb->isKeyDown( KC_D ) )
		move( KC_D, shift );
	if ( keyb->isKeyDown( KC_R ) )
		move( KC_R, shift );
	if ( keyb->isKeyDown( KC_F ) )
		move( KC_F, shift );

	//lookat( xt, yt, zt, rotx, roty );
	this->lookat();
}
void CamWASDRF::move( KeyCode code, bool shift )
{
	switch ( code )
	{
		case KC_W:
			x += sin( rotx )*cos( roty )*speed*delta;
			z += cos( rotx )*cos( roty )*speed*delta;
			y += sin( roty )*speed*delta;
			break;
		case KC_S:
			x -= sin( rotx )*cos( roty )*speed*delta;
			z -= cos( rotx )*cos( roty )*speed*delta;
			y -= sin( roty )*speed*delta;
			break;
		case KC_A:
			x += sin( rotx + PI / 2 )*speed*delta;
			z += cos( rotx + PI / 2 )*speed*delta;
			break;
		case KC_D:
			x += sin( rotx - PI / 2 )*speed*delta;
			z += cos( rotx - PI / 2 )*speed*delta;
			break;
		case KC_R:
			y += speed*delta;
			break;
		case KC_F:
			y -= speed*delta;
			break;
	}
}


/////////////////////////////////////////////////////////////////

glm::mat4 OglCamOrtho::update( frameinfo frinf, glm::vec3 pos )
{
	wi = frinf.rwidth;
	he = frinf.rheight;

	glm::mat4 ViewOrigin = glm::lookAt(
		glm::vec3(0,0,0),
		glm::vec3(xt, yt, zt),
		up
		);

	const double off = 20;

	glm::vec4 offs[8] = 
	{
		glm::vec4(-off, -off, -off, 0),
		glm::vec4(-off, -off, off, 0),
		glm::vec4(-off, off, -off, 0),
		glm::vec4(-off, off, off, 0),
		glm::vec4(off, -off, -off, 0),
		glm::vec4(off, -off, off, 0),
		glm::vec4(off, off, -off, 0),
		glm::vec4(off, off, off, 0)
	};

	double x1 = FLT_MAX, y1 = FLT_MAX, z1 = FLT_MAX;
	double x2 = FLT_MIN, y2 = FLT_MIN, z2 = FLT_MIN;

	glm::vec4 pos4 = glm::vec4(pos.x, pos.y, pos.z, 1);
	for (uint i = 0; i < 8; i++) {
		glm::vec4 pos2;

		pos2 = pos4 + offs[i];
		pos2 = ViewOrigin*pos2;
		pos2.x /= pos2.w;
		pos2.y /= pos2.w;
		pos2.z /= pos2.w;

		x1 = min(x1, pos2.x);
		x2 = max(x2, pos2.x);
		y1 = min(y1, pos2.y);
		y2 = max(y2, pos2.y);
		z1 = min(z1, pos2.z);
		z2 = max(z2, pos2.z);
	}

	pos4 = ViewOrigin*pos4;
	pos4.x /= pos4.w;
	pos4.y /= pos4.w;
	pos4.z /= pos4.w;

	GLfloat left = x1, right = x2, bottom = y1, top = y2, nearr = pos4.z - 600, farr = pos4.z + 600;
	glm::mat4 Projection = glm::ortho( left, right, bottom, top, nearr, farr );

	prevMatrix = matrix;
	glm::mat4 MVPOrigin = Projection*ViewOrigin;
	invMatrix = glm::inverse( MVPOrigin );
	matrix = MVPOrigin;
	return MVPOrigin;
}

///////////////////////////////////////////////////////////

CamFollow::CamFollow()
{
	OglCam();
	lpressed = false;
	dist = 30;
}
void CamFollow::doinput(MouseInfo &mi, Keyboard* keyb, double delta_)
{
	//return;
	//cout << rotx << " " << roty << endl;

	delta = delta_;
	if (mi.l)
	{
		rotx -= 1.*mi.x / sens;
		roty += 1.*mi.y / sens;
	}
	dist -= 1.*mi.z / sensZ;
	roty = min(roty, PI/2-0.0001);
	roty = max(roty, -PI/2+0.0001);

	//lookat( xt, yt, zt, rotx, roty );
	//this->lookat();
}
void CamFollow::follow(glm::vec3 target) {
	xt = target.x;
	yt = target.y;
	zt = target.z;

	x = xt + sin(rotx)*cos(roty)*dist;
	z = zt + cos(rotx)*cos(roty)*dist;
	y = yt + sin(roty)*dist;
}