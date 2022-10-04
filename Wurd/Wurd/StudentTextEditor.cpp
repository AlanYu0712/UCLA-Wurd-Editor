#include "StudentTextEditor.h"
#include "Undo.h"
#include <string>
#include <vector>

//my include
#include <list>
#include <iostream>
#include <fstream>
using namespace std;

TextEditor* createTextEditor(Undo* un)
{
	return new StudentTextEditor(un);
}

StudentTextEditor::StudentTextEditor(Undo* undo)
 : TextEditor(undo) {
	// TODO
	//set position to (0,0)
	m_row = m_col = 0;
	//reasonable set-up, m_text always start off with a line of empty text
	m_text.push_back("");
	m_rowIterator = m_text.begin();
}

StudentTextEditor::~StudentTextEditor()
{
	// TODO:
	//clearing every node in list m_text
	list<string> ::iterator cleanIterator = m_rowIterator;
	while(!m_text.empty()) m_text.pop_front();

}

bool StudentTextEditor::load(std::string file) {
	// TODO: spelling suggestion problem?
	//find file via path, return false if not found
	ifstream infile(file);
	if (!infile)
		return false;
	//reset old file
	this->reset();
	//load in new file
	string s;
	bool first = true;
	while (getline(infile, s)) {
		if (!s.empty()&&s.at(s.size() - 1) == '\r')
			s.erase(s.size() - 1);
		if (first) { //after reset, editor has a line of empty string
			*m_rowIterator = s;
			first = false;
		}
		else {
			m_text.push_back(s);
		}
	}
	m_row = m_col = 0;
	return true;

  
}

bool StudentTextEditor::save(std::string file) {
	// TODO
	ofstream outfile(file);
	//check path availability and status
	if (!outfile)
		return false;
	list<string>::iterator write_inIterator = m_text.begin();
	//traversal write in
	while (write_inIterator != m_text.end()) {
		outfile << *write_inIterator << '\n';
		write_inIterator++;
	}
	return true;
}

void StudentTextEditor::reset() {
	// TODO:
	//clearing every node in list m_text
	list<string> ::iterator cleanIterator = m_rowIterator;
	while(m_text.size()!=0) m_text.pop_front();
	//reset to variable to reasonable state
	m_col = m_row = 0;
	m_text.push_back("");
	m_rowIterator = m_text.begin();
	//clear the undo stack
	this->getUndo()->clear();
}

void StudentTextEditor::move(Dir dir) {
	// TODO
	switch (dir)
	{
	case Dir::UP:
		if (m_row != 0) {
			m_row--;
			m_rowIterator--;
			//check if current line is shorter than previous line
			if (m_col > m_rowIterator->size()) {
				//change m-Col to end of current line
				m_col = m_rowIterator->size();
			}
		}
		break;
	case Dir::DOWN:
		if (m_row < m_text.size() - 1) {
			m_row++;
			m_rowIterator++;
			//check if current line is shorter than previous line
			if (m_col > m_rowIterator->size()) {
				//change m-Col to end of current line
				m_col = m_rowIterator->size();
			}
		}
		break;
	case Dir::LEFT:
		if (m_col != 0) {
			m_col--;
		}
		//if is at the begining of a line, go up a line and cursor to the end of it
		else if (m_row != 0 && m_col == 0) {
			m_row--;
			m_rowIterator--;
			m_col = m_rowIterator->size();
		}
		break;
	case Dir::RIGHT:
		if (m_col < m_rowIterator->size()) {
			m_col++;
		}
		//if is at the very end of a line, go down a line and cursor to the very begining of it
		else if (m_row < m_text.size() - 1 && m_col == m_rowIterator->size()) {
			m_row++;
			m_rowIterator++;
			m_col = 0;
		}
		break;
	case Dir::HOME:
		m_col = 0;
		break;
	case Dir::END:
		m_col = m_rowIterator->size();
		break;
	default:
		break;
	}
}

void StudentTextEditor::del() {
	// TODO:
	//if not at the very end of the line
	if (m_col < m_rowIterator->size()) {
		//store the character that is to be deleted
		char undoCh = m_rowIterator->at(m_col);
		//increment the cursor to right by one and do operation like backspace
		m_col++;
		privDELETE();
		//submit this action to the undo stack
		getUndo()->submit(Undo::Action::DELETE, m_row, m_col, undoCh);
	}
	//otherwise m_col at the very end of the line and the line is not the last line
	else if (m_row != m_text.size() - 1 && m_col == m_rowIterator->size()) {
		//join the line below
		privJOIN();
		//submit this action to the undo stack
		getUndo()->submit(Undo::Action::JOIN, m_row, m_col);
	}
}

