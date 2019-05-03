//Literally other crap to initialize holy shit you'll need a header.

#include <map>
#include <string>

DiscordRichPresence discordPresence;
DiscordEventHandlers handlers;

char p1char;
char p2char;
char* p1name;
char* p2name;
char* cstr = new char[128];
char* iconKey = new char[32];
int mutex = 0; 
std::string obtainChar(char iconKey) const;

void SendDiscordNetplayRP();
void SendDiscordLocalRP();
void InitDiscord();
static void NewPresence();

const std::map<char,std::string> Characters = {
	{'A',"Reimu"},
	{'B',"Marisa"},
	{'C',"Sakuya"},
	{'D',"Alice"},
	{'E',"Patchouli"},
	{'F',"Youmu"},
	{'G',"Remilia"},
	{'H',"Yuyuko"},
	{'I',"Yukari"},
	{'J',"Suika"},
	{'K',"Aya"},
	{'L',"Reisen"},
	{'M',"Komachi"},
	{'N',"Iku"},
	{'O',"Tenshi"},
	{'P',"Sanae"},
	{'Q',"Cirno"},
	{'R',"Meiling"},
	{'S',"Utsuho"},
	{'T',"Suwako"},
	{'U', "Namazu"}
}
