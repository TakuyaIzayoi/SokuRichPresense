//Literally other crap to initialize holy shit you'll need a header.


DiscordRichPresence discordPresence;
DiscordEventHandlers handlers;

std::map<char,std::string> Characters;

char p1char;
char p2char;
char* p1name;
char* p2name;
char* cstr = new char[128];
char* iconKey = new char[32];
int mutex = 0; 
void makeMap();
std::string obtainChar(char iconKey);

void SendDiscordNetplayRP();
void SendDiscordLocalRP();
void InitDiscord();
static void NewPresence();