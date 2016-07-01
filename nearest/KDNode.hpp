//
//  KDNode.hpp
//  kdNearestNeighbours
//
//  Created by shashank hegde on 6/11/16.
//  Copyright © 2016 shashank hegde. All rights reserved.
//

#ifndef KDNode_h
#define KDNode_h

#include <iostream>
#include <sstream>
#include <cmath>
#include <string>
#include <stack>
#include <array>
#include <vector>
#include <algorithm>
#include <functional>
#include <unordered_map>
using namespace std;

template<typename valType, size_t dim>
using point = array<valType, dim>; //using std::array for multi-dimensional point


class fileException : public exception{
public:
    fileException(){
        exceptionStr = "could not open file";
    }
    
    fileException(const string& str){
        exceptionStr = str;
    }
    
    virtual const char* what() const throw(){
        return exceptionStr.c_str();
    }
private:
    string exceptionStr;
};


// comparator defined which takes index based upon which it compares. Used for sorting point based on dimension
template<typename valType, size_t dim>
struct lessOperator{
    lessOperator(size_t ind){
        index = ind;
    }
    
    bool operator()(const point<valType, dim>& lhs, const point<valType, dim>& rhs){
        return lhs.at(index) < rhs.at(index);
    }
    size_t index;
};

// provides string representation of point.
template<typename valType, size_t dim>
string pointToStr(const point<valType, dim>* currPt){
    stringstream sstr;
    size_t currDim = dim;
    sstr << "{" << std::scientific;
    if(!currPt){
        sstr << "NULL";
    }
    else{
        while(currDim--){
            sstr << currPt->at(currDim);
            if(currDim != 0){
                sstr << ", ";
            }
        }
    }
    sstr << "}";
    return sstr.str();
}


// returns Euclidean distance between 2 points
template<typename valType, size_t dim>
valType findDistance(const point<valType, dim>& lhs, const point<valType, dim>& rhs){
    valType dist = 0.0;
    size_t dimIndex = dim;
    while(dimIndex--){
        valType t = lhs.at(dimIndex) - rhs.at(dimIndex);
        dist += t*t;
    }
    return sqrt(dist);
}


template<typename valType, size_t dim>
struct KDNode{
    int indexLabel;
    int splitAxisIndex; // hyperAxis/splittingPlane is perpendicular to the axis pointed by this index. -1 if leaf node.
    valType splitValue;
    KDNode* left;
    KDNode* right;
    point<valType, dim>* nodePoint; // set only when left and right are NULL.
    
    KDNode(int index, int sAxis, valType sValue, KDNode* l, KDNode* r, point<valType, dim>* Point){
        indexLabel = index;
        splitAxisIndex = sAxis;
        splitValue = sValue;
        left = l;
        right = r;
        nodePoint = Point;
    }
    
    KDNode() :indexLabel(-1), splitAxisIndex(-1), splitValue(0), left(nullptr), right(nullptr), nodePoint(0)
    {
    }
    
    KDNode(int index, int sAxis, KDNode* l, KDNode* r, point<valType, dim>* Point){
        indexLabel = index;
        splitAxisIndex = sAxis;
        left = l;
        right=r;
        nodePoint = Point;
    }
};


#endif /* KDNode_h */
