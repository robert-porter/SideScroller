#pragma once

#include "World.h"
#include "TinyXML2.h"
#include "glui\glui.h"
#include "CapsuleLoader.h"
#include "CreateEntity.h"

class Editor
{
public:

	enum EDIT_TYPE
	{
		ET_EDIT_CAPSULE,
		ET_SELECT_OBJECT,
		ET_ENTITY_SELECTED,
		ET_PAN
	};

	Editor(void);
	~Editor(void);

	void Draw(Renderer &renderer);
	void Keyboard(unsigned char key, int mouseX, int mouseY);
	void MouseButton(int, int);
	void MouseMove(int, int);
	void Special(int key, int x, int y);
	void Update();

	void CreateControls();
	void CreateEntityControls();

	bool editMode;

	static void SaveLevelToFile(int);
	void LoadLevelFromFile(const char *filename);

	void ToggleEditMode() 
	{
		editMode = !editMode; 
		vertex = 0;
		selectedEntity = 0;
		editType = ET_SELECT_OBJECT;
	}
	static GLUI *glui;

private:
	bool leftMouseButtonDown;

	Entity *selectedEntity;
	Capsule editCapsule;
	
	EDIT_TYPE editType;

	GLUI_Panel *entityPanel;

	int vertex;
	Color4f color;
	int drawGraphics;
	int drawShapes;
};

