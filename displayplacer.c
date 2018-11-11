//  displayplacer.c
//  Created by Jake Hilborn on 5/16/15.

#include <IOKit/graphics/IOGraphicsLib.h>
#include <ApplicationServices/ApplicationServices.h>
#include <unistd.h>
#include "header.h"

int main(int argc, char * argv[]) {
    if(argc == 1 || strcmp(argv[1], "--help") == 0) {
        printHelp();
        return 0;
    }

    if (strcmp(argv[1], "list") == 0) {
        listScreens();
        return 0;
    }

    ScreenConfig* screenConfigs = malloc((argc - 1) * sizeof(ScreenConfig));
    
    for (int i = 0; i < argc - 1; i++) {
        screenConfigs[i].modeNum = -1; //set modeNum -1 in case user wants to set and use mode 0

        char* propGroup = argv[i + 1];
        
        char* propSetSavePtr = NULL;
        char* propSetToken = strtok_r(propGroup, " \t", &propSetSavePtr);
        while (propSetToken) {
            char* propSavePtr = NULL;
            char* propToken = strtok_r(propSetToken, ":", &propSavePtr);
                
            switch (propToken[0]) {
                case 'i': //id
                    propToken = strtok_r(NULL, ":", &propSavePtr);

                    screenConfigs[i].id = atoi(propToken);
                    break;
                case 'r': //res
                    propToken = strtok_r(NULL, ":", &propSavePtr);
                    
                    char* resSavePtr = NULL;
                    char* resToken = strtok_r(propToken, "x", &resSavePtr);
                    screenConfigs[i].width = atoi(resToken);
                    resToken = strtok_r(NULL, "x", &resSavePtr);
                    screenConfigs[i].height = atoi(resToken);
                    resToken = strtok_r(NULL, "x", &resSavePtr);
                    if (resToken) {
                        screenConfigs[i].hz = atoi(resToken);
                    } else {
                        screenConfigs[i].hz = 0;
                    }
                    
                    break;
                case 's': //scaling
                    propToken = strtok_r(NULL, ":", &propSavePtr);

                    if (strcmp(propToken, "on") == 0) {
                        screenConfigs[i].scaled = true;
                    } else {
                        screenConfigs[i].scaled = false;
                    }
                
                    break;
                case 'o': //origin
                    propToken = strtok_r(NULL, ":", &propSavePtr);
                    
                    char* originSavePtr = NULL;
                    char* originToken = strtok_r(propToken, ",", &originSavePtr);
                    screenConfigs[i].x = atoi(originToken + 1); //skip the '(' character
                    originToken = strtok_r(NULL, ",", &originSavePtr);
                    screenConfigs[i].y = atoi(originToken);
                    
                    break;
                case 'm': //mode
                    propToken = strtok_r(NULL, ":", &propSavePtr);

                    screenConfigs[i].modeNum = atoi(propToken);
                    break;
                case 'd': //rotation degree
                    propToken = strtok_r(NULL, ":", &propSavePtr);

                    screenConfigs[i].degree = atoi(propToken);
                    break;
                default:
                    fprintf(stderr, "Argument parsing error\n");
                    exit(1);
            }
            
            propSetToken = strtok_r(NULL, " \t", &propSetSavePtr);
        }
    }

    bool isSuccess = true;
    CGGetOnlineDisplayList(INT_MAX, NULL, NULL); //must get list of online displays before being able to do operations with a screenId
    for (int i = 0; i < argc - 1; i++) {
        if (CGDisplayRotation(screenConfigs[i].id) != screenConfigs[i].degree) {
            isSuccess = rotateScreen(screenConfigs[i].id, screenConfigs[i].degree) && isSuccess;
        }
        isSuccess = setResolution(screenConfigs[i].id, screenConfigs[i].width, screenConfigs[i].height, screenConfigs[i].scaled, screenConfigs[i].hz, screenConfigs[i].modeNum) && isSuccess;
    }

    if (argc > 2) { //only set layout if more than one screen is being modified
        isSuccess = setLayout(screenConfigs, sizeof(ScreenConfig) * (argc -1)) && isSuccess;
    }

    free(screenConfigs);

    if (isSuccess) {
        return 0;
    } else {
        return 1;
    }
}

void printHelp() {
    printf(
            "Usage:\n"
            "    Show current screen info and possible resolutions: displayplacer list\n"
            "\n"
            "    Set layout: displayplacer 'id:<screenId> res:<width>x<height>x<hz> scaling:<on/off> origin:(<x>,<y>) degree:<0/90/180/270>'\n"
            "\n"
            "    Set layout using mode: displayplacer 'id:<screenId> mode:<modeNum> origin:(<x>,<y>) degree:<0/90/180/270>'\n"
            "\n"
            "    example: displayplacer 'id:1124216227 res:3840x2160x60 scaling:off origin:(0,0) degree:0' 'id:69731906 res:1440x900 scaling:on origin:(-1440,1260) degree:0' 'id:1007295109 res:1920x1200 scaling:off origin:(960,-2160) degree:0'\n"
            "\n"
            "Instructions:\n"
            "    1. Use displayplacer or the system settings to choose your screen resolutions and orientations.\n"
            "    2. Use System Preferences -> Displays -> Arrangement to arrange the screens to your liking.\n"
            "    3. Use 'displayplacer list' to get the info about your current layout so you can create profiles for scripting/hotkeys.\n"
            "    Note: The 'list' option shows resolutions for the screen's current orientation.\n"
            "          The screen set to origin (0,0) will be set as the primary screen.\n"
    );
}

