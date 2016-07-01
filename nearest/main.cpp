//
//  main.cpp
//  kdNearestNeighbours
//
//  Created by shashank hegde on 6/11/16.
//  Copyright © 2016 shashank hegde. All rights reserved.
//

#include <iostream>
#include <sstream>
#include <fstream>
#include "kdTree.hpp"
#include "KDNode.hpp"
using namespace std;

void usage(const char* binaryName){
  cout << "Usage: " << binaryName <<  " <fullPath/to/sample_data.csv> <fullPath/to/query_data.csv> <fullPath/to/queryResultSaveLocation.txt" << endl;
}


// parses the sampleFile to form the KDTree which is returned back.
// throws fileException if it cannot open the file
KDTree<double, 3>* build_KDTree(const char* sampleFileName){
    vector<point<double, 3>> samplePoints;
    KDTree<double, 3> *kdTree = NULL;
    ifstream ifstr(sampleFileName);

        if(ifstr.is_open()){
            string tmp;
            while(getline(ifstr, tmp)){
                istringstream sstr(tmp);
                double a, b, c;
                string s;
                getline(sstr, s, ',');
                a = stod(s);
                getline(sstr, s, ',');
                b = stod(s);
                getline(sstr, s, ',');
                c = stod(s);
                point<double, 3> pt = {a, b, c};
                samplePoints.emplace_back(pt);
            }
            ifstr.close();
            kdTree =  new KDTree<double, 3>(samplePoints);
        }
        else{
            throw fileException("Could not open file [" + string(sampleFileName));
    }
    return kdTree;
}

/*
 * linearly calculates euclidean distance from indexed queryPoint to all sample points, getting the nearest point
 * and then runs KDTree-NearestPoint to validate value are same
 * parameters:  sampleFileName for forming KDTree samplePoints, queryFileName for query points, index of the query point from file to validate for
 */
void checkDistance(const char* sampleFileName, const char* queryFileName, int index){
    vector<point<double, 3>> samplePoints;
    ifstream ifstr(sampleFileName);
    
    if(ifstr.is_open()){
        string tmp;
        while(getline(ifstr, tmp)){
            istringstream sstr(tmp);
            double a, b, c;
            string s;
            getline(sstr, s, ',');
            a = stod(s);
            getline(sstr, s, ',');
            b = stod(s);
            getline(sstr, s, ',');
            c = stod(s);
            point<double, 3> pt = {a, b, c};
            samplePoints.emplace_back(pt);
        }
        ifstr.close();
        point<double, 3> qpt;
        ifstream ifstr2(queryFileName);
        if(ifstr2.is_open()){
            string tmp;
            int qCntr = 1;
            while(std::getline(ifstr2, tmp)){
                istringstream sstr(tmp);
                double a, b, c;
                string s;
                getline(sstr, s, ',');
                a = stod(s);
                getline(sstr, s, ',');
                b = stod(s);
                getline(sstr, s, ',');
                c = stod(s);
                qpt = {a, b, c};
                if(qCntr == index){
                    break;
                }
                qCntr++;
            }

        }
        ifstr2.close();
        KDTree<double, 3>* kdtree = new KDTree<double, 3>(samplePoints);
        point<double, 3> nPt ={0,0,0};
        double bestDistance = numeric_limits<double>::max();
        for(auto& spt:samplePoints){
            double res = findDistance(spt, qpt);
            if(res < bestDistance){
                nPt= spt;
                bestDistance = res;
            }
        }
        cout << "brute force query1_closest_pt" << pointToStr(&nPt) << "with distance=" << bestDistance << endl;
        KDNode<double, 3> *kdnode = kdtree->nearestNeighbour(qpt);
        cout << "kdTree parse query1_closet_pt" << pointToStr(kdnode->nodePoint) << endl;
    }
    

}


// parses the queryFile, searches for nearest neighbour using the kdtree, and writes to ostream ptr if its not null or to console if not.
// throws fileException if it cannot access queryFileName path
void query_kDTree(const char* queryFileName, const KDTree<double, 3>* kdtree, ofstream* ostr=NULL){
    // if ostr is null then use standard outstream
    ifstream ifstr(queryFileName);
    if(ifstr.is_open()){
        string tmp;
        int qCntr = 1;
        while(std::getline(ifstr, tmp)){
            istringstream sstr(tmp);
            double a, b, c;
            string s;
            getline(sstr, s, ',');
            a = stod(s);
            getline(sstr, s, ',');
            b = stod(s);
            getline(sstr, s, ',');
            c = stod(s);
            point<double, 3> pt = {a, b, c};
            KDNode<double, 3> *nearestPt = kdtree->nearestNeighbour(pt);
            if(!ostr) {
                cout << "Query" << to_string(qCntr) << "_closest_index = " << nearestPt->indexLabel << " with point = " << pointToStr<double, 3>(nearestPt->nodePoint) << endl;
            }
            else{
                *ostr << "Query " << to_string(qCntr) << "_closest_index = " << nearestPt->indexLabel << endl;
            }
            qCntr++;
        }
        ifstr.close();
    }
    else{
        throw fileException("Could not open file [" + string(queryFileName));
    }
    
}



int main(int argc, const char * argv[]) {
    if(argc < 3){
        usage(argv[0]);
        return 1;
    }
    else{
        try
        {
            ofstream *ofstr = nullptr ;
            if(argc >= 4) // provided an output file to write query result, else we write result to console.
            {
                ofstr = new ofstream(argv[3]);
            }
            KDTree<double, 3>* kdTree = build_KDTree(argv[1]);
            if(ofstr && ofstr->is_open()){
                query_kDTree(argv[2], kdTree, ofstr);
            }
            else{
                query_kDTree(argv[2], kdTree);
            }
            
            // if we have a filePath for saving/loading KDTree, do that too.
            if(argc >4){
                kdTree->saveKDTreeToFile(argv[4]);
                kdTree->loadKDTreeFromFile(argv[4]);
            }
            
            // TEST2: Check bruteForce vs KDTree
            // Uncomment the below line, to test brute force linear euclidean distance check with KDTree nearest neighbour. last parameter is index of query from queryFile.
            // checkDistance(argv[1], argv[2], 2);
            
            
            // TEST3: check saving and loading KDTree
            // Uncomment below section to run the nearestNeighbour using KDTree created by loading from flatFile.
            /*
            
            *ofstr << "*****************query using loadKDTreeFromFile *****************************" << endl;
            
            if(ofstr && ofstr->is_open()){
                query_kDTree(argv[2], kdTree, ofstr);
            }
            else{
                query_kDTree(argv[2], kdTree);
            }
             */
            
            
            // close result file if one was opened.
            if(ofstr){
                ofstr->close();
            }
            
        }
        catch(exception ex){
            cout << ex.what() << endl;
            return 1;
        }
    }
    
    return 0;
}
