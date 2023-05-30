#import <Foundation/Foundation.h>
#include <ApplicationServices/ApplicationServices.h>

const int kMaxDisplays = 16;

@interface MPDisplayMode : NSObject

+ (id)modeWithDescription:(struct _CGSDisplayModeDescription *)arg1 forDisplay:(id)arg2;
@property(readonly) NSString *refreshString;
@property(readonly) NSString *resolutionString;
- (id)resolutionFormat;
@property(readonly) BOOL isSafeMode;
- (id)description;
- (void)getModeDescription:(struct _CGSDisplayModeDescription *)arg1;
- (BOOL)modeResolutionMatches:(id)arg1;
- (BOOL)resolutionMatches:(struct _CGSDisplayModeDescription *)arg1;
@property(readonly) unsigned int tvModeEquiv;
@property(readonly) unsigned int tvMode;
@property(readonly) BOOL isTVMode;
@property(readonly) BOOL isSimulscan;
@property(readonly) BOOL isInterlaced;
@property(readonly) BOOL isNativeMode;
@property(readonly) BOOL isDefaultMode;
@property(readonly) BOOL isStretched;
@property(readonly) BOOL isUserVisible;
@property(readonly) BOOL isHiDPI;
@property(readonly) BOOL isRetina;
@property(readonly) NSNumber *scanRate;
@property(readonly) int roundedScanRate;
@property(readonly) float scale;
@property(readonly) float aspectRatio;
@property(readonly) int fixPtRefreshRate;
@property(readonly) int refreshRate;
@property(readonly) int dotsPerInch;
@property(readonly) int vertDPI;
@property(readonly) int horizDPI;
@property(readonly) int pixelsHigh;
@property(readonly) int pixelsWide;
@property(readonly) int height;
@property(readonly) int width;
@property(readonly) int modeNumber;
@property(readonly) struct _CGSDisplayModeDescription *modeDescription;
- (void)dealloc;
- (id)initWithModeDescription:(struct _CGSDisplayModeDescription *)arg1 forDisplay:(id)arg2;

@end

@interface MPDisplay : NSObject

@property(readonly) BOOL hasRotationSensor; // @synthesize hasRotationSensor=_hasRotationSensor;
@property(retain) MPDisplayMode *defaultMode; // @synthesize defaultMode=_defaultMode;
@property(retain) MPDisplayMode *nativeMode; // @synthesize nativeMode=_nativeMode;
@property(retain) MPDisplayMode *currentMode; // @synthesize currentMode=_currentMode;
@property(readonly) BOOL hasZeroRate; // @synthesize hasZeroRate=_hasZeroRate;
@property(readonly) BOOL hasMultipleRates; // @synthesize hasMultipleRates=_hasMultipleRates;
@property(readonly) BOOL isSidecarDisplay; // @synthesize isSidecarDisplay=_isSidecarDisplay;
@property(readonly) BOOL isAirPlayDisplay; // @synthesize isAirPlayDisplay=_isAirPlayDisplay;
@property(readonly) BOOL isProjector; // @synthesize isProjector=_isProjector;
@property(readonly) BOOL is4K; // @synthesize is4K=_is4K;
@property(readonly) BOOL isTV; // @synthesize isTV=_isTV;
@property(readonly) BOOL isMirrorMaster; // @synthesize isMirrorMaster=_isMirrorMaster;
@property(readonly) BOOL isMirrored; // @synthesize isMirrored=_isMirrored;
@property(readonly) BOOL isBuiltIn; // @synthesize isBuiltIn=_isBuiltIn;
@property(readonly) BOOL isHiDPI; // @synthesize isHiDPI=_isHiDPI;
@property(readonly) BOOL hasTVModes; // @synthesize hasTVModes=_hasTVModes;
@property(readonly) BOOL hasSimulscan; // @synthesize hasSimulscan=_hasSimulscan;
@property(readonly) BOOL hasSafeMode; // @synthesize hasSafeMode=_hasSafeMode;
@property(readonly) BOOL isSmartDisplay; // @synthesize isSmartDisplay=_isSmartDisplay;
@property(nonatomic) int orientation; // @synthesize orientation=_orientation;
@property unsigned int userFlags; // @synthesize userFlags=_userFlags;
@property(readonly) int aliasID; // @synthesize aliasID=_aliasID;
@property(readonly) int displayID; // @synthesize displayID=_displayID;
- (BOOL)setActivePreset:(id)arg1;
@property(readonly) NSArray *presets;
@property(readonly) BOOL hasPresets;
- (void)buildPresetsList;
- (void)_loadPreviewIconFromServiceDictionary:(id)arg1;
- (id)_imageAndRect:(struct CGRect *)arg1 fromDictionary:(id)arg2 forOrientation:(long long)arg3;
- (id)_iconAtPath:(id)arg1;
@property(readonly) struct CGRect displayResolutionPreviewRect;
@property(readonly) NSImage *displayResolutionPreviewIcon;
@property(readonly) NSImage *displayIcon;
@property(readonly) NSUUID *uuid;
@property(readonly) struct CGRect hardwareBounds;
@property(readonly) int mirrorMasterDisplayID;
- (void)setPreferHDRModes:(BOOL)arg1;
@property(readonly) BOOL preferHDRModes;
@property(readonly) BOOL hasHDRModes;
@property(readonly) BOOL isForcedToMirror;
- (void)setMirrorMaster:(BOOL)arg1;
- (void)setMirrorMode:(id)arg1;
- (void)setMirrorModeNumber:(int)arg1;
- (int)setMode:(id)arg1;
- (int)setModeNumber:(int)arg1;
- (BOOL)isModeNative:(id)arg1;
- (BOOL)inDefaultMode;
- (id)modesForResolution:(id)arg1;
@property(readonly) NSArray *scanRateStrings;
@property(readonly) NSArray *scanRates;
- (id)allModes;
- (id)userModes;
@property BOOL bestForVideoMode;
- (BOOL)supportsBestForVideoMode;
@property int underscan;
@property(readonly) int maxUnderscan;
@property(readonly) int minUnderscan;
- (BOOL)supportsUnderscan;
@property BOOL overscanEnabled;
- (BOOL)supportsOverscan;
- (BOOL)canChangeOrientation;
- (BOOL)hasMultipleScanRates;
@property(readonly) struct CGRect displayBounds;
- (id)modeWithNumber:(int)arg1;
- (id)modeMatchingResolutionOfMode:(id)arg1 withScanRate:(id)arg2;
- (id)modesMatchingResolutionOfMode:(id)arg1;
- (id)modesOfType:(unsigned long long)arg1;
- (id)resolutionsOfType:(unsigned long long)arg1;
- (void)refreshModes;
- (void)refreshResolutions;
- (void)refreshResolutions:(id)arg1 usingModeList:(id)arg2;
- (id)scanRateForString:(id)arg1;
- (id)stringForScanRate:(id)arg1;
- (void)refreshScanRates;
- (void)determineTrimmedModeList;
- (void)bucketizeDisplayModes;
- (void)addMatchingModesToTrimmed;
- (void)addTVModesToPreferred;
- (BOOL)isAlias:(int)arg1;
- (id)multiscanModesForMode:(id)arg1;
@property(readonly) BOOL hasMenuBar;
@property(readonly) BOOL isAppleProDisplay;
@property(readonly) BOOL isBuiltInRetina;
@property(readonly) NSString *titleName;
@property(retain, nonatomic) NSString *displayName;
- (void)dealloc;
- (id)initWithCGSDisplayID:(int)arg1;

