#!/usr/bin/env python3
import subprocess


def main():
    print('These tests only work on the original dev\'s multi monitor setup!')
    print('These tests only work on the original dev\'s multi monitor setup!')
    print('These tests only work on the original dev\'s multi monitor setup!')
    print('')

    intel()
    # apple()


def intel():
    reset_conf_intel()

    print('Test profile that did not work in version 1.2')
    test('set_problematic_profile',
         '"id:34686E82-0CED-DF86-AFC7-AA1A8EB5CFC0+A46D2F5E-487B-CC69-C588-ECFD519016E5 res:756x1344 hz:59 color_depth:8 enabled:true scaling:off origin:(0,0) degree:90" "id:F466F621-B5FA-04A0-0800-CFA6C258DECD res:1440x900 color_depth:4 enabled:true scaling:on origin:(-1440,1344) degree:0" "id:EA487A4B-D9B9-DDDD-91F8-F43E599B7E84 res:1920x1200 color_depth:4 enabled:true scaling:off origin:(-1920,-962) degree:0"',
         'match_input',
         0,
         None)
    reset_conf_intel(expected_code=None)  # resetting after this profile used to not work. The screen rotations do work, but they time out. This is good enough, so we ignore the error code.

    print('Test disabling screen')
    test('enable_mirroring',
         '"id:A46D2F5E-487B-CC69-C588-ECFD519016E5+EA487A4B-D9B9-DDDD-91F8-F43E599B7E84 res:3840x2160 hz:60 color_depth:8 enabled:true scaling:off origin:(0,0) degree:0" "id:F466F621-B5FA-04A0-0800-CFA6C258DECD res:1440x900 color_depth:4 enabled:true scaling:on origin:(-1440,1080) degree:0" "id:34686E82-0CED-DF86-AFC7-AA1A8EB5CFC0 res:1440x2560 hz:59 color_depth:8 enabled:true scaling:off origin:(3840,-238) degree:90"',
         'match_input',
         0,
         None)
    test('disable_mirroring_while_setting_a_screen_to_disabled',
         '"id:A46D2F5E-487B-CC69-C588-ECFD519016E5 res:3840x2160 hz:60 color_depth:8 enabled:true scaling:off origin:(0,0) degree:0" "id:F466F621-B5FA-04A0-0800-CFA6C258DECD res:1440x900 color_depth:4 enabled:true scaling:on origin:(-1440,1080) degree:0" "id:34686E82-0CED-DF86-AFC7-AA1A8EB5CFC0 res:1440x2560 hz:59 color_depth:8 enabled:true scaling:off origin:(3840,-238) degree:90" "id:EA487A4B-D9B9-DDDD-91F8-F43E599B7E84 enabled:false"',
         'match_input',
         0,
         None)
    reset_conf_intel()

    print('Test missing screen')
    test('test_missing_screen_partial_error_conf_applied_for_non_missing_screens',
         '"id:B46D2F5E-487B-CC69-C588-ECFD519016E5 res:3200x1800 hz:60 color_depth:8 enabled:true scaling:on origin:(0,0) degree:0" "id:F466F621-B5FA-04A0-0800-CFA6C258DECD res:1440x900 color_depth:4 enabled:true scaling:on origin:(-1440,1469) degree:0" "id:34686E82-0CED-DF86-AFC7-AA1A8EB5CFC0 res:1440x2560 hz:59 color_depth:8 enabled:true scaling:off origin:(3200,-1109) degree:90" "id:EA487A4B-D9B9-DDDD-91F8-F43E599B7E84 res:1920x1200 color_depth:8 enabled:true scaling:off origin:(425,-1200) degree:0"',
         '"id:A46D2F5E-487B-CC69-C588-ECFD519016E5 res:3840x2160 hz:60 color_depth:8 enabled:true scaling:off origin:(0,0) degree:0" "id:F466F621-B5FA-04A0-0800-CFA6C258DECD res:1440x900 color_depth:4 enabled:true scaling:on origin:(-1440,1469) degree:0" "id:34686E82-0CED-DF86-AFC7-AA1A8EB5CFC0 res:1440x2560 hz:59 color_depth:8 enabled:true scaling:off origin:(3840,-1109) degree:90" "id:EA487A4B-D9B9-DDDD-91F8-F43E599B7E84 res:1920x1200 color_depth:8 enabled:true scaling:off origin:(425,-1200) degree:0"',
         1,
         'Unable to find screen B46D2F5E-487B-CC69-C588-ECFD519016E5 - skipping changes for that screen')
    reset_conf_intel()

    print('Test missing resolution for persistent, contextual, and serial screen ids')
    test('test_missing_resolution_persistent_screen_id',
         '"id:34686E82-0CED-DF86-AFC7-AA1A8EB5CFC0 res:1440x2561 hz:59 color_depth:4 enabled:true scaling:off origin:(3840,-238) degree:90"',
         None,
         1,
         'Screen ID 34686E82-0CED-DF86-AFC7-AA1A8EB5CFC0: could not find res:1440x2561 hz:59 color_depth:4 scaling:off')
    test('test_missing_resolution_contextual_screen_id',
         '"id:722521364 res:1440x2561 hz:59 color_depth:4 enabled:true scaling:off origin:(3840,-238) degree:90"',
         None,
         1,
         'Screen ID 722521364: could not find res:1440x2561 hz:59 color_depth:4 scaling:off')
    test('test_missing_resolution_serial_screen_id',
         '"id:s1161315916 res:1440x2561 hz:59 color_depth:4 enabled:true scaling:off origin:(3840,-238) degree:90"',
         None,
         1,
         'Screen ID s1161315916: could not find res:1440x2561 hz:59 color_depth:4 scaling:off')
    reset_conf_intel()

    print('Test missing screen with quiet mode')
    test('test_missing_screen_quiet_mode_suppresses_error',
         '"id:B46D2F5E-487B-CC69-C588-ECFD519016E5 res:3200x1800 hz:60 color_depth:8 enabled:true scaling:on origin:(0,0) degree:0 quiet:true" "id:F466F621-B5FA-04A0-0800-CFA6C258DECD res:1440x900 color_depth:4 enabled:true scaling:on origin:(-1440,1469) degree:0" "id:34686E82-0CED-DF86-AFC7-AA1A8EB5CFC0 res:1080x1920 hz:60 color_depth:8 enabled:true scaling:off origin:(3840,-1109) degree:90" "id:EA487A4B-D9B9-DDDD-91F8-F43E599B7E84 res:1600x1000 color_depth:8 enabled:true scaling:off origin:(425,-1000) degree:0"',
         '"id:A46D2F5E-487B-CC69-C588-ECFD519016E5 res:3840x2160 hz:60 color_depth:8 enabled:true scaling:off origin:(0,0) degree:0" "id:F466F621-B5FA-04A0-0800-CFA6C258DECD res:1440x900 color_depth:4 enabled:true scaling:on origin:(-1440,1469) degree:0" "id:34686E82-0CED-DF86-AFC7-AA1A8EB5CFC0 res:1080x1920 hz:60 color_depth:8 enabled:true scaling:off origin:(3840,-1109) degree:90" "id:EA487A4B-D9B9-DDDD-91F8-F43E599B7E84 res:1600x1000 color_depth:8 enabled:true scaling:off origin:(425,-1000) degree:0"',
         0,
         None)
    reset_conf_intel()

    print('Test id not required if only one screen active')
    test('test_set_one_display_active_others_are_disabled',
         '"id:A46D2F5E-487B-CC69-C588-ECFD519016E5 res:1920x1200 hz:60 color_depth:8 enabled:true scaling:off origin:(0,0) degree:0" "id:F466F621-B5FA-04A0-0800-CFA6C258DECD enabled:false" "id:34686E82-0CED-DF86-AFC7-AA1A8EB5CFC0 enabled:false" "id:EA487A4B-D9B9-DDDD-91F8-F43E599B7E84 enabled:false"',
         'match_input',
         0,
         None)
    test('test_set_conf_for_single_screen_without_passing_in_id',
         'res:1920x1080',
         '"id:A46D2F5E-487B-CC69-C588-ECFD519016E5 res:1920x1080 hz:60 color_depth:8 enabled:true scaling:off origin:(0,0) degree:0" "id:F466F621-B5FA-04A0-0800-CFA6C258DECD enabled:false" "id:34686E82-0CED-DF86-AFC7-AA1A8EB5CFC0 enabled:false" "id:EA487A4B-D9B9-DDDD-91F8-F43E599B7E84 enabled:false"',
         0,
         None)
    reset_conf_intel()

    print('Test disabling and enabling a mirrored screen')
    test('test_set_mirror',
         '"id:A46D2F5E-487B-CC69-C588-ECFD519016E5+F466F621-B5FA-04A0-0800-CFA6C258DECD+EA487A4B-D9B9-DDDD-91F8-F43E599B7E84 res:1440x900 hz:60 color_depth:8 enabled:true scaling:on origin:(0,0) degree:0" "id:34686E82-0CED-DF86-AFC7-AA1A8EB5CFC0 res:1440x2560 hz:59 color_depth:8 enabled:true scaling:off origin:(1440,-238) degree:90"',
         'match_input',
         0,
         None)
    test('test_disable_mirror_screen',
         '"id:A46D2F5E-487B-CC69-C588-ECFD519016E5+F466F621-B5FA-04A0-0800-CFA6C258DECD res:1440x900 hz:60 color_depth:8 enabled:true scaling:on origin:(0,0) degree:0" "id:34686E82-0CED-DF86-AFC7-AA1A8EB5CFC0 res:1440x2560 hz:59 color_depth:8 enabled:true scaling:off origin:(1440,-238) degree:90" "id:EA487A4B-D9B9-DDDD-91F8-F43E599B7E84 enabled:false"',
         'match_input',
         0,
         None)
    test(step='test_enable_mirror_screen_and_rotate_180',
         conf='"id:A46D2F5E-487B-CC69-C588-ECFD519016E5+F466F621-B5FA-04A0-0800-CFA6C258DECD+EA487A4B-D9B9-DDDD-91F8-F43E599B7E84 res:1440x900 hz:60 color_depth:8 enabled:true scaling:on origin:(0,0) degree:180" "id:34686E82-0CED-DF86-AFC7-AA1A8EB5CFC0 res:1440x2560 hz:59 color_depth:8 enabled:true scaling:off origin:(1440,-238) degree:90"',
         expected_conf='match_input',
         expected_code=None,  # The screen rotations do work, but they time out. This is good enough so we ignore the error code.
         expected_error=None)
    reset_conf_intel(expected_code=None)  # The screen rotations do work, but they time out. This is good enough so we ignore the error code.

    # TODO this is a known bug, but low priority. The code currently will disable mirroring for all screens before applying config. When the user does not pass in the fully described profile, we disable the mirroring but do not have the context to put the mirroring back. This could be fixed by internally calling `printCurrentProfile()` and using that to fill in the missing context for the current profile.
    # print('Test id not required if only one mirroring set active')
    # test('test_set_one_display_active_others_are_disabled_or_mirrored',
    #      '"id:A46D2F5E-487B-CC69-C588-ECFD519016E5+EA487A4B-D9B9-DDDD-91F8-F43E599B7E84 res:1920x1200 hz:60 color_depth:8 enabled:true scaling:off origin:(0,0) degree:0" "id:F466F621-B5FA-04A0-0800-CFA6C258DECD enabled:false" "id:34686E82-0CED-DF86-AFC7-AA1A8EB5CFC0 enabled:false"',
    #      'match_input',
    #      0,
    #      None)
    # test('test_set_conf_for_single_screen_without_passing_in_id',
    #      'res:1920x1080',
    #      '"id:A46D2F5E-487B-CC69-C588-ECFD519016E5+EA487A4B-D9B9-DDDD-91F8-F43E599B7E84 res:1920x1080 hz:60 color_depth:8 enabled:true scaling:off origin:(0,0) degree:0" "id:F466F621-B5FA-04A0-0800-CFA6C258DECD enabled:false" "id:34686E82-0CED-DF86-AFC7-AA1A8EB5CFC0 enabled:false"',
    #      0,
    #      None)
    # reset_conf_intel()

