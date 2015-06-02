//
//  main.m
//  SetRes
//
//  Created by Jake on 5/16/15.
//  Copyright (c) 2015 Jake. All rights reserved.
//

#include <IOKit/graphics/IOGraphicsLib.h>
#include <ApplicationServices/ApplicationServices.h>
#include <unistd.h>
#include "head.h"

int main(int argc, const char * argv[])
{
    if (strcmp(argv[1], "--list") == 0)
    {
        listScreens();
        return 0;
    }
    
    ScreenConfig* screenConfigs = malloc((argc - 1) * sizeof(ScreenConfig));
    
    for (int i = 0; i < argc - 1; i++)
    {
        screenConfigs[i].modeNum = -1; //set modeNum from 0 to -1 in case user wants to set mode to 0 later
        
        char* propGroup = argv[i + 1];
        
        char *propSetSavePtr = NULL;
        char* propSetToken = strtok_r(propGroup, " \t", &propSetSavePtr);
        while (propSetToken)
        {
            char *propSavePtr = NULL;
            char* propToken = strtok_r(propSetToken, ":", &propSavePtr);
                
            switch (propToken[0])
            {
                case 'i': //id
                    propToken = strtok_r(NULL, ":", &propSavePtr);

                    screenConfigs[i].id = atoi(propToken);
                    break;
                case 'r': //res
                    propToken = strtok_r(NULL, ":", &propSavePtr);
                    
                    char *resSavePtr = NULL;
                    char* resToken = strtok_r(propToken, "x", &resSavePtr);
                    screenConfigs[i].width = atoi(resToken);
                    resToken = strtok_r(NULL, "x", &resSavePtr);
                    screenConfigs[i].height = atoi(resToken);
                    resToken = strtok_r(NULL, "x", &resSavePtr);
                    if (resToken)
                        screenConfigs[i].hz = atoi(resToken);
                    
                    break;
                case 's': //scaling
                    propToken = strtok_r(NULL, ":", &propSavePtr);

                    if (strcmp(propToken, "on") == 0)
                        screenConfigs[i].scaled = true;
                    else
                        screenConfigs[i].scaled = false;
                
                    break;
                case 'o': //origin
                    propToken = strtok_r(NULL, ":", &propSavePtr);
                    
                    char *originSavePtr = NULL;
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
                    fprintf(stderr, "parsing error\n");
                    exit(1);
            }
            
            propSetToken = strtok_r(NULL, " \t", &propSetSavePtr);
        }
    }
    
    for (int i = 0; i < argc - 1; i++)
    {
        rotateScreen(screenConfigs[i].id, screenConfigs[i].degree);
        setResolution(screenConfigs[i].id, screenConfigs[i].width, screenConfigs[i].height, screenConfigs[i].scaled, screenConfigs[i].hz, screenConfigs[i].modeNum);
    }
    
    setLayout(screenConfigs, sizeof(ScreenConfig) * (argc -1));
    
    free(screenConfigs);
    return 0;
}

void listScreens()
{
    CGDisplayCount screenCount;
    CGGetActiveDisplayList(INT_MAX, NULL, &screenCount); //get number of active screens and store in screenCount

    CGDirectDisplayID screenList[screenCount];
    CGGetActiveDisplayList(INT_MAX, screenList, &screenCount);
    
    for (int i = 0; i < screenCount; i++)
    {
        UInt32 curScreen = screenList[i];
        
        printf("Screen ID: %i\n", curScreen);
        printf("%lux%lu\n", CGDisplayPixelsWide(curScreen), CGDisplayPixelsHigh(curScreen));
        printf("Rotation: %f\n", CGDisplayRotation(curScreen));
        printf("Origin: (%f, %f)\n", CGDisplayBounds(curScreen).origin.x, CGDisplayBounds(curScreen).origin.y);
        
        int modeCount;
        modes_D4* modes;
        CopyAllDisplayModes(curScreen, &modes, &modeCount);
        for(int i=0; i<modeCount; i++)
        {
            modes_D4 mode = modes[i];
            
            if(mode.derived.density == 1.0) //scaling off
            {
                if(mode.derived.freq) //if screen supports different framerates
                    printf("mode %i: Res=%dx%dx%i\n", i, mode.derived.width, mode.derived.height, mode.derived.freq);
                else
                    printf("mode %i: Res=%dx%d\n", i, mode.derived.width, mode.derived.height);
            }
            else //scaling on
            {
                if(mode.derived.freq) //if screen supports different framerates
                    printf("mode %i: Res=%dx%dx%i, scaled\n", i, mode.derived.width, mode.derived.height, mode.derived.freq);
                else
                    printf("mode %i: Res=%dx%d, scaled\n", i, mode.derived.width, mode.derived.height);
            }
        }
        printf("\n");
    }
}

int rotateScreen(CGDirectDisplayID screen, int degree)
{
    CGGetOnlineDisplayList(NULL, NULL, NULL); //helps prevent hanging from rotation change? Test this.
    io_service_t service = CGDisplayIOServicePort(screen);
    IOOptionBits options;
    
    switch(degree)
    {
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
    if (retVal != 0)
        fprintf(stderr, "error rotating display");
    
    while (CGDisplayRotation(screen) != degree)
    {
        //wait until screen rotates
    }
    
    return 0;
}

int setResolution(CGDirectDisplayID screenId, int width, int height, bool scaled, int hz, int modeNum)
{
    int modeCount;
    modes_D4* modes;
    CopyAllDisplayModes(screenId, &modes, &modeCount);
    
    if (modeNum != -1)
    {
        SetDisplayModeNum(screenId, modeNum);
        return 0;
    }
    
    for (int i = 0; i < modeCount; i++)
    {
        modes_D4 mode = modes[i];
        
        if (mode.derived.width != width)
            continue;
        if (mode.derived.height != height)
            continue;
        if (scaled && mode.derived.density != 2.0)
            continue;
        if (hz && mode.derived.freq != hz)
            continue;
        
        SetDisplayModeNum(screenId, i);
        return 0;
    }
    
    fprintf(stderr, "Could not find resolution %ix%i scaled=%i\n hz=%i\n", width, height, scaled, hz);
    return 1;
}

int setLayout(ScreenConfig screenConfigs[], size_t screenConfigsSize)
{
    CGDisplayConfigRef config;
    CGBeginDisplayConfiguration(&config);

    for (int i = 0; i < screenConfigsSize / sizeof(ScreenConfig); i++)
    {
        CGConfigureDisplayOrigin(config, screenConfigs[i].id, screenConfigs[i].x, screenConfigs[i].y);
    }
    
    return CGCompleteDisplayConfiguration(config, kCGConfigurePermanently);
}