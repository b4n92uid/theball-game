#ifndef _DEFINE_H
#define	_DEFINE_H

#define CAPTION_TITLE "theBall ("__DATE__")"

// BACKGROUND ------------------------------------------------------------------
#define BACKGROUND_MAINMENU     "data/gfxart/background/main.png"
#define BACKGROUND_BULLETTIME   "data/gfxart/background/bullettime.png"
#define BACKGROUND_DAMMAGE      "data/gfxart/background/dammage.png"
#define BACKGROUND_HUD          "data/gfxart/background/hud.png"
#define BACKGROUND_PAUSE        "data/gfxart/background/pause.png"

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
#define ITEM_MEDPACK    "data/items/medpack.obj"
#define ITEM_AMMOPACK   "data/items/ammopack.obj"
#define ITEM_FATALSHOT  "data/items/fatalshot.obj"
#define ITEM_SUPERLIFE  "data/items/superlife.obj"
#define ITEM_ADDFINDER  "data/items/addfinder.obj"
#define ITEM_ADDBOMB    "data/items/addbomb.obj"
#define ITEM_ADDSHOTGUN "data/items/addshotgun.obj"

// PARTICLES -------------------------------------------------------------------
#define PARTICLE_JUMPER         "data/gfxart/particles/jumper.bmp"
#define PARTICLE_TELEPORTER     "data/gfxart/particles/teleporter.bmp"
#define PARTICLE_EXPLODE        "data/gfxart/particles/fire.bmp"
#define PARTICLE_BLEUTEAM       "data/gfxart/particles/bleuteam.png"
#define PARTICLE_REDTEAM        "data/gfxart/particles/redteam.png"

// Dynamics --------------------------------------------------------------------
#define OBJ_JUMPER      "data/scene/jumper.obj"
#define OBJ_TELEPORTER  "data/scene/teleporter.obj"

// WEAPONS ---------------------------------------------------------------------
#define WEAPON_BLASTER  "data/weapons/blaster.bmp"
#define WEAPON_SHOTGUN  "data/weapons/shotgun.bmp"
#define WEAPON_BOMB     "data/weapons/bomb.bmp"
#define WEAPON_FINDER   "data/weapons/finder.bmp"

// SOUND & MUSIC ---------------------------------------------------------------
#define SOUND_MAINTHEME "data/music/research.ogg"

#define SOUND_BLASTER   "data/sfxart/weapons/blaster.wav"
#define SOUND_SHOTGUN   "data/sfxart/weapons/shotgun.wav"
#define SOUND_BOMB      "data/sfxart/weapons/bomb.wav"
#define SOUND_FINDER    "data/sfxart/weapons/finder.wav"

// AI --------------------------------------------------------------------------
#define AI_SWITCH_TARGET_TIME 10000
#define AI_CRITICAL_LIFE_VALUE 25
#define AI_CRITICAL_AMMO_VALUE 25
#define AI_DYNAMIC_INTERACTION 4.0f

// ENUM ------------------------------------------------------------------------

enum GuiSess
{
    MENU_MAIN,
    MENU_ABOUT,
    MENU_SETTING,
    MENU_SCORE,
    MENU_PLAY,
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

