#include "OglShader.h"

#define UNF_MAT4 0
#define UNF_VEC3 1
#define UNF_FLOAT 2
#define UNF_TEXTURE 3
#define UNF_VEC2 4
#define UNF_INT 5

OglShaderProg::OglShaderProg()
{
	textures = 0;
}

void OglShader::compile_( const char * file_path, GLenum type )
{
	ShaderID = glCreateShader( type );

	std::string ShaderCode;
	std::ifstream ShaderStream( file_path, std::ios::in );
	if ( ShaderStream.is_open( ) )
	{
		std::string Line = "";
		while ( getline( ShaderStream, Line ) )
		{
			ShaderCode += "\n" + Line;
			if ( Line.substr( 0, 7 ) == "uniform" )
			{
				int startpos = Line.find_last_of( " " );
				int prestartpos = Line.substr( 0, startpos - 1 ).find_last_of( " " );
				string name = Line.substr( startpos + 1, Line.length( ) - startpos - 2 );
				string type = Line.substr( prestartpos + 1, startpos - prestartpos - 1 );

				char typec = 0;
				if ( type == "mat4" )
					typec = UNF_MAT4;
				else if ( type == "vec3" )
					typec = UNF_VEC3;
				else if ( type == "vec2" )
					typec = UNF_VEC2;
				else if ( type == "float" )
					typec = UNF_FLOAT;
				else if ( type == "int" )
					typec = UNF_INT;
				else if ( type == "sampler2D" )
					typec = UNF_TEXTURE;
				else if ( type == "sampler2DShadow" )
					typec = UNF_TEXTURE;

				UniformNameType uniform( name, typec );
				uniformtypes.push_back( uniform );
			}
		}
		ShaderStream.close( );
	}

	printf( "Compiling shader : %s\n", file_path );
	char const * SourcePointer = ShaderCode.c_str( );
	glShaderSource( ShaderID, 1, &SourcePointer, NULL );
	glCompileShader( ShaderID );

	GLint Result = GL_FALSE;
	int InfoLogLength;

	glGetShaderiv( ShaderID, GL_COMPILE_STATUS, &Result );
	glGetShaderiv( ShaderID, GL_INFO_LOG_LENGTH, &InfoLogLength );
	std::vector<char> ShaderErrorMessage( InfoLogLength );
	if (ShaderErrorMessage.size()) {
		glGetShaderInfoLog(ShaderID, InfoLogLength, NULL, &ShaderErrorMessage[0]);
		fprintf(stdout, "%s\n", &ShaderErrorMessage[0]);
	}
}
OglShader::OglShader()
{
	del = true;
}
OglShader::~OglShader( )
{
	if (del)
		glDeleteShader( ShaderID );
}
void OglShader::notDelete()
{
	del = false;
}

void OglVertexShader::compile( const char * file_path )
{
	compile_( file_path, GL_VERTEX_SHADER );
}
void OglFragmentShader::compile( const char * file_path )
{
	compile_( file_path, GL_FRAGMENT_SHADER );
}
void OglShaderProg::processUniform( UniformNameType unf )
{
	map<int, int> m;


	GLuint id = glGetUniformLocation( ProgramID, unf.name.c_str( ) );
	UniformTypeID uniform( unf.type, id );
	uniforms[unf.name] = uniform;
}
void OglShaderProg::link( OglShader& vsh, OglShader& fsh )
{
	//uniforms._Mysize = 0;

	GLint VertexShaderID = vsh.ShaderID;
	GLint FragmentShaderID = fsh.ShaderID;
	GLint Result = GL_FALSE;
	int InfoLogLength;

	// Link the program
	fprintf( stdout, "Linking program\n" );
	ProgramID = glCreateProgram( );
	glAttachShader( ProgramID, VertexShaderID );
	glAttachShader( ProgramID, FragmentShaderID );
	glLinkProgram( ProgramID );

	// Check the program
	glGetProgramiv( ProgramID, GL_LINK_STATUS, &Result );
	glGetProgramiv( ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength );
	std::vector<char> ProgramErrorMessage( max( InfoLogLength, int( 1 ) ) );
	glGetProgramInfoLog( ProgramID, InfoLogLength, NULL, &ProgramErrorMessage[0] );
	fprintf( stdout, "%s\n", &ProgramErrorMessage[0] );

	// Know about uniforms
	for ( uint i = 0; i < vsh.uniformtypes.size( ); i++ )
		processUniform( vsh.uniformtypes[i] );
	for ( int i = 0; i < fsh.uniformtypes.size( ); i++ )
		processUniform( fsh.uniformtypes[i] );
}
void OglShaderProg::enable( )
{
	glUseProgram( ProgramID );
	textures = 0;
}
void OglShaderProg::setColor( float r, float g, float b )
{
}

void OglShaderProg::regTexture( OglTex *tex )
{
	textureMap[tex] = textures;
	glActiveTexture( GL_TEXTURE0+textures );
	glBindTexture( GL_TEXTURE_2D, tex->getId() );
	textures++;
}

void OglShaderProg::importValue( string name, float data )
{
	UniformTypeID uniform = uniforms[name];
	if ( uniform.type == UNF_FLOAT )
		glUniform1f( uniform.id, data );
	else
		cout << name << ": type float mismatch" << endl;
}

void OglShaderProg::importValue( string name, int data )
{
	UniformTypeID uniform = uniforms[name];
	if ( uniform.type == UNF_INT )
		glUniform1i( uniform.id, data );
	else
		cout << name << ": type int mismatch" << endl;
}

void OglShaderProg::importValue( string name, glm::mat4 data )
{
	UniformTypeID uniform = uniforms[name];
	if ( uniform.type == UNF_MAT4 )
		glUniformMatrix4fv( uniform.id, 1, GL_FALSE, (float*)&data );
	else
		cout << name << ": type matrix mismatch" << endl;
}
void OglShaderProg::importValue( string name, glm::vec3 data )
{
	UniformTypeID uniform = uniforms[name];
	if ( uniform.type == UNF_VEC3 )
		glUniform3f( uniform.id, data.x, data.y, data.z );
	else
		cout << name << ": type vec3 mismatch" << endl;
}
void OglShaderProg::importValue( string name, glm::vec2 data )
{
	UniformTypeID uniform = uniforms[name];
	if ( uniform.type == UNF_VEC2 )
		glUniform2f( uniform.id, data.x, data.y);
	else
		cout << name << ": type vec2 mismatch" << endl;
}
void OglShaderProg::importValue( string name, OglTex *data )
{
	regTexture( data );

	UniformTypeID uniform = uniforms[name];
	if ( uniform.type == UNF_TEXTURE )
		glUniform1i( uniform.id, textures-1 );
	else
		cout << name << ": type texture mismatch" << endl;
}