#include <windows.h>
#include <GL\gl.h>
#include <GL\freeglut.h>
#include "globals.h"
#include "World.h"
#include "Editor.h"
#include "CreateEntity.h"
#include "Jacobson.h"


Editor	editor;
float	lastTime;
float	elapsedTime;
float	currentTime;

JWorld jworld;
Cloth cloth;

void Update(const TimeStep &ts);
void Draw(const TimeStep &ts);


void MainLoop( ) {		
	glutSetWindow(window.id);
	glutPostRedisplay();

	lastTime = currentTime;
	currentTime = static_cast<float>(glutGet(GLUT_ELAPSED_TIME)) / 1000.0f;
	elapsedTime = currentTime - lastTime;

	
	if ( elapsedTime > 0.03666f ) {
		elapsedTime = 0.03666f;
	}
	if ( editor.editMode ) {
		elapsedTime = 0.0f;
	}
	
	TimeStep timeStep;
	timeStep.t0 = lastTime;
	timeStep.t = currentTime;
	timeStep.dt = elapsedTime;
	timeStep.invDt = timeStep.dt > 0.0f ? 1.0f / timeStep.dt : 0.0f;


	Update(timeStep);
	Draw(timeStep);
}

void Update(const TimeStep &ts) {
//	world->Update(ts);
//	world->GarbageCollection();
//	editor.Update();
}

void Draw(const TimeStep &ts) {	

	Vector2 m = ScreenToWorld(Vector2(mouse.posX,mouse.posY), camera, window);
//	jworld.Step(ts.dt, m);
	cloth.Step(m, ts.dt);

	if(!editor.editMode) {
		camera.position = world->dude->body.GetPosition();
	}

	renderer.ClearScreen();
	renderer.SetWorldViewMatrix(camera);

//	for(size_t i = 0; i < world->entities.size(); i++)	{	
//		world->entities[i]->render->Update(ts.dt);
//		world->entities[i]->render->Draw(&renderer);
//	}

//	for(size_t i = 0; i < numParticles; i++)	{	
//		renderer.DrawCircle(jworld.particles[i].x, 0.2, Color4f(1.0f, 0.0f, 0.0f, 1.0f));
//	}

	for(size_t i = 0; i < cloth.joint; i++)	{	
		renderer.DrawCapsule(cloth.joints[i].p0->x, cloth.joints[i].p1->x,  0.1f, Color4f(1.0f, 0.0f, 0.0f, 1.0f));
	}

//	editor.Draw(renderer);
	
	renderer.SwapBuffers();
}


void Init() {
	world->tileMap.Load("test.fmp", 100);

	textures.gunAnim = renderer.CreateTextureFromFile("darkwalltile.bmp");
	textures.anim = renderer.CreateTextureFromFile("plane.bmp");
	textures.dude = renderer.CreateTextureFromFile("idkmonster.bmp");
	textures.sawblade = renderer.CreateTextureFromFile("sawblade.bmp");


	world->dude = CreateMainPlayer(0, 5);
	world->AddEntity(world->dude);


	Entity *floor = CreateBox(-20.0f, 0.0f, 40.0f, 2.0f);
	world->AddEntity(floor);
	Entity *leftWall = CreateBox(-22.0f, 0.0f, 2.0f, 42.0f);
	world->AddEntity(leftWall);
	Entity *ceiling = CreateBox(-20.0f, 40.0f, 40.0f, 2.0f);
	world->AddEntity(ceiling);
	Entity *rightWall = CreateBox(20.0f, 0.0f, 2.0f, 42.0f);
	world->AddEntity(rightWall);
		
	for(int i = 1; i < 12; i++)	{	
		Entity *e = CreateBox(-12.0f + float(i)*1.1f, 13.2f, 1.0f, 1.0f);
		Entity* q = CreateDude(-12.0f + float(i)*1.1f, 15.0f);
		world->AddEntity(e);
		world->AddEntity(q);
	}


	Entity *platform = CreateUpDownMovingPlatform(0, 2, 6, 1);
	world->AddEntity(platform);

	platform = CreateUpDownMovingConveyerPlatform(-8, 2, 6, 1);
	world->AddEntity(platform);

	platform = CreateUpDownMovingConveyerPlatform(0, 12, 6, 1); 
	world->AddEntity(platform);


	Entity * saw = CreateSaw(15, 15);
	world->AddEntity(saw);

	editor.LoadLevelFromFile("level1.txt");
	camera.halfSize = Vector2(12.0f, 12.0f);
	renderer.SetCamera(&camera);
	camera.position = ScreenToWorld(Vector2(mouse.posX,mouse.posY), camera, window);
}

void SpecialKey(int key, int x, int y) {
	editor.Special(key, x, y);
}



void Resize(int newWindowWidth, int newWindowHeight) {
	window.width =newWindowWidth;
	window.height = newWindowHeight;

//	GLUI_Master.auto_set_viewport();
//	GLUI_Master.get_viewport_area(&a, &b, &window.width, &window.width);
//	glViewport(a, b, window.width, window.height);
}

void Keyboard(unsigned char key, int mx, int my) {
	mouse.posX = mx;
	mouse.posY = my;
	if ( key == 'e'  ) {
		editor.ToggleEditMode();
	} else if(editor.editMode) {
		editor.Keyboard(key, mx, my);
	}
}


void MouseButton(int button, int state, int mouseX, int mouseY) {
	mouse.posX = mouseX;
	mouse.posY = mouseY;
	editor.MouseButton(button, state);
}


void MouseActiveMotion(int mx, int my) {
	mouse.posX = mx;
	mouse.posY = my;
}

void MousePassiveMotion(int mx, int my) {
	mouse.posX = mx;
	mouse.posY = my;
}



void main(int argc, char **argv) {

	glutInit(&argc, argv);
	window.width = 1024;
	window.height = 1024;
	glutInitWindowSize(window.width, window.height);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);


	window.id = glutCreateWindow("LOL BRO");
	world = new World;

	glutDisplayFunc(MainLoop);
	//glutIdleFunc(MainLoop);
	glutMotionFunc(MouseActiveMotion);
	glutPassiveMotionFunc(MousePassiveMotion);


	//glfwEnable(GLFW_STICKY_KEYS);
	//glfwEnable(GLFW_STICKY_MOUSE_BUTTONS);
	editor.glui = GLUI_Master.create_glui_subwindow(window.id, GLUI_SUBWINDOW_RIGHT );
	


	GLUI_Master.set_glutIdleFunc(MainLoop);
	GLUI_Master.set_glutKeyboardFunc(Keyboard);
	GLUI_Master.set_glutSpecialFunc(SpecialKey);
	GLUI_Master.set_glutMouseFunc(MouseButton);
	GLUI_Master.set_glutReshapeFunc(Resize);
	 
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	
	int seed = 0x18a8c1db;  // LOL BRO
	srand(seed);

	editor.CreateControls();
	renderer.Init();
	Init();
	jworld.Init();
	cloth.Init();

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();


	glutMainLoop();
}

