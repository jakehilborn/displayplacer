//Thanks to Alin Panaitiu for this header file to access the MonitorPanel framework: https://github.com/alin23/mac-utils

#import <CoreGraphics/CoreGraphics.h>
#import <CommonCrypto/CommonDigest.h>
#import <Foundation/Foundation.h>
#import <MonitorPanel/MPDisplayPreset.h>
#import <MonitorPanel/MPDisplay.h>
#import <MonitorPanel/MPDisplayMgr.h>
#import <MonitorPanel/MPDisplayMode.h>

CFDictionaryRef CoreDisplay_DisplayCreateInfoDictionary(CGDirectDisplayID);
extern int SLSMainConnectionID(void);
CGError SLSGetDisplayList(uint32_t maxDisplays, CGDirectDisplayID *activeDisplays, uint32_t *displayCount);
