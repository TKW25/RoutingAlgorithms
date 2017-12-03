#ifndef _messages
#define _messages

#include <iostream>
#include "node.h"
#include "link.h"
#include "table.h"
class Node;
struct RoutingMessage {
    RoutingMessage();
    RoutingMessage(const RoutingMessage &rhs);
    RoutingMessage &operator=(const RoutingMessage &rhs);
    ostream & Print(ostream &os) const;

    // Anything else you need

    #if defined(LINKSTATE)
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
