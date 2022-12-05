#include <cmath>
#include <algorithm>
#include <cfloat>
#include "assert.h"

class RTree {

    int max, min;
    lNode* root;

public:
    
    RTree(int m) {
        min = m/2;
        max = m;
        root = new lNode(max);
    }

    float calculateCombinedArea(lRange* r1, lRange* r2) {
        float xMin = std::min(r1->x[0], r2->x[0]);
        float xMax = std::max(r1->x[1], r2->x[1]);
        float yMin = std::min(r1->y[0], r2->y[0]);
        float yMax = std::max(r1->y[1], r2->y[1]);
        return (xMax - xMin) * (yMax - yMin);
    }

    void combineRange(lRange *r1, Point* p) {
        r1->x[0] = std::min(r1->x[0], p->x);
        r1->x[1] = std::max(r1->x[1], p->x);
        r1->y[0] = std::min(r1->y[0], p->y);
        r1->y[1] = std::max(r1->y[1], p->y);

        r1->area = (r1->x[1] - r1->x[0]) * (r1->y[1] - r1->y[0]);
    }

    void combineRange(lRange* r1, lRange* r2) {

        r1->x[0] = std::min(r1->x[0], r2->x[0]);
        r1->x[1] = std::max(r1->x[1], r2->x[1]);
        r1->y[0] = std::min(r1->y[0], r2->y[0]);
        r1->y[1] = std::max(r1->y[1], r2->y[1]);

        r1->area = (r1->x[1] - r1->x[0]) * (r1->y[1] - r1->y[0]);
    }

    void insertPoint(Point p) {

        cout << "Inserting point: " <<endl;
        p.display();
        
        lRange range = lRange((float[]){floor(p.x), floor(p.x + 1)}, (float[]){floor(p.y), floor(p.y + 1)});

        cout << "Inserting Range: " << endl;
        range.display();

       _insertRange(root, range, p);
       cout << endl << "Insertion Complete" << endl;
    }

    lNode* _insertRange(lNode* node, lRange range, Point p) {

        /*
            if node is leaf => insert
            else
                for all elements of node:
                    find which one has min area enlargement
                        In case of ties select with smallest area
        
            
        */

       //combineRange(&node->mbr, &range);

        if(node->isLeaf) { // If it a leaf node
            cout << endl << "it a leaf node" << endl;
            for(lRange& r: node->ranges) {
                if(r.equals(range)) { // Range already exists at leaf node
                    r.insertPoint(p);
                    return NULL;
                } 
            }
            range.insertPoint(p);
            
            node->insertRange(range);

            
        } else { // It is an internal node
            cout << "It is an internal node" << endl;
            float minAreaChanges = FLT_MAX;
            float rangeArea;
            lRange* optimalRange;
            for(lRange& r: node->ranges) {
                float areaChanges = calculateCombinedArea(&r, &range) - r.area;
                if(areaChanges < minAreaChanges || (areaChanges == minAreaChanges && r.area < rangeArea)) {
                    minAreaChanges = areaChanges;
                    optimalRange = &r;
                    rangeArea = r.area;
                }
            }
            combineRange(optimalRange, &range);
            /*cout << "Debug*****: " <<endl;
            optimalRange->display();

            cout << "*****Debug: " <<endl;
            */
            lNode* newNode = _insertRange(optimalRange->childLNode, range, p);

            if(newNode != NULL) {
                cout <<endl << "Debug: new node is not null"<<endl;
                /*lNode* updatedOptimalChildNode = optimalRange->childLNode;
                optimalRange = &optimalRange->childLNode->mbr;
                optimalRange->childLNode = updatedOptimalChildNode;
                */
                optimalRange->updateRange(&newNode->mbr);
                optimalRange->childLNode = &newNode[0];
                node->addChild(&newNode[1]);

                /*cout << "Debug*****: " <<endl;
                optimalRange->display();

                cout << "*****Debug: " <<endl;
                */
            }
            //combineRange(optimalRange, &range);
            combineRange(&node->mbr, &range);
            

            
            

        }
        if(node->size() > max) { // Inserting the range has exceeded the maximum capacity at node. // Common for both leaf and internal nodes.
        
            
            cout << "It is overloaded at number of nodes: " << node-> size() << endl;

            lNode* newNodes = splitNode(node);

            cout << "Nodes are split" << endl;
            newNodes[0].display();
            newNodes[1].display();

            if(node->isLeaf == false) {
                newNodes[0].isLeaf = false;
                newNodes[1].isLeaf = false;
            }

            if(node == root) {
                cout << endl << "Split at root" << endl;
                root = new lNode;
                root->isLeaf = false;
                root->addChild(&newNodes[0]);
                root->addChild(&newNodes[1]);
            } else {
                cout << endl << "Split at internal node" << endl;
                //node = &newLeafNodes[0];
                return newNodes;
            }
        }
        return NULL;
    }

