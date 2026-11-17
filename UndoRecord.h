#ifndef UNDORECORD_H
#define UNDORECORD_H

#include "Parcel.h"

// What we remember so a status change can be reversed
struct UndoRecord {
    int trackingId;               // which parcel was changed
    ParcelStatus previousStatus;  // status before the change
    ParcelStatus newStatus;       // status after the change
    bool wasUrgent;               // did it come from the urgent heap?
    int arrivalOrder;             // its arrival number (to put it back correctly)
};

#endif