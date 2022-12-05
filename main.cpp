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

    int pNumber = 0;
    srand(42);
    for(float i = 11.0f; i <= 2000.0; i+=0.25) {
        //if(pNumber == 100) break;
        cout << (++pNumber) << ". ";
        
        /*if(pNumber == 39) {
            cout << endl << endl << endl << "**********Printing Root (Before) *******" << endl;
            rtree.printTree();
            cout << endl << endl << endl << "**********Root Printed (Before) *******" << endl;
        }*/

        rtree.insertPoint(Point((rand()%100000)/1000.0f, (rand()%100000)/1000.0f));
        
        /*if(pNumber == 39) {
            cout << endl << endl << endl << "**********Printing Root (After) *******" << endl;
            rtree.printTree();
            cout << endl << endl << endl << "**********Root Printed (After) *******" << endl;
        }*/

        if(!rtree.validateTree()) { cout << "Tree is not valid"; break; }
    }

    cout << endl << endl << endl << "**********Printing Root *******" << endl;
    //rtree.printTree();


    
    cout <<"Tree inserted: " << pNumber << " points.";

}