    lNode* splitNode(lNode* node) {
        //cout << endl << "Inside Split nodes" << endl;
        lNode* seeds = pickSeeds(node);

        cout << endl << "Seeds picked: " << endl;
        cout << "Seed 1: " << endl;
        seeds[0].display();

        cout << "Seed 2: " << endl;
        seeds[1].display();

        vector<lRange> ranges = node->ranges;
        for(int i = node->size(); i > 0; i--) {
            if(seeds[0].size() + i  <= min) {
                //cout << "Taking min: " << " seeds0 size: " << seeds[0].size();
                seeds[0].insertRange(ranges[i-1]);
            } else if(seeds[1].size() + i  <= min) {
                //cout << "Taking min: " << " seeds1 size: " << seeds[1].size();
                seeds[1].insertRange(ranges[i-1]);
            } else {
                lRange nextRange = pickNext(node, seeds);
                
                cout << "Next Range picked: " << endl;
                nextRange.display();
                
                addToGroup(&seeds[0], &seeds[1], &nextRange);
            }
        }
        //cout << "Leaving splitNodes" << endl;
        return seeds;
    }

    /*
    Pick two lRanges to split using Quadratic splitting.
    Remove those two ranges from the node itself.
    */
    lNode* pickSeeds(lNode* node) {
        //cout << endl << "Inside pickSeeds" << endl;
        
        lNode* seeds = new lNode[2];
        int sz = node->size();
        int maxIndex1, maxIndex2;
        float maxAreaAdded = -FLT_MAX;
        
        //cout << "   Step 1" << endl;
        
        for(int i = 0; i < sz; i++) {
            lRange* range1 = &node->ranges[i];
            for(int j = i+1; j < sz; j++) {
                lRange* range2 = &node->ranges[j];
                float areaAdded = calculateCombinedArea(range1, range2) - range1->area - range2->area;

                if(areaAdded > maxAreaAdded) {
                    maxIndex1 = i; maxIndex2 = j; maxAreaAdded = areaAdded;
                }
            }
        }

        //cout << "   Step 2" << endl;

        seeds[0].insertRange(node->ranges[maxIndex1]);
        seeds[1].insertRange(node->ranges[maxIndex2]);

        //cout << "   Step 3" << endl;

        if(maxIndex1 > maxIndex2) {
            node->ranges.erase(node->ranges.begin() + maxIndex1);
            node->ranges.erase(node->ranges.begin() + maxIndex2);
        } else {
            node->ranges.erase(node->ranges.begin() + maxIndex2);
            node->ranges.erase(node->ranges.begin() + maxIndex1);
        }

        //cout << "Leaving pickSeeds" << endl;
        
        return seeds;
    }

