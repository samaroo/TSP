//1761414855B69983BD8035097EFBD312EB0527F0
#include <limits>
#include <cmath>
#include <iomanip>
#include "Engine.h"

using namespace std;

Engine::Engine(){}

void Engine::readVertices(istream &in){
    int numVertices, row, col;
    string trash;

    in >> numVertices;
    getline(in, trash);

    if(mode == Mode::A){
        verticesAndTypes.resize(numVertices);

        for(int i = 0; i < numVertices; i++){
            in >> row >> col;
            getline(in, trash);
            Vertex v(row, col);
            //adds a vertex and what location its in
            verticesAndTypes[i] = {v, v.determineLocation()};
        }
    }
    else{
        vertices.resize(numVertices);
        for(int i = 0; i < numVertices; i++){
            in >> row >> col;
            getline(in, trash);
            Vertex v(row, col);
            //adds vertex
            vertices[i] = v;
        }
    }
    if(mode == Mode::C){
        pathWeight = 0;

        path.resize((int)vertices.size());
        //fill path with 0-V (path can be anything as long as 0 is first)
        for(int i = 0; i < (int)vertices.size(); i++)
            path[i] = i;

        //fill unvisited with 1-V (excluding 0)
        unvisited = vertices;
        unvisited.erase(unvisited.begin());

        //initialize best seen cost with hueristic
        bestSeenCost = arbitraryInsertion(false);
    }
    

}

double Engine::determineDistance(Vertex one, Vertex two){
    //if the vertices are the same, they should not be connected
    if(one == two)
        return 0;
    return sqrt(pow((one.row - two.row), 2) + pow((one.col - two.col), 2));
}

double Engine::determineDistance_GraphA(Vertex one, TypeOfVertex type1, Vertex two, TypeOfVertex type2){
    //if one of the vertices are outside the medical campus and one is outside
    if(type1 == TypeOfVertex::IN_MEDICAL && type2 == TypeOfVertex::OUT_MEDICAL){
        return numeric_limits<double>::infinity();
    }
    //^^^
    else if(type2 == TypeOfVertex::IN_MEDICAL && type1 == TypeOfVertex::OUT_MEDICAL){
        return numeric_limits<double>::infinity();
    }
    return determineDistance(one, two);
}

void Engine::primsAlgo(){
    //holds the current distance from the nearest innie for each vertex
    vector<double> distanceFromNearestInnie((int)verticesAndTypes.size(), numeric_limits<double>::infinity());
    //holds index of the connected innie for each vertex
    vector<int> nearestInnie((int)verticesAndTypes.size(), -1);
    //tells if a vertex is in the MST
    vector<bool> inMST((int)verticesAndTypes.size(), false);
    vector<pair<int, int>> MST;

    //flags
    bool inMedFound = false;
    bool outMedFound = false;
    bool onBorderFound = false;

    int currentVertex = 0;
    nearestInnie[0] = 0;
    distanceFromNearestInnie[0] = 0;
    double min = numeric_limits<double>::infinity();
    int indexOfMin = -1;
    double totalCost = 0;

    //each iteration finds an edge in the mst
    for(int i = 0; i < (int)verticesAndTypes.size(); i++){

        inMST[currentVertex] = true;

        if(verticesAndTypes[i].second == TypeOfVertex::IN_MEDICAL){
            inMedFound = true;
        }
        else if(verticesAndTypes[i].second == TypeOfVertex::ON_BORDER){
            onBorderFound = true;
        }
        else if(verticesAndTypes[i].second == TypeOfVertex::OUT_MEDICAL){
            outMedFound = true;
        }

        //find the distances between the currentVertex and every other node
        for(int j = 0; j < (int)verticesAndTypes.size(); j++){
            if(inMST[j])
                continue;

            double weight = determineDistance_GraphA(
                verticesAndTypes[j].first, 
                verticesAndTypes[j].second, 
                verticesAndTypes[currentVertex].first, 
                verticesAndTypes[currentVertex].second);
            if(distanceFromNearestInnie[j] > weight){
                distanceFromNearestInnie[j] = weight; 
                nearestInnie[j] = currentVertex;
            }
        }
        //find minimum non-innie distance and make it the next node
        for(int k = 0; k < (int)distanceFromNearestInnie.size(); k++){
            if(inMST[k])
                continue;
            if(min > distanceFromNearestInnie[k]){
                min = distanceFromNearestInnie[k];
                indexOfMin = k;
            }
        }
        if(min == numeric_limits<double>::infinity())
            min = 0;
        totalCost += min;
        MST.push_back({currentVertex, nearestInnie[currentVertex]});
        currentVertex = indexOfMin;
        min = numeric_limits<double>::infinity();
    }


    if(inMedFound && outMedFound && !onBorderFound){
        cerr << "Cannot construct MST";
        exit(1);
    }

    cout << totalCost << endl;
    for(int i = 1; i < (int)MST.size(); i++){
        cout << (MST[i].first > MST[i].second ? MST[i].second : MST[i].first) << " " << (MST[i].first > MST[i].second ? MST[i].first : MST[i].second) << endl;
    }
}

