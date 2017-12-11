#include "linkstate.h"

LinkState::LinkState(unsigned n, SimulationContext* c, double b, double l) :
    Node(n, c, b, l)
{
    //Initialize verairbles
    this->routing_table = new Table();
    this->link_table.clear();
    CostToNode *temp = new CostToNode(0, this);
    this->routing_table->insert(this->GetNumber(), temp);
    deque<Node*> neighbors = *GetNeighbors();
    deque<Node*>::iterator itt = neighbors.begin();
    this->nodes.clear();
    while(itt != neighbors.end()){
        nodes.insert(pair<unsigned, Node*>((*itt)->GetNumber(), *itt));
        itt++;
    }

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
    unsigned dest = l->GetDest();
    unsigned src = l->GetSrc();
    if(dest == this->GetNumber()){
        src = dest;
        dest = src;
    }

    if(link_table.find(src) != link_table.end()){
        if(link_table[src].destCost.find(dest) != link_table[src].destCost.end()){
            //Link exists, update it
            link_table[src].destCost[dest] = l->GetLatency();
            time(&link_table[src].timer);
        }
        else{
            //No dest link exists, create it
            link_table[src].destCost.insert(pair<unsigned, double>(dest, l->GetLatency()));
            time(&link_table[src].timer);
        }
    }
    else{
        //No source entry, don't think this should ever run
        cerr << "Why is this running\n";
        link_table.insert(pair<unsigned, LinkCosts>(src, *new LinkCosts(dest, l->GetLatency())));
    }

    //link table updated, build a new routing table then flood our link table
    buildRoutingTable();
    flood();
}

void LinkState::ProcessIncomingRoutingMessage(RoutingMessage *m) {
    //Process routing message
    bool forward = m->forward;
    unsigned target = m->target;
    map<unsigned, LinkCosts> t = *m->table;
    bool changed = false;
    map<unsigned, LinkCosts>::iterator iter = t.begin();

    while(iter != t.end()){
        if(link_table.find(iter->first) != link_table.end()){
            //We have this entry check timer
            if(difftime(link_table[iter->first].timer, iter->second.timer) < 0){
                //New entry is more up to date, copy it
                link_table[iter->first] = iter->second;
                changed = true;
            }
        }
        else{
            //Don't have this entry copy
            link_table[iter->first] = iter->second;
            changed = true;
        }
        iter++;
    }

    /*while(iter != t.end() && !forward){
        if(link_table.find(iter->first) != link_table.end()){
            time_t temp = link_table[iter->first].timer;
            if(difftime(temp, iter->second.timer > 0)){
                //our current link is "newer" than iters, ignore it
                cout << ":v\n";
                iter++;
                continue;
            }
            map<unsigned, double>::iterator it = iter->second.destCost.begin();
            while(it != iter->second.destCost.end()){
                if(link_table[iter->first].destCost.find(it->first) != link_table[iter->first].destCost.end()){
                    //we currently have this link, see if it's changed
                    if(link_table[iter->first].destCost[it->first] != it->second){
                        changed = true;
                        link_table[iter->first].destCost[it->first] = it->second;
                        link_table[iter->first].timer = iter->second.timer;
                    }
                }
                else{
                    //Don't have this link, add it
                    changed = true;
                    link_table[iter->first].destCost.insert(pair<unsigned, double>(it->first, it->second));
                }
                it++;
            }
        }
        else{
            //Node not in our network, add it
            link_table.insert(pair<unsigned, LinkCosts>(iter->first, *new LinkCosts(iter->second.timer)));
            changed = true;
            link_table[iter->first] = iter->second;
        }
        iter++;
    }*/

    //Check if we are meant to be forwarding this on
    if(forward){
        //Build routing message and forward
        if(this->routing_table->table.find(target) == this->routing_table->table.end())
            cerr << "ERRRRRROOROROROROROROROOROROOROROOROR\n\n\n\n\n";
        Node *temp = this->routing_table->table[target].node;
        RoutingMessage *m;
        if(temp->GetNumber() == target)
            m = new RoutingMessage(&this->link_table, false, 0, nodes);
        else
            m = new RoutingMessage(&this->link_table, true, target, nodes);
        this->SendToNeighbor(temp, m);
    }

    mergeNodes(m->nodes);

    if(changed){
        //Our link table has changed, update routing_table and flood our new link table
        buildRoutingTable();
 //       flood();
    }
}

