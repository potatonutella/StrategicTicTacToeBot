#pragma once

// show move eval
static bool MOVE_EVAL_SHOWN = false;


// player vs player
static int GAMEMODE = 0;

// player vs bot
// static int GAMEMODE = 1;

// bot vs bot
// static int GAMEMODE = 2;


// hide console
#if false
#define DISABLE_CONSOLE
#endif

// depth
static int DEPTH = 8;

// alpha-beta
#if true 
#define ALPHA_BETA_ONLY(...) __VA_ARGS__
#else
#define ALPHA_BETA_ONLY(...)
#endif

// bullying
#if false
#define BULLYING
#endif
