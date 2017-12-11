#ifndef _linkstate
#define _linkstate
class Node;
class Table;
class RoutingMessage;
#include "node.h"
#include <vector>
#include <set>
#include <limits>
#include <queue>
class LinkState: public Node {
    private:
        // Anything you need in addition to Node members

    public:
        LinkState(unsigned, SimulationContext* , double, double);
        LinkState(const LinkState &);
        LinkState & operator=(const LinkState &);
        ~LinkState();

        // Inherited from Node
        void LinkHasBeenUpdated(Link *l);
        void ProcessIncomingRoutingMessage(RoutingMessage *m);
        void TimeOut();
        void buildRoutingTable();
        void flood();
        void mergeNodes(map<unsigned, Node *> n);
        Node* GetNextHop(Node* destination);
        Table* GetRoutingTable();
        ostream & Print(ostream & os) const;

        // Anything else
        map<unsigned, LinkCosts> link_table;
        map<unsigned, Node*> nodes;
};

inline ostream & operator<<(ostream & os, const LinkState & n) {
    return n.Print(os);
}

#endif