@end

int main(int argc, char *argv[]) {
    CGDirectDisplayID display[kMaxDisplays];
    CGDisplayCount numDisplays;
    CGDisplayErr err;
    err = CGGetOnlineDisplayList(kMaxDisplays, display, &numDisplays);
    if (err != CGDisplayNoErr) {
        printf("cannot get list of displays (error %d)\n", err);
        return -1;
    }

    for (CGDisplayCount i = 0; i < numDisplays; ++i) {
        CGDirectDisplayID dspy = display[i];
        CGDisplayModeRef mode = CGDisplayCopyDisplayMode(dspy);
        if (mode == NULL)
            continue;

        printf("display %d (id %d): ", i, dspy);
        if (CGDisplayIsMain(dspy))
            printf("main, ");
        printf("%sactive, %s, %sline, %s%s",
               CGDisplayIsActive(dspy) ? "" : "in",
               CGDisplayIsAsleep(dspy) ? "asleep" : "awake",
               CGDisplayIsOnline(dspy) ? "on" : "off",
               CGDisplayIsBuiltin(dspy) ? "built-in" : "external",
               CGDisplayIsStereo(dspy) ? ", stereo" : "");
        printf(", ID 0x%x\n", (unsigned int)dspy);

        CGRect bounds = CGDisplayBounds(dspy);
        printf("\tresolution %.0f x %.0f pt",
               bounds.size.width, bounds.size.height);
        printf(" (%zu x %zu px)",
               CGDisplayModeGetPixelWidth(mode),
               CGDisplayModeGetPixelHeight(mode));
        double refreshRate = CGDisplayModeGetRefreshRate(mode);
        if (refreshRate != 0)
            printf(" @ %.1f Hz", refreshRate);
        printf(", origin (%.0f, %.0f)\n",
               bounds.origin.x, bounds.origin.y);
        CGSize size = CGDisplayScreenSize(dspy);
        printf("\tphysical size %.0f x %.0f mm",
               size.width, size.height);
        double rotation = CGDisplayRotation(dspy);
        if (rotation)
            printf(", rotated %.0f°", rotation);
        printf("\n\tIOKit flags 0x%x",
               CGDisplayModeGetIOFlags(mode));
        printf("; IOKit display mode ID 0x%x\n",
               CGDisplayModeGetIODisplayModeID(mode));
        if (CGDisplayIsInMirrorSet(dspy)) {
            CGDirectDisplayID mirrorsDisplay = CGDisplayMirrorsDisplay(dspy);
            if (mirrorsDisplay == kCGNullDirectDisplay)
                printf("\tmirrored\n");
            else
                printf("\tmirrors display ID 0x%x\n", mirrorsDisplay);
        }

        printf("\t%susable for desktop GUI%s\n",
               CGDisplayModeIsUsableForDesktopGUI(mode) ? "" : "not ",
               CGDisplayUsesOpenGLAcceleration(dspy) ?
               ", uses OpenGL acceleration" : "");
        CGDisplayModeRelease(mode);

        MPDisplay* mpdisplay = [[MPDisplay alloc] initWithCGSDisplayID:dspy];
        printf("Rotation: %d", [mpdisplay orientation]);
        [mpdisplay setOrientation: 90];
        break;


    }
}