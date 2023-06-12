#include "App.h"
#include "Input.h"
#include "Textures.h"
#include "Audio.h"
#include "Render.h"
#include "Window.h"
#include "W2_Scene_Maze.h"
#include "EntityManager.h"
#include "Map.h"
#include "PathFinding.h"
#include "GuiManager.h"
#include "Fonts.h"

#include "Defs.h"
#include "Log.h"

W2_Scene_Maze::W2_Scene_Maze(bool isActive) : Module(isActive)
{
	name.Create("w2_scene_maze");
}

// Destructor
W2_Scene_Maze::~W2_Scene_Maze()
{}

// Called before render is available
bool W2_Scene_Maze::Awake(pugi::xml_node& config)
{
	LOG("Loading W2_Scene");
	bool ret = true;

	mapName = config.attribute("name").as_string();
	mapFolder = config.attribute("path").as_string();
	
	app->physics->Enable();

	if (config.child("player")) {
		player = (Player*)app->entityManager->CreateEntity(EntityType::PLAYER);
		player->parameters = config.child("player");
		player->position.x = config.child("player").attribute("x").as_int();
		player->position.y = config.child("player").attribute("y").as_int();
	}

	portal = (Portal*)app->entityManager->CreateEntity(EntityType::PORTAL);
	portal->parameters = config.child("portal");

	app->entityManager->Awake(config);

	//uiSpriteTexture = app->tex->Load("Assets/UI/UI_SpriteSheet.png");

	fog = app->tex->Load("Assets/Maps/World_02/Assets/fog.png");

	pugi::xml_node chestNode3 = config.child("chest3");
	chest3 = (Item*)app->entityManager->CreateEntity(EntityType::ITEM);
	chest3->parameters = chestNode3;
	chest3->position.x = chestNode3.attribute("x").as_int();
	chest3->position.y = chestNode3.attribute("y").as_int();
	chest3->width = chestNode3.attribute("width").as_int();
	chest3->height = chestNode3.attribute("height").as_int();
	chest3->chestId = chestNode3.attribute("id").as_int();
	PhysBody* chest3PB = app->physics->CreateRectangleSensor(chest3->position.x + chest3->width / 2, chest3->position.y + chest3->height / 2, chest3->width, chest3->height, bodyType::STATIC);
	chest3PB->ctype = ColliderType::CHEST3;
	PhysBody* chest3Col = app->physics->CreateRectangle(chest3->position.x + chest3->width / 2, chest3->position.y + chest3->height / 2, chest3->width, chest3->height, bodyType::STATIC);
	chest3Col->ctype = ColliderType::UNKNOWN;

	starparticle_texture = app->tex->Load("Assets/UI/star_particle.png");

	inventoryItemsTexture = app->tex->Load("Assets/UI/itemImage_petita.png");

	chestTexture = app->tex->Load("Assets/Maps/World_01/highRes_Assets/hr_chest_spriteSheet.png");


	return ret;
}

// Called before the first frame
bool W2_Scene_Maze::Start()
{
	//player->walkFx = app->audio->LoadFx("Assets/Sounds/FX/fx_grass_walk.wav");
	app->entityManager->Start();
	

	// L03: DONE: Load map
	bool retLoad = app->map->Load(mapName, mapFolder);

	// L12 Create walkability map
	if (retLoad) {
		int w, h;
		uchar* data = NULL;

		//bool retWalkMap = app->map->CreateWalkabilityMap(w, h, &data);
		//if(retWalkMap) app->pathfinding->SetMap(w, h, data);

		RELEASE_ARRAY(data);

	}

	if (isNewGame)
	{
		player->ChangePosition(1648, 176);
		isNewGame = false;
	}
	else
	{
		app->LoadGameRequest();
	}

	pauseMenuActive = false;
	exitButtonBool = false;

	// Tell to UIModule which currentMenuType
	app->uiModule->currentMenuType = DISABLED;
	// Call this function only when buttons change
	app->uiModule->ChangeButtonState(app->uiModule->currentMenuType);

	godMode = false;

	app->audio->PlayMusic("Assets/Sounds/Music/music_maze.ogg", 0.1f);

	//Rect for chest Texture
	chestHRect = { 4, 36, 90, 77 };
	chestVRect = { 12, 135, 71, 101 };
	chestopenHRect = { 105, 3, 88, 108 };
	chestopenVRect = { 107, 134, 74, 100 };

	particle_chest3.x = 979;
	particle_chest3.y = 1352;
	particle_chest3.velocity_x = 0;
	particle_chest3.velocity_y = -70;
	particle_chest3.spreadfactor = 100;
	particle_chest3.lifetime = 1.2;
	particle_chest3.beginscale = 50;
	particle_chest3.endscale = 0;
	particle_chest3.r = 255;
	particle_chest3.g = 0;
	particle_chest3.b = 0;
	particle_chest3.r2 = 0;
	particle_chest3.g2 = 0;
	particle_chest3.b2 = 255;
	particle_chest3.scaleVariation = 1;
	particle_chest3.particlepersecond = 5;
	particle_chest3.particletexture = starparticle_texture;

	ParticleSystem* particlesystem_chest3 = new ParticleSystem(particle_chest3);
	app->moduleParticles->emiters.push_back(particlesystem_chest3);

	return true;
}

