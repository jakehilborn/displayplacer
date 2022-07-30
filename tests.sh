#!/usr/bin/env bash

# set initial conf
conf_args=("id:A46D2F5E-487B-CC69-C588-ECFD519016E5 res:3840x2160 hz:60 color_depth:4 enabled:true scaling:off origin:(0,0) degree:0" "id:F466F621-B5FA-04A0-0800-CFA6C258DECD res:1440x900 color_depth:4 enabled:true scaling:on origin:(-1440,1469) degree:0" "id:582024A7-F302-4665-4100-CCDF549B7794 res:1440x2560 hz:59 color_depth:4 enabled:true scaling:off origin:(3840,-238) degree:90" "id:EA487A4B-D9B9-DDDD-91F8-F43E599B7E84 res:1920x1200 color_depth:4 enabled:true scaling:off origin:(960,-1200) degree:0")
conf_str=$(printf '"%s" ' "${conf_args[@]}") # Reduce array of strings to single string with each element wrapped in quotes. This unfortunately produces a trailing space at the end of the string.
./displayplacer "${conf_args[@]}"

# assert initial conf
current_conf_str=$(./displayplacer list | tail -1)
if [[ "$current_conf_str " != "displayplacer $conf_str" ]]; then
  echo "test_initial_conf failed"
  exit 1
fi

# enable mirroring
conf_args=("id:A46D2F5E-487B-CC69-C588-ECFD519016E5+EA487A4B-D9B9-DDDD-91F8-F43E599B7E84 res:3840x2160 hz:60 color_depth:4 enabled:true scaling:off origin:(0,0) degree:0" "id:F466F621-B5FA-04A0-0800-CFA6C258DECD res:1440x900 color_depth:4 enabled:true scaling:on origin:(-1440,1080) degree:0" "id:582024A7-F302-4665-4100-CCDF549B7794 res:1440x2560 hz:59 color_depth:4 enabled:true scaling:off origin:(3840,-238) degree:90")
conf_str=$(printf '"%s" ' "${conf_args[@]}") # Reduce array of strings to single string with each element wrapped in quotes. This unfortunately produces a trailing space at the end of the string.
./displayplacer "${conf_args[@]}"

# assert mirroring
current_conf_str=$(./displayplacer list | tail -1)
if [[ "$current_conf_str " != "displayplacer $conf_str" ]]; then
  echo "test_mirroring failed"
  exit 1
fi

# disable mirroring and set screen to disabled
conf_args=("id:A46D2F5E-487B-CC69-C588-ECFD519016E5 res:3840x2160 hz:60 color_depth:4 enabled:true scaling:off origin:(0,0) degree:0" "id:F466F621-B5FA-04A0-0800-CFA6C258DECD res:1440x900 color_depth:4 enabled:true scaling:on origin:(-1440,1080) degree:0" "id:582024A7-F302-4665-4100-CCDF549B7794 res:1440x2560 hz:59 color_depth:4 enabled:true scaling:off origin:(3840,-238) degree:90" "id:EA487A4B-D9B9-DDDD-91F8-F43E599B7E84 enabled:false")
conf_str=$(printf '"%s" ' "${conf_args[@]}") # Reduce array of strings to single string with each element wrapped in quotes. This unfortunately produces a trailing space at the end of the string.
./displayplacer "${conf_args[@]}"

# assert mirroring
current_conf_str=$(./displayplacer list | tail -1)
if [[ "$current_conf_str " != "displayplacer $conf_str" ]]; then
  echo "test_disable failed"
  exit 1
fi

# assert missing screen results in error
conf_args=("id:B46D2F5E-487B-CC69-C588-ECFD519016E5 res:3200x1800 hz:60 color_depth:4 enabled:true scaling:on origin:(0,0) degree:0" "id:F466F621-B5FA-04A0-0800-CFA6C258DECD res:1440x900 color_depth:4 enabled:true scaling:on origin:(-1440,1469) degree:0" "id:582024A7-F302-4665-4100-CCDF549B7794 res:1440x2560 hz:59 color_depth:4 enabled:true scaling:off origin:(3200,-1109) degree:90" "id:EA487A4B-D9B9-DDDD-91F8-F43E599B7E84 res:1920x1200 color_depth:4 enabled:true scaling:off origin:(425,-1200) degree:0")
conf_str=$(printf '"%s" ' "${conf_args[@]}") # Reduce array of strings to single string with each element wrapped in quotes. This unfortunately produces a trailing space at the end of the string.

