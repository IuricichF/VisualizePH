#ifndef MESH
#define MESH

#include <vector>
#include <list>
#include <stdlib.h>
#include <iostream>
#include <map>
using namespace std;

struct Mesh{

    vector<vector<float>> vertices;
    vector<vector<int>> triangles;

    vector<list<int> > vt;

    void readOFF(string name){

        ifstream input(name);

        int num_vertices, num_triangles, num_edges;

        string type;
        getline(input, type);

        input >> num_vertices >> num_triangles >> num_edges;

        if(num_vertices == 0 || num_triangles == 0){
            cerr<< "Number of simplexes is 0 Not a valid .off file "<< name <<endl;
            return;
        }

        vertices = vector<vector<float> >(num_vertices,vector<float>(3));
        triangles = vector<vector<int> >(num_triangles,vector<int>(3));
        vt = vector<list<int> >(num_vertices,list<int>());
        //insert vertices
        for(unsigned long int i=0;i<num_vertices;i++){
            input >> vertices[i][0] >> vertices[i][1] >> vertices[i][2];
        }

        //insert triangles
        for(unsigned long int i=0;i<num_triangles;i++){
            int check;
            input >> check >> triangles[i][0] >> triangles[i][1] >> triangles[i][2];

            //create vt
            vt[triangles[i][0]].push_back(i);
            vt[triangles[i][1]].push_back(i);
            vt[triangles[i][2]].push_back(i);

            assert(check == 3);
        }
    }

    void readPLY(string name){

        ifstream input(name);

        int num_vertices, num_triangles;
        string el, type, vers;

        getline(input,el); //ply
        getline(input,el); //format ascii 1.0
        input >> el >> type >> num_vertices; getline(input,el); //element vertex Nv
        getline(input,el); //property float32 x
        getline(input,el); //property float32 y
        getline(input,el); //property float32 z
        input >> vers >> type >> num_triangles; getline(input,el); //element face Nt
        getline(input,el); //property list uint8 int32 vertex_indices
        getline(input,el); //end_header


        if(num_vertices == 0 || num_triangles == 0){
            cerr<< "Number of simplexes is 0 Not a valid .off file "<< name <<endl;
            return;
        }

        vertices = vector<vector<float> >(num_vertices,vector<float>(3));
        triangles = vector<vector<int> >(num_triangles,vector<int>(3));
        vt = vector<list<int> >(num_vertices,list<int>());
        //insert vertices
        for(unsigned long int i=0;i<num_vertices;i++){
            input >> vertices[i][0] >> vertices[i][1] >> vertices[i][2];
        }

        //insert triangles
        for(unsigned long int i=0;i<num_triangles;i++){
            int check;
            input >> check >> triangles[i][0] >> triangles[i][1] >> triangles[i][2];

            //create vt
            vt[triangles[i][0]].push_back(i);
            vt[triangles[i][1]].push_back(i);
            vt[triangles[i][2]].push_back(i);

            assert(check == 3);
        }

    }
};


void readFiltration(string name, vector<int>& filtrO, vector<int>& filtrI, Mesh const& mesh){


    map<float,list<int> > trueFiltr;
    int nF, value;

    if(name == ""){
        for(int i=0; i<filtrO.size(); i++){
            if(trueFiltr.find(mesh.vertices[i][1]) == trueFiltr.end())
                trueFiltr[mesh.vertices[i][1]]=list<int>();
            trueFiltr[mesh.vertices[i][1]].push_back(i);
        }
    }
    else{
        ifstream input(name);
        input >> nF;
        for(int i=0; i<nF; i++){
            input >> value;
            if(trueFiltr.find(value) == trueFiltr.end())
                trueFiltr[value]=list<int>();
            trueFiltr[value].push_back(i);
        }
    }

    int index=0;
    for(auto theList : trueFiltr){
        for(auto l : theList.second){
            filtrO[index]=l;
            filtrI[l]=index++;
        }
    }

    cout << "done " << endl;
}

#endif // MESH

