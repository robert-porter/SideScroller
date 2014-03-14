#include "Renderer.h"
#include <stdio.h>
#include <windows.h>
#include <GL/gl.h>
#include <GL/GLU.h>
#include <GL/freeglut.h>
#include "Camera.h"

Vector2 Renderer::circleVertex[numCircleVerts];

struct Texture {
	TextureID textureID;
	int width, height;
};



void Renderer::SetWorldViewMatrix(const Camera& camera) {
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	gluOrtho2D(camera.position.x - camera.halfSize.x,
			   camera.position.x + camera.halfSize.x, 
			   camera.position.y - camera.halfSize.y,
			   camera.position.y + camera.halfSize.y);
}

void Renderer::ClearScreen( ) {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Renderer::SwapBuffers() {
	glutSwapBuffers();
}

void Renderer::Init() {
	pCamera = NULL;
	
	float theta = 0.0f;

	const float dtheta = 2.0f * PI / numCircleVerts;

	for (int i = 0; i < numCircleVerts; i++) {
		circleVertex[i] = Vector2(cosf(theta), sinf(theta));
		theta += dtheta;
	}
}

Renderer::~Renderer(void) {
}


void Renderer::DrawAABB(const AABB& aabb, const Color4f &color) const {
	//v3v2
	//v0v1
	Vector2 v0(aabb.minExt);
	Vector2 v1(aabb.maxExt.x, aabb.minExt.y);
	Vector2 v2(aabb.maxExt);
	Vector2 v3(aabb.minExt.x, aabb.maxExt.y);


	glColor4f(color.r, color.g, color.b, color.a);
	glBegin(GL_QUADS);
	glVertex2f(v0.x, v0.y);
	glVertex2f(v1.x, v1.y);
	glVertex2f(v2.x, v2.y);
	glVertex2f(v3.x, v3.y);
	glEnd();


	glColor4f(1.0f-color.r, 1.0f-color.g, 1.0f-color.b, 1.0f);
	glBegin(GL_LINE_LOOP);
	glVertex2f(v0.x, v0.y);
	glVertex2f(v1.x, v1.y);
	glVertex2f(v2.x, v2.y);
	glVertex2f(v3.x, v3.y);
	glEnd();
}


void Renderer::DrawCircle(const Vector2& center, float radius, const Color4f color) const {
	glColor4f(color.r, color.g, color.b, color.a);
	glBegin(GL_TRIANGLE_FAN);
	for (int i = 0; i < numCircleVerts; i++) {
		Vector2 v = center + radius * circleVertex[i];
		glVertex2f(v.x, v.y);
	}
	glEnd();

	glColor3f(1-color.r, 1-color.g, 1-color.b);
	glBegin(GL_LINE_LOOP);
	for (int i = 0; i < numCircleVerts; i++) {
		Vector2 v = center + radius * circleVertex[i];
		glVertex2f(v.x, v.y);
	}
	glEnd();
}


void Renderer::DrawCapsule(const Vector2& A, const Vector2& B, float radius, const Color4f color) const {
	Vector2 AB = B-A;
	float ang0 = atan2f(AB.y, AB.x) + PI/2.0f;
	
	glEnable(GL_ALPHA);
	const float N = 16;

	glBegin(GL_TRIANGLE_FAN);
	glColor4f(color.r, color.g, color.b, color.a);
	for (int n = 0; n <= N; n++) {
		float f = ang0 + (float(n) / float(N)) * PI;
		Vector2 v = A + radius * Vector2(cosf(f), sinf(f));
		glVertex2f(v.x, v.y);
	}
	for (int n = 0; n <= N; n++) {
		float f = ang0 + PI + (float(n) / float(N)) * PI;
		Vector2 v = B + radius * Vector2(cosf(f), sinf(f));
		glVertex2f(v.x, v.y);
	}
	glEnd();

	glColor3f(1.0f, 1.0f, 1.0f);
	glBegin(GL_LINE_LOOP);
	for (int n = 0; n <= N; n++) {
		float f = ang0 + (float(n) / float(N)) * PI;
		Vector2 v = A + radius * Vector2(cosf(f), sinf(f));
		glVertex2f(v.x, v.y);
	}
	for (int n = 0; n <= N; n++) {
		float f = ang0 + PI + (float(n) / float(N)) * PI;
		Vector2 v = B + radius * Vector2(cosf(f), sinf(f));
		glVertex2f(v.x, v.y);
	}
	glEnd();
}



void Renderer::DrawTexturedQuad(TextureID texture, const Vector2 &centerPos, 
								float w, float h, 
								float angle, const Vector2 &rotationCenter,
								bool flipX, bool flipY, const Color4f &tint) {

	Vector2 vertices[4];
	Vector2 texCoords[4];

	float hw = w/2.0f;
	float hh = h/2.0f;


	vertices[0] = Vector2(-hw, +hh);
	vertices[1] = Vector2(-hw, -hh);
	vertices[2] = Vector2(+hw, -hh);
	vertices[3] = Vector2(+hw, +hh);

	if(flipX) {
		texCoords[0].x = 1.0f;
		texCoords[1].x = 1.0f;
		texCoords[2].x = 0.0f;
		texCoords[3].x = 0.0f;
	} else {
		texCoords[0].x = 0.0f;
		texCoords[1].x = 0.0f;
		texCoords[2].x = 1.0f;
		texCoords[3].x = 1.0f;
	}
	
	if(flipY) {
		texCoords[0].y = 0.0f;
		texCoords[1].y = 1.0f;
		texCoords[2].y = 1.0f;
		texCoords[3].y = 0.0f;
	} else {
		texCoords[0].y = 1.0f;
		texCoords[1].y = 0.0f;
		texCoords[2].y = 0.0f;
		texCoords[3].y = 1.0f;
	}

	glm::mat2 rotation;
	rotation[0][0] = cosf(angle);
	rotation[0][1] = sinf(angle);
	rotation[1][0] = -sinf(angle);
	rotation[1][1] = cosf(angle);

	for(int i = 0; i < 4; i++) {
		vertices[i] = rotation * (vertices[i] - rotationCenter) + rotationCenter + centerPos;
	}


	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glEnable (GL_BLEND);
	glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glBegin(GL_QUADS);
	glColor4f(tint.r, tint.g, tint.b, tint.a);
	glTexCoord2f(texCoords[0].x, texCoords[0].y);
	glVertex2f(vertices[0].x, vertices[0].y);
	glTexCoord2f(texCoords[1].x, texCoords[1].y);
	glVertex2f(vertices[1].x, vertices[1].y);
	glTexCoord2f(texCoords[2].x, texCoords[2].y);
	glVertex2f(vertices[2].x, vertices[2].y);
	glTexCoord2f(texCoords[3].x, texCoords[3].y);
	glVertex2f(vertices[3].x, vertices[3].y);
	glEnd();
	glDisable(GL_TEXTURE_2D);
};

void Renderer::DrawTexturedQuad(TextureID texture, const Vector2 *vertices, const Vector2 *texCoords)
{
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, texture);
	glBegin(GL_QUADS);
	glTexCoord2f(texCoords[0].x, texCoords[0].y);
	glVertex2f(vertices[0].x, vertices[0].y);
	glTexCoord2f(texCoords[1].x, texCoords[1].y);
	glVertex2f(vertices[1].x, vertices[1].y);
	glTexCoord2f(texCoords[2].x, texCoords[2].y);
	glVertex2f(vertices[2].x, vertices[2].y);
	glTexCoord2f(texCoords[3].x, texCoords[3].y);
	glVertex2f(vertices[3].x, vertices[3].y);
	glEnd();
	glDisable(GL_TEXTURE_2D);
}


