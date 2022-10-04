#include "StudentUndo.h"

//my using namespace
using namespace std;

Undo* createUndo()
{
	return new StudentUndo;
}

void StudentUndo::submit(const Action action, int row, int col, char ch) {
	// TODO
	Action rev = Action::ERROR; //ready for the 'rev' parameter for the node constructor
	bool batched = false; //see if this submit is batched to top
	bool MT = false; //check is m_stack empty
	if (m_stack.empty())
		MT = true;
	switch (action) {
	case Action::INSERT: //TODO: 
		if (!MT && action == m_stack.top().m_oriAction) {
			if (m_stack.top().m_col == col - 1 && m_stack.top().m_row == row) {
				m_stack.top().m_row = row;
				m_stack.top().m_col = col;
				m_stack.top().m_txt += ch;
				m_stack.top().m_count++;
				batched = true;
			}
		}
		rev = Action::DELETE;
		break;
	case Action::DELETE:
		if (!MT && action == m_stack.top().m_oriAction) { //check if top has the same operation as this one
			if (m_stack.top().m_row == row && (m_stack.top().m_col == col || col == m_stack.top().m_col - 1)) {
				if (col == m_stack.top().m_col - 1)
					m_stack.top().m_txt.insert(m_stack.top().m_txt.begin(), ch);
				else
					m_stack.top().m_txt += ch;
				m_stack.top().m_row = row;
				m_stack.top().m_col = col;
				batched = true;
			}
		}
		rev = Action::INSERT;
		break;
	case Action::JOIN:
		rev = Action::SPLIT;
		break;
	case Action::SPLIT:
		rev = Action::JOIN;
		break;
	}

	if (!batched) {
		Node nNode(action, rev, row, col, ch, 1);
		m_stack.push(nNode);
	}
}

StudentUndo::Action StudentUndo::get(int &row, int &col, int& count, std::string& text) {
    // TODO
	//if stack is empty
	if(m_stack.empty())
		return Action::ERROR;
	//pass in values
	row = m_stack.top().m_row;
	col = m_stack.top().m_col;
	count = m_stack.top().m_count;
	text = m_stack.top().m_txt;
	StudentUndo::Action returnAction = m_stack.top().m_revAction;
	//pop the top stack
	m_stack.pop();
	//return value
	return returnAction;

}

void StudentUndo::clear() {
	// TODO
	while (!m_stack.empty()) m_stack.pop();
}
