## displayplacer
macOS command line utility to configure multi-display resolutions and arrangements. Essentially XRandR for macOS.

Install via Homebrew with `brew tap jakehilborn/jakehilborn && brew install displayplacer` or visit the [releases](https://github.com/jakehilborn/displayplacer/releases) tab.

#### Usage:

Show current screen info and possible resolutions: `displayplacer list`

Screen config (hz is optional): `displayplacer "id:<screenId> res:<width>x<height>x<hz> scaling:<on/off> origin:(<x>,<y>) degree:<0/90/180/270>"`

Screen config using mode: `displayplacer "id:<screenId> mode:<modeNum> origin:(<x>,<y>) degree:<0/90/180/270>"`

Set layout with a mirrored screen: `displayplacer "id:<mainScreenId>+<mirrorScreenId>+<mirrorScreenId> res:<width>x<height>x<hz> scaling:<on/off> origin:(<x>,<y>) degree:<0/90/180/270>"`

Example w/ all features: `displayplacer "id:69731906+862792382 res:1440x900 scaling:on origin:(0,0) degree:0" "id:374164677 res:768x1360x60 scaling:off origin:(1440,0) degree:90"`

#### Instructions:
1. Manually set rotations 1st*, resolutions 2nd, and arrangement 3rd. For extra resolutions and rotations read 'Notes' below.
    - Open System Preferences -> Displays
    - Choose desired screen rotations (use displayplacer for rotating internal MacBook screen).
    - Choose desired resolutions (use displayplacer for extra resolutions).
    - Drag the white bar to your desired primary screen.
    - Arrange screens as desired and/or enable mirroring.
    - To enable partial mirroring hold the alt/option key and drag a display on top of another.
2. Use `displayplacer list` to print your current layout's args so you can create profiles for scripting/hotkeys with Automator, BetterTouchTool, etc.
   
#### Notes:
- *`displayplacer list` and system prefs only show resolutions for the screen's current rotation.
- ScreenIDs change when cables are plugged into different ports. To ensure screenIDs match your saved profiles, always plug cables into the same ports.
- Use an extra resolution shown in `displayplacer list` by executing `displayplacer "id:<screenId> mode:<modeNum>"`
- Rotate your internal MacBook screen by executing `displayplacer "id:<screenId> degree:<0/90/180/270>"`
- The screen set to origin (0,0) will be set as the primary screen (white bar in system prefs).
- The first screenId in a mirroring set will be the 'Optimize for' screen in the system prefs. You can only choose resolutions for the 'Optimize for' screen. If there is a mirroring resolution you need but cannot find, try making a different screenId the first of the set.

#### Feedback:
Please create a GitHub Issue for any feedback, feature requests, bugs, Homebrew issues, etc. Happy to accept pull requests too!
