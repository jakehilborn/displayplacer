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