void StudentTextEditor::backspace() {
	// TODO:
	//if not at the very beginning of the line
	if (m_col > 0) {
		//store the deleting character for undo stack
		char undoCh = m_rowIterator->at(m_col - 1);
		//use the prive DELETE  to achieve backspace
		privDELETE();
		//store this action to the undo stack
		getUndo()->submit(Undo::Action::DELETE, m_row, m_col, undoCh);
	}
	//otherwise if not on the very top of line and also at beginning of line
	else if (m_row != 0 && m_col == 0) {
		//move the cursor up to a line and the end of the line
		m_row--;
		m_rowIterator--;
		m_col = m_rowIterator->size();
		//join the line just like what delete does
		privJOIN();
		//store this action to the undo stack
		getUndo()->submit(Undo::Action::JOIN, m_row, m_col);
	}
}

void StudentTextEditor::insert(char ch) {
	// TODO:
	if (ch == '\t') { //if passed in tab
		//insert 4 spaces
		for (int i = 0; i < 4; ++i) {
			//insert character ' '
			privINSERT(ch);
			//store this action to the undo stack
			getUndo()->submit(Undo::Action::INSERT, m_row, m_col, ' ');
		}
	}
	else {
		//insert character
		privINSERT(ch);
		//store this action to the undo stack
		getUndo()->submit(Undo::Action::INSERT, m_row, m_col, ch);
	}

}

void StudentTextEditor::enter() {
	// TODO:
	//store this action to the undo stack
	getUndo()->submit(Undo::Action::SPLIT, m_row, m_col);
	//split the lines
	privSPLIT();
	//increment variables
	m_rowIterator++;
	m_col = 0;
	m_row++;
}

void StudentTextEditor::getPos(int& row, int& col) const {
	// TODO
	row = m_row;
	col = m_col;
}

int StudentTextEditor::getLines(int startRow, int numRows, std::vector<std::string>& lines) const {
	// TODO:WARANDPEACE!!!
	if (startRow < 0 || numRows < 0 || startRow > m_text.size()/*should it be greater and equal to?*/) {
		return -1;
	}
	//clear the lines vector
	vector<string>::iterator clearIt = lines.begin();
	while (clearIt != lines.end()) {
		clearIt = lines.erase(clearIt);
	}
	//have an iterator pointing to the desired row
	list<string>::iterator temp = m_rowIterator;
	advance(temp, startRow - m_row);
	//calculate how many rows to traverse
	int max = m_text.size() - startRow;
	int numTraverse = numRows < max ? numRows : max;
	for (int i = 0; i < numTraverse; ++i) {
		lines.push_back(*temp);
		temp++;
	}
	return numTraverse;
}

void StudentTextEditor::undo() {
	// TODO:
	//set initial variable
	int col, row, count;
	string text = "";
	Undo::Action action = getUndo()->get(row, col, count, text);
	//if stack is empty, do nothing
	if (action == Undo::Action::ERROR)
		return;
	//move cursor to the desired position
	m_col = col;
	advance(m_rowIterator, row - m_row);
	m_row = row;
	switch (action) {	
	case Undo::Action::DELETE:	
		for (int i = 0; i < count; ++i) {
			this->privDELETE();
		}
		break;
	case Undo::Action::INSERT:
		for (int i = 0; i < text.size(); ++i) {
			this->privINSERT(text.at(i));
		}
		//col to where it started
		m_col = col;
		break;
	case Undo::Action::SPLIT:
		privSPLIT();
		break;
	case Undo::Action::JOIN:
		privJOIN();
		break;
	}
	

}

void StudentTextEditor::privDELETE() {
	//use an temperory iterator to iterate to the desired index
	string::const_iterator it = m_rowIterator->begin();
	advance(it, m_col - 1);
	//erase
	m_rowIterator->erase(it);
	m_col--;
}

void StudentTextEditor::privINSERT(char ch) {
	//create a column iterator for inserting ch
	string::const_iterator it = m_rowIterator->begin();
	//set iterator to the correct place
	advance(it, m_col);
	//store the character being added
	if (ch == '\t')  //if passed in a tab, do spaces
			m_rowIterator->insert(it, ' ');
	else 
		m_rowIterator->insert(it, ch);

	m_col++;
}

void StudentTextEditor::privSPLIT() {
	//store the rest of the line right to cursor 
	string str = m_rowIterator->substr(m_col, m_rowIterator->size() - m_col);
	//delete the rest of the line right to cursor
	m_rowIterator->erase(m_col, m_rowIterator->size() - m_col);
	//create a new line
	//if is at last line
	if (m_row == m_text.size() - 1) {
		m_text.push_back(str);
	}
	else {
		list<string>::const_iterator temp = m_rowIterator;
		temp++;
		m_text.insert(temp, str);
	}
	
}

void StudentTextEditor::privJOIN() {
	//have a temp iterator pointing to the line below current line
	list<string>::const_iterator cleanIterator = m_rowIterator;
	cleanIterator++;
	//store the string of the line below
	string str = *cleanIterator;
	//erase the line below
	m_text.erase(cleanIterator);
	//merge the string below to the end of current line
	m_rowIterator->insert(m_col, str);
}