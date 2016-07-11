#include "OglTex.h"

OglTex::OglTex()
{
	intFormat = GL_RGB;
	format = GL_RGB;
	type = GL_UNSIGNED_BYTE;
	minFilter = GL_NEAREST;
	magFilter = GL_NEAREST;
	clampBorder = false;
	compMode = GL_NONE;
	color[0] = color[1] = color[2] = color[3] = 0;
}

void OglTex::init(bool mipmaps)
{
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, minFilter );
	if (mipmaps)
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	else
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, magFilter);
	if ( compMode != GL_NONE )
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, compMode );

	if (mipmaps) {
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	}
	if ( clampBorder )
	{
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER );
		glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER );
		glTexParameterfv( GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, color );
	}
}

void OglTexTarget::init(uint wi, uint he)
{
	glGenTextures( 1, &id );
	glBindTexture( GL_TEXTURE_2D, id );
	glTexImage2D( GL_TEXTURE_2D, 0, intFormat, wi, he, 0, format, type, 0 );
	
	OglTex::init( );
}

void OglTex::init( GLvoid* data, uint wi, uint he )
{
	glGenTextures( 1, &id );
	glBindTexture( GL_TEXTURE_2D, id );
	glTexImage2D( GL_TEXTURE_2D, 0, intFormat, wi, he, 0, format, type, data );

	OglTex::init( );
}

void OglTex::init(string name, bool mipmaps) {

	// DevIL library has to be initialized (ilInit() must be called)
	ilInit();

	// DevIL uses mechanism similar to OpenGL, each image has its ID (name)
	ILuint img_id;
	ilGenImages(1, &img_id); // generate one image ID (name)
	ilBindImage(img_id); // bind that generated id

	// set origin to LOWER LEFT corner (the orientation which OpenGL uses)
	ilEnable(IL_ORIGIN_SET);
	ilSetInteger(IL_ORIGIN_MODE, IL_ORIGIN_LOWER_LEFT);

	// this will load image data to the currently bound image
	if (ilLoadImage(name.c_str()) == IL_FALSE) {
		ilDeleteImages(1, &img_id);
		ILenum devilError = ilGetError();
		cout << __FUNCTION__ << " cannot load image " << name.c_str() << ", error " << devilError << endl;
		return;
	}

	// if the image was correctly loaded, we can obtain some informatins about our image
	ILint width = ilGetInteger(IL_IMAGE_WIDTH);
	ILint height = ilGetInteger(IL_IMAGE_HEIGHT);
	ILenum format2 = ilGetInteger(IL_IMAGE_FORMAT);
	// there are many possible image formats and data types
	// we will convert all image types to RGB or RGBA format, with one byte per channel
	unsigned Bpp = ((format2 == IL_RGBA || format2 == IL_BGRA) ? 4 : 3);
	char * data = new char[width * height * Bpp];
	// this will convert image to RGB or RGBA, one byte per channel and store data to our array
	ilCopyPixels(0, 0, 0, width, height, 1, Bpp == 4 ? IL_RGBA : IL_RGB, IL_UNSIGNED_BYTE, data);
	// image data has been copied, we dont need the DevIL object anymore
	ilDeleteImages(1, &img_id);

	glGenTextures(1, &id);
	glBindTexture(GL_TEXTURE_2D, id);

	if (mipmaps) {
		float aniso = 0.0f;
		glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &aniso);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, aniso);
	}

	if (mipmaps)
		glTexStorage2D(GL_TEXTURE_2D, 5, GL_RGB8, width, height);
	if (mipmaps)
		glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, GL_BGR, GL_UNSIGNED_BYTE, data);
	else
		glTexImage2D(GL_TEXTURE_2D, 0, intFormat, width, height, 0, format, type, data);
	if (mipmaps)
		glGenerateMipmap(GL_TEXTURE_2D);

	OglTex::init(true);
	delete[] data;
}