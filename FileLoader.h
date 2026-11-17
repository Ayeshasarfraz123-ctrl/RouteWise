#ifndef FILELOADER_H
#define FILELOADER_H

#include <iostream>
#include <fstream>
#include <string>
#include "RouteWise.h"
#include "Input.h"
#include "ParcelOps.h"
using namespace std;

// Split "a|b|c" into parts. Returns how many parts were found.
inline int splitLine(const string& line, string* parts, int maxParts) {
    int count = 0;
    string current = "";
    for (int i = 0; i < (int)line.length(); i++) {
        if (line[i] == '|' && count < maxParts - 1) {
            parts[count++] = trim(current);
            current = "";
        }
        else {
            current += line[i];
        }
    }
    parts[count++] = trim(current);
    return count;
}

// Load a data file. Returns false if the file cannot be opened.
// Lines (locations must come before roads and parcels):
//   L|name                                  a location
//   R|nameA|nameB|km                        a road
//   C|name|phone|address                    a customer (ID given automatically: 1, 2, 3 ...)
//   P|customerId|source|destination|kg|priority    a parcel (tracking ID given automatically)
//   # ...                                   a comment
inline bool loadDataFile(RouteWise& sys, const string& filename) {
    ifstream in(filename.c_str());
    if (!in) return false;

    int locations = 0;
    int roads = 0;
    int customers = 0;
    int parcels = 0;
    int lineNumber = 0;
    string line;

    while (getline(in, line)) {
        lineNumber++;

        // Remove the invisible marker some editors put at the start of a file
        if (lineNumber == 1 && line.length() >= 3 &&
            (unsigned char)line[0] == 0xEF && (unsigned char)line[1] == 0xBB &&
            (unsigned char)line[2] == 0xBF)
            line = line.substr(3);

        line = trim(line);
        if (line.length() == 0 || line[0] == '#') continue;

        string parts[6];
        int n = splitLine(line, parts, 6);

        if (parts[0] == "L" && n >= 2) {
            if (sys.city.addLocation(parts[1]) != -1) locations++;
            else cout << "  Line " << lineNumber << ": location skipped (empty or repeated)." << endl;
        }
        else if (parts[0] == "R" && n >= 4) {
            int km;
            if (parseInt(parts[3], km) && sys.city.addRoad(parts[1], parts[2], km)) roads++;
            else cout << "  Line " << lineNumber << ": road skipped (unknown place or bad distance)." << endl;
        }
        else if (parts[0] == "C" && n >= 4) {
            if (createCustomer(sys, parts[1], parts[2], parts[3]) != nullptr) customers++;
            else cout << "  Line " << lineNumber << ": customer skipped (a field is empty)." << endl;
        }
        else if (parts[0] == "P" && n >= 6) {
            int customerId, priority;
            double weight;
            if (parseInt(parts[1], customerId) && parseDouble(parts[4], weight) &&
                parseInt(parts[5], priority)) {
                string error;
                if (createParcel(sys, customerId, parts[2], parts[3], weight, priority, error) != nullptr)
                    parcels++;
                else
                    cout << "  Line " << lineNumber << ": parcel skipped (" << error << ")." << endl;
            }
            else {
                cout << "  Line " << lineNumber << ": parcel skipped (bad number)." << endl;
            }
        }
        else {
            cout << "  Line " << lineNumber << ": not understood, skipped." << endl;
        }
    }

    cout << "Loaded from '" << filename << "': " << locations << " locations, "
        << roads << " roads, " << customers << " customers, "
        << parcels << " parcels." << endl;
    return true;
}

#endif