#include "GuiButton.h"
#include "Render.h"
#include "App.h"
#include "Audio.h"
#include "Log.h"
#include "GuiManager.h"
#include "Fonts.h"
#include "TeamManager.h"
#include "UIModule.h"

GuiButton::GuiButton(uint32 id, SDL_Rect bounds, SDL_Texture* tex, const char* text) : GuiControl(GuiControlType::BUTTON, id) {
	this->bounds = bounds;
	this->text = text;
	this->tex = tex;

	hoverOnce = false;
	pressedOnce = false;

	canClick = true;
	drawBasic = false;
}

GuiButton::~GuiButton()
{

}

bool GuiButton::Update(float dt)
{
	if (state != GuiControlState::DISABLED)
	{
		// L15: DONE 3: Update the state of the GUiButton according to the mouse position
		app->input->GetMousePosition(mouseX, mouseY);

		GuiControlState previousState = state;

		// I'm inside the limitis of the button
		if (mouseX >= bounds.x && mouseX <= bounds.x + bounds.w &&
			mouseY >= bounds.y && mouseY <= bounds.y + bounds.h) {

			state = GuiControlState::FOCUSED;
			if (previousState != state) {
				LOG("Change state from %d to %d", previousState, state);
			}

			if (app->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KeyState::KEY_REPEAT) {
				state = GuiControlState::PRESSED;

				// Please don't touch. Critical code
				if (app->teamManager->team.Count() > 0 && app->teamManager->team.Count() < 4)
				{
					app->audio->PlayFx(app->guiManager->pressedFxId);
				}
			}

			if (app->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KeyState::KEY_UP) {
				NotifyObserver();
			}
		}
		else {
			state = GuiControlState::NORMAL;
		}
	}

	return false;
}




bool GuiButton::Draw(Render* render)
{
	//L15: DONE 4: Draw the button according the GuiControl State

	rect.x = 0;
	rect.y = 0;
	rect.w = bounds.w;
	rect.h = bounds.h;

	//app->render->DrawTexture(app->uiModule->pauseBGTexture, offset + point * (-2500 + offset), -app->render->camera.y);



	switch (state)
	{
	case GuiControlState::DISABLED:
		//render->DrawRectangle(bounds, 128, 64, 0, 255, true, false);
		break;
	case GuiControlState::NORMAL:
		//render->DrawRectangle(bounds, 128, 64, 0, 255, true, false);
		render->DrawTexture(tex, bounds.x - app->render->camera.x, bounds.y - app->render->camera.y, &rect);
		hoverOnce = false;
		pressedOnce = false;
		//render->DrawTexture(app->guiManager->buttonNormalTexture, bounds.x - app->render->camera.x, bounds.y - app->render->camera.y, NULL);
		break;
	case GuiControlState::FOCUSED:
		//render->DrawRectangle(bounds, 244, 168, 92, 255, true, false);
		rect.y = bounds.h;
		render->DrawTexture(tex, bounds.x - app->render->camera.x, bounds.y - app->render->camera.y, &rect);
		if (!hoverOnce) {
			app->audio->PlayFx(app->guiManager->hoverFxId);
			hoverOnce = true;
		}
		//render->DrawTexture(app->guiManager->buttonHoverTexture, bounds.x - app->render->camera.x, bounds.y - app->render->camera.y, NULL);
		break;
	case GuiControlState::PRESSED:
		rect.y = bounds.h * 2;
		//render->DrawRectangle(bounds, 52, 26, 0, 255, true, false);
		render->DrawTexture(tex, -app->render->camera.x + bounds.x, -app->render->camera.y + bounds.y, &rect);
		if (!pressedOnce) {
			app->audio->PlayFx(app->guiManager->pressedFxId);
			pressedOnce = true;
		}
		//render->DrawTexture(app->guiManager->buttonPressedTexture, bounds.x - app->render->camera.x, bounds.y - app->render->camera.y, NULL);
		break;
	}

	app->fonts->DrawText(text.GetString(), bounds.x + bounds.w / 4 - 15, bounds.y, bounds.w, bounds.h, { 255,255,255 }, app->fonts->gameFont, true);

	return false;
}

int GuiButton::GetX()
{
	return bounds.x;
}

int GuiButton::GetY()
{
	return bounds.y;
}

void GuiButton::SetX(int x)
{
	this->bounds.x = x;
}

void GuiButton::SetY(int y)
{
	this->bounds.y = y;
}