//helper function for arbitraryInsertion that finds the right edge for the algo
int Engine::minimizingEdge(vector<pair<int, double>> &edges, int k){

    double min = 
    determineDistance(vertices[k], vertices[0]) + 
    determineDistance(vertices[k], vertices[edges[0].first]) -
    edges[0].second;

    int indexOfMin = 0;

    //go through the edges and find the edge that minimizes cik + ckj − cij
    for(int i = 1; i < (int)edges.size(); i++){
        //if i hasnt been considered yet, it is not part of the solution
        if(edges[i].first == -1)
            continue;

        double cik = determineDistance(vertices[k], vertices[i]);
        double ckj = determineDistance(vertices[k], vertices[edges[i].first]);
        double cji = edges[i].second;

        if(cik + ckj - cji < min){
            min = cik + ckj - cji;
            indexOfMin = i;
        }
    }

    return indexOfMin;
}

double Engine::arbitraryInsertion(bool forPartB){
    //edges[v] will give the vertex v is connected to in the cycle (first) and its distance from it (second)
    edges.resize((int)vertices.size(), {-1,-1});

    double totalCost = 0;

    double min = determineDistance(vertices[0], vertices[1]);
    int indexOfMin = 1;
    //finds closest vertex to 0 manually
    for(int i = 2; i < (int)vertices.size(); i++){
        double distance = determineDistance(vertices[0], vertices[i]);
        if(distance < min){
            min = distance;
            indexOfMin = i;
        }
    }
    //mark which edge should be connected to 0
    edges[0] = {indexOfMin, min};
    edges[indexOfMin] = {0, min};
    totalCost += min * 2;

    //run arbitrary insertion tsp hueristic
    for(int i = 0; i < (int)vertices.size(); i++){
        //if i is already in the solution skip this i
        if(edges[i].first != -1)
            continue;
        
        int indexOfMinimizingtEdge = minimizingEdge(edges, i);

        //inject "i" in between the two vertices in the minimizing edge
        totalCost -= edges[indexOfMinimizingtEdge].second;
        totalCost += determineDistance(vertices[i], vertices[edges[indexOfMinimizingtEdge].first]);
        totalCost += determineDistance(vertices[indexOfMinimizingtEdge], vertices[i]);
        edges[i] = {edges[indexOfMinimizingtEdge].first, determineDistance(vertices[i], vertices[edges[indexOfMinimizingtEdge].first])};
        edges[indexOfMinimizingtEdge] = {i, determineDistance(vertices[indexOfMinimizingtEdge], vertices[i])};
        
    }
    if(forPartB){
        cout << totalCost << endl;
        pathPartB.resize((int)vertices.size(), -1);
        pathPartB[(int)pathPartB.size()-1] = edges[0].first;
        for(int i = (int)pathPartB.size()-2; i >= 0; i--){
            pathPartB[i] = edges[pathPartB[i+1]].first;
        }
        for(auto i : pathPartB){
            cout << i << " ";
        }
    }
    return totalCost;
}

double Engine::partialPrim(){
    //holds the current distance from the nearest innie for each vertex
    vector<double> distanceFromNearestInnie((int)unvisited.size(), numeric_limits<double>::infinity());
    //holds index of the connected innie for each vertex
    vector<int> nearestInnie((int)unvisited.size(), -1);
    //tells if a vertex is in the MST
    vector<bool> inMST((int)unvisited.size(), false);

    int currentVertex = 0;
    nearestInnie[0] = 0;
    distanceFromNearestInnie[0] = 0;
    double min = numeric_limits<double>::infinity();
    int indexOfMin = -1;
    double totalCost = 0;

    //each iteration finds an edge in the mst
    for(int i = 0; i < (int)unvisited.size(); i++){

        inMST[currentVertex] = true;

        //find the distances between the currentVertex and every other node
        for(int j = 0; j < (int)unvisited.size(); j++){
            if(inMST[j])
                continue;

            double weight = determineDistance(unvisited[j], unvisited[currentVertex]);

            if(distanceFromNearestInnie[j] > weight){
                distanceFromNearestInnie[j] = weight; 
                nearestInnie[j] = currentVertex;
            }
        }
        //find minimum non-innie distance and make it the next node
        for(int k = 0; k < (int)distanceFromNearestInnie.size(); k++){
            if(inMST[k])
                continue;
            if(min > distanceFromNearestInnie[k]){
                min = distanceFromNearestInnie[k];
                indexOfMin = k;
            }
        }
        if(min == numeric_limits<double>::infinity())
            min = 0;
        totalCost += min;
        currentVertex = indexOfMin;
        min = numeric_limits<double>::infinity();
    }

    return totalCost;
}

