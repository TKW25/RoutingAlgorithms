#ifndef _messages
#define _messages
#include <iostream>
#include "node.h"
class Node;
#include "link.h"
#include "table.h"
#include "linkstate.h"
class LinkState;

struct LinkCosts{
    LinkCosts(){ destCost.clear(); src = 0; }
    LinkCosts(unsigned s, double d, unsigned ss){
        destCost.clear();
        destCost.insert(pair<unsigned, double>(s, d));
        src = ss;
    }
    map<unsigned, double> destCost;
    unsigned src;
};

struct RoutingMessage {
    RoutingMessage();
    RoutingMessage(const RoutingMessage &rhs);
    RoutingMessage &operator=(const RoutingMessage &rhs);
    ostream & Print(ostream &os) const;

    // Anything else you need

    #if defined(LINKSTATE)
        RoutingMessage(map<unsigned, LinkCosts> *t, bool f, unsigned tt): forward(f), target(tt), table(t) {};
        bool forward;
        unsigned target;
        map<unsigned, LinkCosts> *table;
    #endif
    #if defined(DISTANCEVECTOR)
        RoutingMessage(Table *t, unsigned s, Node *n): table(t), sender(s), sending_node(n) {}
        Table *table;
        unsigned sender;
        Node *sending_node;
    #endif
};

inline ostream & operator<<(ostream &os, const RoutingMessage & m) { return m.Print(os);}

#endif
