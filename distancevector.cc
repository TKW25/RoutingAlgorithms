#include "distancevector.h"

DistanceVector::DistanceVector(unsigned n, SimulationContext* c, double b, double l) :
    Node(n, c, b, l)
{
    routing_table = new Table();
    deque<Node*> neighbors = *GetNeighbors();
    //add self
    CostToNode temp = *new CostToNode(0, this);
    cout << temp.node << endl;
    routing_table->insert(this->GetNumber(), temp);
    deque<Node*>::iterator it = neighbors.begin();
    //add neighbors
    while(it != neighbors.end()){
        temp = *new CostToNode(0, *it);
        routing_table->insert((*it)->GetNumber(), temp);
        it++;
    }

    deque<Link*> links = *GetOutgoingLinks();
    std::deque<Link*>::iterator itt = links.begin();
    //update latency to all neighbors appropriately
    while(itt != links.end()){
        routing_table->addLinkLatency((this->number), **itt);
        itt++;
    }

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
    cout << "Link has been updated, composing message\n";
    //Get sending node number
    unsigned n = this->GetNumber();
    
    unsigned dest;
    if(n == l->GetSrc())
        dest = l->GetDest();
    else
        dest = l->GetSrc();
    //Update table
    routing_table->updateTable(n, l->GetLatency());
    //Create and send routing message
    RoutingMessage *m = new RoutingMessage(routing_table, n);
    SendToNeighbors(m);
}

void DistanceVector::ProcessIncomingRoutingMessage(RoutingMessage *m) {
    //For distance vector our message just contains a table
    cout << "Processing routing message\n";
    Table *temp = m->table;
    bool changed = false;
    map<unsigned, CostToNode>::iterator it = temp->table.begin();

    unsigned sender = m->sender;
    while(it != temp->table.end()){
        //Check if we can get to any new nodes faster
        unsigned targetNode = it->first;
        if(routing_table->table.find(targetNode) == routing_table->table.end()){
            cout << "Entry not in our table, not our nieghbor, skipping\n";
            break;
        }
        cout << "~~~~~~~~~~~~~~~~~~Calculating~~~~~~~~~~~~~~~~~~~~~~~~\n";
        unsigned nexthop = routing_table->table[targetNode].node->GetNumber();
        double tempCost = it->second.cost;
        double curCost = routing_table->table[targetNode].cost;
        double senderCost = routing_table->table[sender].cost;

        if(tempCost + senderCost < curCost){
            //It's cheaper for us to go to target node through sender than how we currently do it
            Node *n = it->second.node;
            routing_table->table[targetNode].cost = tempCost + senderCost;
            routing_table->table[targetNode].node = n;
            changed = true;
        }
        else if(sender == nexthop){
            if(curCost != tempCost){
                //Route has changed, needs to be updated
                routing_table->table[targetNode].cost = tempCost;
                changed = true;
            }
        }
    }
}

void DistanceVector::TimeOut() {
    cerr << *this << " got a timeout: (ignored)" << endl;
}

Node* DistanceVector::GetNextHop(Node *destination) {
    cout << "Getting next hop\n";
    if(routing_table->table.find(destination->GetNumber()) != routing_table->table.end()){
        //We have this node in our table
        cout << routing_table->table[destination->GetNumber()].node << endl;
        Node *temp = new Node(*routing_table->table[destination->GetNumber()].node);
        return temp;
    }
    else
        cerr << *this << "Didn't have the correct node\n";
    return NULL;
}

Table* DistanceVector::GetRoutingTable() {
    cout << "Gettign routing table\n";
    return NULL;
}

ostream & DistanceVector::Print(ostream &os) const { 
    Node::Print(os);
    return os;
}
