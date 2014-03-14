#include "Render.h"
#include "Renderer.h"
#include "Entity.h"
#include <windows.h>
#include <GL\GL.h>
void Graphics::Draw(Renderer *renderer)
{
	Fixture *fixture =  owner->body.GetFixtureList();

	while ( fixture )
	{
		if(visible)
		{
		if ( fixture->GetType() == eCircle )
		{
			renderer->DrawTexturedQuad(
				texture, 
				owner->body.GetPosition() + static_cast<Circle*>(fixture->GetShape())->localPos,
				static_cast<Circle*>(fixture->GetShape())->radius * 2.0f, 
				static_cast<Circle*>(fixture->GetShape())->radius * 2.0f, 
				angle, 
				Vector2(0.0f, 0.0f), //owner->body.GetPosition() + static_cast<Circle*>(fixture->GetShape())->localPos,
				flipX, 
				flipY, 
				color
				);
		}
		else if ( fixture->GetType() == eAABox )
		{
			Vector2 minExt = static_cast<AABox*>(fixture->GetShape())->minExt;
			Vector2 maxExt = static_cast<AABox*>(fixture->GetShape())->maxExt;
			Vector2 boxSize = maxExt - minExt;


			renderer->DrawTexturedQuad(
				texture, 
				owner->body.GetPosition() + boxSize/2.0f,
				boxSize.x, 
				boxSize.y, 
				0.0f, 
				owner->body.GetPosition() + boxSize/2.0f,
				flipX, 
				flipY, 
				color
				);

		}
		else if ( fixture->GetType() == eCapsule )
		{

			Vector2 pos0 = static_cast<Capsule*>(fixture->GetShape())->localPos0 + owner->body.GetPosition();
			Vector2 pos1 = static_cast<Capsule*>(fixture->GetShape())->localPos1 + owner->body.GetPosition();
			float radius = static_cast<Capsule*>(fixture->GetShape())->radius;

			Vector2 normal = normalize(pos1-pos0);
			Vector2 tangent = Vector2(-normal.y, normal.x);
			
			Vector2 verts[4];
			verts[0] = pos0 - normal * radius - tangent * radius; 
			verts[1] = pos1 + normal * radius - tangent * radius; 
			verts[2] = pos1 + normal * radius + tangent * radius; 
			verts[3] = pos0 - normal * radius + tangent * radius; 


			Vector2 texCoords[4];
			if ( !flipX )
			{
			texCoords[0] = Vector2(0.0f, 0.0f);
			texCoords[1] = Vector2(1.0f, 0.0f);
			texCoords[2] = Vector2(1.0f, 1.0f);
			texCoords[3] = Vector2(0.0f, 1.0f);
			}
			else
			{
				texCoords[2] = Vector2(0.0f, 0.0f);
				texCoords[0] = Vector2(1.0f, 0.0f);
				texCoords[1] = Vector2(1.0f, 1.0f);
				texCoords[3] = Vector2(0.0f, 1.0f);
			}
			glColor4f(color.r, color.g, color.b, color.a);
			renderer->DrawTexturedQuad(texture, verts, texCoords);
		}
		
		}
		if ( drawShape )
		{
			fixture->GetShape()->Draw(*renderer, owner->body.GetPosition());
		}
		fixture = fixture->GetNext();
		}

}


void Animation::Draw(Renderer *renderer)
{
	texture = textureIDs[currentFrame];
	Graphics::Draw(renderer);
}