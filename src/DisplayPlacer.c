#include <IOKit/graphics/IOGraphicsLib.h>
#include <ApplicationServices/ApplicationServices.h>
#include <mach/mach.h>
#include <math.h>
#include <stdio.h>
#include "Header.h"

int main(int argc, char* argv[]) {
    if (argc == 1 || strcmp(argv[1], "--help") == 0) {
        printHelp();
        return 0;
    }

    if (strcmp(argv[1], "--version") == 0) {
        printVersion();
        return 0;
    }

    if (argc == 3 && strcmp(argv[1], "list") == 0 && strcmp(argv[2], "--v1.3.0") == 0) {
        v130_listScreens();
        v130_printCurrentProfile();
        return 0;
    }

    if (strcmp(argv[1], "list") == 0) {
        listScreens();
        printCurrentProfile();
        return 0;
    }

    ScreenConfig* screenConfigs = malloc((argc - 1) * sizeof(ScreenConfig));

    for (int i = 0; i < argc - 1; i++) {
        //overwrite garbage in memory with default values
        strlcpy(screenConfigs[i].uuid, "\0", sizeof(screenConfigs[i].uuid));
        screenConfigs[i].depth = 0;
        screenConfigs[i].hz = 0;
        screenConfigs[i].enabled = true;
        screenConfigs[i].quietMissingScreen = false;
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
                case 'e': //enabled
                    propToken = strtok_r(NULL, ":", &propSavePtr);

                    if (strcmp(propToken, "false") == 0) {
                        screenConfigs[i].enabled = false;
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
                case 'q': //quiet
                    propToken = strtok_r(NULL, ":", &propSavePtr);

                    if (strcmp(propToken, "true") == 0) {
                        screenConfigs[i].quietMissingScreen = true;
                    }
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

    //If there is only one active screen and no screen id was provided, don't require a screen id and instead default to the only available screen id.
    //Active means the display is not disabled and is not mirroring another screen.
    if (screenConfigs[0].uuid[0] == '\0') {
        CGDisplayCount activeScreenCount; //count of active screens
        CGGetActiveDisplayList(INT_MAX, NULL, &activeScreenCount); //get number of active screens and store in activeScreenCount
        CGDirectDisplayID activeScreenList[activeScreenCount];
        CGGetActiveDisplayList(INT_MAX, activeScreenList, &activeScreenCount); //store active display list in array of size activeScreenCount

        if (activeScreenCount == 1) {
            char curScreenUUID[UUID_SIZE];
            CFStringGetCString(CFUUIDCreateString(kCFAllocatorDefault, CGDisplayCreateUUIDFromDisplayID(activeScreenList[0])), curScreenUUID, sizeof(curScreenUUID), kCFStringEncodingUTF8);
            strlcpy(screenConfigs[0].uuid, curScreenUUID, sizeof(screenConfigs[0].uuid));
        }
    }

    bool isSuccess = true; //returns non-zero exit code on any errors but allows for completing remaining program execution

    isSuccess = setEnableds(screenConfigs, argc, screenList, screenCount) && isSuccess; //Enable/disable screens and call CGCompleteDisplayConfiguration as a prereq to applying other config.
    isSuccess = unsetMirrors(screenConfigs, argc, screenList, screenCount) && isSuccess; //Disable all mirroring prior and call CGCompleteDisplayConfiguration as a prereq to ensure displays are in a known starting state.
    isSuccess = setRotations(screenConfigs, argc, screenList, screenCount) && isSuccess; //Set all display rotations as a prereq so the portrait or landscape resolutions can be found when setting the resolutions. Also, disable mirroring afer each rotation alteration since macOS will often times oddly auto-enable mirroring when a screen is rotated.

    CGDisplayConfigRef configRef;
    CGBeginDisplayConfiguration(&configRef); //Share a configRef for the remainder of the program since these configs do not interrupt each other. This reduces the number of screen flashes when running displayplacer.
    isSuccess = setMirrors(screenConfigs, argc, configRef, screenList, screenCount) && isSuccess;
    isSuccess = setResolutions(screenConfigs, argc, configRef, screenList, screenCount) && isSuccess;
    isSuccess = setPositions(screenConfigs, argc, configRef, screenList, screenCount) && isSuccess;

    int retVal = CGCompleteDisplayConfiguration(configRef, kCGConfigurePermanently);

    if (retVal != 0) {
        fprintf(stderr, "Error finalizing mirroring, resolutions, and/or positions\n");
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
            "    Apply screen config (hz & color_depth are optional): displayplacer \"id:<screenId> res:<width>x<height> hz:<num> color_depth:<num> scaling:<on/off> origin:(<x>,<y>) degree:<0/90/180/270>\"\n"
            "\n"
            "    Apply screen config using mode: displayplacer \"id:<screenId> mode:<modeNum> origin:(<x>,<y>) degree:<0/90/180/270>\"\n"
            "\n"
            "    Apply screen config with mirrored screens: displayplacer \"id:<mainScreenId>+<1stMirrorScreenId>+<2ndMirrorScreenId> res:<width>x<height> scaling:<on/off> origin:(<x>,<y>) degree:<0/90/180/270>\"\n"
            "\n"
            "    Silence errors per-screen using quiet: displayplacer \"id:<screenId> mode:<modeNum> origin:(<x>,<y>) degree:0 quiet:true\"\n"
            "\n"
            "    Disable a screen: displayplacer \"id:<screenId> enabled:false\"\n"
            "\n"
            "Instructions:\n"
            "    1. Manually set rotations 1st*, resolutions 2nd, and arrangement 3rd. For extra resolutions and rotations read 'Notes' below.\n"
            "        - Open System Preferences -> Displays\n"
            "        - Choose desired screen rotations (use displayplacer for rotating internal MacBook screen).\n"
            "        - Choose desired resolutions (use displayplacer for extra resolutions).\n"
            "        - Drag the white bar to your desired primary screen.\n"
            "        - Arrange screens as desired and/or enable mirroring. To enable partial mirroring hold the alt/option key and drag a display on top of another.\n"
            "    2. Use `displayplacer list` to print your current layout's args, so you can create profiles for scripting/hotkeys with Automator, BetterTouchTool, etc.\n"
            "\n"
            "ScreenIds Switching:\n"
            "    Unfortunately, macOS sometimes changes the persistent screenIds when there are race conditions from external screens waking up in non-determinisic order. If none of the screenId options below work for your setup, please search around in the GitHub Issues for conversation regarding this. Many people have written shell scripts to work around this issue.\n"
            "\n"
            "    You can mix and match screenId types across your setup.\n"
            "    - Persistent screenIds usually stay the same. They are recommended for most use cases.\n"
            "    - Contextual screenIds change when switching GPUs or when cables switch ports. If you notice persistent screenIds switching around, try using the contextual screenIds.\n"
            "    - Serial screenIds are tied to your display hardware. If the serial screenIds are unique for all of your monitors, use these.\n"
            "Notes:\n"
            "    - *`displayplacer list` and system prefs only show resolutions for the screen's current rotation.\n"
            "    - Use an extra resolution shown in `displayplacer list` by executing `displayplacer \"id:<screenId> mode:<modeNum>\"`. Some of the resolutions listed do not work. If you select one, displayplacer will default to another working resolution.\n"
            "    - Rotate your internal MacBook screen by executing `displayplacer \"id:<screenId> degree:<0/90/180/270>\"`\n"
            "    - If you disable a screen, you may need to unplug/replug it to bring it back. However, on some setups, you can re-enable it with `displayplacer \"id:<screenId> enabled:true\"`\n"
            "    - The screen set to origin (0,0) will be set as the primary screen (white bar in system prefs).\n"
            "    - The first screenId in a mirroring set will be the 'Optimize for' screen in the system prefs. You can only choose resolutions for the 'Optimize for' screen. If there is a mirroring resolution you need but cannot find, try making a different screenId the first of the set.\n"
            "    - hz and color_depth are optional. If left out, the highest hz and then the highest color_depth will be auto applied.\n"
            "    - screenId is optional if there is only one screen. Rule of thumb is that displayplacer is expecting the entire profile config per screen though, so this may be buggy.\n"
            "\n"
            "Feedback:\n"
            "    Please create a GitHub Issue for any feedback, feature requests, bugs, Homebrew issues, etc. Happy to accept pull requests too! https://github.com/jakehilborn/displayplacer\n"
    );
}

void printVersion() {
    printf(
        "displayplacer v1.4.0-dev\n"
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
        printf("Persistent screen id: %s\n", curScreenUUID);
        printf("Contextual screen id: %i\n", curScreen);
        UInt32 serialID = CGDisplaySerialNumber(screenList[i]);
        printf("Serial screen id: s%u\n", serialID);

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
        printf("Scaling: %s\n", scaling);

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

        char* enabled = isScreenEnabled(curScreen) ? "true" : "false";
        printf("Enabled: %s\n", enabled);

        int modeCount;
        modes_D4* modes;
        CopyAllDisplayModes(curScreen, &modes, &modeCount);

        printf("Resolutions for rotation %i:\n", (int) CGDisplayRotation(curScreen));
        for (int j = 0; j < modeCount; j++) {
            modes_D4 mode = modes[j];

            printf("  mode %i: res:%dx%d", j, mode.derived.width, mode.derived.height);

            if (mode.derived.freq) {
                printf(" hz:%i", mode.derived.freq);
            }

            printf(" color_depth:%i", mode.derived.depth);

            if (mode.derived.density == 2.0) {
                printf(" scaling:on");
            }

            if (j == curModeId) {
                printf(" <-- current mode");
            }

            printf("\n");
        }
        printf("\n");
        free(modes);
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

    printf("Execute the command below to set your screens to the current arrangement. If screen ids are switching, please run `displayplacer --help` for info on using contextual or serial ids instead of persistent ids.\n\n");
    printf("displayplacer");
    for (int i = 0; i < screenCount; i++) {
        ScreenConfig curScreen = screenConfigs[i];

        if (curScreen.id == -1) { //earlier we set this to -1 since it will be represented as a mirror on output
            continue;
        }

        char curScreenUUID[UUID_SIZE];
        CFStringGetCString(CFUUIDCreateString(kCFAllocatorDefault, CGDisplayCreateUUIDFromDisplayID(curScreen.id)), curScreenUUID, sizeof(curScreenUUID), kCFStringEncodingUTF8);
        int curModeId;
        CGSGetCurrentDisplayMode(curScreen.id, &curModeId);
        modes_D4 curMode;
        CGSGetDisplayModeDescriptionOfLength(curScreen.id, curModeId, &curMode, 0xD4);
        bool enabled = CGDisplayIsActive(curScreen.id);

        if (!enabled) {
            printf(" \"id:%s enabled:false\"", curScreenUUID);
            continue;
        }

        char hz[8]; //hz:999 \0
        strlcpy(hz, "", sizeof(hz)); //most displays do not have hz option
        if (curMode.derived.freq) {
            snprintf(hz, sizeof(hz), "hz:%i ", curMode.derived.freq);
        }

        char* scaling = (curMode.derived.density == 2.0) ? "on" : "off";

        char mirrors[(UUID_SIZE + 1) * MIRROR_MAX + 1];
        strlcpy(mirrors, "", sizeof(mirrors));
        for (int j = 0; j < curScreen.mirrorCount; j++) {
            char mirrorUUID[UUID_SIZE];
            CFStringGetCString(CFUUIDCreateString(kCFAllocatorDefault, CGDisplayCreateUUIDFromDisplayID(curScreen.mirrors[j])), mirrorUUID, sizeof(mirrorUUID), kCFStringEncodingUTF8);

            strlcat(mirrors, "+", sizeof(mirrors));
            strlcat(mirrors, mirrorUUID, sizeof(mirrors));
        }

        printf(" \"id:%s%s res:%ix%i %scolor_depth:%i enabled:true scaling:%s origin:(%i,%i) degree:%i\"",
            curScreenUUID, mirrors,
            (int) CGDisplayPixelsWide(curScreen.id), (int) CGDisplayPixelsHigh(curScreen.id),
            hz,
            curMode.derived.depth,
            scaling,
            (int) CGDisplayBounds(curScreen.id).origin.x, (int) CGDisplayBounds(curScreen.id).origin.y,
            (int) CGDisplayRotation(curScreen.id)
        );
    }
    printf("\n");
}

CGDirectDisplayID convertUUIDtoID(char* uuid) {
    if (strstr(uuid, "s") != NULL) { //serial screen id starts with "s", for example "s123456789"
        return convertSerialToID(uuid);
    }

    if (strstr(uuid, "-") == NULL) { //contextual screen id is just an integer
        return atoi(uuid);
    }

    //uuid contains "-" but does not contain "s" since it is hexadecimal
    CFStringRef uuidStringRef = CFStringCreateWithCString(kCFAllocatorDefault, uuid, kCFStringEncodingUTF8);
    CFUUIDRef uuidRef = CFUUIDCreateFromString(kCFAllocatorDefault, uuidStringRef);
    return CGDisplayGetDisplayIDFromUUID(uuidRef);
}

CGDirectDisplayID convertSerialToID(char* serialIdString) {
    int serialId = atoi(serialIdString + 1); //"s123456789" -> 123456789

    CGDisplayCount screenCount;
    CGGetOnlineDisplayList(INT_MAX, NULL, &screenCount); //get number of online screens and store in screenCount

    CGDirectDisplayID screenList[screenCount];
    CGGetOnlineDisplayList(INT_MAX, screenList, &screenCount);

    for (int i = 0; i < screenCount; i++) {
        CGDirectDisplayID curScreen = screenList[i];
        if (CGDisplaySerialNumber(curScreen) == serialId){
            return curScreen;
        }
    }

    fprintf(stderr, "Error converting serialId %s to a screenId\n", serialIdString);
    return 0;
}

bool validateScreenOnline(CGDirectDisplayID onlineDisplayList[], CGDisplayCount screenCount, CGDirectDisplayID screenId, char* screenUUID, bool quietMissingScreen) {
    for (int i = 0; i < screenCount; i++) {
        if (onlineDisplayList[i] == screenId) {
            return true;
        }
    }

    if (!quietMissingScreen) {
        fprintf(stderr, "Unable to find screen %s - skipping changes for that screen\n", screenUUID); //TODO only print once per screen
    }
    return false;
}

bool isScreenEnabled(CGDirectDisplayID screenId) {
    return CGDisplayIsActive(screenId) || CGDisplayIsInMirrorSet(screenId);
}

bool unsetMirrors(ScreenConfig* screenConfigs, int argc, CGDirectDisplayID screenList[], CGDisplayCount screenCount) {
    CGDisplayConfigRef configRef;
    CGBeginDisplayConfiguration(&configRef);
    bool isSuccess = true;

    for (int i = 0; i < argc - 1; i++) {
        screenConfigs[i].id = convertUUIDtoID(screenConfigs[i].uuid);
        if (!validateScreenOnline(screenList, screenCount, screenConfigs[i].id, screenConfigs[i].uuid, screenConfigs[i].quietMissingScreen)) {
            if (!screenConfigs[i].quietMissingScreen) {
                isSuccess = false;
            }
            continue;
        }

        if (!screenConfigs[i].enabled) {
            continue; //screen is disabled, no need to apply other configs for this screen
        }

        for (int j = 0; j < screenConfigs[i].mirrorCount; j++) {
            screenConfigs[i].mirrors[j] = convertUUIDtoID(screenConfigs[i].mirrorUUIDs[j]);
            if (!validateScreenOnline(screenList, screenCount, screenConfigs[i].mirrors[j], screenConfigs[i].mirrorUUIDs[j], screenConfigs[i].quietMissingScreen)) {
                if (!screenConfigs[i].quietMissingScreen) {
                    isSuccess = false;
                }
                continue;
            }

            isSuccess = unsetMirror(configRef, screenConfigs[i].mirrors[j], screenConfigs[i].mirrorUUIDs[j]) && isSuccess;
        }

        isSuccess = unsetMirror(configRef, screenConfigs[i].id, screenConfigs[i].uuid) && isSuccess;
    }

    if (CGCompleteDisplayConfiguration(configRef, kCGConfigurePermanently)) {
        fprintf(stderr, "Error unsetting mirroring as a prerequisite to applying profiles\n");
        isSuccess = false;
    }

    return isSuccess;
}

bool unsetMirror(CGDisplayConfigRef configRef, CGDirectDisplayID mirrorScreenId, char* mirrorScreenUUID) {
    if (CGDisplayIsInMirrorSet(mirrorScreenId) && CGDisplayMirrorsDisplay(mirrorScreenId) != 0) { //this screen is a secondary screen in a mirroring set
        if (CGConfigureDisplayMirrorOfDisplay(configRef, mirrorScreenId, kCGNullDirectDisplay) != 0) {
            fprintf(stderr, "Error disabling mirroring on screen %s\n", mirrorScreenUUID);
            return false;
        }
    }

    return true;
}

bool setEnableds(ScreenConfig* screenConfigs, int argc, CGDirectDisplayID screenList[], CGDisplayCount screenCount) {
    CGDisplayConfigRef configRef;
    CGBeginDisplayConfiguration(&configRef);
    bool isSuccess = true;

    for (int i = 0; i < argc - 1; i++) {
        screenConfigs[i].id = convertUUIDtoID(screenConfigs[i].uuid);
        if (!validateScreenOnline(screenList, screenCount, screenConfigs[i].id, screenConfigs[i].uuid, screenConfigs[i].quietMissingScreen)) {
            if (!screenConfigs[i].quietMissingScreen) {
                isSuccess = false;
            }
            continue;
        }

        for (int j = 0; j < screenConfigs[i].mirrorCount; j++) {
            screenConfigs[i].mirrors[j] = convertUUIDtoID(screenConfigs[i].mirrorUUIDs[j]);
            if (!validateScreenOnline(screenList, screenCount, screenConfigs[i].mirrors[j], screenConfigs[i].mirrorUUIDs[j], screenConfigs[i].quietMissingScreen)) {
                if (!screenConfigs[i].quietMissingScreen) {
                    isSuccess = false;
                }
                continue;
            }

            isSuccess = setEnabled(configRef, screenConfigs[i].mirrors[j], screenConfigs[i].mirrorUUIDs[j], screenConfigs[i].enabled) && isSuccess;
        }

        isSuccess = setEnabled(configRef, screenConfigs[i].id, screenConfigs[i].uuid, screenConfigs[i].enabled) && isSuccess;
    }

    if (CGCompleteDisplayConfiguration(configRef, kCGConfigurePermanently)) {
        fprintf(stderr, "Error altering enabled/disabled config\n");
        isSuccess = false;
    }

    return isSuccess;
}

bool setEnabled(CGDisplayConfigRef configRef, CGDirectDisplayID screenId, char* screenUUID, bool isEnabled) {
    if (isScreenEnabled(screenId) != isEnabled) {
        CGError retVal = CGSConfigureDisplayEnabled(configRef, screenId, isEnabled);

        if (retVal != kCGErrorSuccess) {
            fprintf(stderr, "Error setting screen %s to enabled:%s\n", screenUUID, (isEnabled) ? "true" : "false");
            return false;
        }
    }

    return true;
}

bool setRotations(ScreenConfig* screenConfigs, int argc, CGDirectDisplayID screenList[], CGDisplayCount screenCount) {
    bool isSuccess = true;

    for (int i = 0; i < argc - 1; i++) {
        screenConfigs[i].id = convertUUIDtoID(screenConfigs[i].uuid);
        if (!validateScreenOnline(screenList, screenCount, screenConfigs[i].id, screenConfigs[i].uuid, screenConfigs[i].quietMissingScreen)) {
            if (!screenConfigs[i].quietMissingScreen) {
                isSuccess = false;
            }
            continue;
        }

        if (!screenConfigs[i].enabled) {
            continue; //screen is disabled, no need to apply other configs for this screen
        }

        for (int j = 0; j < screenConfigs[i].mirrorCount; j++) {
            screenConfigs[i].mirrors[j] = convertUUIDtoID(screenConfigs[i].mirrorUUIDs[j]);
            if (!validateScreenOnline(screenList, screenCount, screenConfigs[i].mirrors[j], screenConfigs[i].mirrorUUIDs[j], screenConfigs[i].quietMissingScreen)) {
                if (!screenConfigs[i].quietMissingScreen) {
                    isSuccess = false;
                }
                continue;
            }

            if (CGDisplayRotation(screenConfigs[i].mirrors[j]) != screenConfigs[i].degree) {
                isSuccess = setRotation(screenConfigs[i].mirrors[j], screenConfigs[i].mirrorUUIDs[j], screenConfigs[i].degree) && isSuccess;
                isSuccess = unsetMirrors(screenConfigs, argc, screenList, screenCount) && isSuccess;
            }
        }

        if (CGDisplayRotation(screenConfigs[i].id) != screenConfigs[i].degree) {
            isSuccess = setRotation(screenConfigs[i].id, screenConfigs[i].uuid, screenConfigs[i].degree) && isSuccess;
            isSuccess = unsetMirrors(screenConfigs, argc, screenList, screenCount) && isSuccess;
        }
    }

    return isSuccess;
}

bool setMirrors(ScreenConfig* screenConfigs, int argc, CGDisplayConfigRef configRef, CGDirectDisplayID screenList[], CGDisplayCount screenCount) {
    bool isSuccess = true;

    for (int i = 0; i < argc - 1; i++) {
        screenConfigs[i].id = convertUUIDtoID(screenConfigs[i].uuid);
        if (!validateScreenOnline(screenList, screenCount, screenConfigs[i].id, screenConfigs[i].uuid, screenConfigs[i].quietMissingScreen)) {
            if (!screenConfigs[i].quietMissingScreen) {
                isSuccess = false;
            }
            continue;
        }

        if (!screenConfigs[i].enabled) {
            continue; //screen is disabled, no need to apply other configs for this screen
        }

        for (int j = 0; j < screenConfigs[i].mirrorCount; j++) {
            screenConfigs[i].mirrors[j] = convertUUIDtoID(screenConfigs[i].mirrorUUIDs[j]);
            if (!validateScreenOnline(screenList, screenCount, screenConfigs[i].mirrors[j], screenConfigs[i].mirrorUUIDs[j], screenConfigs[i].quietMissingScreen)) {
                if (!screenConfigs[i].quietMissingScreen) {
                    isSuccess = false;
                }
                continue;
            }

            isSuccess = setMirror(configRef, screenConfigs[i].id, screenConfigs[i].uuid, screenConfigs[i].mirrors[j], screenConfigs[i].mirrorUUIDs[j]) && isSuccess;
        }
    }

    return isSuccess;
}

bool setMirror(CGDisplayConfigRef configRef, CGDirectDisplayID primaryScreenId, char* primaryScreenUUID, CGDirectDisplayID mirrorScreenId, char* mirrorScreenUUID) {
    int retVal = CGConfigureDisplayMirrorOfDisplay(configRef, mirrorScreenId, primaryScreenId);

    if (retVal != 0) {
        fprintf(stderr, "Error making the secondary screen %s mirror the primary screen %s\n", mirrorScreenUUID, primaryScreenUUID);
        return false;
    }

    return true;
}

bool setResolutions(ScreenConfig* screenConfigs, int argc, CGDisplayConfigRef configRef, CGDirectDisplayID screenList[], CGDisplayCount screenCount) {
    bool isSuccess = true;

    for (int i = 0; i < argc - 1; i++) {
        screenConfigs[i].id = convertUUIDtoID(screenConfigs[i].uuid);
        if (!validateScreenOnline(screenList, screenCount, screenConfigs[i].id, screenConfigs[i].uuid, screenConfigs[i].quietMissingScreen)) {
            if (!screenConfigs[i].quietMissingScreen) {
                isSuccess = false;
            }
            continue;
        }

        if (!screenConfigs[i].enabled) {
            continue; //screen is disabled, no need to apply other configs for this screen
        }

        isSuccess = setResolution(configRef, screenConfigs[i].id, screenConfigs[i].uuid, screenConfigs[i].width, screenConfigs[i].height, screenConfigs[i].hz, screenConfigs[i].depth, screenConfigs[i].scaled, screenConfigs[i].modeNum) && isSuccess;
    }

    return isSuccess;
}

bool setResolution(CGDisplayConfigRef configRef, CGDirectDisplayID screenId, char* screenUUID, int width, int height, int hz, int depth, bool scaled, int modeNum) {
    int modeCount;
    modes_D4* modes;

    if (modeNum != -1) { //user specified modeNum instead of height/width/hz
        CGSConfigureDisplayMode(configRef, screenId, modeNum);
        return true;
    }

    CopyAllDisplayModes(screenId, &modes, &modeCount);

    modes_D4 bestMode = modes[0];
    bool modeFound = false;

    //loop through all modes looking for one that matches user input params
    for (int i = 0; i < modeCount; i++) {
        modes_D4 curMode = modes[i];

        //prioritize exact matches of user input params
        if (curMode.derived.width != width) continue;
        if (curMode.derived.height != height) continue;
        if (hz && curMode.derived.freq != hz) continue;
        if (depth && curMode.derived.depth != depth) continue;
        if (scaled && curMode.derived.density != 2.0) continue;
        if (!scaled && curMode.derived.density == 2.0) continue;

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
        free(modes);
        return true;
    }

    fprintf(stderr, "Screen ID %s: could not find res:%ix%i", screenUUID, width, height);
    if (hz) {
        fprintf(stderr, " hz:%i", hz);
    }
    if (depth) {
        fprintf(stderr, " color_depth:%i", depth);
    }
    char* scalingString = (scaled) ? "on" : "off";
    fprintf(stderr, " scaling:%s", scalingString);

    fprintf(stderr, "\n");

    free(modes);
    return false;
}

bool setPositions(ScreenConfig* screenConfigs, int argc, CGDisplayConfigRef configRef, CGDirectDisplayID screenList[], CGDisplayCount screenCount) {
    bool isSuccess = true;

    for (int i = 0; i < argc - 1; i++) {
        screenConfigs[i].id = convertUUIDtoID(screenConfigs[i].uuid);
        if (!validateScreenOnline(screenList, screenCount, screenConfigs[i].id, screenConfigs[i].uuid, screenConfigs[i].quietMissingScreen)) {
            if (!screenConfigs[i].quietMissingScreen) {
                isSuccess = false;
            }
            continue;
        }

        if (!screenConfigs[i].enabled) {
            continue; //screen is disabled, no need to apply other configs for this screen
        }

        CGPoint curOrigin = CGDisplayBounds(screenConfigs[i].id).origin;
        if (((int) curOrigin.x != screenConfigs[i].x || (int) curOrigin.y != screenConfigs[i].y) && screenCount > 1) { //setting a screen to its current origin makes displayplacer hang for a couple seconds. If there is only one screen, macOS will force the origin to be (0,0) so we do not need to set it.
            isSuccess = setPosition(configRef, screenConfigs[i].id, screenConfigs[i].uuid, screenConfigs[i].x, screenConfigs[i].y) && isSuccess;
        }
    }

    return isSuccess;
}

bool setPosition(CGDisplayConfigRef configRef, CGDirectDisplayID screenId, char* screenUUID, int x, int y) {
    int retVal = CGConfigureDisplayOrigin(configRef, screenId, x, y);

    if (retVal != 0) {
        fprintf(stderr, "Error moving screen %s to %ix%i\n", screenUUID, x, y);
        return false;
    }

    return true;
}
