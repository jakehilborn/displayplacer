#include "Headers/Bridging-Header.h"

bool rotateScreen(CGDirectDisplayID screenId, char* screenUUID, int degree) {
    MPDisplay* mpdisplay = [[MPDisplay alloc] initWithCGSDisplayID:screenId];
    [mpdisplay setOrientation: degree];

    return true;
}
