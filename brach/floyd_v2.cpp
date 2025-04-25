#include <iostream>
#include <climits>
#include <chrono>
using namespace std;

#define V 4  // Number of vertices in graph

// Floyd-Warshall algorithm without branches for min calculations
void floydWarshallWithoutBranches(int graph[V][V]) {
    int dist[V][V];
    
    // Initialize distance array
    for (int i = 0; i < V; i++) {
        for (int j = 0; j < V; j++) {
            dist[i][j] = graph[i][j];
        }
    }

    // Floyd-Warshall without conditionals (min calculation without branches)
    for (int k = 0; k < V; k++) {
        for (int i = 0; i < V; i++) {
            for (int j = 0; j < V; j++) {
                dist[i][j] = min(dist[i][j], dist[i][k] + dist[k][j]);
            }
        }
    }

    // Print the shortest distance matrix
    cout << "Shortest distances without branches: " << endl;
    for (int i = 0; i < V; i++) {
        for (int j = 0; j < V; j++) {
            if (dist[i][j] == INT_MAX) {
                cout << "INF ";
            } else {
                cout << dist[i][j] << " ";
            }
        }
        cout << endl;
    }
}

int main() {
    // Example graph (4 vertices)
    int graph[V][V] = { {0, 3, INT_MAX, INT_MAX},
                        {3, 0, 1, INT_MAX},
                        {INT_MAX, 1, 0, 7},
                        {INT_MAX, INT_MAX, 7, 0} };

    auto start = chrono::high_resolution_clock::now();
    floydWarshallWithoutBranches(graph);
    auto end = chrono::high_resolution_clock::now();

    chrono::duration<double> duration = end - start;
    cout << "Time taken without branches: " << duration.count() << " seconds\n";
    
    return 0;
}
