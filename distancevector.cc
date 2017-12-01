#include "distancevector.h"

DistanceVector::DistanceVector(unsigned n, SimulationContext* c, double b, double l) :
    Node(n, c, b, l)
{
    deque<Node*> neighbors = *GetNeighbors();
    //add self
    CostToNode *temp = new CostToNode(this->GetLatency(), this);
    routing_table->insert(0, *temp);
    deque<Node*>::iterator it = neighbors.begin();
    //add neighbors
    while(it != neighbors.end()){
        temp = new CostToNode((*it)->GetLatency(), *it);
        routing_table->insert((*it)->GetNumber(), *temp);
    }

    deque<Link*> links = *GetOutgoingLinks();
    std::deque<Link*>::iterator itt = links.begin();
    //update latency to all neighbors appropriately
    while(itt != links.end())
        routing_table->addLinkLatency((this->number), **itt);

    //Next we need to send each of our neighbors our distance vector
}

DistanceVector::DistanceVector(const DistanceVector & rhs) :
    Node(rhs)
{
    *this = rhs;
}

DistanceVector & DistanceVector::operator=(const DistanceVector & rhs) {
    Node::operator=(rhs);
    return *this;
}

DistanceVector::~DistanceVector() {}


/** Write the following functions.  They currently have dummy implementations **/
void DistanceVector::LinkHasBeenUpdated(Link* l) {
    cerr << *this << ": Link Update: " << *l << endl;
    SendToNeighbors(new RoutingMessage());
}

void DistanceVector::ProcessIncomingRoutingMessage(RoutingMessage *m) {
    cerr << *this << " got a routing message: " << *m << " (ignored)" << endl;
}

void DistanceVector::TimeOut() {
    cerr << *this << " got a timeout: (ignored)" << endl;
}

Node* DistanceVector::GetNextHop(Node *destination) { 
    return NULL;
}

Table* DistanceVector::GetRoutingTable() {
    return NULL;
}

ostream & DistanceVector::Print(ostream &os) const { 
    Node::Print(os);
    return os;
}