// Called each loop iteration
bool W2_Scene_Maze::PreUpdate()
{
	return true;
}



// Called each loop iteration
bool W2_Scene_Maze::Update(float dt)
{
	std::cout << "X: " << player->position.x << std::endl;
	std::cout << "Y: " << player->position.y << std::endl;

	Camera();

	// L03: DONE 3: Request App to Load / Save when pressing the keys F5 (save) / F6 (load)
	if (app->input->GetKey(SDL_SCANCODE_F5) == KEY_DOWN)
	{
		app->SaveGameRequest();
	}

	if (app->input->GetKey(SDL_SCANCODE_F6) == KEY_DOWN)
		app->LoadGameRequest();

	GodMode();

	// Menu appear
	if (app->input->GetKey(SDL_SCANCODE_P) == KEY_DOWN)
	{
		// If player is in pause, close it
		if (player->playerState == player->PlayerState::PAUSE)
		{
			player->playerState = player->playerPrevState;

			app->uiModule->currentMenuType = DISABLED;
			// Call this function only when scene is changed
			app->uiModule->ChangeButtonState(app->uiModule->currentMenuType);
		}
		// If player is NOT in pause, open it
		else
		{
			// Save previous state to go back
			player->playerPrevState = player->playerState;
			player->playerState = player->PlayerState::PAUSE;

			app->uiModule->currentMenuType = PAUSE;
			// Call this function only when scene is changed
			app->uiModule->ChangeButtonState(app->uiModule->currentMenuType);
		}
	}

	// Draw map
	app->map->Draw();

	if (chest3->isPicked) app->render->DrawTexture(app->w2_scene_maze->chestTexture, 979, 1352, &chestopenHRect);
	else app->render->DrawTexture(app->w2_scene_maze->chestTexture, 979, 1352, &chestHRect);

	return true;
}

// Called each loop iteration
bool W2_Scene_Maze::PostUpdate()
{
	bool ret = true;

	if(app->input->GetKey(SDL_SCANCODE_ESCAPE) == KEY_DOWN)
		ret = false;


	// When exit button click, close app
	if (exitButtonBool == true)
	{
		return false;
	}

	if (!godMode) {
		app->render->DrawTexture(fog, player->position.x - 1920, player->position.y - 1080);
	}

	return ret;
}

bool W2_Scene_Maze::OnGuiMouseClickEvent(GuiControl* control)
{
	LOG("Event by %d ", control->id);

	switch (control->id)
	{
		// Option 1
	case 12:
		app->scene->player->buttonOption1 = true;
		break;
		// Option 2
	case 13:
		app->scene->player->buttonOption2 = true;
		break;
		// Option 3
	case 14:
		app->scene->player->buttonOption3 = true;
		break;
		// Option 4
	case 15:
		app->scene->player->buttonOption4 = true;
	default:
		break;
	}
	return true;
}

// Called before quitting
bool W2_Scene_Maze::CleanUp()
{
	LOG("Freeing scene");
	//app->fonts->Unload(font);
	app->map->CleanUp(); 
	app->entityManager->CleanUp(); 
	app->physics->Disable();

	

	return true;
}

//Toggles god mode on and off when the F10 key is pressed. When god mode is on, the camera follows the player's position without any boundaries.
void W2_Scene_Maze::GodMode()
{
	if (app->input->GetKey(SDL_SCANCODE_F10) == KEY_DOWN)
	{
		if (godMode)
		{
			godMode = false;
		}
		else
		{
			godMode = true;
		}
	}
}

//Updates the camera position based on the player's position. If god mode is on, the camera follows the player's position without any boundaries. If god mode is off, the camera follows the player's position while respecting the game's boundaries.
void W2_Scene_Maze::Camera()
{
	if (godMode)
	{
		app->render->FollowObject(-(int)player->position.x, -(int)player->position.y - 35,
			app->render->camera.w / 2, app->render->camera.h / 2);
	}
	else
	{
		app->render->FollowObjectRespectBoundaries(-(int)player->position.x, -(int)player->position.y - 35,
		app->render->camera.w / 2, app->render->camera.h / 2, -4394, -93, -3674, -52);
	}

}

bool W2_Scene_Maze::LoadState(pugi::xml_node& data)
{
	loadPlayerPosX = data.child("player").attribute("x").as_int();
	loadPlayerPosY = data.child("player").attribute("y").as_int();

	if (active)
	{
		player->ChangePosition(loadPlayerPosX, loadPlayerPosY);
	}

	return true;
}

bool W2_Scene_Maze::SaveState(pugi::xml_node& data)
{
	pugi::xml_node playerNode = data.append_child("player");

	if (player != nullptr) {
		playerNode.append_attribute("x") = player->position.x;
		playerNode.append_attribute("y") = (player->position.y + 100);
	}

	if (active)
	{
		pugi::xml_node chestGameSave = data.append_child("chests");
		pugi::xml_node chestNodeSave3 = chestGameSave.append_child("chest3");
		chestNodeSave3.append_attribute("isPicked").set_value(chest3->isPicked);
	}

	return true;
}
