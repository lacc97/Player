/////////////////////////////////////////////////////////////////////////////
// This file is part of EasyRPG Player.
// 
// EasyRPG Player is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// EasyRPG Player is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with EasyRPG Player. If not, see <http://www.gnu.org/licenses/>.
/////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include <algorithm>
#include <vector>
#include "game_system.h"
#include "game_party.h"
#include "input.h"
#include "data.h"
#include "scene_save.h"
#include "window_savetitle.h"

////////////////////////////////////////////////////////////
Scene_Save::Scene_Save() :
	title_window(NULL) {
	Scene::type = Scene::Save;
	top_index = 0;
	index = 0;
}

////////////////////////////////////////////////////////////
void Scene_Save::Start() {
	// Create the windows
	title_window = new Window_SaveTitle(0, 0, 320, 32);
	title_window->Set(Data::terms.save_game_message);

	for (int i = 0; i < 15; i++) {
		Window_Save *w = new Window_Save(0, 40 + i * 64, 320, 64);
		w->SetIndex(i);
		// TODO: read party from save file
		std::vector<Game_Actor*> party = Game_Party::GetActors();
		party.push_back(party[0]);
		w->SetParty(party);
		w->Refresh();
		save_windows.push_back(w);
	}

	Refresh();
}

////////////////////////////////////////////////////////////
void Scene_Save::Terminate() {
	delete title_window;
	for (int i = 0; (size_t) i < save_windows.size(); i++)
		delete save_windows[i];
}

////////////////////////////////////////////////////////////
void Scene_Save::Refresh() {
	for (int i = 0; (size_t) i < save_windows.size(); i++) {
		Window_Save *w = save_windows[i];
		w->SetY(40 + (i - top_index) * 64);
		w->SetActive(i == index);
		w->SetVisible(i >= top_index && i < top_index + 3);
	}
}

////////////////////////////////////////////////////////////
void Scene_Save::Update() {
	for (int i = 0; (size_t) i < save_windows.size(); i++)
		save_windows[i]->Update();

	if (Input::IsTriggered(Input::CANCEL)) {
		Game_System::SePlay(Data::system.cancel_se);
		Scene::Pop();
	} else if (Input::IsTriggered(Input::DECISION)) {
		Game_System::SePlay(Data::system.decision_se);
		// TODO: save game
		Scene::Pop();
	}

	int old_top_index = top_index;
	int old_index = index;

	if (Input::IsRepeated(Input::DOWN)) {
		Game_System::SePlay(Data::system.cursor_se);
		index++;
		if ((size_t) index >= save_windows.size())
			index--;
		top_index = std::max(top_index, index - 3 + 1);
	}
	if (Input::IsRepeated(Input::UP)) {
		Game_System::SePlay(Data::system.cursor_se);
		index--;
		if (index < 0)
			index++;
		top_index = std::min(top_index, index);
	}

	if (top_index != old_top_index || index != old_index)
		Refresh();
}

