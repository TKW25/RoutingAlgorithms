#ifndef _table
#define _table

#include <iostream>
#include <map>
using namespace std;
#include "node.h"
class Node;

struct TopoLink {
    TopoLink(): cost(-1), age(0) {}

    TopoLink(const TopoLink & rhs) {
        *this = rhs;
    }

    TopoLink & operator=(const TopoLink & rhs) {
        this->cost = rhs.cost;
        this->age = rhs.age;

        return *this;
    }

    int cost;
    int age;
};

struct CostToNode{
    CostToNode(): cost(-1), node(NULL){}

    CostToNode(const CostToNode &rhs){ *this = rhs; }

    CostToNode & operator=(const CostToNode &rhs){
        this->cost = rhs.cost;
        this->node = rhs.node;
        return *this;
    }

    CostToNode(double c, Node *n): cost(c), node(n){}

    double cost;
    Node *node;
};

// Students should write this class
class Table {
    private:
        map < int, map < int, TopoLink > > topo;
    public:
        Table();
        Table(const Table &);
        Table & operator=(const Table &);

        ostream & Print(ostream &os) const;

        // Anything else you need

        #if defined(LINKSTATE)
        #endif

        #if defined(DISTANCEVECTOR)
            map<unsigned, CostToNode> table;
            /**
             * Update table with a new cost to node n
             */
            void updateTable(unsigned n, double new_cost);

            /**
             * Add the link latency to the cost from node src to node dest
             */
            void addLinkLatency(unsigned source, Link link);

            void insert(unsigned n, CostToNode *ctn);
        #endif
};

inline ostream & operator<<(ostream &os, const Table & t) { return t.Print(os);}


#endif
