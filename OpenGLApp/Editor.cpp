#include "Editor.h"
#include <GL\freeglut.h>
#include "glui\glui.h"
#include "globals.h"

void Pan(int);
void SelectObject(int);
GLUI_RadioGroup *group = 0;
GLUI_Spinner* gravityScaleSpinner;

GLUI* Editor::glui;





struct EntityEdit {
	EntityEdit( ) {
		panel = Editor::glui->add_panel("Entity");

		gravityScale = Editor::glui->add_spinner_to_panel(panel, "Gravity scale", GLUI_SPINNER_FLOAT); 
		gravityScale->set_float_limits(-2.0f, 2.0f);

		velocityDamping = Editor::glui->add_spinner_to_panel(panel, "Gravity scale", GLUI_SPINNER_FLOAT); 
		velocityDamping->set_float_limits(0.0f, 20.0f);

		bodyType = Editor::glui->add_radiogroup();

		Editor::glui->add_radiobutton_to_group(bodyType, "Static");
		Editor::glui->add_radiobutton_to_group(bodyType, "Kinematic");
		Editor::glui->add_radiobutton_to_group(bodyType, "Dynamic");
	}


	void SyncGUI(Entity *e)	{
		gravityScale->set_float_val( e->body.gravityScale ); 
		velocityDamping->set_float_val( e->body.velocityDamping ); 
		bodyType->set_int_val( (int)e->body.GetType() ); 
	}

	void SyncEntity(Entity *e) {
		e->body.gravityScale = gravityScale->get_float_val(); 
		e->body.velocityDamping = velocityDamping->get_float_val(); 

		BodyType bt = (BodyType)bodyType->get_int_val();
		
		/*
			TODO : Check that the shape IS a circle if the type
			is dynamic.

			Need this to avoid crashing.
		*/

		e->body.SetType( (BodyType)bodyType->get_int_val() ); 
	}

	void SetVisible(bool visible)
	{
		if ( visible )
			panel->enable();
		else
			panel->disable();
	}

	GLUI_Panel *panel;
	GLUI_Spinner *gravityScale;
	GLUI_Spinner *velocityDamping;
	GLUI_RadioGroup *bodyType;
};

struct FixtureEdit
{
	GLUI_Spinner *density;
	GLUI_Spinner *restitution;
	GLUI_Spinner *friction;
};

struct CircleEdit
{
	GLUI_Spinner *radius;
};

struct CapsuleEdit {
	GLUI_Spinner *radius;
};



void Editor::CreateControls() {
	glui->add_button("Save Level", -1,  Editor::SaveLevelToFile);

	color = Color4f(1.0f, 1.0f, 1.0f, 1.0f);
	group = glui->add_radiogroup();

	glui->add_checkbox("Draw Graphics",(int *) &drawGraphics);
	glui->add_checkbox("Draw Shapes",(int *) &drawShapes);

	glui->add_radiobutton_to_group(group, "Create Cylinder");
	glui->add_radiobutton_to_group(group, "Select Object");
	glui->add_radiobutton_to_group(group, "Pan");

	CreateEntityControls();			
}

void Editor::CreateEntityControls()
{	
	entityPanel = glui->add_panel("Entity");
	gravityScaleSpinner = glui->add_spinner_to_panel(entityPanel, "Gravity scale", GLUI_SPINNER_FLOAT); 
	gravityScaleSpinner->set_float_limits(-20.0f, 20.0f);
	//velocityDampingSpinner = glui->add_spinner_to_panel(entityPanel, "Velocity Damping", GLUI_SPINNER_FLOAT); 
	//->set_float_limits(-20.0f, 20.0f);
}

Editor::Editor() {	
	leftMouseButtonDown = false;
	editMode = true;
	selectedEntity = 0;
	editCapsule;
	vertex = 0;
	drawGraphics = true;
	drawShapes = true;
	editType = ET_SELECT_OBJECT;
}

void Editor::Keyboard(unsigned char key, int mouseX, int mouseY) {
	if(key == 27 && vertex == 1) {
		vertex = 0;
	}

	if(key == 127) {
		if(selectedEntity && editType == ET_SELECT_OBJECT) {
			world->RemoveEntity(selectedEntity);
			selectedEntity = 0;
		}
	}
}

