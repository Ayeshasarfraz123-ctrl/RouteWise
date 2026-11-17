#ifndef PARCELOPS_H
#define PARCELOPS_H

#include <iostream>
#include <iomanip>
#include <string>
#include "RouteWise.h"
#include "Input.h"
#include "NetworkOps.h"
using namespace std;

// ---------- Creating records (used by the menu AND by the data file) ----------

// Create a customer. Returns nullptr if any field is empty.
inline Customer* createCustomer(RouteWise& sys, const string& name,
    const string& phone, const string& address) {
    if (name.length() == 0 || phone.length() == 0 || address.length() == 0)
        return nullptr;

    Customer* c = new Customer;
    c->customerId = sys.nextCustomerId++;
    c->name = name;
    c->phone = phone;
    c->address = address;
    sys.customers.insert(c->customerId, c);
    return c;
}

// Create a parcel and add it to the hash table, the BST and the AVL tree.
// Returns nullptr (and sets 'error') if something is wrong.
inline Parcel* createParcel(RouteWise& sys, int customerId, const string& source,
    const string& destination, double weight,
    int priority, string& error) {
    if (sys.customers.search(customerId) == nullptr) {
        error = "customer ID not found";
        return nullptr;
    }
    int s = sys.city.findLocation(source);
    int d = sys.city.findLocation(destination);
    if (s == -1) { error = "source location not found"; return nullptr; }
    if (d == -1) { error = "destination location not found"; return nullptr; }
    if (s == d) { error = "source and destination must be different"; return nullptr; }
    if (weight <= 0) { error = "weight must be more than 0"; return nullptr; }
    if (priority < 1 || priority > 5) { error = "priority must be 1 to 5"; return nullptr; }

    Parcel* p = new Parcel;
    p->trackingId = sys.nextTrackingId++;
    p->customerId = customerId;
    p->source = sys.city.getName(s);          // stored with the proper spelling
    p->destination = sys.city.getName(d);
    p->weight = weight;
    p->priority = priority;
    p->status = REGISTERED;

    sys.parcels.insert(p->trackingId, p);     // fast lookup by tracking ID
    sys.bst.insert(p->trackingId, p);         // ordered index (plain BST)
    sys.avl.insert(p->trackingId, p);         // ordered index (balanced AVL)
    return p;
}

// ---------- Showing records ----------

inline void printParcelDetails(RouteWise& sys, const Parcel* p) {
    Customer* c = sys.customers.search(p->customerId);
    cout << fixed << setprecision(2);
    cout << "  Tracking ID : " << p->trackingId << endl;
    cout << "  Customer    : " << p->customerId
        << " (" << (c != nullptr ? c->name : string("unknown")) << ")" << endl;
    cout << "  From        : " << p->source << endl;
    cout << "  To          : " << p->destination << endl;
    cout << "  Weight      : " << p->weight << " kg" << endl;
    cout << "  Priority    : " << p->priority << " (1 = lowest, 5 = highest)" << endl;
    cout << "  Status      : " << statusToString(p->status) << endl;
    cout.unsetf(ios::fixed);
    cout << setprecision(6);
}

// Called once for every parcel while walking through the tree
inline void printParcelRow(int /*key*/, Parcel* p) {
    cout << left << setw(10) << p->trackingId
        << setw(7) << p->customerId
        << setw(18) << p->source
        << setw(18) << p->destination
        << right << setw(8) << p->weight
        << setw(6) << p->priority
        << "  " << left << statusToString(p->status) << endl;
}

// ---------- Menu options ----------

// Option 1
inline void registerCustomerOption(RouteWise& sys) {
    cout << "--- Register Customer (press Enter on any question to cancel) ---" << endl;
    string name = readLine("Name: ");
    if (name.length() == 0) { cout << "Cancelled." << endl; return; }
    string phone = readLine("Phone number: ");
    if (phone.length() == 0) { cout << "Cancelled." << endl; return; }
    string address = readLine("Address: ");
    if (address.length() == 0) { cout << "Cancelled." << endl; return; }

    Customer* c = createCustomer(sys, name, phone, address);
    if (c != nullptr)
        cout << "Customer registered. Customer ID: " << c->customerId << endl;
}

// Option 2
inline void registerParcelOption(RouteWise& sys) {
    if (sys.customers.size() == 0) {
        cout << "Register a customer first (option 1)." << endl;
        return;
    }
    if (sys.city.getVertexCount() < 2) {
        cout << "The city needs at least 2 locations first (option 11)." << endl;
        return;
    }

    cout << "--- Register Parcel ---" << endl;
    int customerId = 0;
    while (true) {
        customerId = readInt("Customer ID (0 to cancel): ", 0, 999999999);
        if (inputEnded() || customerId == 0) { cout << "Cancelled." << endl; return; }
        if (sys.customers.search(customerId) != nullptr) break;
        cout << "  No customer with that ID." << endl;
    }

    showLocationNames(sys.city);
    int src = askLocation(sys.city, "Source");
    if (src < 0) { cout << "Cancelled." << endl; return; }
    int dst = askLocation(sys.city, "Destination");
    if (dst < 0) { cout << "Cancelled." << endl; return; }
    if (src == dst) {
        cout << "Source and destination must be different." << endl;
        return;
    }

    double weight = readDouble("Weight in kg (0.1 to 1000): ", 0.1, 1000.0);
    int priority = readInt("Priority (1 = lowest, 5 = highest): ", 1, 5);
    if (inputEnded()) return;

    string error;
    Parcel* p = createParcel(sys, customerId, sys.city.getName(src),
        sys.city.getName(dst), weight, priority, error);
    if (p == nullptr) {
        cout << "Could not register the parcel: " << error << "." << endl;
        return;
    }
    cout << "Parcel registered. Tracking ID: " << p->trackingId << endl;
    cout << "Use option 4 or 5 to request its delivery." << endl;
}

// Option 3
inline void searchParcelOption(RouteWise& sys) {
    int id = readInt("Tracking ID to search (0 to cancel): ", 0, 999999999);
    if (inputEnded() || id == 0) return;

    Parcel* p = sys.parcels.search(id);      // hash table lookup
    int probes = sys.parcels.getLastProbes();
    if (p == nullptr) {
        cout << "No parcel with tracking ID " << id << " was found." << endl;
        return;
    }
    cout << "Found using the hash table (checked " << probes
        << (probes == 1 ? " slot)." : " slots).") << endl;
    printParcelDetails(sys, p);
}

// Option 10
inline void displayParcelsOption(RouteWise& sys) {
    if (sys.parcels.size() == 0) {
        cout << "No parcels registered yet." << endl;
        return;
    }
    cout << "--- Parcel Records (sorted by tracking ID, read from the AVL tree) ---" << endl;
    cout << left << setw(10) << "Tracking" << setw(7) << "Cust."
        << setw(18) << "From" << setw(18) << "To"
        << right << setw(8) << "Kg" << setw(6) << "Pri"
        << "  " << left << "Status" << endl;
    cout << "-----------------------------------------------------------------------------" << endl;

    cout << fixed << setprecision(2);
    sys.avl.inorder(printParcelRow);
    cout.unsetf(ios::fixed);
    cout << setprecision(6);

    cout << "Total parcels: " << sys.parcels.size() << endl;
}

#endif