//  displayplacer.c
//  Created by Jake Hilborn on 5/16/15.

#include <IOKit/graphics/IOGraphicsLib.h>
#include <ApplicationServices/ApplicationServices.h>
#include <unistd.h>
#include <math.h>
#include <stdio.h>
#include "header.h"

int main(int argc, char* argv[]) {
    if (argc == 1 || strcmp(argv[1], "--help") == 0) {
        printHelp();
        return 0;
    }

    if (strcmp(argv[1], "--version") == 0) {
        printVersion();
        return 0;
    }

    if (strcmp(argv[1], "list") == 0) {
        listScreens();
        printCurrentProfile();
        return 0;
    }

    ScreenConfig* screenConfigs = malloc((argc - 1) * sizeof(ScreenConfig));
    
    for (int i = 0; i < argc - 1; i++) {
        screenConfigs[i].depth = 0; //overwrite garbage in memory for optional params
        screenConfigs[i].hz = 0;
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

                    char* idToken = strtok_r(propToken, "+", &propToken);
                    strlcpy(screenConfigs[i].uuid, idToken, sizeof(screenConfigs[i].uuid));

                    int j = 0;
                    while ((idToken = strtok_r(propToken, "+", &propToken))) {
                        strlcpy(screenConfigs[i].mirrorUUIDs[j], idToken, sizeof(screenConfigs[i].mirrorUUIDs[j]));
                        j++;

                        if (j > 127) {
                            fprintf(stderr, "Current code only supports 128 screens mirroring. Please execute `displayplacer --version` for info on contacting the developer to change this.\n");
                        }
                    }
                    screenConfigs[i].mirrorCount = j;

                    break;
                case 'r': //res
                    propToken = strtok_r(NULL, ":", &propSavePtr);
                    
                    char* resSavePtr = NULL;
                    char* resToken = strtok_r(propToken, "x", &resSavePtr);
                    screenConfigs[i].width = atoi(resToken);
                    resToken = strtok_r(NULL, "x", &resSavePtr);
                    screenConfigs[i].height = atoi(resToken);

                    //backward compatability with legacy hz format "res:3840x2160x60"
                    resToken = strtok_r(NULL, "x", &resSavePtr);
                    if (resToken) {
                        screenConfigs[i].hz = atoi(resToken);
                    }

                    break;
                case 'h': //hertz
                    propToken = strtok_r(NULL, ":", &propSavePtr);

                    screenConfigs[i].hz = atoi(propToken);
                    break;
                case 'c': //color_depth
                    propToken = strtok_r(NULL, ":", &propSavePtr);

                    screenConfigs[i].depth = atoi(propToken);
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

    CGDisplayCount screenCount;
    CGGetOnlineDisplayList(INT_MAX, NULL, &screenCount); //get number of online screens and store in screenCount
    CGDirectDisplayID screenList[screenCount];
    CGGetOnlineDisplayList(INT_MAX, screenList, &screenCount); //store display list in array of size screenCount

    CGDisplayConfigRef configRef;
    CGBeginDisplayConfiguration(&configRef);
    bool isSuccess = true; //returns non-zero exit code on any errors but allows for completing remaining program execution
    for (int i = 0; i < argc - 1; i++) {
        screenConfigs[i].id = convertUUIDtoID(screenConfigs[i].uuid);
        if (!validateScreenOnline(screenList, screenCount, screenConfigs[i].id, screenConfigs[i].uuid)) {
            isSuccess = false;
            continue;
        }

        if (CGDisplayRotation(screenConfigs[i].id) != screenConfigs[i].degree) {
            isSuccess = rotateScreen(screenConfigs[i].id, screenConfigs[i].uuid, screenConfigs[i].degree) && isSuccess;
        }

        for (int j = 0; j < screenConfigs[i].mirrorCount; j++) {
            screenConfigs[i].mirrors[j] = convertUUIDtoID(screenConfigs[i].mirrorUUIDs[j]);
            if (!validateScreenOnline(screenList, screenCount, screenConfigs[i].mirrors[j], screenConfigs[i].mirrorUUIDs[j])) {
                isSuccess = false;
                continue;
            }

            if (CGDisplayRotation(screenConfigs[i].mirrors[j]) != screenConfigs[i].degree) {
                isSuccess = rotateScreen(screenConfigs[i].mirrors[j], screenConfigs[i].mirrorUUIDs[j], screenConfigs[i].degree) && isSuccess;
            }

            isSuccess = configureMirror(configRef, screenConfigs[i].id, screenConfigs[i].uuid, screenConfigs[i].mirrors[j], screenConfigs[i].mirrorUUIDs[j]) && isSuccess;
        }

        isSuccess = configureResolution(configRef, screenConfigs[i].id, screenConfigs[i].uuid, screenConfigs[i].width, screenConfigs[i].height, screenConfigs[i].hz, screenConfigs[i].depth, screenConfigs[i].scaled, screenConfigs[i].modeNum) && isSuccess;
        isSuccess = configureOrigin(configRef, screenConfigs[i].id, screenConfigs[i].uuid, screenConfigs[i].x, screenConfigs[i].y) && isSuccess;
    }

    int retVal = CGCompleteDisplayConfiguration(configRef, kCGConfigurePermanently);

    if (retVal != 0) {
        fprintf(stderr, "Error finalizing display configurations\n");
        isSuccess = false;
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
            "    Screen config: displayplacer \"id:<screenId> res:<width>x<height> hz:<hz> scaling:<on/off> origin:(<x>,<y>) degree:<0/90/180/270>\"\n"
            "\n"
            "    Screen config using mode: displayplacer \"id:<screenId> mode:<modeNum> origin:(<x>,<y>) degree:<0/90/180/270>\"\n"
            "\n"
            "    Set layout with a mirrored screen: displayplacer \"id:<mainScreenId>+<mirrorScreenId>+<mirrorScreenId> res:<width>x<height> hz:<hz> scaling:<on/off> origin:(<x>,<y>) degree:<0/90/180/270>\"\n"
            "\n"
            "    Example w/ all features: displayplacer \"id:18173D22-3EC6-E735-EEB4-B003BF681F30+F466F621-B5FA-04A0-0800-CFA6C258DECD res:1440x900 scaling:on origin:(0,0) degree:0\" \"id:4C405A05-8798-553B-3550-F93E7A7722BB res:768x1360 hz:60 scaling:off origin:(1440,0) degree:90\" \"id:A46D2F5E-487B-CC69-C588-ECFD519016E5 mode:3 origin:(-1440,0) degree:270\"\n"
            "\n"
            "Instructions:\n"
            "    1. Manually set rotations 1st*, resolutions 2nd, and arrangement 3rd. For extra resolutions and rotations read 'Notes' below.\n"
            "        - Open System Preferences -> Displays\n"
            "        - Choose desired screen rotations (use displayplacer for rotating internal MacBook screen).\n"
            "        - Choose desired resolutions (use displayplacer for extra resolutions).\n"
            "        - Drag the white bar to your desired primary screen.\n"
            "        - Arrange screens as desired and/or enable mirroring.\n"
            "        - To enable partial mirroring hold the alt/option key and drag a display on top of another.\n"
            "    2. Use `displayplacer list` to print your current layout's args so you can create profiles for scripting/hotkeys with Automator, BetterTouchTool, etc.\n"
            "\n"
            "Notes:\n"
            "    - *`displayplacer list` and system prefs only show resolutions for the screen's current rotation.\n"
            "    - Use an extra resolution shown in `displayplacer list` by executing `displayplacer \"id:<screenId> mode:<modeNum>\"`\n"
            "    - Rotate your internal MacBook screen by executing `displayplacer \"id:<screenId> degree:<0/90/180/270>\"`\n"
            "    - The screen set to origin (0,0) will be set as the primary screen (white bar in system prefs).\n"
            "    - The first screenId in a mirroring set will be the 'Optimize for' screen in the system prefs. You can only choose resolutions for the 'Optimize for' screen. If there is a mirroring resolution you need but cannot find, try making a different screenId the first of the set.\n"
            "\n"
            "Feedback:\n"
            "    Please create a GitHub Issue for any feedback, feature requests, bugs, Homebrew issues, etc. Happy to accept pull requests too! https://github.com/jakehilborn/displayplacer\n"
    );
}

void printVersion() {
    printf(
        "displayplacer v1.2.0-dev\n"
        "\n"
        "Developer: Jake Hilborn\n"
        "GitHub: https://github.com/jakehilborn/displayplacer\n"
        "LinkedIn: https://www.linkedin.com/in/jakehilborn\n"
        "Email: jakehilborn@gmail\n"
    );
}

void listScreens() {
    CGDisplayCount screenCount;
    CGGetOnlineDisplayList(INT_MAX, NULL, &screenCount); //get number of online screens and store in screenCount

    CGDirectDisplayID screenList[screenCount];
    CGGetOnlineDisplayList(INT_MAX, screenList, &screenCount);

    for (int i = 0; i < screenCount; i++) {
        CGDirectDisplayID curScreen = screenList[i];

        int curModeId;
        CGSGetCurrentDisplayMode(curScreen, &curModeId);
        modes_D4 curMode;
        CGSGetDisplayModeDescriptionOfLength(curScreen, curModeId, &curMode, 0xD4);

        char curScreenUUID[UUID_SIZE];
        CFStringGetCString(CFUUIDCreateString(kCFAllocatorDefault, CGDisplayCreateUUIDFromDisplayID(curScreen)), curScreenUUID, sizeof(curScreenUUID), kCFStringEncodingUTF8);
        printf("Screen ID: %s\n", curScreenUUID);

        if (CGDisplayIsBuiltin(curScreen)) {
            printf("Type: MacBook built in screen\n");
        } else {
            CGSize size = CGDisplayScreenSize(curScreen);
            int diagonal = round(sqrt((size.width * size.width) + (size.height * size.height)) / 25.4); //25.4mm in an inch
            printf("Type: %i inch external screen\n", diagonal);
        }

        printf("Resolution: %ix%i\n", (int) CGDisplayPixelsWide(curScreen), (int) CGDisplayPixelsHigh(curScreen));

        if (curMode.derived.freq) {
            printf("Hertz: %i\n", curMode.derived.freq);
        } else {
            printf("Hertz: N/A\n");
        }

        printf("Color Depth: %i\n", curMode.derived.depth);

        char* scaling = (curMode.derived.density == 2.0) ? "on" : "off";
        printf("Scaling:%s\n", scaling);

        printf("Origin: (%i,%i)", (int) CGDisplayBounds(curScreen).origin.x, (int) CGDisplayBounds(curScreen).origin.y);
        if (CGDisplayIsMain(curScreen)) {
            printf(" - main display");
        }
        printf("\n");

        printf("Rotation: %i", (int) CGDisplayRotation(curScreen));
        if (CGDisplayIsBuiltin(curScreen)) {
            printf(" - rotate internal screen example (may crash computer, but will be rotated after rebooting): `displayplacer \"id:%s degree:90\"`", curScreenUUID);
        }
        printf("\n");
        
        int modeCount;
        modes_D4* modes;
        CopyAllDisplayModes(curScreen, &modes, &modeCount);

        printf("Resolutions for rotation %i:\n", (int) CGDisplayRotation(curScreen));
        for (int i = 0; i < modeCount; i++) {
            modes_D4 mode = modes[i];

            printf("  mode %i: res:%dx%d", i, mode.derived.width, mode.derived.height);

            if (mode.derived.freq) {
                printf(" hz:%i", mode.derived.freq);
            }

            printf(" color_depth:%i", mode.derived.depth);

            if (mode.derived.density == 2.0) {
                printf(" scaling:on");
            }

            if (i == curModeId) {
                printf(" <-- current mode");
            }

            printf("\n");
        }
        printf("\n");
    }
}

void printCurrentProfile() {
    CGDisplayCount screenCount;
    CGGetOnlineDisplayList(INT_MAX, NULL, &screenCount); //get number of online screens and store in screenCount

    CGDirectDisplayID screenList[screenCount];
    CGGetOnlineDisplayList(INT_MAX, screenList, &screenCount);

    ScreenConfig screenConfigs[screenCount];
    for (int i = 0; i < screenCount; i++) {
        screenConfigs[i].id = screenList[i];
        screenConfigs[i].mirrorCount = 0;
    }

    for (int i = 0; i < screenCount; i++) {
        if (CGDisplayIsInMirrorSet(screenConfigs[i].id) && CGDisplayMirrorsDisplay(screenConfigs[i].id) != 0) { //this screen is a secondary screen in a mirroring set
            int primaryScreenId = CGDisplayMirrorsDisplay(screenConfigs[i].id);
            int secondaryScreenId = screenConfigs[i].id;

            for (int j = 0; j < screenCount; j++) {
                if (screenConfigs[j].id == primaryScreenId) {
                    screenConfigs[j].mirrors[screenConfigs[j].mirrorCount] = secondaryScreenId;
                    screenConfigs[j].mirrorCount++;
                }
            }

            screenConfigs[i].id = -1;
        }
    }

    printf("Execute the command below to set your screens to the current arrangement:\n\n");
    printf("displayplacer");
    for (int i = 0; i < screenCount; i++) {
        ScreenConfig curScreen = screenConfigs[i];

        if (curScreen.id == -1) { //earlier we set this to -1 since it will be represented as a mirror on output
            continue;
        }

        int curModeId;
        CGSGetCurrentDisplayMode(curScreen.id, &curModeId);
        modes_D4 curMode;
        CGSGetDisplayModeDescriptionOfLength(curScreen.id, curModeId, &curMode, 0xD4);

        char hz[8]; //hz:999 \0
        strlcpy(hz, "", sizeof(hz)); //most displays do not have hz option
        if (curMode.derived.freq) {
            snprintf(hz, sizeof(hz), "hz:%i ", curMode.derived.freq);
        }

        char* scaling = (curMode.derived.density == 2.0) ? "on" : "off";

        char mirrors[UUID_SIZE * MIRROR_MAX];
        strlcpy(mirrors, "", sizeof(mirrors));
        for (int j = 0; j < curScreen.mirrorCount; j++) {
            char mirrorUUID[UUID_SIZE];
            CFStringGetCString(CFUUIDCreateString(kCFAllocatorDefault, CGDisplayCreateUUIDFromDisplayID(curScreen.mirrors[j])), mirrorUUID, sizeof(mirrorUUID), kCFStringEncodingUTF8);

            strlcat(mirrors, "+", sizeof(mirrors));
            strlcat(mirrors, mirrorUUID, sizeof(mirrors));
        }

        char curScreenUUID[UUID_SIZE];
        CFStringGetCString(CFUUIDCreateString(kCFAllocatorDefault, CGDisplayCreateUUIDFromDisplayID(curScreen.id)), curScreenUUID, sizeof(curScreenUUID), kCFStringEncodingUTF8);

        printf(" \"id:%s%s res:%ix%i %scolor_depth:%i scaling:%s origin:(%i,%i) degree:%i\"", curScreenUUID, mirrors, (int) CGDisplayPixelsWide(curScreen.id), (int) CGDisplayPixelsHigh(curScreen.id), hz, curMode.derived.depth, scaling, (int) CGDisplayBounds(curScreen.id).origin.x, (int) CGDisplayBounds(curScreen.id).origin.y, (int) CGDisplayRotation(curScreen.id));
    }
    printf("\n");
}

CGDirectDisplayID convertUUIDtoID(char* uuid) {
    if (strstr(uuid, "-") == NULL) { //backward compatability with legacy integer ID format
        return atoi(uuid);
    }

    CFStringRef uuidStringRef = CFStringCreateWithCString(kCFAllocatorDefault, uuid, kCFStringEncodingUTF8);
    CFUUIDRef uuidRef = CFUUIDCreateFromString(kCFAllocatorDefault, uuidStringRef);
    return CGDisplayGetDisplayIDFromUUID(uuidRef);
}

bool validateScreenOnline(CGDirectDisplayID onlineDisplayList[], int screenCount, CGDirectDisplayID screenId, char* screenUUID) {
    for (int i = 0; i < screenCount; i++) {
        if (onlineDisplayList[i] == screenId) {
            return true;
        }
    }

    fprintf(stderr, "Unable to find screen %s - skipping changes for that screen\n", screenUUID);
    return false;
}

bool rotateScreen(CGDirectDisplayID screenId, char* screenUUID, int degree) {
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
        fprintf(stderr, "Error rotating screen %s\n", screenUUID);
        return false;
    }

    return true;
}

