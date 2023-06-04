#include "Headers/Bridging-Header.h"

bool setRotation(CGDirectDisplayID screenId, char* screenUUID, int degree) {
    bool isSuccess = true;

    MPDisplay* mpDisplay = [[MPDisplay alloc] initWithCGSDisplayID:screenId];
    [mpDisplay setOrientation: degree];

    // block until non-blocking rotation change completes
    int waitSeconds = 5;
    unsigned long beginTime = time(NULL);
    unsigned long secondsElapsed = 0;
    while (CGDisplayRotation(screenId) != degree ) {
        unsigned long curSecondsElapsed = time(NULL) - beginTime;

        if (curSecondsElapsed > secondsElapsed) {
            printf("Waited %lu of %i seconds for screen %s to complete rotating\n", curSecondsElapsed, waitSeconds, screenUUID);
        }

        if (curSecondsElapsed >= waitSeconds) {
            fprintf(stderr, "Unable to confirm if screen %s successfully rotated to degree:%i\n", screenUUID, degree);
            isSuccess = false;
            break;
        }

        secondsElapsed = curSecondsElapsed;
    }

    [mpDisplay release];
    return isSuccess;
}