void Editor::Special(int key, int x, int y) {	
	float speed = 0.3f;
	if(key == GLUT_KEY_UP) { 
		camera.position += Vector2(0, speed);
	} else if(key == GLUT_KEY_DOWN) { 
		camera.position += Vector2(0, -speed);
	} 
	if(key == GLUT_KEY_RIGHT) { 
		camera.position += Vector2(speed, 0.0f);
	} else if(key == GLUT_KEY_LEFT) {
		camera.position += Vector2(-speed, 0.0f);
	}
}

void Editor::Draw(Renderer &renderer) {	
	Vector2 worldMousePos = ScreenToWorld(Vector2(mouse.posX, mouse.posY), camera, window);

	if ( vertex == 1 && editType == ET_EDIT_CAPSULE ) {
		editCapsule.localPos1 = worldMousePos;
		editCapsule.Draw(renderer, Vector2(0, 0));
	}

}

void Editor::MouseButton(int button, int state)
{
	if(button == GLUT_LEFT_BUTTON )	{
		if(state == GLUT_DOWN) {
			leftMouseButtonDown = true;
			
			if(	editType == ET_EDIT_CAPSULE ) {
				if(vertex == 0)	{
					editCapsule.radius = 0.2f;
					Vector2 vm = ScreenToWorld(Vector2(mouse.posX, mouse.posY), camera, window);
					editCapsule.localPos0 = vm; 
					vertex++;
				} else if(vertex == 1)	{
					Vector2 vm = ScreenToWorld(Vector2(mouse.posX, mouse.posY), camera, window);
					editCapsule.localPos1 = vm; 

					Entity *e = new Entity();
					FixtureDef def;
					def.shape = &editCapsule;
					e->body.AttachFixture(def);
					e->render = new Graphics(e);
					world->AddEntity(e);
					vertex = 0;
				}
			}

			if( editType = ET_SELECT_OBJECT) {
				Fixture *fixture = 0;
				
				if(fixture = world->QueryPoint(ScreenToWorld(Vector2(mouse.posX, mouse.posY), camera, window))) {
					selectedEntity = fixture->GetBody()->owner;
					gravityScaleSpinner->set_float_val(fixture->GetBody()->gravityScale);
				}
			}
		}
		else if(state == GLUT_UP) {
			leftMouseButtonDown = false;
		}
	}
}


void Editor::SaveLevelToFile(int) {

	const char *filename = "level1.txt";
	
	FILE *file = fopen(filename, "w");
	tinyxml2::XMLPrinter printer(file);

	printer.OpenElement("Level");

	for(int i = 0; i < world->entities.size(); i++) {
		if(world->entities[i]->body.GetFixtureList()->GetType() == eCapsule) {
			Capsule *capsule = (Capsule *)world->entities[i]->body.GetFixtureList()->GetShape();
			SaveShapeToFile(printer, capsule);
		}
	}

	printer.CloseElement();
	fclose(file);
}

void Editor::LoadLevelFromFile(const char *filename) {
	tinyxml2::XMLDocument levelDocument;
	
	levelDocument.LoadFile(filename);
	//world.Clear();
	if(!levelDocument.RootElement()) { 
		return;
	}

	tinyxml2::XMLElement *entityElement = levelDocument.RootElement()->FirstChildElement("Entity");
	while(entityElement != 0) {
		Entity *e = new Entity;
		Shape *s = CreateShapeFromFile(entityElement);
		FixtureDef def;
		def.shape = s;
		def.friction = 1.0f;
		e->body.SetPosition( Vector2(0.0f, 0.0f) );
		e->body.SetType( eStatic );
		e->body.AttachFixture(def);
		e->render = new Graphics(e);
		world->AddEntity(e);
		entityElement = entityElement->NextSiblingElement("Shape");
	}
}

void Editor::Update() {
	
	for(int i = 0; i < world->entities.size(); i++) {
		if(world->entities[i]->render) {
			world->entities[i]->render->drawShape = drawShapes;
		}
	}
	for(int i = 0; i < world->entities.size(); i++) {
		if(world->entities[i]->render) { 
			world->entities[i]->render->visible = drawGraphics;
		}
	}


	if(group->get_int_val() == 0) { 
		editType = ET_EDIT_CAPSULE;
	} else if(group->get_int_val() == 1) { 
		editType = ET_SELECT_OBJECT;
	} else if(group->get_int_val() == 2) { 
		editType = ET_PAN;
	}

	if ( selectedEntity ) {
		selectedEntity->body.gravityScale = gravityScaleSpinner->get_float_val();
	}
}
Editor::~Editor(void) {

}
