#include <iostream>
#include <unordered_map>
#include <string>
#include "include/crow_all.h"
#include <vector>
#include "graph.h"
#include <nlohmann/json.hpp>
#include <sstream>
using json = nlohmann::json;
using namespace std;

// Function to parse URL-encoded form data
unordered_map<string, string> parseFormData(const string& body) {
    unordered_map<string, string> form_data;
    istringstream stream(body);
    string pair;
    while (getline(stream, pair, '&')) {
        auto position = pair.find('=');
        if (position != string::npos) {
            auto key = pair.substr(0, position);
            auto value = pair.substr(position + 1);
            form_data[key] = value;
        }
    }
    return form_data;
}

int main(){
    Graph graph;
    graph.insertEdge("Messi", "Neymar");
    graph.insertEdge("Neymar", "Vini");
    graph.insertEdge("Messi", "Neymar");
    graph.insertEdge("Neymar", "Carlos");


    graph.displayGraph();

    crow::SimpleApp app;

    CROW_ROUTE(app, "/")([](){
        return R"(
            <!DOCTYPE html>
            <html lang="en">
            <head>
                <meta charset="UTF-8">
                <meta name="viewport" content="width=device-width, initial-scale=1.0">
                <title>BFS Visualization</title>
                <script src="https://d3js.org/d3.v7.min.js"></script>
                <style>
                    .node {
                        stroke: #fff;
                        stroke-width: 3.5px;
                    }
                    .link {
                        stroke: #999;
                        stroke-opacity: 0.6;
                        stroke-width: 3
                    }
                </style>

            </head>
            <body>
                <h1>Find the connections between players</h1>
                <form id="algorithmForm">
                    <label for="start">First Player:</label>
                    <input type="text" id="start" name="start" required><br><br>
                    <label for="end">Second Player:</label>
                    <input type="text" id="end" name="end" required><br><br>
                    <input type="submit" value="Submit">
                </form>
                <div id="graphBFS"></div>
                <div id="graphDijkstra"></div>
                <script>
                    document.getElementById('algorithmForm').onsubmit = async function(event) { //states where to get inputs later
                        // will be done on submission hence "onsubmit"
                        event.preventDefault(); //by default the page refreshes
                        console.log('Form submitted');

                        // Getting values from input fields
                        let start = document.getElementById('start').value;
                        let end = document.getElementById('end').value;

                        // Sending start and end information to server
                        let bfsData = await fetchGraphData('/bfs', start, end);
                        let dijkstraData = await fetchGraphData('/dijkstra', start, end);

                        if (bfsData) visualizeGraph(bfsData, 'graphBFS');
                        if (dijkstraData) visualizeGraph(dijkstraData, 'graphDijkstra');
                    };

                    async function fetchGraphData(endpoint, start, end) {
                        let initialDataFormat = await fetch(endpoint, {  //await is used to wait for the decision of the fetch to be resolved(AKA not pending status)
                            //fetch sends request and this specifies the method for the request
                            method: 'POST',

                            headers: {
                                'Content-Type': 'application/x-www-form-urlencoded' //url encoded form data
                            },
                            body: `start=${start}&end=${end}` // sets the form of the body of the request
                        });

                        // Checking if the request was successful
                        if (!initialDataFormat.ok) {
                            console.error('Failed the fetch', endpoint);
                            return null;
                        }

                        return await initialDataFormat.json();
                    }


                    function visualizeGraph(data, graphID) {
                        document.getElementById(graphID).innerHTML = '';

                        const width = 800;
                        const height = 600;

                        const svg = d3.select(`#${graphID}`).append("svg")
                                      .attr("width", width)
                                      .attr("height", height);

                        // Convert nodes to objects with initial positions and some random jitter
                        const nodes = data.nodes.map(node => ({
                            id: node,
                            x: width / 2 + (Math.random() - 0.5) * width * 0.2,
                            y: height / 2 + (Math.random() - 0.5) * height * 0.2
                        }));

                        console.log('Nodes:', nodes);

                        // Convert links to include weight and ensure a default weight of 1 if not provided
                        const links = data.links.map(link => ({
                            source: link.source,
                            target: link.target,
                            weight: link.weight || 1
                        }));

                        console.log('Links:', links);

                        const distanceScalingFactor = 200;  // Adjust this factor to scale the link lengths more distinctly

                        const simulation = d3.forceSimulation(nodes)
                                             .force("link", d3.forceLink(links).id(d => d.id).distance(d => {
                                                 const distance = (1 / Math.sqrt(d.weight)) * distanceScalingFactor;
                                                 console.log(`Link from ${d.source.id} to ${d.target.id} with weight ${d.weight} has distance ${distance}`);
                                                 return distance;
                                             }))
                                             .force("charge", d3.forceManyBody().strength(-500)) // Stronger repulsion to avoid overlap
                                             .force("center", d3.forceCenter(width / 2, height / 2))
                                             .force("x", d3.forceX(width / 2).strength(0.05))
                                             .force("y", d3.forceY(height / 2).strength(0.05));

                        const link = svg.append("g")
                                        .attr("class", "links")
                                        .selectAll("line")
                                        .data(links)
                                        .enter().append("line")
                                        .attr("class", "link")
                                        //.attr("stroke-width", d => d.weight * 2); //Changing width based on weight if we want

                        const node = svg.append("g")
                                        .attr("class", "nodes")
                                        .selectAll("circle")
                                        .data(nodes)
                                        .enter().append("circle")
                                        .attr("class", "node")
                                        .attr("r", 20)
                                        .attr("fill", "blue");

                        simulation.on("tick", () => {
                            link.attr("x1", d => d.source.x)
                                .attr("y1", d => d.source.y)
                                .attr("x2", d => d.target.x)
                                .attr("y2", d => d.target.y);

                            node.attr("cx", d => d.x)
                                .attr("cy", d => d.y);
                        });
                    }
                </script>

            </body>
            </html>
        )";
    });

    // Route to handle BFS request
    CROW_ROUTE(app, "/bfs").methods(crow::HTTPMethod::POST)([&graph](const crow::request& req){
        auto form_data = parseFormData(req.body);
        auto start_iter = form_data.find("start");
        auto end_iter = form_data.find("end");

        cout << "Received BFS request with data: " << req.body << endl;
        if (start_iter == form_data.end() || end_iter == form_data.end()) {
            cout << "Missing start or end node" << endl;
            return crow::response(400, "Bad Request: Missing input");
        }

        string start = start_iter->second;
        string end = end_iter->second;

        cout << "Start node: " << start << ", End node: " << end << endl;

        if (graph.bfs(start, end).empty()) {
            cout << "Invalid nodes provided" << endl;

            return crow::response(400, "Bad Request: Invalid nodes");
        }

        vector<string> path = graph.bfs(start, end);

        cout << "BFS Path: ";
        for (const auto& node : path) {
            cout << node << " ";
        }
        cout << endl;

        // Preparing for visualization
        json result;
        result["nodes"] = path;
        result["links"] = json::array();
        for (size_t i = 0; i < path.size() - 1; ++i) {
            string source = path[i];
            string target = path[i + 1];
            float weight = 0;
            for (const auto& neighbor : graph.adjacency_list[graph.name_dob_to_int[source]]) {  // Change
                if (neighbor.first == graph.name_dob_to_int[target]) {
                    weight = neighbor.second;
                    break;
                }
            }
            result["links"].push_back({{"source", source}, {"target", target}, {"weight", weight}});
        }

        return crow::response(result.dump());

    });

    //Route to handle Dijkstra request
    CROW_ROUTE(app, "/dijkstra").methods(crow::HTTPMethod::POST)([&graph](const crow::request& req) {
        auto form_data = parseFormData(req.body);
        auto start_iter = form_data.find("start");
        auto end_iter = form_data.find("end");

        cout << "Received Dijkstra request with data: " << req.body << endl;
        if (start_iter == form_data.end() || end_iter == form_data.end()) {
            cout << "Missing start or end node" << endl;
            return crow::response(400, "Bad Request: Missing input");
        }

        string start = start_iter->second;
        string end = end_iter->second;

        cout << "Start node: " << start << ", End node: " << end << endl;

        if (graph.dijkstra(start, end).empty()) {
            cout << "Invalid nodes provided" << endl;
            return crow::response(400, "Bad Request: Invalid nodes");
        }

        vector<string> path = graph.dijkstra(start, end);

        cout << "Dijkstra Path: ";
        for (const auto& node : path) {
            cout << node << " ";
        }
        cout << endl;

        // Preparing for visualization
        json result;
        result["nodes"] = path;
        result["links"] = json::array();
        for (size_t i = 0; i < path.size() - 1; ++i) {
            string source = path[i];
            string target = path[i + 1];
            float weight = 0;
            for (const auto& neighbor : graph.adjacency_list[graph.name_dob_to_int[source]]) {
                if (neighbor.first == graph.name_dob_to_int[target]) {
                    weight = neighbor.second;
                    break;
                }
            }
            result["links"].push_back({{"source", source}, {"target", target}, {"weight", weight}});
        }

        return crow::response(result.dump());
    });

    app.port(1555).multithreaded().run();

}