void LinkState::mergeNodes(map<unsigned, Node *> n){
    map<unsigned, Node *>::iterator it = n.begin();
    while(it != n.end()){
        if(this->nodes.find(it->first) == this->nodes.end()){
            nodes.insert(pair<unsigned, Node*>(it->first, it->second));
        }
        it++;
    }
}

void LinkState::TimeOut() {
    cerr << *this << " got a timeout: (ignored)" << endl;

}

void LinkState::buildRoutingTable(){
    //Get neighbors
    cout << "Building..." << this->GetNumber() << endl;
   deque<Node*> ttt = *GetNeighbors();
    deque<Node*>::iterator iter = ttt.begin();
    map<unsigned, Node*> neighbors;
    neighbors.clear();

    while(iter != ttt.end()){ //inefficient
        cout << **iter << endl;
        neighbors.insert(pair<unsigned, Node*>((*iter)->GetNumber(), *iter));
        if(this->link_table.find(this->GetNumber()) != this->link_table.end()){
            if(this->link_table[this->GetNumber()].destCost.find((*iter)->GetNumber()) != this->link_table[this->GetNumber()].destCost.end()){
                //Have link to our neighbor, add it to routing table
                if(this->routing_table->table.find((*iter)->GetNumber()) != this->routing_table->table.end()){
                    //link exists, update it
                    if(this->routing_table->table[(*iter)->GetNumber()].cost > this->link_table[this->GetNumber()].destCost[(*iter)->GetNumber()]){
                        this->routing_table->table[(*iter)->GetNumber()].cost = this->link_table[this->GetNumber()].destCost[(*iter)->GetNumber()];
                    }
                }
                else{
                    //link doesn't exist, create it
                    CostToNode *cts = new CostToNode(this->link_table[this->GetNumber()].destCost[(*iter)->GetNumber()], *iter);
                    this->routing_table->insert((*iter)->GetNumber(), cts);
                }
            }
        }
        iter++;
    } 

    //Reset routing_table
    /*routing_table->table.clear();

    typedef pair<double, unsigned> M;
    priority_queue<M, vector<M>, greater<M> > pq;

    set<unsigned> check;

    pq.push(pair<double, unsigned>(0, this->GetNumber()));

    while(!pq.empty()){
        unsigned target = pq.top().second;
    
        if(check.find(target) != check.end()){
            pq.pop();
            continue;
        }

        double path = 0;
        cout << "this: " << this->GetNumber() << " target: "
             << target << endl;
        set<unsigned>::iterator ic;
        for(ic = check.begin(); ic != check.end(); ic++){
            if(target == this->GetNumber())
                break;

            if(link_table[*ic].destCost.find(target) != link_table[*ic].destCost.end()){
                path = link_table[*ic].destCost[target];
            }
            else if(routing_table->table.find(target) != routing_table->table.end()){
                path = routing_table->table[target].cost;
            }
            else
                cerr << "somethignsoemthing\n";
        }
cout << "made it\n";
        if(nodes.find(target) == nodes.end()){
            pq.pop();
        }
        else if(routing_table->table.find(target) == routing_table->table.end()){
            //No entry, add it
            Node *n = nodes[target];
            CostToNode *cts = new CostToNode(pq.top().first + path, n);
            routing_table->insert(target, cts);
        }
        else{
            //Entry, see if new one is better
            double d = routing_table->table[target].cost;
            if(d > pq.top().first + path){
                routing_table->updateTable(target, pq.top().first + path);
            }
        }
        pq.pop();
        check.insert(target);
        //Add links
        map<unsigned, double>::iterator i = link_table[target].destCost.begin();
        while(i != link_table[target].destCost.end()){
            pq.push(pair<double, unsigned>(i->second, i->first));
            i++;
        }
    }
    cout << "break\n";*/
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
                if(this->link_table[this->GetNumber()].destCost.find(tempp) != this->link_table[this->GetNumber()].destCost.end() && temp == this->GetNumber()){
                    //We have a direct path to tempp
                    if(this->routing_table->table.find(tempp) != this->routing_table->table.end()){
                        //We have an existing entry for tempp, see if this one is better
                        double m = this->link_table[this->GetNumber()].destCost[tempp];
                        if(m < this->routing_table->table[tempp].cost){
                            this->routing_table->table[tempp].cost = m;
                        }
                        else if(m != this->routing_table->table[temp].cost && this->routing_table->table[temp].node->GetNumber() == tempp){
                            this->routing_table->table[tempp].cost = m;
                        }
                    }
                    else{
                        //No entry in our table, add this one
                        Node *nnn;
                        if(neighbors.find(tempp) != neighbors.end()){
                            nnn = neighbors[tempp];
                            CostToNode *cts = new CostToNode(this->link_table[this->GetNumber()].destCost[tempp], nnn);
                            this->routing_table->insert(tempp, cts);
                        }
                        else{
                            //Not sure what to do in this case, maybe just skip?
                            cerr << "Don't have way to access node\n";
                            it++;
                            continue;
                        }
                    }
                }
                else{
                    //We don't have a direct path to tempp
                    if(temp == this->GetNumber()){
                        cerr << "Don't have link to tempp\n";
                        it++;
                        continue;
                    }
                    else{
                        if(this->link_table[temp].destCost.find(tempp) != this->link_table[temp].destCost.end()){
                            //We can access tempp through temp
                            Node *nnn;
                            if(neighbors.find(temp) != neighbors.end()){
                                //temp is our neighbor
                                nnn = neighbors[temp];
                                double cost = this->link_table[this->GetNumber()].destCost[temp] + this->link_table[temp].destCost[tempp];
                                CostToNode *cts = new CostToNode(cost, nnn);
                                this->routing_table->insert(tempp, cts);
                            }
                            else{
                                //temp isn't our neighbor
                                cerr << "Dont have a way to access node\n";
                                it++;
                                continue;
                            }
                        }
                        else{
                            //We have no way to get to tempp, runs if we're building for two+ nodes of seperation
                            cerr << "nope\n";
                            it++;
                            continue;
                        }
                    }
                }
                it++;
            }

        }

        if(check.empty())
            break; //No more nodes to explore in network
    }
}

//void LinkState::recurseFind(unsigned target, unsigned through){
    
//}

void LinkState::flood(){
    //Send our link table to all known nodes in the network
    map<unsigned, CostToNode>::iterator iter = routing_table->table.begin();
    while(iter != routing_table->table.end()){
        RoutingMessage *m;
        unsigned target = iter->first;
        bool forward = false;

        LinkCosts temp = *new LinkCosts();
        //cout << "Target: " << target << endl;
        //cout << "At: " << this->GetNumber() << endl << *routing_table << endl;

        cout << *routing_table->table[target].node << endl;
        unsigned sendTo = routing_table->table[target].node->GetNumber();
        if(sendTo != target)
            forward = true;
        m = new RoutingMessage(&this->link_table, forward, sendTo, nodes);
        SendToNeighbor(routing_table->table[target].node, m);
        iter++;
    }
}

Node* LinkState::GetNextHop(Node *destination) {
    cout << "Getting next hop\n";
    cout << *this->routing_table << endl;
    if(this->routing_table->table.find(destination->GetNumber()) != this->routing_table->table.end()){
        Node *temp = new Node(*this->routing_table->table[destination->GetNumber()].node);
        return temp;
    }
    else
        cerr << *this << " Didn't have the correct node\n";
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
