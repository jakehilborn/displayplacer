//modes_D4 type created by Phoenix Dev for SwitchResX
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

typedef struct
{
    CGDirectDisplayID id; //screen identifier
    int width;            //pixels wide
    int height;           //pixels tall
    int hz;               //refresh rate
    bool scaled;          //scaling or downsampling
    int x;                //origin x position
    int y;                //origin y position
    int modeNum;             //display mode id
    int degree;           //rotation degree
} ScreenConfig;

void CGSGetNumberOfDisplayModes(CGDirectDisplayID display, int* nModes);
void CGSGetDisplayModeDescriptionOfLength(CGDirectDisplayID display, int idx, modes_D4* mode, int length);

//CopyAllDisplayModes is from Retina Display Manager (https://www.reddit.com/r/apple/comments/vi9yf/set_your_retina_macbook_pros_resolution_to)
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

void SetDisplayModeNum(CGDirectDisplayID display, int modeNum)
{
	CGDisplayConfigRef config;
	CGBeginDisplayConfiguration(&config);
	CGSConfigureDisplayMode(config, display, modeNum);
	CGCompleteDisplayConfiguration(config, kCGConfigurePermanently);
}

void printHelp();
void listScreens();
int rotateScreen(CGDirectDisplayID, int degree);
int setResolution(CGDirectDisplayID screenId, int width, int height, bool scaled, int hz, int modeNum);
int setLayout(ScreenConfig screenConfigs[], size_t screenConfigsSize);