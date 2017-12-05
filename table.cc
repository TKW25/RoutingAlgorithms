#include "table.h"

Table::Table() {
    topo.clear();
#if defined(DISTANCEVECTOR)
    table.clear();
#endif
}

Table::Table(const Table & rhs) {
    *this = rhs;
}

Table & Table::operator=(const Table & rhs) {
    /* For now,  Change if you add more data members to the class */
    topo = rhs.topo;

    return *this;
}

#if defined(GENERIC)
ostream & Table::Print(ostream &os) const
{
  os << "Generic Table()";
  return os;
}
#endif

#if defined(LINKSTATE)
ostream & Table::Print(ostream &os) const
{
  os << "LinkState Table()";
  return os;
}
#endif

#if defined(DISTANCEVECTOR)
void Table::addLinkLatency(unsigned source, Link link){
    unsigned i;
    if(source == link.GetSrc())
        i = link.GetDest();
    else
        i = link.GetSrc();
    cout << "Node: " << source << " has cost " << link.GetLatency() << " to node: " << i << endl;
    std::map<unsigned, CostToNode>::iterator it = table.find(i);
    if(it != table.end())
        it->second.cost = link.GetLatency();
    else
        cout << "Something is wrong in addLinkLatency\n";
}

void Table::insert(unsigned n, CostToNode *cts){
    table.insert(pair<unsigned, CostToNode>(n, *cts));
}

void Table::updateTable(unsigned n, double new_cost){
    std::map<unsigned, CostToNode>::iterator it = table.find(n);
    if(it != table.end())
        it->second.cost = new_cost;
    else
        cout << "Trying to update a value that doesn't exist in table\n";
}

ostream & Table::Print(ostream &os) const
{
  os << "Printing distance vector table: " << endl;
  std::map<unsigned, CostToNode>::const_iterator it = table.begin();
  while(it != table.end()){
      os << "Going to node: " << it->first << " has cost: " << it->second.cost << " through: " << it->second.node->GetNumber() << endl;
      it++;
  }
  return os;
}
#endif
