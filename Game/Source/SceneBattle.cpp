#include "App.h"
#include "Input.h"
#include "Textures.h"
#include "Audio.h"
#include "Render.h"
#include "Window.h"
#include "SceneBattle.h"
#include "EntityManager.h"
#include "Entity.h"
#include "Map.h"
#include "GuiManager.h"
#include "Fonts.h"
#include "Physics.h"
#include "Timmy.h"
#include "TeamManager.h"
#include "BattleManager.h"

#include "Defs.h"
#include "Log.h"

SceneBattle::SceneBattle(bool isActive) : Module(isActive) {

	name.Create("sceneBattle");
}

// Destructor
SceneBattle::~SceneBattle()
{}

// Called before render is available
bool SceneBattle::Awake(pugi::xml_node& config) {

	LOG("Loading Scene");
	bool ret = true;

	app->render->camera.x = 0;
	app->render->camera.y = 0;

	mapName = config.attribute("name").as_string();
	mapFolder = config.attribute("path").as_string();
	
	//Add allies from teamManager
	int i = 0;
	for (ListItem<Entity*>* teamItem = app->teamManager->team.start; teamItem != NULL; teamItem = teamItem->next) {
		app->battleManager->AddCharacter(teamItem->data, 7+i, 6, false);
		app->entityManager->AddEntity(teamItem->data);
		i++;
	}
	
	//if (config.child("timmy")) {
	//	timmy = (Timmy*)app->entityManager->CreateEntity(EntityType::TIMMY);
	//	timmy->parameters = config.child("timmy");
	//	timmy->stats = config.parent().child("timmy");
	//	app->battleManager->AddCharacter(timmy, timmy->parameters.attribute("x").as_int() / 120, timmy->parameters.attribute("y").as_int() / 120, false);
	//}

	//Add enemies from config
	if (config.child("enemy_angryVillager")) {
		villager = (Enemy_AngryVillager*)app->entityManager->CreateEntity(EntityType::ANGRYVILLAGER);
		villager->parameters = config.child("enemy_angryVillager");
		villager->stats = config.parent().child("enemy_angryVillager");
		app->battleManager->AddCharacter(villager, villager->parameters.attribute("x").as_int()/120, villager->parameters.attribute("y").as_int() / 120, true);
	}

	app->entityManager->Awake(config);
	app->battleManager->Enable();
	
	return ret;
}


// Called before the first frame
bool SceneBattle::Start() {

	bool retLoad = app->map->Load(mapName, mapFolder);
	timmyPrevPosBool = false;
	bunnyPrevPosBool = false;
	villagerPrevPosBool = false;

	if (retLoad) {
		int w, h;
		uchar* data = NULL;

		bool retWalkMap = app->map->CreateWalkabilityMap(w, h, &data);
		if (retWalkMap) app->pathfinding->SetMap(w, h, data);

		RELEASE_ARRAY(data);

	}
	app->audio->PlayMusic("Assets/Sounds/Music/music_battle_2.wav", 0.2f);
	app->battleManager->MakeCombatMap();
	
	//bunnyPrevPos = bunny->position;
	villagerPrevPos = villager->position;

	return true;
}


// Called each loop iteration
bool SceneBattle::PreUpdate() {

	//Buttons :)

	

	bool ret = true;
	return true;
}


// Called each loop iteration
bool SceneBattle::Update(float dt) {

	

	app->map->Draw();
	return true;
}

void SceneBattle::RunTree()
{
	//Esto tiene que ser false por defecto para que se ejecute solo la parte del arbol que es la que
	//utilizaremos debido a que al final la IA del EnemyVillager es diferente
	villager->isEnemyTooClose->SetCondition(false);

	//Miramos si tiene que atacar o tiene que moverse
	villager->inRangeChecker->SetCondition(conditionToRangeChecker); //true=atacar, false=mover

	//Si se tiene que mover:
	if (!conditionToRangeChecker)
	{
		//Chequeamos que tiene suficente stamina
		if (!noStaminaToMove)
		{
			//si no tiene no se mueve
			return;
		}
		{
			//si tiene se mueve
			app->battleManager->battleState = BattleState::INACTION;
		}
	}
	
	//ejecutamos el arbol
	villager->behaviorTree->Run();
}

// Called each loop iteration
bool SceneBattle::PostUpdate() {

	bool ret = true;

	
	return ret;
}


void SceneBattle::UpdateAnimation(const char* name)
{
	//Movimiento y Idle
	MoveAnimation(name);

}

bool SceneBattle::CleanUp(){

	LOG("Freeing sceneBattle");

	app->map->CleanUp();
	app->entityManager->CleanUp(); 
	app->battleManager->Disable(); 

	return true;
}

void SceneBattle::SaveResult()
{

	if (app->battleManager->win)
	{
		app->UpdateXMLAttributeFromNode("save_game.xml", "BattleInfo", "isAngryVillagerDefeated", "true");
	}
	else if (app->battleManager->lose)
	{
		app->UpdateXMLAttributeFromNode("save_game.xml", "BattleInfo", "isAngryVillagerDefeated", "false");
	}

}

void SceneBattle::MoveAnimation(const char* name)
{
	if (strcmp(name,"enemy_angryVillager") == 0)
	{
		//Moverse a la derecha
		if (villager->position.x > villagerPrevPos.x)
		{
			villager->currentAnimation = &villager->walkRightAnim;
		}
		//Moverse a la izquierda
		else if (villager->position.x < villagerPrevPos.x)
		{
			villager->currentAnimation = &villager->walkLeftAnim;
		}
		//Moverse a abajo
		else if (villager->position.y > villagerPrevPos.y)
		{
			villager->currentAnimation = &villager->walkDownAnim;
		}
		//Moverse a arriba
		else if (villager->position.y < villagerPrevPos.y)
		{
			villager->currentAnimation = &villager->walkUpAnim;
		}
		else if (villager->position.y == villagerPrevPos.y && villager->position.x == villagerPrevPos.x)
		{

			if (frames > 60)
			{
				villager->currentAnimation = &villager->idleAnim;
				frames = 0;
			}
			else
			{
				frames++;
			}
		}

		villagerPrevPos.x = villager->position.x;
		villagerPrevPos.y = villager->position.y;
	}

}

void SceneBattle::TakeDamageAnimation(const char* name)
{
	//Animaci�n de ser da�ado
	if (strcmp(name, "enemy_angryVillager") == 0)
	{
		villager->currentAnimation = &villager->takedmgAnim;
	}

}
