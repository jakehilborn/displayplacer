//Thanks to Phoenix Dev for modes_D4 type for use in Apple's private core graphics APIs
typedef union
{
    uint8_t rawData[0xDC];
    struct
    {
        uint32_t mode;
        uint32_t flags;		// 0x4
        uint32_t width;		// 0x8
        uint32_t height;	// 0xC
        uint32_t depth;		// 0x10
        uint32_t dc2[42];
        uint16_t dc3;
        uint16_t freq;		// 0xBC
        uint32_t dc4[4];
        float density;		// 0xD0
    } derived;
} modes_D4;

//Apple's private core graphics APIs https://github.com/NUIKit/CGSInternal
void CGSGetCurrentDisplayMode(CGDirectDisplayID display, int* modeNum);
void CGSGetNumberOfDisplayModes(CGDirectDisplayID display, int* nModes);
void CGSGetDisplayModeDescriptionOfLength(CGDirectDisplayID display, int idx, modes_D4* mode, int length);
void CGSConfigureDisplayMode(CGDisplayConfigRef config, CGDirectDisplayID display, int modeNum);
CGError CGSConfigureDisplayEnabled(CGDisplayConfigRef config, CGDirectDisplayID display, bool enabled);

//Thanks to Phoenix Dev for CopyAllDisplayModes to fill the modes_D4 structures
void CopyAllDisplayModes(CGDirectDisplayID display, modes_D4** modes, int* cnt)
{
    int nModes;
    CGSGetNumberOfDisplayModes(display, &nModes);

    if(nModes)
        *cnt = nModes;

    if(!modes)
        return;

    *modes = (modes_D4*) malloc(sizeof(modes_D4)* nModes);
    for(int i=0; i<nModes; i++)
    {
        CGSGetDisplayModeDescriptionOfLength(display, i, &(*modes)[i], 0xD4);
    }
}

const int UUID_SIZE = 37;
const int MIRROR_MAX = 127;

typedef struct
{
    char uuid[UUID_SIZE];                    //user input display identifier that stays consistent despite GPU or port changes (persistent screen id)
    char mirrorUUIDs[MIRROR_MAX][UUID_SIZE]; //user input display UUIDs that mirror this display
    CGDirectDisplayID id;                    //display identifier used for Quartz Display Services (contextual screen id)
    CGDirectDisplayID mirrors[MIRROR_MAX];   //display IDs that mirror this display used for Quartz Display Services
    int mirrorCount;                         //number of displays that mirror this display
    int width;                               //pixels wide
    int height;                              //pixels tall
    int hz;                                  //refresh rate
    int depth;                               //color depth
    bool enabled;                            //disables screen from macOS - does not turn screen off, just turns it black
    bool scaled;                             //scaling
    int x;                                   //origin x position
    int y;                                   //origin y position
    int modeNum;                             //display mode id
    int degree;                              //rotation degree
    bool quietMissingScreen;                 //prevent printing error logs and exiting non-zero when this screen cannot be found
} ScreenConfig;

//DisplayPlacer.c
void printHelp();
void printVersion();
void listScreens();
void printCurrentProfile();
CGDirectDisplayID convertUUIDtoID(char* uuid);
bool validateScreenOnline(CGDirectDisplayID onlineDisplayList[], int screenCount, CGDirectDisplayID screenId, char* screenUUID, bool quietMissingScreen);
bool isScreenEnabled(CGDirectDisplayID screenId);
bool configureMirror(CGDisplayConfigRef configRef, CGDirectDisplayID primaryScreenId, char* primaryScreenUUID, CGDirectDisplayID mirrorScreenId, char* mirrorScreenUUID);
bool disableAllMirrors(ScreenConfig* screenConfigs, int argc, CGDisplayConfigRef configRef, CGDirectDisplayID screenList[], int screenCount);
bool disableMirror(CGDisplayConfigRef configRef, CGDirectDisplayID mirrorScreenId, char* mirrorScreenUUID);
bool configureResolution(CGDisplayConfigRef configRef, CGDirectDisplayID screenId, char* screenUUID, int width, int height, int hz, int depth, bool scaled, int modeNum);
bool configureOrigin(CGDisplayConfigRef configRef, CGDirectDisplayID screenId, char* screenUUID, int x, int y);
bool rotateAllScreens(ScreenConfig* screenConfigs, int argc, CGDisplayConfigRef configRef, CGDirectDisplayID screenList[], int screenCount);
bool mirrorScreens(ScreenConfig* screenConfigs, int argc, CGDisplayConfigRef configRef, CGDirectDisplayID screenList[], int screenCount);
bool setResolutions(ScreenConfig* screenConfigs, int argc, CGDisplayConfigRef configRef, CGDirectDisplayID screenList[], int screenCount);
bool setPositions(ScreenConfig* screenConfigs, int argc, CGDisplayConfigRef configRef, CGDirectDisplayID screenList[], int screenCount);

//MonitorPanel.m
bool rotateScreen(CGDirectDisplayID screenId, char* screenUUID, int degree);