if output=$(./displayplacer "${conf_args[@]}" 2>&1); then
  echo "test_missing_screen_exit_code failed"
  exit 1
fi

if [[ "$output" != "Unable to find screen B46D2F5E-487B-CC69-C588-ECFD519016E5 - skipping changes for that screen" ]]; then
  echo "test_missing_screen_error_message failed"
  exit 1
fi

# assert conf applied for non-missing screens
current_conf_str=$(./displayplacer list | tail -1)
if [[ "$current_conf_str" != 'displayplacer "id:A46D2F5E-487B-CC69-C588-ECFD519016E5 res:3840x2160 hz:60 color_depth:4 enabled:true scaling:off origin:(0,0) degree:0" "id:F466F621-B5FA-04A0-0800-CFA6C258DECD res:1440x900 color_depth:4 enabled:true scaling:on origin:(-1440,1469) degree:0" "id:582024A7-F302-4665-4100-CCDF549B7794 res:1440x2560 hz:59 color_depth:4 enabled:true scaling:off origin:(3840,-1109) degree:90" "id:EA487A4B-D9B9-DDDD-91F8-F43E599B7E84 res:1920x1200 color_depth:4 enabled:true scaling:off origin:(425,-1200) degree:0"' ]]; then
  echo "test_missing_screen_apply_other_screens failed"
  exit 1
fi

# assert missing screen with quiet mode does not result in error
conf_args=("id:B46D2F5E-487B-CC69-C588-ECFD519016E5 res:3200x1800 hz:60 color_depth:4 enabled:true scaling:on origin:(0,0) degree:0 quiet:true" "id:F466F621-B5FA-04A0-0800-CFA6C258DECD res:1440x900 color_depth:4 enabled:true scaling:on origin:(-1440,1469) degree:0" "id:582024A7-F302-4665-4100-CCDF549B7794 res:1080x1920 hz:60 color_depth:4 enabled:true scaling:off origin:(3840,-1109) degree:90" "id:EA487A4B-D9B9-DDDD-91F8-F43E599B7E84 res:1600x1000 color_depth:4 enabled:true scaling:off origin:(425,-1000) degree:0")
conf_str=$(printf '"%s" ' "${conf_args[@]}") # Reduce array of strings to single string with each element wrapped in quotes. This unfortunately produces a trailing space at the end of the string.

if ! output=$(./displayplacer "${conf_args[@]}" 2>&1); then
  echo "test_missing_screen_quiet_exit_code failed"
  exit 1
fi

if [[ "$output" == "Unable to find screen B46D2F5E-487B-CC69-C588-ECFD519016E5 - skipping changes for that screen" ]]; then
  echo "test_missing_screen_quiet_error_message failed"
  exit 1
fi

# assert conf applied for non-missing screens
current_conf_str=$(./displayplacer list | tail -1)
if [[ "$current_conf_str" != 'displayplacer "id:A46D2F5E-487B-CC69-C588-ECFD519016E5 res:3840x2160 hz:60 color_depth:4 enabled:true scaling:off origin:(0,0) degree:0" "id:F466F621-B5FA-04A0-0800-CFA6C258DECD res:1440x900 color_depth:4 enabled:true scaling:on origin:(-1440,1469) degree:0" "id:582024A7-F302-4665-4100-CCDF549B7794 res:1080x1920 hz:60 color_depth:4 enabled:true scaling:off origin:(3840,-1109) degree:90" "id:EA487A4B-D9B9-DDDD-91F8-F43E599B7E84 res:1600x1000 color_depth:4 enabled:true scaling:off origin:(425,-1000) degree:0"' ]]; then
  echo "test_missing_screen_quiet_apply_other_screens failed"
  exit 1
fi

