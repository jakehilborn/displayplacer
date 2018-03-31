# displayplacer
macOS command line utility to configure multi-display resolutions and arrangements. Essentially XRandR for macOS.

[<b>Download latest executable binary</b>](displayplacer)

Usage:
   
    Show current screen info and possible resolutions: displayplacer list
    
    Set layout: displayplacer 'id:<screenId> res:<width>x<height>x<hz> scaling:<on/off> origin:(<x>,<y>) degree:<0/90/180/270>'
    
    Set layout using mode: displayplacer 'id:<screenId> mode:<modeNum> origin:(<x>,<y>) degree:<0/90/180/270>'
    
    example: displayplacer 'id:1124216227 res:3840x2160x60 scaling:off origin:(0,0) degree:0' 'id:69731906 res:1440x900 scaling:on origin:(-1440,1260) degree:0' 'id:1007295109 res:1920x1200 scaling:off origin:(960,-2160) degree:0'

Instructions:
   1. Use displayplacer or the system settings to choose your screen resolutions and orientations.
   2. Use System Preferences -> Displays -> Arrangement to arrange the screens to your liking.
   3. Use 'displayplacer list' to get the info about your current layout so you can create profiles for scripting/hotkeys.
   
Notes:
   1. The 'list' option shows resolutions for the screen's current orientation.
   2. The screen set to origin (0,0) will be set as the primary screen.
