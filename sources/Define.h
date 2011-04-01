#ifndef _DEFINE_H
#define	_DEFINE_H

#define CAPTION_TITLE "theBall ("__DATE__")"

// BACKGROUND ------------------------------------------------------------------
#define BACKGROUND_MAINMENU     "data/gfxart/background/main.png"
#define BACKGROUND_BULLETTIME   "data/gfxart/background/bullettime.png"
#define BACKGROUND_DAMMAGE      "data/gfxart/background/dammage.png"
#define BACKGROUND_HUD          "data/gfxart/background/hud.png"
#define BACKGROUND_PAUSE        "data/gfxart/background/pause.png"
#define BACKGROUND_LOAD         "data/gfxart/background/pause.png"

// GUI -------------------------------------------------------------------------
#define GUI_TEXTBOX_V   "data/gfxart/gui/textboxv.png"
#define GUI_SCORE       "data/gfxart/gui/score.png"
#define GUI_NOTIFY      "data/gfxart/gui/notify.png"
#define GUI_TEXTBOX_H   "data/gfxart/gui/textboxh.png"
#define GUI_LISTBOX_V   "data/gfxart/gui/listboxv.png"
#define GUI_LISTBOX_H   "data/gfxart/gui/listboxh.png"
#define GUI_PREVIEW     "data/gfxart/gui/previewframe.png"
#define GUI_BUTTON      "data/gfxart/gui/button.png"
#define GUI_GAUGE       "data/gfxart/gui/gauge.png"
#define GUI_EDIT        "data/gfxart/gui/edit.png"
#define GUI_SWITCH      "data/gfxart/gui/switch.png"
#define GUI_VECTOR      "data/gfxart/gui/vectorbox.png"
#define GUI_FONT        "data/gfxart/gui/comicbd.ttf"
#define GUI_FONTSIZE    10

// GUI HUD ---------------------------------------------------------------------
#define GUI_CROSHAIR    "data/gfxart/gui/croshair.png"
#define GUI_BOOST       "data/gfxart/gui/boost.png"
#define GUI_ITEM        "data/gfxart/gui/item.png"

// ITEM ------------------------------------------------------------------------
#define ITEM_MEDPACK    "data/models/items/medpack.obj"
#define ITEM_AMMOPACK   "data/models/items/ammopack.obj"
#define ITEM_FATALSHOT  "data/models/items/fatalshot.obj"
#define ITEM_SUPERLIFE  "data/models/items/superlife.obj"
#define ITEM_ADDFINDER  "data/models/items/addfinder.obj"
#define ITEM_ADDBOMB    "data/models/items/addbomb.obj"
#define ITEM_ADDSHOTGUN "data/models/items/addshotgun.obj"

// PARTICLES -------------------------------------------------------------------
#define PARTICLE_JUMPER         "data/gfxart/particles/jumper.bmp"
#define PARTICLE_TELEPORTER     "data/gfxart/particles/teleporter.bmp"
#define PARTICLE_EXPLODE        "data/gfxart/particles/fire.bmp"
#define PARTICLE_BLEUTEAM       "data/gfxart/particles/bleuteam.png"
#define PARTICLE_REDTEAM        "data/gfxart/particles/redteam.png"
#define PARTICLE_PLAYER         "data/gfxart/particles/player.png"

// Dynamics --------------------------------------------------------------------
#define OBJ_JUMPER      "data/models/dynamics/jumper.obj"
#define OBJ_TELEPORTER  "data/models/dynamics/teleporter.obj"
#define OBJ_SPAWN       "data/models/dynamics/spawn.obj"
#define OBJ_LOGO        "data/models/logo.obj"

// WEAPONS ---------------------------------------------------------------------
#define WEAPON_BLASTER  "data/weapons/blaster.bmp"
#define WEAPON_SHOTGUN  "data/weapons/shotgun.bmp"
#define WEAPON_BOMB     "data/weapons/bomb.bmp"
#define WEAPON_FINDER   "data/weapons/finder.bmp"

// SOUND & MUSIC ---------------------------------------------------------------
#define SOUND_MAINTHEME "data/music/research.ogg"
#define SOUND_GONG      "data/sfxart/gong.ogg"

#define SOUND_BLASTER   "data/sfxart/weapons/blaster.wav"
#define SOUND_SHOTGUN   "data/sfxart/weapons/shotgun.wav"
#define SOUND_BOMB      "data/sfxart/weapons/bomb.wav"
#define SOUND_FINDER    "data/sfxart/weapons/finder.wav"

// Paths -----------------------------------------------------------------------
#define MAPS_DIR            "data/maps/"
#define PLAYERS_DIR         "data/players/"
#define SKYBOX_DIR          "data/gfxart/skybox/"
#define MUSIC_DIR           "data/music/"
#define MODELS_MAPS_DIR     "data/models/maps/"
#define MODELS_PLAYERS_DIR  "data/models/players/"

// ENUM ------------------------------------------------------------------------

enum GuiSess
{
    MENU_MAIN,
    MENU_ABOUT,
    MENU_SETTING,
    MENU_SCORE,
    MENU_QUICKPLAY,
    MENU_CAMPAIGN,
    MENU_SETTING_KEYS,
    MENU_LOAD,
    MENU_EDIT,

    SCREEN_HUD,
    SCREEN_GAMEOVER,
    SCREEN_PAUSEMENU,
    SCREEN_PLAYERSLIST,

    EDITOR_EDIT_ENTITY,
    EDITOR_EDIT_LIGHT,
    EDITOR_EDIT_FOG,
    EDITOR_EDIT_SKYBOX,
    EDITOR_EDIT_MUSIC,
    EDITOR_VIEW,
    EDITOR_GUI,
};

#endif	/* _DEFINE_H */

