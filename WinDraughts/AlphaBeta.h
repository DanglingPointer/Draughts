#pragma once
/** GENERIC ALPHA-BETA PRUNING ALGORITHM **
Preconditions:
* class Node_type represents game state
* Value_type represents heuristic value of a node, set to int per default
* operators <, >, <= as well as assignment operator, copy constrctor and default constructor must work 
  properly for Value_type
* macros MAX_RESULT and MIN_RESULT must be defined as maximum and minimum values of Value_type, respectively,
  unless the default Value_type is used
* realization of the abstract class INodeTool must be implemented
**/

#include<set>
#include<utility>

#ifndef MAX_RESULT
#define MAX_RESULT 1
#endif

#ifndef MIN_RESULT
#define MIN_RESULT -1
#endif

template <typename Node, typename Value = int> class INodeTool
{
public:
	typedef Node Node_type;
	typedef Value Value_type;
	virtual ~INodeTool() { }
	// sets node to be processed
	virtual void set_Node(Node*) = 0;
	// @returns the heuristic value of the node
	virtual Value NodeValue() const = 0;
	// @returns true if the node is terminal, false otherwise
	virtual bool TerminalNode() const = 0;
	// @returns a set containing all child nodes of the given node
	virtual std::set<Node*> ChildNodes(bool maximizing_side) = 0;
};

// Tool_type must be derived from INodeTool
template <typename Tool_type> class AlphaBeta
{
public:
	typedef typename Tool_type::Node_type Node;
	typedef typename Tool_type::Value_type Value;

	// without_depth should be true when no depth restrictions are applied
	AlphaBeta(Tool_type* ptool, bool without_depth = true) : m_pfunc(ptool), m_with_depth(!without_depth)
	{ }
	AlphaBeta(const AlphaBeta<Tool_type>&) = delete;
	
	// Returns a pointer to a dynamic object representing best next state
	// might return a nullptr if MIN_RESULT is defined incorrectly
	Node* NextState(Node* pcurrentState, int depth = 3) const
	{
		m_pfunc->set_Node(pcurrentState);
		std::set<Node*> childStates = m_pfunc->ChildNodes(true);

		std::pair<Value, Node*> best = { MIN_RESULT, nullptr };
		for (std::set<Node*>::const_iterator it = childStates.begin(); it != childStates.end(); ++it)
		{
			Value temp = Algorithm(*it, false, depth);
			if (temp >= best.first)
				best = { temp, *it };
		}
		for (std::set<Node*>::const_iterator it = childStates.begin(); it != childStates.end(); ++it)
			if (*it != best.second)
				delete *it;
		return best.second;
	}
private:
	Value Algorithm(Node* node, bool maximizing_player, int depth, Value alpha = MIN_RESULT, Value beta = MAX_RESULT) const
	{
		m_pfunc->set_Node(node);
		if (m_pfunc->TerminalNode() || (m_with_depth && depth == 0))
			return m_pfunc->NodeValue();

		std::set<Node*> childs = m_pfunc->ChildNodes(maximizing_player);
		Value v;
		if (maximizing_player)
		{
			v = MIN_RESULT;
			for (std::set<Node*>::const_iterator it = childs.begin(); it != childs.end(); ++it)
			{
				Value temp = Algorithm(*it, false, depth - 1, alpha, beta);
				if (temp > v) v = temp;
				if (v > alpha) alpha = v;
				if (beta <= alpha)
				{
					for (std::set<Node*>::const_iterator it = childs.begin(); it != childs.end(); ++it)
						delete *it;
					return v;
				}
			}
		}
		else
		{
			v = MAX_RESULT;
			for (std::set<Node*>::const_iterator it = childs.begin(); it != childs.end(); ++it)
			{
				Value temp = Algorithm(*it, true, depth - 1, alpha, beta);
				if (temp < v) v = temp;
				if (v < beta) beta = v;
				if (beta <= alpha)
				{
					for (std::set<Node*>::const_iterator it = childs.begin(); it != childs.end(); ++it)
						delete *it;
					return v;
				}
			}
		}
		for (std::set<Node*>::const_iterator it = childs.begin(); it != childs.end(); ++it)
			delete *it;
		return v;
	}
private:
	Tool_type* m_pfunc;
	bool m_with_depth;
};