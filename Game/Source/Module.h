#ifndef __MODULE_H__
#define __MODULE_H__

#include "SString.h"

#include "PugiXml/src/pugixml.hpp"

class App;
class PhysBody;
class GuiControl;

class Module
{
public:

	Module(bool isActive) : active(isActive)
	{}

	void Init()
	{
	}

	// Called before render is available
	virtual bool Awake(pugi::xml_node&)
	{
		return true;
	}

	// Called before the first frame
	virtual bool Start()
	{
		return true;
	}

	// Called each loop iteration
	virtual bool PreUpdate()
	{
		return true;
	}

	// Called each loop iteration
	virtual bool Update(float dt)
	{
		return true;
	}

	// Called each loop iteration
	virtual bool PostUpdate()
	{
		return true;
	}

	// Called before quitting
	virtual bool CleanUp()
	{
		return true;
	}

	// L03: DONE 2: Create new virtual methods to LoadState / SaveState
	virtual bool LoadState(pugi::xml_node&)
	{
		return true;
	}

	virtual bool SaveState(pugi::xml_node&)
	{
		return true;
	}

	virtual void OnCollision(PhysBody* bodyA, PhysBody* bodyB)
	{

	}

	virtual bool OnGuiMouseClickEvent(GuiControl* control)
	{
		return true;
	}

	void Enable()
	{
		if (!active)
		{
			active = true;
			pugi::xml_document configFile;
			pugi::xml_node configNode;

			pugi::xml_parse_result parseResult = configFile.load_file("config.xml");

			// L01: DONE 3: Check result for loading errors
			if (parseResult) {
				configNode = configFile.child("config");
			}

			pugi::xml_node node = configNode.child(name.GetString());
			Awake(node);
			Start();
		}
	}

	void Disable()
	{
		if (active)
		{
			active = false;
			CleanUp();
		}
	}

public:

	SString name;
	bool active;

};

#endif // __MODULE_H__