def apple():
    reset_conf_apple()

    print('Test profile that did not work in version 1.2')
    test('set_problematic_profile',
         '"id:37D8832A-2D66-02CA-B9F7-8F30A301B230 res:1728x1080 hz:120 color_depth:8 enabled:true scaling:on origin:(0,0) degree:0" "id:7746278E-D1CF-4842-8286-26BDEBFD9F1A+EF59C37B-E9D6-409F-B5B2-226384796D30 res:1080x1920 hz:60 color_depth:8 enabled:true scaling:off origin:(1728,-226) degree:90" "id:F2DDEF8B-97AE-4083-8C90-3B0B71B6858E res:1920x1200 hz:60 color_depth:8 enabled:true scaling:off origin:(-1920,-962) degree:0"',
         'match_input',
         0,
         None)
    reset_conf_apple(expected_code=None)  # resetting after this profile used to not work. The screen rotations do work, but they time out. This is good enough, so we ignore the error code.

    print('Test disabling and enabling a mirrored screen')
    test('test_set_mirror',
         '"id:7746278E-D1CF-4842-8286-26BDEBFD9F1A res:1440x2560 hz:60 color_depth:8 enabled:true scaling:off origin:(0,0) degree:90" "id:37D8832A-2D66-02CA-B9F7-8F30A301B230+EF59C37B-E9D6-409F-B5B2-226384796D30+F2DDEF8B-97AE-4083-8C90-3B0B71B6858E res:1728x1117 hz:120 color_depth:8 enabled:true scaling:on origin:(-1728,1193) degree:0"',
         'match_input',
         0,
         None)
    test('test_disable_mirror_screen',
         '"id:F2DDEF8B-97AE-4083-8C90-3B0B71B6858E res:1920x1200 hz:60 color_depth:8 enabled:true scaling:off origin:(0,0) degree:0" "id:37D8832A-2D66-02CA-B9F7-8F30A301B230+EF59C37B-E9D6-409F-B5B2-226384796D30 res:1728x1117 hz:120 color_depth:8 enabled:true scaling:on origin:(-1728,1193) degree:0" "id:7746278E-D1CF-4842-8286-26BDEBFD9F1A res:1440x2560 hz:60 color_depth:8 enabled:true scaling:off origin:(1920,-339) degree:90"',
         'match_input',
         0,
         None)
    test(step='test_enable_mirror_screen_and_rotate_180',
         conf='"id:7746278E-D1CF-4842-8286-26BDEBFD9F1A res:1440x2560 hz:60 color_depth:8 enabled:true scaling:off origin:(0,0) degree:90" "id:37D8832A-2D66-02CA-B9F7-8F30A301B230+EF59C37B-E9D6-409F-B5B2-226384796D30+F2DDEF8B-97AE-4083-8C90-3B0B71B6858E res:1728x1117 hz:120 color_depth:8 enabled:true scaling:on origin:(-1728,1193) degree:180"',
         expected_conf='match_input',
         expected_code=None,  # The screen rotations do work, but they time out. This is good enough so we ignore the error code.
         expected_error=None)
    reset_conf_apple(expected_code=None)  # The screen rotations do work, but they time out. This is good enough so we ignore the error code.


