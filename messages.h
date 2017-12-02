#ifndef _messages
#define _messages

#include <iostream>
#include "node.h"
#include "link.h"
#include "table.h"

struct RoutingMessage {
    RoutingMessage();
    RoutingMessage(const RoutingMessage &rhs);
    RoutingMessage &operator=(const RoutingMessage &rhs);
    ostream & Print(ostream &os) const;

    // Anything else you need

    #if defined(LINKSTATE)
    #endif
    #if defined(DISTANCEVECTOR)
        RoutingMessage(Table *t, unsigned s): table(t), sender(s) {}
        Table *table;
        unsigned sender;
    #endif
};

inline ostream & operator<<(ostream &os, const RoutingMessage & m) { return m.Print(os);}

#endif