int num_texture=-1; //Counter to keep track of the last loaded texture

Texture LoadBitmap(const char *filename) 
{
	Texture texture;
    int i, j=0; //Index variables
    FILE *l_file; //File pointer
    unsigned char *l_texture; //The pointer to the memory zone in which we will load the texture
     
    // windows.h gives us these types to work with the Bitmap files
    BITMAPFILEHEADER fileheader; 
    BITMAPINFOHEADER infoheader;
    RGBTRIPLE rgb;

    num_texture++; // The counter of the current texture is increased

    if( (l_file = fopen(filename, "rb"))==NULL) 
	{
		texture.textureID = -1;
		texture.width = 0;
		texture.height = 0;
		return texture;
	}
    
    fread(&fileheader, sizeof(fileheader), 1, l_file); // Read the fileheader
    
    fseek(l_file, sizeof(fileheader), SEEK_SET); // Jump the fileheader
    fread(&infoheader, sizeof(infoheader), 1, l_file); // and read the infoheader

    // Now we need to allocate the memory for our image (width * height * color deep)
    l_texture = (byte *) malloc(infoheader.biWidth * infoheader.biHeight * 4);
    // And fill it with zeros
    memset(l_texture, 0, infoheader.biWidth * infoheader.biHeight * 4);
 
    // At this point we can read every pixel of the image
    for (i=0; i < infoheader.biWidth*infoheader.biHeight; i++)
    {            
            // We load an RGB value from the file
            fread(&rgb, sizeof(rgb), 1, l_file); 

            // And store it
            l_texture[j+0] = rgb.rgbtRed; // Red component
            l_texture[j+1] = rgb.rgbtGreen; // Green component
            l_texture[j+2] = rgb.rgbtBlue; // Blue component
			if(rgb.rgbtRed == 0 && rgb.rgbtGreen == 0 && rgb.rgbtBlue == 0)
				l_texture[j+3] = 0; // Alpha value
			else
				l_texture[j+3] = 255; // Alpha value
            j += 4; // Go to the next position
    }

    fclose(l_file); // Closes the file stream
     
    glBindTexture(GL_TEXTURE_2D, num_texture); // Bind the ID texture specified by the 2nd parameter

    // The next commands sets the texture parameters
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); // If the u,v coordinates overflow the range 0,1 the image is repeated
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); // The magnification function ("linear" produces better results)
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); //The minifying function

    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE); // We don't combine the color with the original surface color, use only the texture map.

    // Finally we define the 2d texture
    glTexImage2D(GL_TEXTURE_2D, 0, 4, infoheader.biWidth, infoheader.biHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, l_texture);

    // And create 2d mipmaps for the minifying function
  //  gluBuild2DMipmaps(GL_TEXTURE_2D, 4, infoheader.biWidth, infoheader.biHeight, GL_RGBA, GL_UNSIGNED_BYTE, l_texture);

    free(l_texture); // Free the memory we used to load the texture

	texture.textureID = num_texture;
	texture.width = infoheader.biWidth;
	texture.height = infoheader.biHeight;

	return texture;
}


TextureID Renderer::CreateTextureFromFile(const char *fileName) {
	return LoadBitmap(fileName).textureID;
}