//
//  kdTree.h
//  kdNearestNeighbours
//
//  Created by shashank hegde on 6/11/16.
//  Copyright © 2016 shashank hegde. All rights reserved.
//

#ifndef KdTree_h
#define KdTree_h
#include "KDNode.hpp"
#include <iostream>
#include <fstream>
using namespace std;

template<typename valType, size_t dim>
class KDTree{
public:
    KDTree(){ root = NULL;} // default constructor
    
    // constructs KDTree from spacial vectors
    KDTree(vector<point<valType, dim>>& spaceVectors);
    
    // ensure the copyConstructor does deep-copy
    KDTree(const KDTree& a);
    
    
    KDNode<valType, dim>* getRoot() {return root;}
    
    ~KDTree();
    
    // method to check if point exists in KDTree.
    bool doesPointExist(const point<valType, dim>& pt, stack<KDNode<valType, dim>*>& stkNode) const;
    
    // nearest neighbour using KDTree that was constructed.
    KDNode<valType, dim>* nearestNeighbour(const point<valType, dim>& pt) const;
    
    // save the KDTree to disk.
    void saveKDTreeToFile(string fileName);
    
    // load the KDTree from disk. Replaces root with KDNode that was formed by loading the file.
    void loadKDTreeFromFile(string fileName);
    
protected:
    
    // helper functions
    
    // recursive method that constructs the KDTree by splitting into 2 halves. Uses the left and right half constructed tree and then add's itself. Returns KDNode pointer.
    KDNode<valType, dim>* createKDNode(vector<point<valType, dim>>& spaceVectors, int start, int end, int index);
    
    // helper function to recursively copy KDTree
    KDNode<valType, dim>* copyKDNode(const KDNode<valType, dim>* lhs);
    
    bool doesPointExistHelper(const point<valType, dim>& pt, stack<KDNode<valType, dim>*>& stkNode, KDNode<valType, dim>* startingNode) const;
    
    point<valType, dim>* getBetterNeigbour(const point<valType, dim>& pt, KDNode<valType, dim>* startingNode,  const point<valType, dim>& nearestNbr);
    
    // helper function to recursively delete node.
    void removeNode(KDNode<valType, dim>* curr);
    
    // helper function to postOrder Traverse the tree and save the structure to file.
    string saveKDTreeToFileHelper(ofstream* oFileStr, KDNode<valType, dim>* curr);
    
private:
    KDNode<valType, dim>* root;
    
};



template<typename valType, size_t dim>
KDTree<valType, dim>::KDTree(vector<point<valType, dim>>& spaceVectors){
    root = createKDNode(spaceVectors, 0, spaceVectors.size()-1, 0);
}

// copy constructor for KDTree. Uses copyKDNode to deepCopy from the other tree
template<typename valType, size_t dim>
KDTree<valType, dim>::KDTree(const KDTree<valType, dim>& a){
    this->root = copyKDNode(a.getRoot());
}

// recursively deep-copies all the other KDNode reference and then creates another copy of itself, using it.
template<typename valType, size_t dim>
KDNode<valType, dim>* KDTree<valType, dim>::copyKDNode(const KDNode<valType, dim>* other){
    if(!other){
        return nullptr;
    }
    KDNode<valType,dim> *left = nullptr, *right = nullptr;
    if(other->left){
        left = createKDNode(other->left);
    }
    if(other->right){
        right = createKDNode(other->right);
    }
    point<valType, dim>* currPoint = other->nodePoint ? new point<valType, dim>(*other->nodePoint) : nullptr;
    return new KDNode<valType,dim>(other->indexLabel, other->splitAxisIndex, other->splitValue, left, right, currPoint);
}

// recursively splits on the median value of the dimension with maxRange until it reaches individual spatial points. Returns KDNode leaf if start and end are equal.
template<typename valType, size_t dim>
KDNode<valType, dim>* KDTree<valType, dim>::createKDNode(vector<point<valType, dim>>& spaceVectors, int start, int end, int index)
{
    
    if(start > end){
        return nullptr;
    }
    if(start == end){
        return new KDNode<valType, dim>(index, -1, nullptr, nullptr, &(spaceVectors.at(start)));
    }
    size_t currDim = dim;
    int indexWithMaxRange=0;
    valType max = numeric_limits<valType>::min(), min = numeric_limits<valType>::max(), distance = 0;
    while(currDim--){
        lessOperator<valType, dim> comp(currDim);
        nth_element(spaceVectors.begin() + start, spaceVectors.begin()+end-1, spaceVectors.begin()+end, comp);
        max = spaceVectors.at(end).at(currDim);
        nth_element(spaceVectors.begin() + start, spaceVectors.begin() + start, spaceVectors.begin() + end, comp);
        min = spaceVectors.at(start).at(currDim);
        valType currDist = max - min;
        if(currDist > distance){
            indexWithMaxRange = currDim;
            distance = currDist;
        }
    }
    lessOperator<valType, dim> comp(indexWithMaxRange);
    sort(spaceVectors.begin() + start, spaceVectors.begin() + end, comp);
    int med = start + (end-start)/2;
    

    KDNode<valType, dim> *left = (end-start) > 1 ? createKDNode(spaceVectors,start, med-1, 2*index+1 )
                        : new KDNode<valType, dim>(2*index + 1, -1, 0, nullptr, nullptr, &spaceVectors.at(start) );
    

    KDNode<valType, dim> *right = end-start > 1 ? createKDNode(spaceVectors, med, end, 2*index + 2 )
                        : new KDNode<valType, dim>(2*index + 2, -1, 0, nullptr, nullptr, &spaceVectors.at(end));

    
     KDNode<valType, dim> *currNode = new KDNode<valType, dim>(index, indexWithMaxRange, spaceVectors.at(med).at(indexWithMaxRange), left, right, nullptr);
     return currNode;
}

