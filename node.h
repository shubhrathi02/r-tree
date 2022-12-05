#include <iostream>
#include <array>

using std :: string;
using std :: vector;
using std :: stringstream;
using std :: cout;
using std :: endl;

class Point {
    

public:
    float x, y;
    
    Point(float x1, float y1) {
        x = x1; y = y1;
    }

    void display() {
        cout << "x : " << x << " y: " << y << endl;
    }
};

class lNode;
class iNode;


class iRange {

//protected:

public:
    float x[2], y[2];
    float area = 0;


//public:

    //lNode* child;

    iRange() {}
    iRange(float *x1, float *y1) {
        x[0] = x1[0]; x[1] = x1[1]; 
        y[0] = y1[0]; y[1] = y1[1]; 

        area = (x[1] - x[0]) * (y[1] - y[0]);
    }

    bool equals(iRange r) {
        return x[0] == r.x[0] && x[1] == r.x[1] && y[0] == r.y[0] && y[1] == r.y[1];
    }

    void display() {
        cout << "x[0] : " << x[0] << " x[1]: " << x[1] << " y[0]: " << y[0] << " y[1]: " << y[1] << endl;
    }
};

class lRange : public iRange {

private:
    vector<Point> points;

public:
    lNode* childLNode = nullptr;

    
    //lRange() {}
    lRange(float *x1, float *y1) :iRange(x1, y1){  }

    void insertPoint(Point p) {
        points.push_back(p);
        //sz += 1;
    }

    void display();

    long size() { return points.size(); }
    
};


class iNode {
    long sz;
public:
    vector<lRange> ranges;

    long size() { return ranges.size(); }
    
    iNode(int m) { }//ranges.reserve(m); }
};

class lNode {

public:
    bool isLeaf = true;  //replace with iNode and lNode inheritence

    vector<lRange> ranges;

    lRange mbr = lRange((float[]){0.0, 0.0}, (float[]){0.0, 0.0});;

    long size() { return ranges.size(); }
    lNode(int m = 4) { } //ranges.reserve(m); }

    void insertRange(lRange r) {
        ranges.push_back(r);
        
        if(mbr.area == 0) mbr = lRange(r.x, r.y);
        else {
            mbr = lRange((float[]){std::min(r.x[0], mbr.x[0]), std::max(r.x[1], mbr.x[1])}, (float[]){std::min(r.y[0], mbr.y[0]), std::max(r.y[1], mbr.y[1])}); //Save memory by updating instead of creating new instance.
        }
    }

    void addChild(lNode* childNode) {
        lRange r = childNode->mbr;
        r.childLNode = childNode;
        insertRange(r);
    }

    void display() {
        cout << "Node Range: "; mbr.display();
        cout <<"Total range intervals in the node: " << ranges.size() << endl;
        for(lRange r: ranges) {
            r.display();
        }
    }
};

void lRange:: display() {
    iRange::display();

    cout << "Points:    " << points.size() << endl;
    for(Point p: points) {
        cout << "x : " << p.x << " y: " << p.y << endl;
    }
    
    cout << endl;

    //cout << "Between" << endl;

    if(childLNode) {
        cout << "Printing Child Nodes:" << endl;
        
        childLNode->display(); 
        
        cout << "Child Node printing done" << endl;
    }

    //cout << "After" << endl;
}