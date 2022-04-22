#include "Graph.hpp"

#include <sstream>
#include <fstream>

std::vector<int> get_digits(const std::string& string) {
    std::vector<int> result;
    std::stringstream ss(string);
    while (ss) {
        std::string word;
        ss >> word;
        if (word == "")  break;
        if (word.back() == ',') {
            word.pop_back();
        }
        result.push_back(std::stoi(word));
    }
    return result;
}

void Graph::load(const std::string& path) {
    std::fstream f;
    f.open(path, std::ios::in);

    while (f) {
        std::string row;
        std::getline(f, row);
        if (row.empty())  break;

        Node node;
        auto digits = get_digits(row);
        for (int i = 0; i < digits.size(); ++i) {
            if (digits[i]) {
                Connection conn;
                conn.node_index = i;
                conn.weight = digits[i];
                node.connections.push_back(conn);
            }
        }
        nodes.push_back(node);
    }
}