bool configureMirror(CGDisplayConfigRef configRef, CGDirectDisplayID primaryScreenId, char* primaryScreenUUID, CGDirectDisplayID mirrorScreenId, char* mirrorScreenUUID) {
    int retVal = CGConfigureDisplayMirrorOfDisplay(configRef, mirrorScreenId, primaryScreenId);

    if (retVal != 0) {
        fprintf(stderr, "Error making the secondary screen %s mirror the primary screen %s\n", mirrorScreenUUID, primaryScreenUUID);
        return false;
    }

    return true;
}

bool configureResolution(CGDisplayConfigRef configRef, CGDirectDisplayID screenId, char* screenUUID, int width, int height, int hz, int depth, bool scaled, int modeNum) {
    int modeCount;
    modes_D4* modes;

    if (modeNum != -1) { //user specified modeNum instead of height/width/hz
        CGSConfigureDisplayMode(configRef, screenId, modeNum);
        return true;
    }

    CopyAllDisplayModes(screenId, &modes, &modeCount);

    modes_D4 bestMode = modes[0];
    bool modeFound = false;

    //loop through all modes looking for one that matches user input resolution
    for (int i = 0; i < modeCount; i++) {
        modes_D4 curMode = modes[i];
        
        //prioritize exact matches of user input params
        if (curMode.derived.width != width) continue;
        if (curMode.derived.height != height) continue;
        if (hz && curMode.derived.freq != hz) continue;
        if (depth && curMode.derived.depth != depth) continue;
        if (scaled && curMode.derived.density != 2.0) continue;

        if (!modeFound) {
            modeFound = true;
            bestMode = curMode;
        }

        if (curMode.derived.freq > bestMode.derived.freq || (curMode.derived.freq == bestMode.derived.freq && curMode.derived.depth > bestMode.derived.depth)) {
            bestMode = curMode;
        }
    }

    if (modeFound) {
        CGSConfigureDisplayMode(configRef, screenId, bestMode.derived.mode);
        return true;
    }

    fprintf(stderr, "Screen ID %s: could not find res:%ix%i", screenUUID, width, height);
    if (hz) {
        fprintf(stderr, "x%i", hz);
    }
    if (depth) {
        fprintf(stderr, " color_depth:%i", depth);
    }
    char* scalingString = (scaled == 2.0) ? "on" : "off";
    fprintf(stderr, " scaling:%s", scalingString);

    fprintf(stderr, "\n");

    return false;
}

bool configureOrigin(CGDisplayConfigRef configRef, CGDirectDisplayID screenId, char* screenUUID, int x, int y) {
    int retVal = CGConfigureDisplayOrigin(configRef, screenId, x, y);

    if (retVal != 0) {
        fprintf(stderr, "Error moving screen %s to %ix%i\n", screenUUID, x, y);
        return false;
    }

    return true;
}
