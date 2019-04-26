#pragma once

enum SAVESTATE_MODE {
	NONE,
	CONSERVED,
	CONTINUED
};

typedef struct {
	UINT save_pos;
	UINT reset_pos;
	UINT display_states;
	UINT randomCH;
	UINT reset_skills;
} Keys;

typedef struct {
	bool save_pos;
	bool reset_pos;
	bool randomCH;
	bool reset_skills;
} Toggles;


typedef struct {
	float x;
	float y;
	float xspeed;
	float yspeed;
	float gravity;
} Position;

typedef struct {
	void *p;

	int x_pressed;//left is -, right is +
	int y_pressed;//down is -, up is +
	Position position;

	void* framedata;
	int frameflag;
	short current_sequence;
	short elapsed_in_subseq;

	short health;
	short spirit;

	short untech;
	char card;
} Player;


typedef struct {
	int A;
	int B;
	int C;
	int D;
	int Sw;
	int Sc;
	int Up;
	int Down;
	int Left;
	int Right;
} Commands;

extern Keys savestate_keys;
extern Toggles toggle_keys;
extern Commands commands;

/* UPDATE FUNCTIONS */
void update_position(Player *);
void update_playerinfo(Player *, int, int);

/* POSITION FUNCTIONS */
Position init_pos(float);
Position save_checkpoint(Player *);
void set_position(Player *, Position, int);
void position_management(Player *, Player *);

/* FRAMECOUNT FUNCTIONS */
void trademash_count(Player *);
void frameadvantage_count(Player *, Player *);
void hjcadvantage_count(Player *, Player *);
bool untight_check(Player *);
void is_tight(Player *);

/* MACROS */
void random_CH(Player *);
void fixed_tech(Player *);

/* MISCELLANEOUS */
void state_display(Player *);
void set_health(Player *, short);
void set_spirit(Player *, short, short);
void reset_skills(Player *);