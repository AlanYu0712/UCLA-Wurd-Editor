#ifndef STUDENTSPELLCHECK_H_
#define STUDENTSPELLCHECK_H_

#include "SpellCheck.h"

#include <string>
#include <vector>

//using namespace std


class StudentSpellCheck : public SpellCheck {
public:
    StudentSpellCheck() { 
		head = new Node('\0', false); 
		m_letters = "abcdefghijklmnopqrstuvwxyz\'";
	}
	virtual ~StudentSpellCheck();
	bool load(std::string dict_file);
	bool spellCheck(std::string word, int maxSuggestions, std::vector<std::string>& suggestions);
	void spellCheckLine(const std::string& line, std::vector<Position>& problems);

private:
	struct Node {
		Node(char val, bool end) :m_val(val), isEnd(end){}
		std::vector<Node*> children;
		char m_val;
		bool isEnd;
	};

	Node* head;
	std::string m_letters;

	bool privExist(std::string word);
	void clearTrie(Node* head);
};

#endif  // STUDENTSPELLCHECK_H_
