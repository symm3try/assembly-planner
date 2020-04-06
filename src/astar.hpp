#pragma once

#include <iostream>
#include <climits>
#include <set>
#include <vector>
#include <queue>

#include "expander.hpp"

/* Comparator function.
    Used to sort the priosirty queue inside AStarSearch.
    Orders elements by ascending f_score.
**/
struct LessThan
{
    bool operator()(const Node<SearchData> *lhs, const Node<SearchData> *rhs) const
    {
        return (lhs->data.f_score) > (rhs->data.f_score);
    }
};

/* Class representing the A* Search Algorithm.
    It executes the search on a given graph.
    The provided Exapander-Object is used to perform the Node-Expansion step.
**/
struct AStarSearch
{
    AStarSearch(Graph<AssemblyData,EdgeData>& assembly);
    ~AStarSearch() = default;
    Node<SearchData>* search(Graph<SearchData,EdgeData>&, Node<SearchData>*, NodeExpander&);
    bool isGoal(Graph<AssemblyData,EdgeData>&, Node<SearchData>*);
    double calc_hscore(Node<SearchData>* current);
    double calc_fscore(Node<SearchData>* current);

    Graph<AssemblyData,EdgeData>& assembly_;
};

/* Check if given sueprnode is Goal.
   Check whether a given supernode inside the A* search 
   has any subassemblies to continue the search.
**/
bool AStarSearch::isGoal(Graph<AssemblyData,EdgeData>& graph, Node<SearchData>* current)
{
    for (auto &x : current->data.subassemblies)
    {
        if (graph.hasSuccessor(x.second))
        {
            return false;
        }
    }
    return true;
}

/* Calculate the h_Score needed for the A* search.
    Is declared as a member of the NodeData contained within Nodes.
    Placed within this files due to linking issues.
    //TODO: Move into containers.hpp and fix linking issues.
**/
double AStarSearch::calc_hscore(Node<SearchData>* current)
{
    std::size_t maximum_length_subassembly = 0;
    for (auto &x : current->data.subassemblies)
    {
        auto node = assembly_.getNode(x.second);
        if (node->data.name.length() > maximum_length_subassembly)
            maximum_length_subassembly = node->data.name.length();
    }
    
    double temp = log2f(maximum_length_subassembly) * current->data.minimum_cost_action;
    return temp;
}

/* Calculate the f_Score needed for the A* search.
    Is declared as a member of the NodeData contained within Nodes.
    Placed within this files due to linking issues.
    //TODO: Move into containers.hpp and fix linking issues.
**/
double AStarSearch::calc_fscore(Node<SearchData>* current)
{
    return current->data.g_score + current->data.h_score;
}


AStarSearch::AStarSearch(Graph<AssemblyData,EdgeData>& assembly)
  : assembly_(assembly)
{};

/* Perform the A* graph search.
    @graph: pointer to graph on which the search should be performed.
    @root: pointer to node at which the search should begin.
    @exapnder: exapnder object used for node expansion.
**/
Node<SearchData>* AStarSearch::search(Graph<SearchData,EdgeData>& graph, Node<SearchData>* root, NodeExpander& expander)
{

    Node<SearchData> *current = nullptr;
    std::priority_queue<Node<SearchData> *, std::vector<Node<SearchData> *>, LessThan> openSet;

    // Closed-Set is redundant as the search is performed on a acyclic graph where every path is unique.
    // -> Nodes can only be reached in one way.
    // Not using the closed-set saves some processing time needed to lookuo-stuff.
    // NodeSet closedSet; 
    expander.expandNode(root->id);
    root->data.h_score  = this->calc_hscore(root);
    root->data.f_score = this->calc_fscore(root);

    openSet.push(root);

    while (!openSet.empty())
    {
        current = openSet.top();
        openSet.pop();

        if (this->isGoal(assembly_, current))
        {
            return current;
        }
        
        current->data.marked = true;

        for (auto &edge : graph.getSuccessorEdges(current->id))
        {
            Node<SearchData> *child = graph.getNode(edge->getDestination());
            expander.expandNode(child->id);

            child->data.g_score = current->data.g_score + edge->data.cost;
            child->data.h_score = this->calc_hscore(child);
            child->data.f_score = this->calc_fscore(child);

            openSet.push(child);
        }
    }
    return current;
}