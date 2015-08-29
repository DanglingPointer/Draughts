#pragma once

/*
Preconditions:
* operators <, >, <= as well as the assignment operator must work properly for Value_type
* macros MAX_RESULT and MIN_RESULT must be defined as maximum and minimum values of Value_type, respectively
* realization of the abstract class INodeTool must be implemented
*/

#include<set>
#include<utility>

//#ifndef MAX_RESULT
//#define MAX_RESULT 1
//#endif
//
//#ifndef MIN_RESULT
//#define MIN_RESULT -1
//#endif

template <typename Node_type, typename Value_type> class INodeTool
{
public:
	typedef Node_type Node;
	typedef Value_type Value;
	virtual ~INodeTool() { }
	// sets node to be processed
	virtual void set_Node(Node*) = 0;
	// @returns the heuristic value of the node
	virtual Value NodeValue() const = 0;
	// @returns true if the node is terminal, false otherwise
	virtual bool TerminalNode() const = 0;
	// @returns all child nodes of the given node
	virtual std::set<Node*> ChildNodes(bool maximizing_side) const = 0;
};

template <typename Node_type, typename Value_type> class AlphaBeta
{
public:
	typedef INodeTool<Node_type, Value_type> Tool_type;
	typedef typename Tool_type::Node Node;
	typedef typename Tool_type::Value Value;

	// without_depth should be true when no depth restrictions are applied
	explicit AlphaBeta(Tool_type* ptool, bool without_depth = true) : m_pfunc(ptool), m_with_depth(!without_depth)
	{ }
	AlphaBeta(const AlphaBeta<Tool_type>&) = delete;
	
	// Returns a pointer to a dynamic object representing best next state
	// might return a nullptr if MIN_RESULT is defined wrong
	Node* NextState(const Node& currentState, int depth = 0) const
	{
		m_pfunc->set_Node(currentState);
		std::set<Node*> childStates = m_pfunc->ChildNodes();

		std::pair<Value, Node*> best = { MIN_RESULT, nullptr };
		for (std::set<Node*>::const_iterator it = childStates.begin(); it != childStates.end(); ++it)
		{
			Value temp = Algorithm(*it, false);
			if (temp >= best.first)
				best = { temp, *it };
		}
		for (std::set<Node*>::const_iterator it = childStates.begin(); it != childStates.end(); ++it)
			if (*it != best.second)
				delete *it;
		return best.second;
	}
private:
	Value Algorithm(Node* node, bool maximizing_player, int depth = 0, Value alpha = MIN_RESULT, Value beta = MAX_RESULT) const
	{
		m_pfunc->set_Node(node);
		if (m_pfunc->TerminalNode() || (m_with_depth && depth == 0))
			return m_pfunc->NodeValue();

		std::set<Node*> childs = m_pfunc->ChildNodes(maximizing_player);
		if (maximizing_player)
		{
			Value v = MIN_RESULT;
			for (std::set<Node*>::const_iterator it = childs.begin(); it != childs.end(); ++it)
			{
				Value temp = Algorithm(**it, false, depth - 1, alpha, beta);
				if (temp > v) v = temp;
				if (v > alpha) alpha = v;
				if (beta <= alpha)
				{
					for (std::set<Node*>::const_iterator it = childs.begin(); it != childs.end(); ++it)
						delete *it;
					return v;
				}
				m_pfunc->set_Node(node);
			}
		}
		else
		{
			Value v = MAX_RESULT;
			for (std::set<Node*>::const_iterator it = childs.begin(); it != childs.end(); ++it)
			{
				Value temp = Algorithm(**it, true, depth - 1, alpha, beta);
				if (temp < v) v = temp;
				if (v < beta) beta = v;
				if (beta <= alpha)
				{
					for (std::set<Node*>::const_iterator it = childs.begin(); it != childs.end(); ++it)
						delete *it;
					return v;
				}
				m_pfunc->set_Node(node);
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