void listScreens() {
    CGDisplayCount screenCount;
    CGGetActiveDisplayList(INT_MAX, NULL, &screenCount); //get number of active screens and store in screenCount

    CGDirectDisplayID screenList[screenCount];
    CGGetActiveDisplayList(INT_MAX, screenList, &screenCount);
    
    for (int i = 0; i < screenCount; i++) {
        UInt32 curScreen = screenList[i];
        
        printf("Screen ID: %i\n", curScreen);
        printf("Resolution: %lux%lu\n", CGDisplayPixelsWide(curScreen), CGDisplayPixelsHigh(curScreen));
        printf("Rotation: %i\n", (int) CGDisplayRotation(curScreen));
        printf("Origin: (%i,%i)\n", (int) CGDisplayBounds(curScreen).origin.x, (int) CGDisplayBounds(curScreen).origin.y);
        
        int modeCount;
        modes_D4* modes;
        CopyAllDisplayModes(curScreen, &modes, &modeCount);

        for(int i = 0; i < modeCount; i++) {
            modes_D4 mode = modes[i];
            
            if(mode.derived.density == 2.0) { //scaling on
                if(mode.derived.freq) { //if screen supports different framerates
                    printf("mode %i: Res=%dx%dx%i, scaled\n", i, mode.derived.width, mode.derived.height, mode.derived.freq);
                } else {
                    printf("mode %i: Res=%dx%d, scaled\n", i, mode.derived.width, mode.derived.height);
                }
            }
            else { //scaling off
                if(mode.derived.freq) { //if screen supports different framerates
                    printf("mode %i: Res=%dx%dx%i\n", i, mode.derived.width, mode.derived.height, mode.derived.freq);
                } else {
                    printf("mode %i: Res=%dx%d\n", i, mode.derived.width, mode.derived.height);
                }
            }
        }
        printf("\n");
    }
}

bool rotateScreen(CGDirectDisplayID screenId, int degree) {
    bool isSuccess = true;
    io_service_t service = CGDisplayIOServicePort(screenId);
    IOOptionBits options;
    
    switch(degree) {
        default:
            options = (0x00000400 | (kIOScaleRotate0)  << 16);
            break;
        case 90:
            options = (0x00000400 | (kIOScaleRotate90)  << 16);
            break;
        case 180:
            options = (0x00000400 | (kIOScaleRotate180)  << 16);
            break;
        case 270:
            options = (0x00000400 | (kIOScaleRotate270)  << 16);
            break;
    }

    int retVal = IOServiceRequestProbe(service, options);

    if (retVal != 0) {
        isSuccess = false;
        fprintf(stderr, "Error rotating screen ID %i\n", screenId);
    }
    
    return isSuccess;
}

bool setResolution(CGDirectDisplayID screenId, int width, int height, bool scaled, int hz, int modeNum) {
    int modeCount;
    modes_D4* modes;

    if (modeNum != -1) { //user specified modeNum instead of height/width/hz
        SetDisplayModeNum(screenId, modeNum);
        return true;
    }

    CopyAllDisplayModes(screenId, &modes, &modeCount);

    //loop through all modes looking for one that matches user input resolution
    for (int i = 0; i < modeCount; i++) {
        modes_D4 mode = modes[i];
        
        if (mode.derived.width != width) {
            continue;
        }
        if (mode.derived.height != height) {
            continue;
        }
        if (scaled && mode.derived.density != 2.0) {
            continue;
        }
        if (hz && mode.derived.freq != hz) {
            continue;
        }

        //matching resolution found
        SetDisplayModeNum(screenId, i);
        return true;
    }

    //no matching resolution found
    if(scaled) {
        if(hz) { //if screen supports different framerates
            fprintf(stderr, "Screen ID %i: could not find Res=%ix%ix%i, scaled\n", screenId, width, height, hz);
        } else {
            fprintf(stderr, "Screen ID %i: could not find Res=%ix%i, scaled\n", screenId, width, height);
        }
    }
    else { //scaling off
        if(hz) { //if screen supports different framerates
            fprintf(stderr, "Screen ID %i: could not find Res=%ix%ix%i\n", screenId, width, height, hz);
        } else {
            fprintf(stderr, "Screen ID %i: could not find Res=%ix%i\n", screenId, width, height);
        }
    }

    return false;
}

bool setLayout(ScreenConfig screenConfigs[], size_t screenConfigsSize) {
    bool isSuccess = true;
    CGDisplayConfigRef config;
    CGBeginDisplayConfiguration(&config);

    //Set the origin for each screen
    for (int i = 0; i < screenConfigsSize / sizeof(ScreenConfig); i++) {
        CGConfigureDisplayOrigin(config, screenConfigs[i].id, screenConfigs[i].x, screenConfigs[i].y);
    }

    int retVal = CGCompleteDisplayConfiguration(config, kCGConfigurePermanently);

    if (retVal != 0) {
        isSuccess = false;
        fprintf(stderr, "Error setting screen layout\n");
    }

    return isSuccess;
}
