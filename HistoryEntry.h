#ifndef HISTORYENTRY_H
#define HISTORYENTRY_H

#include "Parcel.h"

// One line of the delivery history
struct HistoryEntry {
    int sequence;          // 1, 2, 3 ... order of events
    int trackingId;
    ParcelStatus event;    // PROCESSING (dispatched) or DELIVERED
};

#endif