#ifndef GRAPH_H
#define GRAPH_H

#include <iostream>
#include <iomanip>
#include <string>
#include "LinkedList.h"
using namespace std;

// One road going out of a location
struct Edge {
    int to;       // index of the location it leads to
    int weight;   // distance (km) or cost
};

// City road network. Roads work both ways (undirected).
// Stored two ways: adjacency list AND adjacency matrix.
class Graph {
private:
    string* names;               // names[i] = name of location i
    LinkedList<Edge>** adj;      // adj[i] = list of roads starting at i
    int** matrix;                // matrix[i][j] = distance, 0 = no road
    int vertexCount;
    int capacity;
    int edgeCount;

    // lowercase one letter (so "airport" matches "Airport")
    char lower(char c) const {
        if (c >= 'A' && c <= 'Z') return c + 32;
        return c;
    }

    bool sameName(const string& a, const string& b) const {
        if (a.length() != b.length()) return false;
        for (int i = 0; i < (int)a.length(); i++)
            if (lower(a[i]) != lower(b[i])) return false;
        return true;
    }

    // Allocate a zero-filled matrix of size n x n
    int** newMatrix(int n) const {
        int** m = new int* [n];
        for (int i = 0; i < n; i++) {
            m[i] = new int[n];
            for (int j = 0; j < n; j++) m[i][j] = 0;
        }
        return m;
    }

    // Double the capacity when we run out of room
    void grow() {
        int newCap = capacity * 2;

        string* newNames = new string[newCap];
        LinkedList<Edge>** newAdj = new LinkedList<Edge>*[newCap];
        int** newMat = newMatrix(newCap);

        for (int i = 0; i < newCap; i++) newAdj[i] = nullptr;
        for (int i = 0; i < vertexCount; i++) {
            newNames[i] = names[i];
            newAdj[i] = adj[i];                  // keep the same lists
            for (int j = 0; j < vertexCount; j++)
                newMat[i][j] = matrix[i][j];
        }

        for (int i = 0; i < capacity; i++) delete[] matrix[i];
        delete[] matrix;
        delete[] adj;                            // only the pointer array
        delete[] names;

        names = newNames;
        adj = newAdj;
        matrix = newMat;
        capacity = newCap;
    }

    // Set the weight of road from->to in the list (add it if missing)
    void setListEdge(int from, int to, int weight) {
        for (Node<Edge>* cur = adj[from]->getHead(); cur != nullptr; cur = cur->next) {
            if (cur->data.to == to) {
                cur->data.weight = weight;
                return;
            }
        }
        Edge e;
        e.to = to;
        e.weight = weight;
        adj[from]->pushBack(e);
    }

public:
    Graph(int startCapacity = 8) {
        capacity = startCapacity;
        vertexCount = 0;
        edgeCount = 0;
        names = new string[capacity];
        adj = new LinkedList<Edge>*[capacity];
        for (int i = 0; i < capacity; i++) adj[i] = nullptr;
        matrix = newMatrix(capacity);
    }

    ~Graph() {
        for (int i = 0; i < vertexCount; i++) delete adj[i];
        delete[] adj;
        for (int i = 0; i < capacity; i++) delete[] matrix[i];
        delete[] matrix;
        delete[] names;
    }

    Graph(const Graph&) = delete;
    Graph& operator=(const Graph&) = delete;

    int getVertexCount() const { return vertexCount; }
    int getEdgeCount() const { return edgeCount; }
    string getName(int i) const { return names[i]; }
    const LinkedList<Edge>& getAdjacency(int i) const { return *adj[i]; }
    int getWeight(int i, int j) const { return matrix[i][j]; }

    // Returns the index of a location, or -1 if it does not exist
    int findLocation(const string& name) const {
        for (int i = 0; i < vertexCount; i++)
            if (sameName(names[i], name)) return i;
        return -1;
    }

    // Returns the new location's index, or -1 if it already exists / is empty
    int addLocation(const string& name) {
        if (name.length() == 0) return -1;
        if (findLocation(name) != -1) return -1;
        if (vertexCount == capacity) grow();
        names[vertexCount] = name;
        adj[vertexCount] = new LinkedList<Edge>();
        vertexCount++;
        return vertexCount - 1;
    }

    // Add a two-way road. If the road already exists, its weight is updated.
    // Returns false for unknown locations, same location twice, or weight <= 0.
    bool addRoad(const string& from, const string& to, int weight) {
        int a = findLocation(from);
        int b = findLocation(to);
        if (a == -1 || b == -1) return false;
        if (a == b) return false;
        if (weight <= 0) return false;

        if (matrix[a][b] == 0) edgeCount++;   // brand new road

        matrix[a][b] = weight;
        matrix[b][a] = weight;
        setListEdge(a, b, weight);
        setListEdge(b, a, weight);
        return true;
    }

    void displayList() const {
        cout << "--- Adjacency List ---" << endl;
        for (int i = 0; i < vertexCount; i++) {
            cout << names[i] << " -> ";
            Node<Edge>* cur = adj[i]->getHead();
            if (cur == nullptr) cout << "(no roads)";
            while (cur != nullptr) {
                cout << names[cur->data.to] << " (" << cur->data.weight << " km)";
                if (cur->next != nullptr) cout << ", ";
                cur = cur->next;
            }
            cout << endl;
        }
    }

    void displayMatrix() const {
        cout << "--- Adjacency Matrix (0 = no road) ---" << endl;
        for (int i = 0; i < vertexCount; i++)
            cout << i << " = " << names[i] << endl;
        cout << endl << "    ";
        for (int j = 0; j < vertexCount; j++) cout << setw(4) << j;
        cout << endl;
        for (int i = 0; i < vertexCount; i++) {
            cout << setw(4) << i;
            for (int j = 0; j < vertexCount; j++)
                cout << setw(4) << matrix[i][j];
            cout << endl;
        }
    }
};

#endif