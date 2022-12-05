#include <cmath>
#include <algorithm>
#include <cfloat>

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

    void insertPoint(Point p) {

        cout << "Inserting point: " <<endl;
        p.display();
        
        lRange range = lRange((float[]){floor(p.x), floor(p.x + 1)}, (float[]){floor(p.y), floor(p.y + 1)});

        cout << "Inserting Range: " << endl;
        range.display();

       _insertRange(root, range, p);
    }

    lNode* _insertRange(lNode* node, lRange range, Point p) {

        /*
            if node is leaf => insert
            else
                for all elements of node:
                    find which one has min area enlargement
                        In case of ties select with smallest area
        
            
        */

        if(node->isLeaf) { // If it a leaf node
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
            for(lRange r: node->ranges) {
                float areaChanges = calculateCombinedArea(&r, &range) - r.area;
                if(areaChanges < minAreaChanges || (areaChanges == minAreaChanges && r.area < rangeArea)) {
                    minAreaChanges = areaChanges;
                    optimalRange = &r;
                    rangeArea = r.area;
                }
            }

            lNode* newNode = _insertRange(optimalRange->childLNode, range, p);
            if(newNode != NULL) {
                lNode* updatedOptimalChildNode = optimalRange->childLNode;
                optimalRange = &optimalRange->childLNode->mbr;
                optimalRange->childLNode = updatedOptimalChildNode;

                node->addChild(newNode);
            }
            

        }
        if(node->size() > max) { // Inserting the range has exceeded the maximum capacity at node. // Common for both leaf and internal nodes.
        
            
            cout << "It is overloaded at number of nodes: " << node-> size() << endl;

            lNode* newLeafNodes = splitNode(node);

            cout << "Nodes are split" << endl;
            newLeafNodes[0].display();
            newLeafNodes[1].display();
            if(node == root) {
                root = new lNode;
                root->isLeaf = false;
                root->addChild(&newLeafNodes[0]);
                root->addChild(&newLeafNodes[1]);
            } else {
                node = &newLeafNodes[0];
                return &newLeafNodes[1];
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
                
                //cout << "Next Range picked: " << endl;
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

};