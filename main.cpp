#include <iostream>
#include <vector>
#include <string>
#include "node.h"
#include "rtree.h"

using namespace std;

int main()
{
    float p[2] = {0.0, 1.0};

    //iRange in = iRange(p, p);

    //in.display();

    //lRange ln = lRange((float[]){0.0, 1.0}, (float[]){0.0, 1.0});
    //ln.insertPoint(Point(2.0, 3.0));

    //ln.display();
    
    //char a = 'b';
    //cout << a << a << endl;

    vector<int> a;


    //for(int i = 0; i < 10; i++) {

    //}


    RTree rtree = RTree(4);

    int pNumber = 1;
    for(float i = 11.0f; i <= 18.0; i+=0.25) {
        cout << (pNumber++) << ". ";
        rtree.insertPoint(Point(i, i*2.0));
    }

    cout << endl << endl << endl << "**********Printing Root *******" << endl;
    //rtree.printTree();

}