bool Engine::promising(size_t permLength){
    if((int)path.size() - (int)permLength < 5){
        return true;
    }
    double estimate = partialPrim();
    if(estimate >= bestSeenCost)
        return false;
    double min1 = determineDistance(vertices[path[0]], vertices[path[(int)permLength]]);
    double min2 = determineDistance(vertices[path[(int)permLength - 1]], vertices[path[(int)permLength]]);
    for(int i = (int)permLength + 1; i < (int)path.size(); i++){
        double consider1 = determineDistance(vertices[path[0]], vertices[path[i]]);
        if(consider1 < min1)
            min1 = consider1;
        double consider2 = determineDistance(vertices[path[(int)permLength - 1]], vertices[path[i]]);
        if(consider2 < min2)
            min2 = consider2;
    }
    return estimate + min1 + min2 + pathWeight <= bestSeenCost;
}

void Engine::genPerms(size_t permLength) {


    if ((int)permLength == (int)path.size()) {
        double lastEdge = determineDistance(vertices[path[(int)permLength -1]], vertices[0]);
        if(pathWeight + lastEdge <= bestSeenCost){
            bestSeenCost = pathWeight + lastEdge;
            bestSeenPath = path;
        } 
        return;
    }

    if (!promising(permLength))
        return;

    for (size_t i = permLength; i < path.size(); ++i) {
        swap(path[permLength], path[i]);
        update((int)permLength);
        genPerms(permLength + 1);
        revert((int)permLength);
        swap(path[permLength], path[i]);
    }
}

void Engine::OPTTSP(){
    genPerms(1);
    cout << bestSeenCost << '\n';
    for(auto i : bestSeenPath){
        cout << i << " ";
    }
}

void Engine::FASTTSP(){
    arbitraryInsertion(true);
}


void Engine::update(int permLength){
    double tempDist = determineDistance(vertices[path[permLength]], vertices[path[permLength - 1]]);
    unvisited.erase(std::remove(unvisited.begin(), unvisited.end(), vertices[path[permLength]]), unvisited.end());
    pathWeight += tempDist;
}

void Engine::revert(int permLength){
    double tempDist = determineDistance(vertices[path[permLength]], vertices[path[permLength - 1]]);
    unvisited.push_back(vertices[path[permLength]]);
    pathWeight -= tempDist;
}

void Engine::get_options(int argc, char **argv) {
    int option_index = 0, option = 0;

    // display getopt error messages about options
    opterr = true;

    // use getopt to find command line options
    struct option longOpts[] = {{"mode", required_argument, nullptr, 'm'},
                                {"help", no_argument, nullptr, 'h'},
                                {nullptr, 0, nullptr, '\0'}};

    string s;

    while ((option = getopt_long(argc, argv, "m:h", longOpts,
                                 &option_index)) != -1) {
      switch (option) {
        case 'm':
          s = optarg;

          if(s == "MST"){
              mode = Mode::A;
          }
          else if(s == "FASTTSP"){
              mode = Mode::B;
          }
          else if(s == "OPTTSP"){
              mode = Mode::C;
          }
          break;

        case 'h':
          cout << "You actually thought you'd get help, come back when u get a brain";
          exit(0);

        default:
          string error = "Invalid flag";
          throw error;
      }
    }
}

void Engine::run(){
    if(mode == Mode::A){
        primsAlgo();
    }
    else if(mode == Mode::B){
        FASTTSP();
    }
    else{
        OPTTSP();
    }
}

int main(int argc, char **argv) {

    ios::sync_with_stdio(false);

    cout << fixed << setprecision(2);

    Engine e;

    e.get_options(argc, argv);

    e.readVertices(cin);

    e.run();

}