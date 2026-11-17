#ifndef URGENTREQUEST_H
#define URGENTREQUEST_H

// One urgent delivery request
struct UrgentRequest {
    int trackingId;
    int priority;        // higher number = more urgent
    int arrivalOrder;    // 1, 2, 3 ... smaller = arrived earlier
};

// Rule: higher priority first. If equal, the earlier arrival first.
inline bool moreUrgent(const UrgentRequest& a, const UrgentRequest& b) {
    if (a.priority != b.priority)
        return a.priority > b.priority;
    return a.arrivalOrder < b.arrivalOrder;
}

#endif