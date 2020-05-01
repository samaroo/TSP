//1761414855B69983BD8035097EFBD312EB0527F0
#pragma once
#include <algorithm>
#include <unordered_map>
#include <iostream>
#include <string>
#include <vector>
#include <getopt.h>
#include <unordered_map>
#include <map>
#include <queue>
#include <vector>
#include "getopt.h"

using namespace std;

enum Mode{
    A,
    B,
    C
};

enum TypeOfVertex{
    IN_MEDICAL,
    OUT_MEDICAL,
    ON_BORDER
};

struct Vertex{

    int row;
    int col;

    Vertex(): row{0}, col{0} {};

    Vertex(int r, int c): row{r}, col{c} {};

    TypeOfVertex determineLocation(){
        if(row < 0 && col < 0){
            return TypeOfVertex::IN_MEDICAL;
        }
        else if((row == 0 && col <= 0) || (row <= 0 && col == 0)){
            return TypeOfVertex::ON_BORDER;
        }
        else{
            return TypeOfVertex::OUT_MEDICAL;
        }
    }

    bool operator ==(Vertex comp){
        return (row == comp.row && col == comp.col);
    }
};

class Engine{

    private:

    Mode mode;
    //vector used for any mode other than A
    vector<Vertex> vertices;
    //vector used only for Part A
    vector<pair<Vertex, TypeOfVertex>> verticesAndTypes;
    //used in when generating Hamiltonian cycle
    vector<int> path;
    //vector where index is the vertex and holds bool, true for visited
    vector<Vertex> unvisited;
    //running total for a path weight
    double pathWeight;
    //vectors used in Prim's algo
    /*vector<double> distanceFromNearestInnie;
    vector<int> nearestInnie;
    vector<bool> inMST;*/
    //pq where each node is a pair, first being the weight and second being the corrisponsing vertex
    priority_queue<pair<double, int>, vector <pair<double, int>> , greater<pair<double, int>> > pq;
    //double vector where the the coordinates (i,j) holds the distance between i and j
    vector<vector<double>> edgeWeights;
    //used to hold edges in arbitraryInsertion
    vector<pair<int, double>> edges;
    //keep track of the lowest seen path cost
    double bestSeenCost;
    vector<int> bestSeenPath;
    vector<int> pathPartB;

    public:

    //constructs Engine with what mode
    Engine();

    //read vertices from cin
    void readVertices(istream &in);

    ////takes two vertices and determines their length
    double determineDistance(Vertex one, Vertex two);

    //takes two vertices and determines their length for GRAPH A
    //if the two points cannot be connected, will return infinity
    double determineDistance_GraphA(Vertex one, TypeOfVertex type1, Vertex two, TypeOfVertex type2);

    //calculates a MST using Prim's algo
    void primsAlgo();

    //generate a Hamiltonian cycle using a TSP hueristic
    double arbitraryInsertion(bool forPartB);

    int minimizingEdge(vector<pair<int, double>> &edges, int index);

    void genPerms(size_t permLength);

    //used to exclude branches that cant possibly be optimal during TSP brute force
    bool promising(size_t permLength);

    //does prims algorithm that only considers a certian set of vertices rather than all
    double partialPrim();

    //returns an edge cost, updates 2d vector, only used for part C
    double getEdgeCost(int vOne, int vTwo);

    void update(int permLength);
    
    void revert(int permLength);

    void OPTTSP();

    void FASTTSP();

    void get_options(int argc, char **argv);

    void run();
};