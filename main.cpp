#include <iostream>
#include <string>
#include "RouteWise.h"
#include "Input.h"
#include "FileLoader.h"
#include "NetworkOps.h"
#include "ParcelOps.h"
#include "DispatchOps.h"
#include "StatsOps.h"
using namespace std;

void printMenu() {
    cout << endl;
    cout << "=========================================" << endl;
    cout << "        ROUTEWISE DELIVERY SYSTEM" << endl;
    cout << "=========================================" << endl;
    cout << " 1. Register Customer" << endl;
    cout << " 2. Register Parcel" << endl;
    cout << " 3. Search Parcel" << endl;
    cout << " 4. Add Normal Delivery Request" << endl;
    cout << " 5. Add Urgent Delivery Request" << endl;
    cout << " 6. Process Next Delivery" << endl;
    cout << " 7. Mark Parcel as Delivered" << endl;
    cout << " 8. Undo Last Operation" << endl;
    cout << " 9. Display Pending Deliveries" << endl;
    cout << "10. Display Parcel Records" << endl;
    cout << "11. Display City Network" << endl;
    cout << "12. BFS Network Traversal" << endl;
    cout << "13. DFS Network Traversal" << endl;
    cout << "14. Find Shortest Delivery Route" << endl;
    cout << "15. Generate Minimum Delivery Network" << endl;
    cout << "16. Compare BST and AVL Performance" << endl;
    cout << "17. Display System Statistics" << endl;
    cout << "18. Exit" << endl;
    cout << "=========================================" << endl;
}

int main() {
    RouteWise sys;

    // Load the sample data file at start-up
    string fileName = "routewise_data.txt";
    if (!loadDataFile(sys, fileName)) {
        cout << "Could not open '" << fileName << "'." << endl;
        fileName = readLine("Type another data file name (or press Enter to start empty): ");
        if (fileName.length() > 0 && !loadDataFile(sys, fileName))
            cout << "Could not open '" << fileName << "'. Starting empty." << endl;
    }

    while (!inputEnded()) {
        printMenu();
        int choice = readInt("Enter your choice (1-18): ", 1, 18);
        if (inputEnded()) break;
        cout << endl;

        switch (choice) {
        case 1: registerCustomerOption(sys); break;
        case 2: registerParcelOption(sys); break;
        case 3: searchParcelOption(sys); break;
        case 4: requestOption(sys, false); break;
        case 5: requestOption(sys, true); break;
        case 6: processNextOption(sys); break;
        case 7: markDeliveredOption(sys); break;
        case 8: undoOption(sys); break;
        case 9: displayPendingOption(sys); break;
        case 10: displayParcelsOption(sys); break;
        case 11: networkMenu(sys); break;
        case 12: traversalOption(sys, true); break;
        case 13: traversalOption(sys, false); break;
        case 14: shortestRouteOption(sys); break;
        case 15: minimumNetworkOption(sys); break;
        case 16: compareTreesOption(sys); break;
        case 17: statisticsOption(sys); break;
        case 18:
            cout << "Goodbye!" << endl;
            return 0;
        default:
            break;
        }
    }
    return 0;
}