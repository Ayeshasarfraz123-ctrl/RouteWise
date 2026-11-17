#ifndef PARCEL_H
#define PARCEL_H

#include <string>
using namespace std;

// Status of a parcel
enum ParcelStatus { REGISTERED, PENDING, PROCESSING, DELIVERED };

struct Parcel {
    int trackingId;
    int customerId;
    string source;
    string destination;
    double weight;
    int priority;          // higher number = more urgent
    ParcelStatus status;
};

// Turns a status into readable text
inline string statusToString(ParcelStatus s) {
    switch (s) {
    case REGISTERED: return "Registered";
    case PENDING:    return "Pending";
    case PROCESSING: return "Processing";
    case DELIVERED:  return "Delivered";
    }
    return "Unknown";
}

#endif