#include <IOKit/graphics/IOGraphicsLib.h>
#include <ApplicationServices/ApplicationServices.h>
#include <mach/mach.h>
#include <math.h>
#include <stdio.h>
#import "../Header.h"

void v130_listScreens() {
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

void v130_printCurrentProfile() {
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
