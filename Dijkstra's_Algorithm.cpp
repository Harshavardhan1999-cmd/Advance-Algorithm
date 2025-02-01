#include <iostream>
#include <vector>
#include <queue>
#include <unordered_map>
#include <climits>

using namespace std;

class Graph {
    unordered_map<string, vector<pair<string, int>>> adjList;

public:
    void addEdge(const string& src, const string& dest, int weight) {
        adjList[src].emplace_back(dest, weight);
    }

    pair<int, vector<string>> dijkstra(const string& start, const string& end) {
        unordered_map<string, int> dist;
        unordered_map<string, string> prev;
        auto cmp = [](pair<int, string>& a, pair<int, string>& b) { return a.first > b.first; };
        priority_queue<pair<int, string>, vector<pair<int, string>>, decltype(cmp)> pq(cmp);

        // Initialize distances
        for (const auto& node : adjList) {
            dist[node.first] = INT_MAX;
        }
        dist[start] = 0;

        pq.push({0, start});

        while (!pq.empty()) {
            auto [currentDist, currentNode] = pq.top();
            pq.pop();

            if (currentNode == end) {
                vector<string> path;
                for (string at = end; !at.empty(); at = prev[at]) {
                    path.insert(path.begin(), at);
                }
                return {currentDist, path};
            }

            for (const auto& [neighbor, weight] : adjList[currentNode]) {
                int newDist = currentDist + weight;
                if (newDist < dist[neighbor]) {
                    dist[neighbor] = newDist;
                    prev[neighbor] = currentNode;
                    pq.push({newDist, neighbor});
                }
            }
        }

        return {INT_MAX, {}};
    }
};

int main() {
    Graph graph;

    // Hardcoded graph
    graph.addEdge("A", "B", 10);
    graph.addEdge("A", "E", 3);
    graph.addEdge("B", "E", 4);
    graph.addEdge("B", "C", 2);
    graph.addEdge("C", "D", 9);
    graph.addEdge("D", "C", 7);
    graph.addEdge("E", "B", 1);
    graph.addEdge("E", "D", 2);
    graph.addEdge("E", "C", 8);

    string startNode, endNode;
    cout << "Enter the starting node: ";
    cin >> startNode;
    cout << "Enter the ending node: ";
    cin >> endNode;

    auto [cost, path] = graph.dijkstra(startNode, endNode);

    if (cost == INT_MAX) {
        cout << "No valid path from " << startNode << " to " << endNode << "." << endl;
    } else {
        cout << "The cost of the shortest path from " << startNode << " to " << endNode << " is " << cost << "." << endl;
        cout << "The shortest path is: ";
        for (const auto& node : path) {
            cout << node << " ";
        }
        cout << endl;
    }

    return 0;
}
