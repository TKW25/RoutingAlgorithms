#include "distancevector.h"

DistanceVector::DistanceVector(unsigned n, SimulationContext* c, double b, double l) :
    Node(n, c, b, l)
{
    this->routing_table = new Table();
    deque<Node*> neighbors = *GetNeighbors();
    //add self
    CostToNode *temp = new CostToNode(0, this);
    this->routing_table->insert(this->GetNumber(), temp);
    deque<Node*>::iterator it = neighbors.begin();
    //cout << "We have neighbors of count: " << distance(neighbors.begin(), neighbors.end()) << endl;
    //add neighbors
    while(it != neighbors.end()){
        temp = new CostToNode(0, *it);
        this->routing_table->insert((*it)->GetNumber(), temp);
        it++;
    }

    deque<Link*> links = *GetOutgoingLinks();
    std::deque<Link*>::iterator itt = links.begin();
    //cout << "We have links of count: " << distance(links.begin(), links.end()) << endl;
    //update latency to all neighbors appropriately
    while(itt != links.end()){
        this->routing_table->addLinkLatency((this->number), **itt);
        itt++;
    }

    //Next we need to send each of our neighbors our distance vector
    RoutingMessage *m = new RoutingMessage(this->routing_table, this->GetNumber(), this);
    cout << "Node " << this->GetNumber() << " has Table size: " << this->routing_table->table.size() << endl;
    SendToNeighbors(m);
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
    this->routing_table->updateTable(n, l->GetLatency());
    //Create and send routing message
    RoutingMessage *m = new RoutingMessage(this->routing_table, n, this);
    SendToNeighbors(m);
}

void DistanceVector::ProcessIncomingRoutingMessage(RoutingMessage *m) {
    //For distance vector our message just contains a table
//    cout << "Processing routing message\n";
    Table *temp = m->table;
    bool changed = false;
    map<unsigned, CostToNode>::iterator it = temp->table.begin();

    unsigned sender = m->sender;
    
    if(this->routing_table->table.find(sender) == this->routing_table->table.end()){
        cout << "Sender not in our table, adding it to our table\n";
        double ncost = temp->table.find(this->GetNumber())->second.cost;
        CostToNode *cts = new CostToNode(ncost, m->sending_node);
        this->routing_table->insert(sender, cts);
        changed = true;
    }

    while(it != temp->table.end()){
        //Check if we can get to any new nodes faster
        unsigned targetNode = it->first;
        double tempCost = it->second.cost;
        double senderCost = this->routing_table->table[sender].cost;
        if(this->routing_table->table.find(targetNode) == this->routing_table->table.end()){
            cout << "Entry not in our table, adding it a route through sender\n";
            CostToNode *cts = new CostToNode(tempCost + senderCost, it->second.node);
            this->routing_table->insert(targetNode, cts);
            it++;
            changed = true;
            break;
        }
        unsigned nexthop = this->routing_table->table[targetNode].node->GetNumber();
        double curCost = this->routing_table->table[targetNode].cost;

        if(tempCost + senderCost < curCost){
            //It's cheaper for us to go to target node through sender than how we currently do it
            cout << "New fastest path to targetNode, updating table appropriately\n";
            Node *n = it->second.node;
            this->routing_table->table[targetNode].cost = tempCost + senderCost;
            routing_table->table[targetNode].node = n;
            changed = true;
        }
        else if(sender == nexthop){
            if(curCost != tempCost){
                cout << "Linkt to sender has changed, updating table appropriately\n";
                //Route has changed, needs to be updated
                this->routing_table->table[targetNode].cost = tempCost;
                changed = true;
            }
        }
        it++;
    }

    if(changed){
        //Table updated, inform neighbors
        RoutingMessage *m = new RoutingMessage(this->routing_table, this->GetNumber(), this);
        SendToNeighbors(m);
    }
}

void DistanceVector::TimeOut() {
    cerr << *this << " got a timeout: (ignored)" << endl;
}

Node* DistanceVector::GetNextHop(Node *destination) {
    cout << "Getting next hop\n";
    if(this->routing_table->table.find(destination->GetNumber()) != this->routing_table->table.end()){
        //We have this node in our table
        cout << this->routing_table->table[destination->GetNumber()].node << endl;
        Node *temp = new Node(*this->routing_table->table[destination->GetNumber()].node);
        cout << "At: " << *this << "\nGoing to: " << *temp << endl;
        return temp;
    }
    else
        cerr << this << "Didn't have the correct node\n";
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
