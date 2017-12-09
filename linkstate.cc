#include "linkstate.h"

LinkState::LinkState(unsigned n, SimulationContext* c, double b, double l) :
    Node(n, c, b, l)
{
    //Initialize verairbles
    this->routing_table = new Table();
    this->link_table.clear();
    CostToNode *temp = new CostToNode(0, this);
    this->routing_table->insert(this->GetNumber(), temp);

    deque<Link*> links = *GetOutgoingLinks();
    deque<Link*>::iterator it = links.begin();
    unsigned src = this->GetNumber();
    //Build link table
    while(it != links.end()){
        unsigned dest = (*it)->GetDest();
        //Make sure we have the right dest, I don't think this should ever be true
        if(dest == src)
            dest = (*it)->GetSrc();
        //insert new link latency
        if(link_table.find(src) == link_table.end()){
            LinkCosts *ttttt =  new LinkCosts(dest, (*it)->GetLatency());
            link_table.insert(pair<unsigned, LinkCosts>(src, *ttttt));
        }
        else{
            if(link_table[src].destCost.find(dest) != link_table[src].destCost.end()){
                //We have two links going to the same node, we only care about the cheapest one
                if(link_table[src].destCost[dest] > (*it)->GetLatency())
                    link_table[src].destCost[dest] = (*it)->GetLatency();
            }
            else{
                link_table[src].destCost.insert(pair<unsigned, double>(src, (*it)->GetLatency()));
            }
        }
        it++;
    }
    //Since some of our nodes won't have any links at creation time we might not be able to do this for everyone
    if(!link_table.empty()){
        //Build our routing table
        buildRoutingTable();
        //Send our link table to all known nodes
        flood();
    }
}

LinkState::LinkState(const LinkState & rhs) :
    Node(rhs)
{
    *this = rhs;
}

LinkState & LinkState::operator=(const LinkState & rhs) {
    Node::operator=(rhs);
    return *this;
}

LinkState::~LinkState() {}


/** Write the following functions.  They currently have dummy implementations **/
void LinkState::LinkHasBeenUpdated(Link* l) {
    cerr << *this << ": Link Update: " << *l << endl;
    SendToNeighbors(new RoutingMessage());
}

void LinkState::ProcessIncomingRoutingMessage(RoutingMessage *m) {
    cerr << *this << " got a routing message: " << *m << " (ignored)" << endl;
}

void LinkState::TimeOut() {
    cerr << *this << " got a timeout: (ignored)" << endl;
}

void LinkState::buildRoutingTable(){
    //Get neighbors
    deque<Node*>::iterator iter = GetNeighbors()->begin();
    map<unsigned, Node*> neighbors;
    neighbors.clear();
    while(iter != GetNeighbors()->end()){ //inefficient
        neighbors.insert(pair<unsigned, Node*>((*iter)->GetNumber(), *iter));
    }
    //Dijkstra's
    vector <unsigned> check;
    check.push_back(this->GetNumber());
    for(unsigned i = 0; i < this->link_table.size(); i++){
        unsigned temp = check.back();
        unsigned tempp;
        if(this->link_table.find(temp) != this->link_table.end()){
            map<unsigned, double>::iterator it = this->link_table[temp].destCost.begin();
            check.pop_back();
            while(it != this->link_table[temp].destCost.end()){
                check.push_back(it->first);
                tempp = it->first;
                double tempCost = it->second; //Cost of link
                double curCost = numeric_limits<double>::max(); //Current cost to link destination
                double neighborCost = 0; //Cost to neighbor needed to reach destination if not a direct link
                if(this->routing_table->table.find(it->first) != this->routing_table->table.end()){
                    curCost = this->routing_table->table[it->first].cost;
                    if(this->GetNumber() != temp){
                        if(this->routing_table->table.find(temp) != this->routing_table->table.end()){
                            neighborCost = this->routing_table->table[temp].cost;
                        }
                        else
                            neighborCost = numeric_limits<double>::max(); //no way to reach this route
                    }
                }
                //Got costs, see if this is a new fastest path
                if(curCost > tempCost + neighborCost && neighborCost != numeric_limits<double>::max()){
                    //new fastest path
                    if(this->routing_table->table.find(tempp) != this->routing_table->table.end()){
                        //Update old path
                        this->routing_table->table[tempp].cost = tempCost + neighborCost;
                        if(neighborCost != 0)
                            this->routing_table->table[tempp].node = neighbors[temp];
                        else
                            this->routing_table->table[tempp].node = neighbors[tempp];
                    }
                    else{
                        //Create new path
                        Node *nn;
                        double d = tempCost + neighborCost;
                        if(neighborCost != 0)
                            nn = neighbors[temp];
                        else
                            nn = neighbors[tempp];
                        CostToNode *cts = new CostToNode(d, nn);
                        this->routing_table->insert(tempp, cts);
                    }
                }
                it++;
            }

        }

        if(check.empty())
            break; //No more nodes to explore in network
    }
}

void LinkState::flood(){

}

Node* LinkState::GetNextHop(Node *destination) {
    cout << "Getting next hop\n";
    cout << *this->routing_table << endl;
    if(this->routing_table->table.find(destination->GetNumber()) != this->routing_table->table.end()){
        Node *temp = new Node(*this->routing_table->table[destination->GetNumber()].node);
        return temp;
    }
    else
        cerr << this << "Didn't have the correct node\n";
    return NULL;
}

Table* LinkState::GetRoutingTable() {
    cout << "Getting routing table\n";
    return new Table(*routing_table);
}

ostream & LinkState::Print(ostream &os) const { 
    Node::Print(os);
    return os;
}
