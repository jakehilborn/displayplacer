# displayplacer
macOS command line utility to configure multi-display resolutions and arrangements. Essentially XRandR for macOS.

[<b>Download latest executable binary</b>](displayplacer)

#### Usage:

Show current screen info and possible resolutions: `displayplacer list`

Screen config (hz is optional): `displayplacer 'id:<screenId> res:<width>x<height>x<hz> scaling:<on/off> origin:(<x>,<y>) degree:<0/90/180/270>'`

Screen config using mode: `displayplacer 'id:<screenId> mode:<modeNum> origin:(<x>,<y>) degree:<0/90/180/270>'`

Set layout with a mirrored screen: `displayplacer 'id:<mainScreenId>+<mirrorScreenId>+<mirrorScreenId> res:<width>x<height>x<hz> scaling:<on/off> origin:(<x>,<y>) degree:<0/90/180/270>'`

Example w/ all features: `displayplacer 'id:69731906+862792382 res:1440x900 scaling:on origin:(0,0) degree:0' 'id:374164677 res:768x1360x60 scaling:off origin:(1440,0) degree:90'`

#### Instructions:
1. Use the system settings or displayplacer to choose your screen orientations first* and resolutions second.
2. Open System Preferences -> Displays -> Arrangement:
    - Drag the white bar to your desired primary screen.
    - Arrange screens as desired or enable mirroring.
    - To enable partial mirroring hold the option key and drag a display on top of another.
3. Use `displayplacer list` to get the info about your current layout so you can create profiles for scripting/hotkeys.
   
#### Notes:
- *`displayplacer list` only shows resolutions for the screen's current orientation.
- The screen set to origin (0,0) will be set as the primary screen (white bar in system prefs).
- The first screenId in a mirroring set will be the 'Optimize for' screen in the system prefs. You can only choose resolutions for the 'Optimize for' screen. If there is a mirroring resolution you need but cannot find, try making a different screenId to first of the set.