// saves the KDTree to a flat file by using saveKDTreeHelper to orchestrate recursive postOrder traversal for saving.
template<typename valType, size_t dim>
void KDTree<valType, dim>::saveKDTreeToFile(string fileName){
    ofstream ofstr(fileName);
    
    if(ofstr.is_open()){
        saveKDTreeToFileHelper(&ofstr, root);
        ofstr.close();
    }
    else{
        throw fileException("could not open for file:" + fileName + " for saving KDTree");
    }


    
}


// recursively calls save on left/right childs until it reaches a leafNode. After leafNode are saved, saves parent using child references created earlier.
template<typename valType, size_t dim>
string KDTree<valType, dim>::saveKDTreeToFileHelper(ofstream *oFileStr, KDNode<valType, dim> *curr){
    string leftName("NULL"), rightName("NULL");
    if(curr->left){
        leftName = saveKDTreeToFileHelper(oFileStr, curr->left);
    }
    if(curr->right){
        rightName = saveKDTreeToFileHelper(oFileStr, curr->right);
    }
    //string KDNodeStr = nodeToStr(curr);
    string commaDel = ", ";
    *oFileStr << "{" <<  curr->indexLabel << commaDel
                << curr->splitAxisIndex << commaDel
                << curr->splitValue << commaDel
                << "ref(" << leftName << ")" << commaDel
                << "ref(" << rightName << ")" << commaDel
                << pointToStr(curr->nodePoint)
                << "}." << endl;
    
    return to_string(curr->indexLabel);
}


template<typename valType, size_t dim>
KDTree<valType, dim>::~KDTree(){
    // PostOrder traversal for deletion.
    removeNode(root);
}

// postOrder recursive traversal for deleting the childs then the parent.
template<typename valType, size_t dim>
void KDTree<valType, dim>::removeNode(KDNode<valType, dim>* curr){
    if(curr->left){
        removeNode(left);
    }
    if(curr->right){
        removeNode(right);
    }
    delete curr;
}

/*
 * checks whether the point is in the KDTree. Also populates the stack of KDNode's parsed in the process of hunting for the point. Use doesPointExistHelper for recursion.
 */
template<typename valType, size_t dim>
bool KDTree<valType, dim>::doesPointExist(const point<valType, dim>& pt, stack<KDNode<valType, dim>*>& stkNode ) const
{
    return doesPointExistHelper(pt, stkNode, root);
}

template<typename valType, size_t dim>
bool KDTree<valType, dim>::doesPointExistHelper(const point<valType, dim>& pt, stack<KDNode<valType, dim>*>& stkNode, KDNode<valType, dim>* startingNode) const
{
    KDNode<valType, dim> *curr = startingNode;
    while(curr){
        stkNode.push(curr);
        if(curr->splitAxisIndex != -1 && (curr->left || curr->right) ) // check that this is not childNode and splitIndex was set
        {
            if(pt.at(curr->splitAxisIndex) < curr->splitValue){
                curr = curr->left;
            }
            else{
                curr = curr->right;
            }
        }
        else if(!curr->left && !curr->right) // this is the child node
        {
            if((pt == *(curr->nodePoint)) ){
                return true;
            }
            else{
                return false;
            }
        }
        else{
            curr = nullptr;
        }
        
    }
    return false;
}




