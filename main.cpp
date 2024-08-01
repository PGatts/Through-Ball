#include <iostream>
#include <unordered_map>
#include <string>
#include <vector>
#include "graph.h"

using namespace std;
int main(){
    Graph graph;
    graph.insertEdge("Messi", "Neymar");
    graph.insertEdge("Neymar", "Vini");

    graph.displayGraph();
    vector<string> path = graph.bfs("Messi", "Vini");
    for(string s: path){
        cout << s << " ";
    }
    return 0;
}
