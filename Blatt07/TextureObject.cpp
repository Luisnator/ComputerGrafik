#include "TextureObject.h"

// image-file loader
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

std::map<TextureSampling, std::string> SamplingString {
		std::make_pair(Nearest, "GL_NEAREST"),
		std::make_pair(Linear,  "GL_LINEAR"),
		std::make_pair(NearestNearest, "GL_NEAREST_MIPMAP_NEAREST"),
		std::make_pair(NearestLinear,  "GL_NEAREST_MIPMAP_LINEAR"),
		std::make_pair(LinearNearest,  "GL_LINEAR_MIPMAP_NEAREST"),
		std::make_pair(LinearLinear,   "GL_LINEAR_MIPMAP_LINEAR")
	};
std::map<TextureSampling, TextureSampling> SamplingNext {
		std::make_pair(Nearest,        Linear),
		std::make_pair(Linear,         NearestNearest),
		std::make_pair(NearestNearest, NearestLinear),
		std::make_pair(NearestLinear,  LinearNearest),
		std::make_pair(LinearNearest,  LinearLinear),
		std::make_pair(LinearLinear,   Nearest)
	};

TextureObject::TextureObject()
  : m_id (0)
{
}

TextureObject::~TextureObject ()
{
   glDeleteTextures(1, &m_id);
}

void TextureObject::initTexture (const std::string& fName)
{
   m_name = fName;
   m_id = loadTexture(m_name);
}

void TextureObject::initCubeMap (const std::string& baseName)
{
   m_name = baseName;
   m_id   = loadCubeMap(m_name);
}

void TextureObject::use (void)
{
   glActiveTexture(GL_TEXTURE0);
   glBindTexture  (GL_TEXTURE_2D, getHandle());
}

GLuint TextureObject::loadTexture( const std::string & fName )
{
   int width, height;
   unsigned char * data = TextureObject::loadPixels(fName, width, height);

   if( data != nullptr ) {
        GLuint tex;
        glGenTextures(1, &tex);
        glBindTexture(GL_TEXTURE_2D, tex);
        glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8, width, height);
        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, data);

	glGenerateMipmap(GL_TEXTURE_2D);
	
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, Nearest);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, Nearest);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // GL_REPEAT
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE); // GL_REPEAT

        stbi_image_free(data);
        return tex;
   }

   return 0;
}

void TextureObject::setTexParameter(TextureSampling minification, TextureSampling magnification) {
	use(); // activate texture
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magnification);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minification);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, magnification);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, minification);
}
unsigned char* TextureObject::loadPixels(const std::string &fName, int & width, int & height)
{
    int bytesPerPix;
    stbi_set_flip_vertically_on_load(true);
    unsigned char *data = stbi_load(fName.c_str(), &width, &height, &bytesPerPix, 4);
    return data;
}

GLuint TextureObject::loadCubeMap(const std::string &baseName)
{
    GLuint texID;
    glGenTextures(1, &texID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, texID);

    const char * suffixes[] = { "posx", "negx", "posy", "negy", "posz", "negz" };
    GLuint targets[] = {
            GL_TEXTURE_CUBE_MAP_POSITIVE_X, GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
            GL_TEXTURE_CUBE_MAP_POSITIVE_Y, GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
            GL_TEXTURE_CUBE_MAP_POSITIVE_Z, GL_TEXTURE_CUBE_MAP_NEGATIVE_Z
    };

    stbi_set_flip_vertically_on_load(true);
    GLint w, h;

    // Load the first one to get width/height
    std::string texName = baseName + "_" + suffixes[0] + ".png";
    GLubyte * data = TextureObject::loadPixels(texName.c_str(), w, h);

    // Allocate immutable storage for the whole cube map texture
    glTexStorage2D(GL_TEXTURE_CUBE_MAP, 1, GL_RGBA8, w, h);

    glTexSubImage2D(targets[0], 0, 0, 0, w, h, GL_RGBA, GL_UNSIGNED_BYTE, data);
    stbi_image_free(data);

    // Load the other 5 cube-map faces
    for( int i = 1; i < 6; i++ ) {
        std::string texName = baseName + "_" + suffixes[i] + ".png";
        GLubyte * data = TextureObject::loadPixels(texName.c_str(), w, h);
        glTexSubImage2D(targets[i], 0, 0, 0, w, h, GL_RGBA, GL_UNSIGNED_BYTE, data);
        stbi_image_free(data);
    }

    glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, Nearest);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, Nearest);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // GL_REPEAT
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE); // GL_REPEAT
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE); // GL_REPEAT

    return texID;
}


