#pragma once
#include <unordered_map>
#include <unordered_set>
#include <string>
#include <vector>
#include <queue>

using namespace std;

class Graph{
private:


public:
    unordered_map<string, int> name_dob_to_int;
    unordered_map<int, string> int_to_name_dob;
    unordered_map<int, vector<pair<int, float>>> adjacency_list;
    //default constructor
    Graph() = default;

    void insertEdge(string from, string to){
        int from_int, to_int;
        if(name_dob_to_int.find(from) != name_dob_to_int.end()){//If the key is already in the map
            from_int = name_dob_to_int[from];
        }else{//Add name to map if not already present
            from_int = name_dob_to_int.size();
            name_dob_to_int[from] = from_int;
            int_to_name_dob[from_int] = from;
        }
        //Repeat process for target node
        if(name_dob_to_int.find(to) != name_dob_to_int.end()){//If the key is already in the map
            to_int = name_dob_to_int[to];
        }else{//Add name to map if not already present
            to_int = name_dob_to_int.size();
            name_dob_to_int[to] = to_int;
            int_to_name_dob[to_int] = to;
        }

        //Add the new edge to the adjacency list
        bool edge_already_exists = false;
        for(int i = 0; i < adjacency_list[from_int].size(); i++){
            if(adjacency_list[from_int][i].first == to_int) {//Increment weight if the edge already exists
                adjacency_list[from_int][i].second++;
                edge_already_exists = true;
            }
        }
        if(edge_already_exists){
            for(int i = 0; i < adjacency_list[to_int].size(); i++){
                if(adjacency_list[to_int][i].first == from_int){//Increment weight if the edge already exists
                    adjacency_list[to_int][i].second++;
                    return;
                }
            }
        }
        //Add new edges with weight 1 if connection does not already exist
        adjacency_list[from_int].push_back({to_int, 1.0f});
        adjacency_list[to_int].push_back({from_int, 1.0f});

    }

    vector<string> bfs(string from, string to){
        vector<string> path;
        //Return vector with message if either one of the players are not in the graph
        if(name_dob_to_int.find(from) == name_dob_to_int.end() || name_dob_to_int.find(to) == name_dob_to_int.end()){
            path.push_back("player not found");
            return path;
        }
        int from_int = name_dob_to_int[from], to_int = name_dob_to_int[to];
        queue<int> q;
        unordered_map<int, int> predecessors; //To build the full path after reaching destination
        unordered_set<int> visited;

        q.push(from_int);
        visited.insert(from_int);
        predecessors[from_int] = -1;

        while(!q.empty()){
            int curr = q.front();
            q.pop();
            for(pair<int, float> edge : adjacency_list[curr]){
                if(visited.find(edge.first) == visited.end()){
                    if(edge.first == to_int){//Reached the target vertex
                        path.push_back(to);
                        while(curr != -1){//Add players along the path
                            path.push_back(int_to_name_dob[curr]);
                            curr = predecessors[curr];
                        }
                        //First element of return vector indicates outcome
                        //path.push_back("successful");
                        cout << "successful bfs find" << endl;
                        //Reverse the order of vertices in the path because they were added back to front
                        reverse(path.begin(), path.end());

                        for(auto s: path){
                            cout << s << " ";
                        }
                        cout << endl;
                        return path;
                    }
                    //At a vertex along the path
                    q.push(edge.first);
                    visited.insert(edge.first);
                    predecessors[edge.first] = curr;
                }

            }
        }
        //Return vector with message if no path was found
        path.push_back("no path exists");
        return path;
    }

    void displayGraph(){
        for(pair<string, int> player : name_dob_to_int){
            cout << player.first << ", " << player.second << " : ";
            for(pair<int,float> edge : adjacency_list[player.second]){
                cout << int_to_name_dob[edge.first] << ", ";
            }
            cout << endl;
        }
    }

    void read_csv(string filepath){
        //open the file

        ifstream file(filepath);
        string line;

        //gets rid of first line "from, to"

        getline(file, line);

        //read the file line by line
        while(getline(file, line)){
            int index = line.find(',');
            string from = line.substr(0, index);
            string to = line.substr(index + 1);
            this->insertEdge(from, to);
        }

        file.close();
    }

    //got code from stepik problem
    vector<string> dijkstra(string start, string end){
        vector<string> path;
        unordered_map<int, int> prevNode;
        unordered_map<int, float> distance;

        //pair<int, int> : distance, node
        priority_queue<pair<float, int>, vector<pair<float, int>>, greater<pair<float,int>> > pq;
        int sourceNum = name_dob_to_int[start];
        int endNum = name_dob_to_int[end];

        if (name_dob_to_int.find(start) == name_dob_to_int.end() || name_dob_to_int.find(end) == name_dob_to_int.end()) {
            cout << "player not found" << endl;
            path.push_back("player not found");
            return path;
        }


        //setting all distances and prevNodes
        for(auto node: name_dob_to_int){

            prevNode[node.second] = -1;
            distance[node.second] = INT_MAX;
        }

        pq.push(make_pair(0, sourceNum));
        distance[sourceNum] = 0;

        while(!pq.empty()){
            pair<int, int> curr = pq.top();
            pq.pop();

            //get current node
            int u = curr.second;

            //look at each neighbor
            for(auto it: adjacency_list[u]){
                int v = it.first; // the neighbor
                float w = 1/(it.second * it.second); //the inverted weight: The larger the weight the closer the nodes should be

                if(distance[v] > distance[u] + w){
                    distance[v] = distance[u] + w;
                    prevNode[v] = u;
                    pq.push(make_pair(distance[v], v));
                }
            }
        }

        // Making path from end to start using prevNode map
        for (int at = endNum; at != -1; at = prevNode[at]) {
            path.push_back(int_to_name_dob[at]);
        }
        reverse(path.begin(), path.end());

        return path;

    }
};