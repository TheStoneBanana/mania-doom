//
// Copyright(C) 2020 TheStoneBanana
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// DESCRIPTION:  Retro Engine interface stuff
//
#include "../pch.h"
#include <map>
#include <stdexcept>
#include <string>

extern "C" {
#include "d_event.h"
#include "deh_str.h"
#include "m_misc.h"

#include "sounds.h"
}

#include "re_main.h"

// ----------------------------------------------------------------------------
// Leaving the DOOM engine state and returning to Mania
// ----------------------------------------------------------------------------
void RE_Doom_Exit() {
	SonicMania::ChangeScene(SonicMania::Scene_MainMenu);
	SonicMania::Engine_ClearSoundFXData();
}

// ----------------------------------------------------------------------------
// Dumping out DOOM's framebuffer to Mania's framebuffer
// ----------------------------------------------------------------------------
void RE_Framebuffer_Dump(char* image, doomerpal* pal) {
	for (int y = 0; y < 240; y++) {
		for (int x = 0; x < 424; x++) {
			unsigned char index = image[(y * 424) + x];
			SonicMania::Framebuffer[(y * 432) + x] = SonicMania::ToRGB565(pal[index].r, pal[index].g, pal[index].b);
		}
	}
}

// ----------------------------------------------------------------------------
// Reading the action buttons
// ----------------------------------------------------------------------------
int RE_Control_ReadIn() {
	int res = 0;
	if ((SonicMania::PlayerControllers[0].Y.Down) && (SonicMania::PlayerControllers[0].B.Down)) {
		if (SonicMania::PlayerControllers[0].Up.Down || SonicMania::PlayerControllers[0].Up.Press) {
			res |= (1 << 6);		// swap weapon up
		}
		else if (SonicMania::PlayerControllers[0].Down.Down || SonicMania::PlayerControllers[0].Down.Press) {
			res |= (1 << 5);		// swap weapon down
		}
		return (res);
	}
	else {
		if (SonicMania::PlayerControllers[0].Y.Down || SonicMania::PlayerControllers[0].Y.Press) {
			res |= (1 << 3);		// use item
		}
		if (SonicMania::PlayerControllers[0].B.Down || SonicMania::PlayerControllers[0].B.Press) {
			res |= (1 << 1);		// strafe
		}
	}
	if (SonicMania::PlayerControllers[0].A.Down || SonicMania::PlayerControllers[0].A.Press) {
		res |= (1 << 0);		// fire gun
	}
	if (SonicMania::PlayerControllers[0].X.Down || SonicMania::PlayerControllers[0].X.Press) {
		res |= (1 << 2);		// run
	}

	if (SonicMania::PlayerControllers[0].Start.Press) {
		res |= (1 << 4);		// activate menu
	}

	return res;
}

// ----------------------------------------------------------------------------
// Reading the D-Pad
// ----------------------------------------------------------------------------
int RE_Joy_ReadIn(boolean axis) {
	int res = 0;
	// If we're checking the X-axis...
	if (axis == false) {
		if (SonicMania::PlayerControllers[0].Left.Down || SonicMania::PlayerControllers[0].Left.Press) {
			res -= 0x7FFF;
		}
		if (SonicMania::PlayerControllers[0].Right.Down || SonicMania::PlayerControllers[0].Right.Press) {
			res += 0x7FFF;
		}
	}
	// Otherwise, we're checking the Y-axis...
	else {
		if (SonicMania::PlayerControllers[0].Down.Down || SonicMania::PlayerControllers[0].Down.Press) {
			res += 0x7FFF;
		}
		if (SonicMania::PlayerControllers[0].Up.Down || SonicMania::PlayerControllers[0].Up.Press) {
			res -= 0x7FFF;
		}
	}
	return res;
}

// ----------------------------------------------------------------------------
// Constructing a joystick input event to post to DOOM
// ----------------------------------------------------------------------------
void RE_Control() {
	event_t ev;

	// read in input
	ev.type = ev_joystick;
	ev.data1 = RE_Control_ReadIn();
	// if the player is swapping weapons, don't listen to the input joysticks
	if (!(ev.data1 & (1 << 5)) && !(ev.data1 & (1 << 6))) {
		ev.data2 = RE_Joy_ReadIn(false);
		ev.data3 = RE_Joy_ReadIn(true);
	}
	else {
		ev.data2 = ev.data3 = 0;
	}
	ev.data4 = ev.data5 = 0;


	D_PostEvent(&ev);
}

typedef struct {
	const char* file;
	unsigned int loop;
} maniamusicinfo;

const std::map<std::string, maniamusicinfo> musicmap = {
	{ "introa", {"d_intro.ogg", 0} },
	// Episode 1
	{ "e1m1", {"d_e1m1.ogg", 94131} },
	{ "e1m2", {"d_e1m2.ogg", 142653} },
	{ "e1m3", {"d_e1m3.ogg", 216905} },
	{ "e1m4", {"d_e1m4.ogg", 101156} },
	{ "e1m5", {"d_e1m5.ogg", 347759} },
	{ "e1m6", {"d_e1m6.ogg", 129897} },
	{ "e1m7", {"d_e1m7.ogg", 297731} },
	{ "e1m8", {"d_e1m8.ogg", 346645} },
	{ "e1m9", {"d_e1m9.ogg", 135375} },
	// Episode 2
	{ "e2m1", {"d_e2m1.ogg", 122076} },
	{ "e2m2", {"d_e2m2.ogg", 86574} },
	{ "e2m3", {"d_e2m3.ogg", 150340} },
	{ "e2m4", {"d_e2m4.ogg", 176063} },
	{ "e2m5", {"d_e2m5.ogg", 297731} },
	{ "e2m6", {"d_e2m6.ogg", 316010} },
	{ "e2m7", {"d_e2m7.ogg", 408705} },
	{ "e2m8", {"d_e2m8.ogg", 350665} },
	{ "e2m9", {"d_e2m9.ogg", 224927} },
	// Episode 3
	{ "e3m1", {"d_e3m1.ogg", 224927} },
	{ "e3m2", {"d_e3m2.ogg", 114372} },
	{ "e3m3", {"d_e3m3.ogg", 186970} },
	{ "e3m4", {"d_e3m4.ogg", 346645} },
	{ "e3m5", {"d_e3m5.ogg", 297731} },
	{ "e3m6", {"d_e3m6.ogg", 129897} },
	{ "e3m7", {"d_e3m7.ogg", 408705} },
	{ "e3m8", {"d_e3m8.ogg", 343273} },
	{ "e3m9", {"d_e3m9.ogg", 135375} },
	// Misc.
	{ "inter", {"d_inter.ogg", 150340} },
	{ "bunny", {"d_bunny.ogg", 0} }
};

// Map music pack OGGs to internal music IDs and play them in the Retro Engine
void RE_Music_Play(musicinfo_t* song) {
	maniamusicinfo curSong;
	try {
		curSong = musicmap.at(song->name);
	}
	catch (const std::out_of_range& oor) {
		printf("RE_Music_Play: Match for %.6s not found\n", song->name);
		return;	// if the requested song doesn't have a matching music pack entry, ignore it
	}

	SonicMania::PlayMusic(curSong.file, 0, 0, curSong.loop, true);
}