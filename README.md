<a href="#"><img align="right" src="animation.gif"></a>

## displayplacer
macOS command line utility to configure multi-display resolutions and arrangements. Essentially XRandR for macOS.

#### Download:

Install via Homebrew with `brew tap jakehilborn/jakehilborn && brew install displayplacer` or visit the [releases](https://github.com/jakehilborn/displayplacer/releases) tab.

#### Usage:

Show current screen info and possible resolutions: `displayplacer list`

Apply screen config (hz & color_depth are optional): `displayplacer "id:<screenId> res:<width>x<height> hz:<num> color_depth:<num> scaling:<on/off> origin:(<x>,<y>) degree:<0/90/180/270>"`

Apply screen config using mode: `displayplacer "id:<screenId> mode:<modeNum> origin:(<x>,<y>) degree:<0/90/180/270>"`

Apply screen config with mirrored screens: `displayplacer "id:<mainScreenId>+<1stMirrorScreenId>+<2ndMirrorScreenId> res:<width>x<height> scaling:<on/off> origin:(<x>,<y>) degree:<0/90/180/270>"`

Silence errors per-screen using quiet: `displayplacer "id:<screenId> mode:<modeNum> origin:(<x>,<y>) degree:0 quiet:true"`

Disable a screen: `displayplacer "id:<screenId> enabled:false"`

#### Instructions:
1. Manually set rotations 1st*, resolutions 2nd, and arrangement 3rd. For extra resolutions and rotations read 'Notes' below.
    - Open System Preferences -> Displays
    - Choose desired screen rotations (use displayplacer for rotating internal MacBook screen).
    - Choose desired resolutions (use displayplacer for extra resolutions).
    - Drag the white bar to your desired primary screen.
    - Arrange screens as desired and/or enable mirroring. To enable partial mirroring hold the alt/option key and drag a display on top of another.
2. Use `displayplacer list` to print your current layout's args, so you can create profiles for scripting/hotkeys with [Automator](https://github.com/jakehilborn/displayplacer/issues/13), BetterTouchTool, etc.

#### Notes:
- *`displayplacer list` and system prefs only show resolutions for the screen's current rotation.
- Use an extra resolution shown in `displayplacer list` by executing `displayplacer "id:<screenId> mode:<modeNum>"`. Some of the resolutions listed do not work. If you select one, displayplacer will default to another working resolution.
- Rotate your internal MacBook screen by executing `displayplacer "id:<screenId> degree:<0/90/180/270>"`
- If you disable a screen, you may need to unplug/replug it to bring it back. However, on some setups, you can re-enable it with `displayplacer "id:<screenId> enabled:true"`
- The screen set to origin (0,0) will be set as the primary screen (white bar in system prefs).
- The first screenId in a mirroring set will be the 'Optimize for' screen in the system prefs. You can only choose resolutions for the 'Optimize for' screen. If there is a mirroring resolution you need but cannot find, try making a different screenId the first of the set.
- hz and color_depth are optional. If left out, the highest hz and then the highest color_depth will be auto applied.
- screenId is optional if there is only one screen. Rule of thumb is that displayplacer is expecting the entire profile config per screen though, so this may be buggy.
- Persistent screenIds usually stay the same. Contextual screenIds change when switching GPUs or when cables switch ports. It's recommended to use persistent screenIds. In some cases, you may need to use contextual screenIds since the modes list changes when macOS switches GPUs. Unfortunately, macOS also changes the persistent screenIds when there are race conditions from external screens waking up in non-determinisic order. Please search around in the GitHub Issues for conversation regarding this. Many people have written shell scripts to work around this issue.

#### Feedback:
Please create a GitHub Issue for any feedback, feature requests, bugs, Homebrew issues, etc. Happy to accept pull requests too!