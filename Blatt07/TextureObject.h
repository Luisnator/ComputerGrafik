#pragma once

#ifndef TEXTUREOBJECT_H
#define TEXTUREOBJECT_H

#include <GL/glew.h>
#include <string>
#include <map>

enum TextureSampling {
  Nearest = GL_NEAREST,
  Linear = GL_LINEAR,
  NearestNearest = GL_NEAREST_MIPMAP_NEAREST,
  NearestLinear = GL_NEAREST_MIPMAP_LINEAR,
  LinearNearest = GL_LINEAR_MIPMAP_NEAREST,
  LinearLinear = GL_LINEAR_MIPMAP_LINEAR
};
extern std::map<TextureSampling, std::string> SamplingString; 
extern std::map<TextureSampling, TextureSampling> SamplingNext;

/** OpenGL texture object. Created from image files: JPG, PNG, BMP, TIFF.
 */
class TextureObject
{
public:  
    TextureObject ();
   ~TextureObject ();

   void initTexture (const std::string& fName);
   void initCubeMap (const std::string& baseName);

   void use (void);
   
   inline GLuint getHandle () const {
     return m_id;
   }
   inline std::string getFilename () const {
     return m_name;
   }

   void setTexParameter(TextureSampling minification, TextureSampling magnification);
   
private:
   GLuint          loadTexture( const std::string & fName );
   GLuint          loadCubeMap( const std::string & baseName );
   static unsigned char * loadPixels ( const std::string & fName, int & w, int & h );
   
   GLuint      m_id;
   std::string m_name; // filename or basename of six files
};

#endif