    /*
    Pick the next range to be inserted.
    Remove the range from the node.
    */
    lRange pickNext(lNode* node, lNode* seeds) {
        //cout << endl << "Inside pickNext" << endl;
        
        int maxIndex = 0;
        float maxArea1, maxArea2, maxAreaDiff = -FLT_MAX;
        
        for(int i = 0; i < node->size(); i++) {
            lRange* range = &node->ranges[i];

            //iRange tempRange1 = iRange({std::min(range.x[0], seeds[0].mbr.x[0]), std::max(range.x[1], seeds[0].mbr.x[1])}, {std::min(range.y[0], seeds[0].mbr.y[0]), std::max(range.y[1], seeds[0].mbr.y[1])});
            //iRange tempRange1 = iRange({std::min(range.x[0], seeds[1].mbr.x[0]), std::max(range.x[1], seeds[1].mbr.x[1])}, {std::min(range.y[0], seeds[1].mbr.y[0]), std::max(range.y[1], seeds[1].mbr.y[1])});

            float area1 = calculateCombinedArea(range, &seeds[0].mbr);
            float area2 = calculateCombinedArea(range, &seeds[1].mbr);

            if(std::abs(area1 - area2) > maxAreaDiff) {
                maxAreaDiff = std::abs(area1 - area2);
                maxIndex = i;
            }

        }

        lRange selectedRange = node->ranges[maxIndex];

        
        node->ranges.erase(node->ranges.begin() + maxIndex);
        
        //cout << "Leaving pickNext" << endl;  
        
        return selectedRange;
    }


    

    void addToGroup(lNode* group1, lNode* group2, lRange* range) {
        //cout << endl << "Inside addToGroup" << endl;
        
        float changedArea1, changedArea2;
        changedArea1 = calculateCombinedArea(&group1->mbr, range) - group1->mbr.area ;
        changedArea2 = calculateCombinedArea(&group2->mbr, range) - group1->mbr.area ;

        if(changedArea1 < changedArea2)         group1->insertRange(range[0]);
        else if (changedArea1 > changedArea2)   group2->insertRange(range[0]);
        else {
            if(group1->mbr.area < group2->mbr.area)       group1->insertRange(range[0]);
            else if(group1->mbr.area > group2->mbr.area)  group2->insertRange(range[0]);
            else {
            if(group1->size() < group2->size())             group1->insertRange(range[0]);
                else                                        group2->insertRange(range[0]);
            }
        }
        //cout << "Leaving addToGroup" << endl;  
        
    }

    void printTree() {
        root->display();
    }

    bool validateTree() {
        //assert(1 == 0);

        assert(validateLNode(root) == true);
        return true;
    }

    bool validateLNode(lNode* node) {
        lRange combinedRange = node->ranges[0];
        for(lRange r: node->ranges) {
            assert(validateRange(&r) == true);
            
            combineRange(&combinedRange, &r);
        }
        //assert(combinedRange.equals(node->mbr));
        if(!combinedRange.equals(node->mbr)) {
            cout << endl << "Assertion failed at: " << endl;
                
            node->display();
            //node->mbr.display();
            return false;
        }

        return true;
    }

    bool validateRange(lRange* r) {
        
        if(r->childLNode == nullptr) { // leafnode
            if(r->points.size() == 0) return false;

            lRange combinedRange = lRange((float[]) {r->points[0].x, r->points[0].x}, (float[]) {r->points[0].y, r->points[0].y});
            for(Point p: r->points) {
                combineRange(&combinedRange, &p);
            }

            assert(isContained(r, &combinedRange) == true);

        } else {
            if(r->points.size() > 0) return false;

            assert(isContained(r, &r->childLNode->mbr) == true);
            /*if (!isContained(r, &r->childLNode->mbr)) {
                cout << endl << "Assertion failed at: " << endl;
                r->display();
                cout << endl;
                return false;
            }*/

            return validateLNode(r->childLNode);

        }

        return true;
    }

    bool isContained(lRange *lr, lRange* sr) {
        if(lr->x[0] <= sr->x[0] && lr-> y[0] <= sr->y[0] && lr->x[1] >= sr->x[1] && lr->y[1] >= sr->y[1]) return true;
        else return false;
    }
};