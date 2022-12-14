#pragma once

#include <ostream>
#include <vector>
#include <unordered_map>
#include <cmath>
#include <iomanip>

using NodeIndex = size_t;
using EdgeIndex = size_t;

struct AgentActionAssignment
{
    std::string agent;
    std::string action;
    size_t action_node_id;
};

struct SearchData
{
    bool marked = false;

    double g_score = 0;
    double f_score = 0;
    double h_score = 0;

    double minimum_cost_action = MAXFLOAT;

    std::unordered_map<std::string, size_t> subassemblies;
    std::unordered_map<std::string, size_t> actions;
};

enum class NodeType
{
    ACTION,
    SUBASSEMBLY,
    INTERACTION,
    INTERASSEMBLY
};

struct AssemblyData
{
    NodeType type;
    std::string name = "";
    
    // Only utilized for ACTION type nodes
    std::string assigned_agent;
    size_t interaction_prev;
    size_t interaction_or;
    size_t interaction_next;
};

struct EdgeData
{
    std::vector<AgentActionAssignment> planned_assignments;
    double cost = 0;
};

namespace config
{

    struct Action
    {
        using agentname = std::string;
        
        std::string name;
        std::unordered_map<agentname, double> costs;

        friend std::ostream &operator<<(std::ostream &os, const Action &a)
        {
            os << "| Action " << std::setw(5) << a.name << std::setw(29) << "|" << std::endl;
            for(const auto &cost_agent : a.costs)
            {
                os << "|    Agent: " << std::setw(4)  << cost_agent.first 
                    << "    Cost"    << std::setw(15) << cost_agent.second 
                    << std::setw(4)  << "|" <<std::endl;
            }
            return os;
        }        
    };

    struct Reach
    {
        bool reachable;
        Action interaction;

        friend std::ostream &operator<<(std::ostream &os, const Reach &r)
        {
            os << "   Reachable: " <<  r.reachable << "     "
                << "Interaction: " << std::setw(2) 
                 << r.interaction.name << "  |" << std::endl;
            return os;
        }
    };

    struct Subassembly
    {
        std::string name;
        std::unordered_map<std::string, Reach> reachability;

        friend std::ostream &operator<<(std::ostream &os, const Subassembly &s)
        {
            os << "| " << std::setw(51) << std::left << s.name << "|" << std::endl;
            for(const auto &agent_reach : s.reachability)
            {
                os << "|    Agent: " << std::setw(4) << agent_reach.first
                    << agent_reach.second;
            }
            return os;
        }
    };

    struct Agent
    {
        std::string name;
        std::string hostname;
        std::string port;

        friend std::ostream &operator<<(std::ostream &os, const Agent &a)
        {
            os << "Name: " << std::setw(4)   << a.name     << " | "
               << "Host: "   << std::setw(15)  << a.hostname << " | "
               << "Port: "   << std::setw(5)   << a.port;
            return os;
        }
    };

    struct Configuration
    {
        std::unordered_map<std::string, Agent> agents;
        std::unordered_map<std::string, Action> actions;
        std::unordered_map<std::string, Subassembly> subassemblies;

        friend std::ostream &operator<<(std::ostream &os, const Configuration &c)
        {
            os << "+---------------------------------------------------+" << std::endl;
            os << "| AGENTS                                            |" << std::endl;
            os << "+---------------------------------------------------+" << std::endl;
            for (const auto &at : c.agents)
            {
                os << "|  " << at.second << " |" << std::endl;
            }
            os << "+---------------------------------------------------+" << std::endl;
            os << std::endl;
            os << "+-----------------------------------------+" << std::endl;
            os << "| ACTIONS                                 |" << std::endl;
            os << "+-----------------------------------------+" << std::endl;
            for (const auto &ia : c.actions)
            {
                os << ia.second;
                os << "+-----------------------------------------+" << std::endl;
            }
            os << std::endl;
            os << "+----------------------------------------------------+" << std::endl;
            os << "| SUBASSEMBLIES                                      |" << std::endl;
            os << "+----------------------------------------------------+" << std::endl;
            for (const auto &sa : c.subassemblies)
            {
                os << sa.second;
                os << "+----------------------------------------------------+" << std::endl;
            }
            return os;
        }
    };
}

bool is_float(std::string my_string)
{
    std::istringstream iss(my_string);
    float f;
    iss >> std::noskipws >> f;
    return iss.eof() && !iss.fail();
}