# set initial conf
conf_args=("id:A46D2F5E-487B-CC69-C588-ECFD519016E5 res:3840x2160 hz:60 color_depth:4 enabled:true scaling:off origin:(0,0) degree:0" "id:F466F621-B5FA-04A0-0800-CFA6C258DECD res:1440x900 color_depth:4 enabled:true scaling:on origin:(-1440,1469) degree:0" "id:582024A7-F302-4665-4100-CCDF549B7794 res:1440x2560 hz:59 color_depth:4 enabled:true scaling:off origin:(3840,-238) degree:90" "id:EA487A4B-D9B9-DDDD-91F8-F43E599B7E84 res:1920x1200 color_depth:4 enabled:true scaling:off origin:(960,-1200) degree:0")
conf_str=$(printf '"%s" ' "${conf_args[@]}") # Reduce array of strings to single string with each element wrapped in quotes. This unfortunately produces a trailing space at the end of the string.
./displayplacer "${conf_args[@]}"

# assert initial conf
current_conf_str=$(./displayplacer list | tail -1)
if [[ "$current_conf_str " != "displayplacer $conf_str" ]]; then
  echo "test_initial_conf failed"
  exit 1
fi

# Set only one display active (others are disabled or mirrored)
conf_args=("id:A46D2F5E-487B-CC69-C588-ECFD519016E5+EA487A4B-D9B9-DDDD-91F8-F43E599B7E84 res:1920x1200 hz:60 color_depth:4 enabled:true scaling:off origin:(0,0) degree:0" "id:F466F621-B5FA-04A0-0800-CFA6C258DECD enabled:false" "id:582024A7-F302-4665-4100-CCDF549B7794 enabled:false")
conf_str=$(printf '"%s" ' "${conf_args[@]}") # Reduce array of strings to single string with each element wrapped in quotes. This unfortunately produces a trailing space at the end of the string.
./displayplacer "${conf_args[@]}"

# assert all but one screen disabled
current_conf_str=$(./displayplacer list | tail -1)
if [[ "$current_conf_str " != "displayplacer $conf_str" ]]; then
  echo "test_one_active_screen failed"
  exit 1
fi

# set config for single screen without passing in id arg
conf_args=("res:1920x1080")
conf_str=$(printf '"%s" ' "${conf_args[@]}") # Reduce array of strings to single string with each element wrapped in quotes. This unfortunately produces a trailing space at the end of the string.
./displayplacer "${conf_args[@]}"

# assert resolution set for single screen
current_conf_str=$(./displayplacer list | tail -1)
if [[ "$current_conf_str" != 'displayplacer "id:A46D2F5E-487B-CC69-C588-ECFD519016E5+EA487A4B-D9B9-DDDD-91F8-F43E599B7E84 res:1920x1080 hz:60 color_depth:8 enabled:true scaling:off origin:(0,0) degree:0" "id:F466F621-B5FA-04A0-0800-CFA6C258DECD enabled:false" "id:582024A7-F302-4665-4100-CCDF549B7794 enabled:false"' ]]; then
  echo "test_single_screen_default_id failed"
  exit 1
fi

# set initial conf
conf_args=("id:A46D2F5E-487B-CC69-C588-ECFD519016E5 res:3840x2160 hz:60 color_depth:4 enabled:true scaling:off origin:(0,0) degree:0" "id:F466F621-B5FA-04A0-0800-CFA6C258DECD res:1440x900 color_depth:4 enabled:true scaling:on origin:(-1440,1469) degree:0" "id:582024A7-F302-4665-4100-CCDF549B7794 res:1440x2560 hz:59 color_depth:4 enabled:true scaling:off origin:(3840,-238) degree:90" "id:EA487A4B-D9B9-DDDD-91F8-F43E599B7E84 res:1920x1200 color_depth:4 enabled:true scaling:off origin:(960,-1200) degree:0")
conf_str=$(printf '"%s" ' "${conf_args[@]}") # Reduce array of strings to single string with each element wrapped in quotes. This unfortunately produces a trailing space at the end of the string.
./displayplacer "${conf_args[@]}"

# assert initial conf
current_conf_str=$(./displayplacer list | tail -1)
if [[ "$current_conf_str " != "displayplacer $conf_str" ]]; then
  echo "test_initial_conf failed"
  exit 1
fi
