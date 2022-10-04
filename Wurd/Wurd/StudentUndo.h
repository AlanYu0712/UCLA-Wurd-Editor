#ifndef STUDENTUNDO_H_
#define STUDENTUNDO_H_

#include "Undo.h"

//my include
#include <stack>

class StudentUndo : public Undo {
public:

	void submit(Action action, int row, int col, char ch = 0);
	Action get(int& row, int& col, int& count, std::string& text);
	void clear();

private:
	struct Node {
		Node(Undo::Action ori, Undo::Action rev, int row, int col, char txt, int count = 1) {
			m_oriAction = ori;
			m_revAction = rev;
			m_row = row;
			m_col = col;
			m_count = count;
			if(txt != '\000')
				m_txt = txt;
			else
				m_txt = "";
		}
		Action m_oriAction;
		Action m_revAction;
		int m_row;
		int m_col;
		int m_count;
		std::string m_txt;
	};

	std::stack<StudentUndo::Node> m_stack;
};

#endif // STUDENTUNDO_H_