def test(step, conf, expected_conf, expected_code, expected_error):
    print('Executing ' + step)
    output, code = displayplacer(conf)

    try:
        if expected_error:
            assert expected_error in output

        if expected_code is not None:
            assert code == expected_code

        if expected_conf:
            list_output, list_code = displayplacer('list')

            if expected_conf == 'match_input':
                assert list_output.splitlines()[-1] == 'displayplacer ' + conf
            else:
                assert list_output.splitlines()[-1] == 'displayplacer ' + expected_conf

            assert list_code == 0
    except Exception as e:
        print('code=' + str(code))
        print('output=' + output)
        raise e


def reset_conf_intel(expected_code=0):
    test('reset_conf',
         '"id:A46D2F5E-487B-CC69-C588-ECFD519016E5 res:3840x2160 hz:60 color_depth:8 enabled:true scaling:off origin:(0,0) degree:0" "id:F466F621-B5FA-04A0-0800-CFA6C258DECD res:1440x900 color_depth:4 enabled:true scaling:on origin:(-1440,1469) degree:0" "id:34686E82-0CED-DF86-AFC7-AA1A8EB5CFC0 res:1440x2560 hz:59 color_depth:8 enabled:true scaling:off origin:(3840,-238) degree:90" "id:EA487A4B-D9B9-DDDD-91F8-F43E599B7E84 res:1920x1200 color_depth:8 enabled:true scaling:off origin:(960,-1200) degree:0"',
         'match_input',
         expected_code,
         None)


def reset_conf_apple(expected_code=0):
    test('reset_conf',
         '"id:EF59C37B-E9D6-409F-B5B2-226384796D30 res:3840x2160 hz:60 color_depth:8 enabled:true scaling:off origin:(0,0) degree:0" "id:37D8832A-2D66-02CA-B9F7-8F30A301B230 res:1496x967 hz:120 color_depth:8 enabled:true scaling:on origin:(-1496,1193) degree:0" "id:7746278E-D1CF-4842-8286-26BDEBFD9F1A res:1440x2560 hz:60 color_depth:8 enabled:true scaling:off origin:(3840,-226) degree:90" "id:F2DDEF8B-97AE-4083-8C90-3B0B71B6858E res:1920x1200 hz:60 color_depth:8 enabled:true scaling:off origin:(960,-1200) degree:0"',
         'match_input',
         expected_code,
         None)


def displayplacer(args):
    p = subprocess.Popen('../src/displayplacer ' + args, shell=True, stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
    output = p.communicate()[0].decode('utf-8').strip()
    code = p.wait()

    return output, code


if __name__ == '__main__':
    main()
