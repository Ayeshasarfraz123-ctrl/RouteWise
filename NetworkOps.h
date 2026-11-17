#ifndef NETWORKOPS_H
#define NETWORKOPS_H

#include <iostream>
#include <string>
#include "RouteWise.h"
#include "Input.h"
#include "Traversal.h"
#include "Dijkstra.h"
#include "MST.h"
using namespace std;

// Print all location names, to help the user type them correctly
inline void showLocationNames(const Graph& g) {
    cout << "Locations: ";
    for (int i = 0; i < g.getVertexCount(); i++) {
        cout << g.getName(i);
        if (i < g.getVertexCount() - 1) cout << ", ";
    }
    cout << endl;
}

// Ask until a real location is typed. Returns its index, or -1 if cancelled (Enter).
inline int askLocation(const Graph& g, const string& question) {
    while (!inputEnded()) {
        string name = readLine(question + " (Enter to cancel): ");
        if (name.length() == 0) return -1;
        int index = g.findLocation(name);
        if (index != -1) return index;
        cout << "  Location not found. Check the spelling." << endl;
    }
    return -1;
}

inline void displayNetwork(RouteWise& sys) {
    Graph& g = sys.city;
    if (g.getVertexCount() == 0) {
        cout << "The city network is empty. Add locations first." << endl;
        return;
    }
    cout << "Locations: " << g.getVertexCount()
        << " | Roads: " << g.getEdgeCount() << endl << endl;
    g.displayList();
    cout << endl;
    g.displayMatrix();
}

inline void addLocationOption(RouteWise& sys) {
    string name = readLine("New location name: ");
    if (inputEnded()) return;
    if (sys.city.addLocation(name) == -1)
        cout << "Could not add: the name is empty or the location already exists." << endl;
    else
        cout << "Location '" << name << "' added." << endl;
}

inline void addRoadOption(RouteWise& sys) {
    Graph& g = sys.city;
    if (g.getVertexCount() < 2) {
        cout << "You need at least 2 locations before adding a road." << endl;
        return;
    }
    showLocationNames(g);
    int a = askLocation(g, "First location");
    if (a < 0) return;
    int b = askLocation(g, "Second location");
    if (b < 0) return;
    if (a == b) {
        cout << "A road must connect two different locations." << endl;
        return;
    }
    int km = readInt("Distance in km (1 to 1000): ", 1, 1000);
    if (inputEnded()) return;

    bool existed = g.getWeight(a, b) > 0;
    g.addRoad(g.getName(a), g.getName(b), km);
    if (existed) cout << "Road already existed. Distance updated to " << km << " km." << endl;
    else cout << "Road added: " << g.getName(a) << " <-> " << g.getName(b) << " (" << km << " km)." << endl;
}

// Menu option 11
inline void networkMenu(RouteWise& sys) {
    while (!inputEnded()) {
        cout << endl << "--- City Network ---" << endl;
        cout << "1. Display network" << endl;
        cout << "2. Add location" << endl;
        cout << "3. Add road" << endl;
        cout << "0. Back to main menu" << endl;
        int choice = readInt("Choice: ", 0, 3);
        if (inputEnded() || choice == 0) return;
        if (choice == 1) displayNetwork(sys);
        else if (choice == 2) addLocationOption(sys);
        else addRoadOption(sys);
    }
}

// Menu options 12 (BFS) and 13 (DFS)
inline void traversalOption(RouteWise& sys, bool useBfs) {
    Graph& g = sys.city;
    int n = g.getVertexCount();
    if (n == 0) {
        cout << "The city network is empty. Add locations first." << endl;
        return;
    }
    showLocationNames(g);
    int start = askLocation(g, "Start from");
    if (start < 0) return;

    int* order = new int[n];
    int count = useBfs ? bfs(g, start, order) : dfs(g, start, order);

    cout << endl << (useBfs ? "BFS" : "DFS") << " traversal from "
        << g.getName(start) << ":" << endl;
    printOrder(g, order, count);
    cout << "Locations reached: " << count << " of " << n << endl << endl;
    delete[] order;

    int target = askLocation(g, "Check if this destination is reachable");
    if (target < 0) return;
    if (isReachable(g, start, target))
        cout << g.getName(target) << " IS reachable from " << g.getName(start) << "." << endl;
    else
        cout << g.getName(target) << " is NOT reachable from " << g.getName(start) << "." << endl;
}

// Menu option 14
inline void shortestRouteOption(RouteWise& sys) {
    Graph& g = sys.city;
    if (g.getVertexCount() < 2) {
        cout << "You need at least 2 locations first." << endl;
        return;
    }
    showLocationNames(g);
    int src = askLocation(g, "Source");
    if (src < 0) return;
    int dst = askLocation(g, "Destination");
    if (dst < 0) return;

    int* path = new int[g.getVertexCount()];
    int length = 0;
    int total = shortestPath(g, src, dst, path, length);

    cout << endl << "Source: " << g.getName(src) << endl;
    cout << "Destination: " << g.getName(dst) << endl;
    if (total == -1)
        cout << "No route exists between these locations." << endl;
    else
        printRoute(g, path, length, total);
    delete[] path;
}

// Menu option 15
inline void minimumNetworkOption(RouteWise& sys) {
    Graph& g = sys.city;
    int n = g.getVertexCount();
    if (n < 2 || g.getEdgeCount() == 0) {
        cout << "The network needs at least 2 locations and 1 road." << endl;
        return;
    }
    MSTEdge* edges = new MSTEdge[n];
    int edgeCount, parts;
    int total = minimumSpanningTree(g, edges, edgeCount, parts);

    cout << "=== Minimum Delivery Network ===" << endl;
    printMST(g, edges, edgeCount, parts, total);

    int allRoads = 0;
    for (int i = 0; i < n; i++)
        for (int j = i + 1; j < n; j++)
            allRoads += g.getWeight(i, j);
    cout << "All existing roads together: " << allRoads << " km" << endl;
    cout << "Roads kept: " << edgeCount << " of " << g.getEdgeCount() << endl;
    delete[] edges;
}

#endif