#pragma once



#include "MathUtils.h"
#include "Render.h"


class Camera;

const int numCircleVerts = 16;

class Renderer
{
public:
	~Renderer(void);

	TextureID CreateTextureFromFile(const char *fileName);

	void Init();
	void SetWorldViewMatrix(const Camera& camera);

	void ClearScreen();

	void SwapBuffers();

	void DrawTexturedQuad( 
		TextureID texture, 
		const Vector2 &centerPos,
		float w, float h, 
		float angle,
		const Vector2 &rotationCenter, 
		bool flipX, bool flipY,
		const Color4f &tint
		// UVRect uvRect
		);

	void DrawTexturedQuad(TextureID texture, const Vector2 *vertices, const Vector2 *texCoords);

	void DrawAABB(const AABB& aabb, const Color4f &color) const;
	void DrawCircle(const Vector2& center, float radius, const Color4f color) const;
	void DrawCapsule(const Vector2& A, const Vector2 &B, float radius, const Color4f color) const;

	void SetCamera(Camera *pCam) { pCamera = pCam; }
	Camera *GetCamera() const { return pCamera; }
private:
	Camera *pCamera;
	

	static Vector2 circleVertex[numCircleVerts];
};
