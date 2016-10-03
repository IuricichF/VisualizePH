#include <iostream>

#include <phat/compute_persistence_pairs.h>

#include <phat/representations/vector_vector.h>
#include <phat/representations/vector_heap.h>
#include <phat/representations/vector_set.h>
#include <phat/representations/vector_list.h>
#include <phat/representations/sparse_pivot_column.h>
#include <phat/representations/heap_pivot_column.h>
#include <phat/representations/full_pivot_column.h>
#include <phat/representations/bit_tree_pivot_column.h>

#include <phat/algorithms/twist_reduction.h>
#include <phat/algorithms/standard_reduction.h>
#include <phat/algorithms/row_reduction.h>
#include <phat/algorithms/chunk_reduction.h>
#include <phat/algorithms/spectral_sequence_reduction.h>

#include <phat/helpers/dualize.h>

#include "mesh.h"

using namespace phat;

int realIndex(vector<int> simpl, vector<int> const& filtration){

    int filtr=0;
    for(auto v : simpl){
        if(filtr < filtration[v])
            filtr = filtration[v];
    }
    return filtr;
}


int main(int argc, char* argv[])
{

    Mesh mesh;

    if(strstr(argv[1],".off") != NULL)
        mesh.readOFF(string(argv[1]));
    else if(strstr(argv[1],".ply") != NULL)
        mesh.readPLY(string(argv[1]));
    else{
        cout << "Supported format: OFF | PLY" << endl;
        return 1;
    }

    cout << "Mesh read: " << mesh.vertices.size() << " vertices and " << mesh.triangles.size() << " triangles" << endl;

    vector<int> filtrationOrder(mesh.vertices.size()); //filtrationOrder[0] contains the index of the first vertex in the filtration order
    vector<int> filtrationIndex(mesh.vertices.size()); //filtrationIndex[0] contains the position in the order of vertex 0

    if(argc < 3)
        readFiltration("", filtrationOrder, filtrationIndex, mesh);
    else
        readFiltration(string(argv[2]), filtrationOrder, filtrationIndex, mesh);

    cout << "Done with filtration " << endl;
    map<vector<int>, int> simplToIndex;
    map<int,vector<int>> orderedSimplices;

    int totalOrder=0;
    for(int i=0; i<mesh.vertices.size(); i++){

        int vert = filtrationOrder[i];
        list<int> vt = mesh.vt[vert];

        vector<int> vVec = {vert};
        simplToIndex[vVec]=totalOrder;
        orderedSimplices[totalOrder++]=vVec;

        for(auto t : vt){
            vector<int> tri = mesh.triangles[t];
            sort(tri.begin(),tri.end());
            int pos = find(tri.begin(), tri.end(), vert) - tri.begin();

            if(filtrationIndex[tri[pos]] > filtrationIndex[tri[(pos+1)%3]]){
                vector<int> edge = {tri[pos], tri[(pos+1)%3]};
                sort(edge.begin(),edge.end());
                if(simplToIndex.find(edge) == simplToIndex.end()){
                    simplToIndex[edge]=totalOrder;
                    orderedSimplices[totalOrder++]=edge;
                }
            }

            if(filtrationIndex[tri[pos]] > filtrationIndex[tri[(pos+2)%3]]){
                vector<int> edge = {tri[pos], tri[(pos+2)%3]};
                sort(edge.begin(),edge.end());
                if(simplToIndex.find(edge) == simplToIndex.end()){
                    simplToIndex[edge]=totalOrder;
                    orderedSimplices[totalOrder++]=edge;
                }
            }

            if(filtrationIndex[tri[pos]] > filtrationIndex[tri[(pos+1)%3]] && filtrationIndex[tri[pos]] > filtrationIndex[tri[(pos+2)%3]]){
                if(simplToIndex.find(tri) == simplToIndex.end()){
                    simplToIndex[tri]=totalOrder;
                    orderedSimplices[totalOrder++]=tri;
                }
            }
        }
    }

    boundary_matrix< phat::vector_vector > bMatrix;
    bMatrix.set_num_cols( totalOrder );
    cout << totalOrder << endl;

    for(auto simplIndex : simplToIndex){
        bMatrix.set_dim( simplIndex.second , simplIndex.first.size()-1 );
    }

    for(auto simplIndex : simplToIndex){
        vector<int> simpl = simplIndex.first;
        sort(simpl.begin(),simpl.end());
        std::vector< phat::index > indices(simplIndex.first.size(),0);

        if(simpl.size()==1){
            indices.clear();
        }
        else
        {

            for(int i=0; i<simplIndex.first.size(); i++){
                vector<int> bdSimpl = simpl;
                bdSimpl.erase(bdSimpl.begin()+i);

                assert(simplToIndex.find(bdSimpl) != simplToIndex.end());
                indices[i]=simplToIndex[bdSimpl];
            }

            sort(indices.begin(), indices.end());
        }

        bMatrix.set_col( simplIndex.second, indices );
    }

    phat::persistence_pairs pairs;
    phat::compute_persistence_pairs< phat::twist_reduction >( pairs, bMatrix );
    pairs.sort();

    //std::cout << std::endl;
    //std::cout << "There are " << pairs.get_num_pairs() << " persistence pairs: " << std::endl;

    list<float> h0Values, h1Values;

    for( phat::index idx = 0; idx < pairs.get_num_pairs(); idx++ ){

        if(pairs.get_pair( idx ).second-pairs.get_pair( idx ).first!=1){

            vector<int> simpl = orderedSimplices[pairs.get_pair( idx ).first];
            vector<int> simpl2 = orderedSimplices[pairs.get_pair( idx ).second];

            if(realIndex(simpl,filtrationIndex) == realIndex(simpl2,filtrationIndex))
                continue;

            vector<float> bar1 = vector<float>(3,0);
            for(auto v : simpl){
                for(int i=0;i<3;i++)
                    bar1[i] += mesh.vertices[v][i];
            }
            for(int i=0;i<3;i++)
                bar1[i] = bar1[i]/(float)simpl.size();

            vector<float> bar2 = vector<float>(3,0);
            for(auto v : simpl2){
                for(int i=0;i<3;i++)
                    bar2[i] += mesh.vertices[v][i];
            }
            for(int i=0;i<3;i++)
                bar2[i] = bar2[i]/(float)simpl2.size();


            if(simpl.size()==1){
                h0Values.push_back(realIndex(simpl,filtrationIndex));
                h0Values.push_back(realIndex(simpl2,filtrationIndex));
                h0Values.push_back(bar1[0]);
                h0Values.push_back(bar1[1]);
                h0Values.push_back(bar1[2]);
                h0Values.push_back(bar2[0]);
                h0Values.push_back(bar2[1]);
                h0Values.push_back(bar2[2]);
            }
            else{
                h1Values.push_back(realIndex(simpl,filtrationIndex));
                h1Values.push_back(realIndex(simpl2,filtrationIndex));
                h1Values.push_back(bar1[0]);
                h1Values.push_back(bar1[1]);
                h1Values.push_back(bar1[2]);
                h1Values.push_back(bar2[0]);
                h1Values.push_back(bar2[1]);
                h1Values.push_back(bar2[2]);
            }
        }
    }


    ofstream hpairs("pairs.json");
    hpairs << "{\"pairs\": [ \n{";
    hpairs << "\"h0\": [";
    for(auto f : h0Values){
        hpairs << f << ",";
    }
    hpairs.seekp(-1,ios::cur); hpairs << "]},\n";

    hpairs << "{\"h1\": [";
    for(auto f : h1Values){
        hpairs << f << ",";
    }
    hpairs.seekp(-1,ios::cur); hpairs << "]}\n";
    hpairs << "]}";

    hpairs.close();

    return 0;
}

