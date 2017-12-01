#include "table.h"

Table::Table() {
    topo.clear();
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

    std::map<unsigned, CostToNode>::iterator it = table.find(i);
    if(it != table.end())
        it->second.cost += link.GetLatency();
    else
        cout << "Something is wrong in addLinkLatency";
}

void Table::insert(unsigned n, CostToNode cts){
    table.insert(pair<unsigned, CostToNode>(n, cts));
}

void Table::updateTable(unsigned n, double new_cost){

}

ostream & Table::Print(ostream &os) const
{
  os << "DistanceVector Table()";
  return os;
}
#endif
