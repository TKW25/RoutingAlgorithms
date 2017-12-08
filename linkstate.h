#ifndef _linkstate
#define _linkstate
class Node;
class Table;
class RoutingMessage;
#include "node.h"
#include <vector>
#include <limits>
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
        Node* GetNextHop(Node* destination);
        Table* GetRoutingTable();
        ostream & Print(ostream & os) const;

        // Anything else
        map<unsigned, LinkCosts> link_table;
};

inline ostream & operator<<(ostream & os, const LinkState & n) {
    return n.Print(os);
}

#endif
