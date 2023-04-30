#ifndef __APP_H__
#define __APP_H__

#include "Module.h"
#include "List.h"
#include "PerfTimer.h"
#include "Timer.h"
#include "PerformanceBar2.h"

#include "PugiXml/src/pugixml.hpp"

#define CONFIG_FILENAME		"config.xml"
#define SAVE_STATE_FILENAME "save_game.xml"

// Modules
class Window;
class Input;
class Render;
class Textures;
class Audio;

//Scenes
class SceneIntro;
class SceneMainMenu;
class Scene;
class SceneCombatLHHR;
class SceneBattle;
class SceneGrandma;
class EntityManager;
class Map;
class Fonts;
class FadeToBlack;
class SceneManager;
class BatlleManager;
//L07 DONE 2: Add Physics module
class Physics;
class PathFinding;
class GuiManager;
class UIModule;

class App
{
public:

	// Constructor
	App(int argc, char* args[]);

	// Destructor
	virtual ~App();

	// Called before render is available
	bool Awake();

	// Called before the first frame
	bool Start();

	// Called each loop iteration
	bool Update();

	// Called before quitting
	bool CleanUp();

	// Add a new module to handle
	void AddModule(Module* module);

	// Exposing some properties for reading
	int GetArgc() const;
	const char* GetArgv(int index) const;
	const char* GetTitle() const;
	const char* GetOrganization() const;

	// L03: DONE 1: Create methods to control that the real Load and Save happens at the end of the frame
	void LoadGameRequest();
	void SaveGameRequest() ;
	bool LoadFromFile();
	bool SaveToFile() ;

private:

	// Load config file
	bool LoadConfig();

	// Call modules before each loop iteration
	void PrepareUpdate();

	// Call modules before each loop iteration
	void FinishUpdate();

	// Call modules before each loop iteration
	bool PreUpdate();

	// Call modules on each loop iteration
	bool DoUpdate();

	// Call modules after each loop iteration
	bool PostUpdate();

public:

	// Modules
	Window* win;
	Input* input;
	Render* render;
	Textures* tex;
	Audio* audio;
	//Scenes
	Scene* scene;
	SceneIntro* sceneIntro;
	SceneMainMenu* sceneMainMenu;
	SceneCombatLHHR* sceneCombatLHHR;
	SceneBattle* sceneBattle;
	SceneGrandma* sceneGrandma;
	EntityManager* entityManager;
	Map* map;
	Fonts* fonts; 
	FadeToBlack* fadeToBlack;
	SceneManager* sceneManager; 
	BattleManager* battleManager;

	Physics* physics;
	PathFinding* pathfinding;
	GuiManager* guiManager;
	UIModule* uiModule;

	double prepareUpdate;
	double finishUpdate;
	double preUpdate;
	double doUpdate;
	double postUpdate;
	double update;
	PerformanceBar perfBar; 
	PerfTimer barTimer;

private:

	int argc;
	char** args;
	SString title;
	SString organization;

	List<Module*> modules;

	pugi::xml_document configFile;
	pugi::xml_node configNode;

	uint frames;
	float dt;

    bool saveGameRequested;
	bool loadGameRequested;
	
	Timer timer;
	PerfTimer ptimer;

	Timer startupTime;
	Timer frameTime;
	Timer lastSecFrameTime;

	uint64 frameCount = 0;
	uint32 framesPerSecond = 0;
	uint32 lastSecFrameCount = 0;

	float averageFps = 0.0f;
	float secondsSinceStartup = 0.0f;

	uint32 maxFrameDuration = 0;

};

extern App* app;

#endif	// __APP_H__