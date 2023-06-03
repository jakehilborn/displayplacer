#include "Headers/Bridging-Header.h"

bool setRotation(CGDirectDisplayID screenId, char* screenUUID, int degree) {
    MPDisplay* mpDisplay = [[MPDisplay alloc] initWithCGSDisplayID:screenId];
    [mpDisplay setOrientation: degree];

    while(true) {
        MPDisplay* mpDisplayRotated = [[MPDisplay alloc] initWithCGSDisplayID:screenId];
        int curDegree = mpDisplayRotated.orientation;
        [mpDisplayRotated release];

        if (curDegree == degree) {
            break;
        } else {
            printf("Waiting for rotation to apply on screen id: %s\n", screenUUID);
            // wait until non-blocking rotation change is complete
        }
    }

    [mpDisplay release];
    return true;
}