template<typename valType, size_t dim>
KDNode<valType, dim>* KDTree<valType, dim>::nearestNeighbour(const point<valType, dim>& pt) const
{
    // do recursive parsing to leaf node. calculate the distance of that point to testpt. push each node onto stack. do stack-rewind and then check if any of the un-visited points are closer to hyperPlane.
    stack<KDNode<valType, dim>*> stkNode;
    KDNode<valType, dim>* bestNbr = nullptr;
    if(doesPointExist(pt, stkNode)){
        return stkNode.top();
    }
    bestNbr = stkNode.top();
    stkNode.pop();
    valType bstDistance = findDistance(pt, *(bestNbr->nodePoint));
    // by stack-unWinding, check if the hyperPlane on the unexplored side is closer. If it is, then check the distance by exploring other side, using doesPointExistHelper starting from other-side node
    bool wasLeftExplored = false;
    KDNode<valType, dim>* curr = bestNbr;
    while(curr && !stkNode.empty()){
        KDNode<valType, dim>* parentNode = stkNode.top();
        stkNode.pop();
        wasLeftExplored = parentNode->left == curr ? true : false;
        
        // check if the splittingHyperAxis is intersecting the hyperSphere with radius =  distance between bestNeighbour and p pt
        if(parentNode && parentNode->splitAxisIndex != -1 && fabs(parentNode->splitValue - pt.at(parentNode->splitAxisIndex)) < bstDistance )
        {
            stack<KDNode<valType, dim>*>  dummyStack;
            KDNode<valType, dim>* unExplrdChild = wasLeftExplored ? parentNode->right : parentNode->left;
            if(unExplrdChild->splitAxisIndex == -1){
                dummyStack.push(unExplrdChild); // if node is leaf no need to explore
            }
            else{
                doesPointExistHelper(pt, dummyStack, unExplrdChild);
            }
            valType nextBestDistance = findDistance(pt, *(dummyStack.top()->nodePoint));
            
            // update the bestNeighbour(bestNbr) only if the distance from pt is lesser than original bestDistance
            if(nextBestDistance < bstDistance){
                bstDistance = nextBestDistance;
                bestNbr = dummyStack.top();
                dummyStack.pop();
            }
        }
        curr = parentNode;
    }
    
    // uncomment for debugging
    // std::cout << "best distance thru kdtree was:" << bstDistance << endl;
    
    return bestNbr;
}


// loads KDTree from the file. Replaces the root if the KDNode formed is not null.
template<typename valType, size_t dim>
void KDTree<valType, dim>::loadKDTreeFromFile(string fileName){
    
    ifstream ifstr(fileName);
    KDNode<valType, dim>* newRoot = nullptr;
    unordered_map<int, KDNode<valType, dim>*> indexToKdnode; // Used to replace indexLabel with actual KDNode reference for left and right of KDNode.
    if(ifstr.is_open()){
        string currLine;
        
        while(getline(ifstr,currLine)){
            currLine.erase(remove(currLine.begin(), currLine.end(), ',')); // removes commas
            stringstream sstr(currLine.erase(0,1));
            KDNode<valType, dim> *currNode = new KDNode<valType, dim>();
            sstr >> currNode->indexLabel;
            sstr >> currNode->splitAxisIndex;
            sstr >> currNode->splitValue;
            // loads the left and right references.
            int refIndex=0;
            while(refIndex<2){
                string nextStr;
                sstr >> nextStr;
                if(nextStr.find("NULL") != string::npos){
                    refIndex == 1 ? currNode->left = nullptr : currNode->right = nullptr;
                }
                else{
                    size_t i = nextStr.find("ref(");
                    if(i != string::npos ){
                        nextStr.erase(i, 4);
                    }
                    i = nextStr.find("),");
                    if(i != string::npos ){
                        nextStr.erase(i, 2);
                    }
                    refIndex == 0 ? currNode->left = indexToKdnode.at(stoi(nextStr)) : currNode->right = indexToKdnode.at(stoi(nextStr));
                }
                refIndex++;
            }
            
            // loads the point if its leaf.
            string pointStr;
            sstr >> pointStr;
            if(pointStr.find("NULL") != string::npos){
                currNode->nodePoint = nullptr;
            }
            else{
                size_t currDim = 0;
                point<valType, dim>* currPt = new point<valType, dim>();
                while(currDim < dim){
                    size_t i = pointStr.find("}}.");
                    if(i != string::npos ){
                        pointStr.erase(i, 3);
                    }
                    pointStr.erase(remove(pointStr.begin(), pointStr.end(), '{'));
                    
                    stringstream dsstr(pointStr);
                    dsstr >> currPt->at(currDim);
                    
                    currNode->nodePoint = currPt;
                    sstr >> pointStr;
                    currDim++;
                }
            }
            indexToKdnode[currNode->indexLabel] = currNode; // Save the KDNode with its index as key
        }
        newRoot = indexToKdnode.at(0);
    }
    
    else{
        throw fileException("In loadKDTreeeFromFile, could not open file:" + fileName);
    }
    if(newRoot){ // set the KDTree root to newRoot if it is not null
        root = newRoot;
    }
    
}

/*
template<typename valType, size_t dim>
point<valType, dim>* KDTree<valType, dim>::getBetterNeigbour(const point<valType, dim>& pt, KDNode<valType, dim>* startingNode,  const point<valType, dim>& nearestNbr)
{
    valType hyperSphereRad = findDistance(pt, nearestNbr);
    stack<KDNode<valType, dim>*> currStk;
    currStk.push(startingNode);
    while(!currStk.empty()){
        KDNode<valType, dim>* currNode = currStk.top();
        currStk.pop();
        if(!currNode)
            
        if(currNode->indexAxis == -1){
            return currNode->nodePoint;
        }
        bool wasChildLeft = abs(pt.at(currNode->splittingHyperAxis) - currNode->splittingValue) < hyperSphereRad ? false : true;
        
    }
    return nullptr;
    
}
*/

#endif /* KdTree_h */
