<a href="#"><img align="right" src="animation.gif"></a>

## displayplacer
macOS command line utility to configure multi-display resolutions and arrangements. Essentially XRandR for macOS.

Install via Homebrew with `brew tap jakehilborn/jakehilborn && brew install displayplacer` or visit the [releases](https://github.com/jakehilborn/displayplacer/releases) tab.

#### Usage:

Show current screen info and possible resolutions: `displayplacer list`

Apply screen config (hz & color_depth are optional): `displayplacer "id:<screenId> res:<width>x<height> hz:<num> color_depth:<num> scaling:<on/off> origin:(<x>,<y>) degree:<0/90/180/270>"`

Apply screen config using mode: `displayplacer "id:<screenId> mode:<modeNum> origin:(<x>,<y>) degree:<0/90/180/270>"`

Apply screen config with mirrored screens: `displayplacer "id:<mainScreenId>+<1stMirrorScreenId>+<2ndMirrorScreenId> res:<width>x<height> scaling:<on/off> origin:(<x>,<y>) degree:<0/90/180/270>"`

Example w/ all features: `displayplacer "id:18173D22-3EC6-E735-EEB4-B003BF681F30+F466F621-B5FA-04A0-0800-CFA6C258DECD res:1440x900 scaling:on origin:(0,0) degree:0" "id:4C405A05-8798-553B-3550-F93E7A7722BB res:768x1360 hz:60 color_depth:8 scaling:off origin:(1440,0) degree:90" "id:A46D2F5E-487B-CC69-C588-ECFD519016E5 mode:3 origin:(-1440,0) degree:270"`

#### Instructions:
1. Manually set rotations 1st*, resolutions 2nd, and arrangement 3rd. For extra resolutions and rotations read 'Notes' below.
    - Open System Preferences -> Displays
    - Choose desired screen rotations (use displayplacer for rotating internal MacBook screen).
    - Choose desired resolutions (use displayplacer for extra resolutions).
    - Drag the white bar to your desired primary screen.
    - Arrange screens as desired and/or enable mirroring. To enable partial mirroring hold the alt/option key and drag a display on top of another.
2. Use `displayplacer list` to print your current layout's args so you can create profiles for scripting/hotkeys with Automator, BetterTouchTool, etc.

#### Notes:
- *`displayplacer list` and system prefs only show resolutions for the screen's current rotation.
- Use an extra resolution shown in `displayplacer list` by executing `displayplacer "id:<screenId> mode:<modeNum>"`
- Rotate your internal MacBook screen by executing `displayplacer "id:<screenId> degree:<0/90/180/270>"`
- The screen set to origin (0,0) will be set as the primary screen (white bar in system prefs).
- The first screenId in a mirroring set will be the 'Optimize for' screen in the system prefs. You can only choose resolutions for the 'Optimize for' screen. If there is a mirroring resolution you need but cannot find, try making a different screenId the first of the set.
- hz and color_depth are optional. If left out, the highest hz and then the highest color_depth will be auto applied.
- Persistent screen ids always stay the same. Contextual screen ids change when switching GPUs or when switching ports. It's recommended to use persistent screen ids. In some rare cases, you may need to use contextual screen ids since the modes list changes when macOS switches GPUs.

#### Feedback:
Please create a GitHub Issue for any feedback, feature requests, bugs, Homebrew issues, etc. Happy to accept pull requests too!
