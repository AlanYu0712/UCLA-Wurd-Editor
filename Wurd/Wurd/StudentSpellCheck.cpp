#include "StudentSpellCheck.h"
#include <string>
#include <vector>

//my include
#include <iostream>
#include <fstream>
#include <cctype>
using namespace std;

SpellCheck* createSpellCheck()
{
	return new StudentSpellCheck;
}

StudentSpellCheck::~StudentSpellCheck() {
	// TODO
	clearTrie(head);

}

bool StudentSpellCheck::load(std::string dictionaryFile) {
	// TODO
	ifstream infile(dictionaryFile);
	if (!infile)
		return false;
	char c;
	Node* nPtr = head;
	while (infile.get(c)) {
		unsigned char extendChar = c;
		if (extendChar == '\'' || isalpha(extendChar)) {
			//transfer c to lowercase
			extendChar = tolower(extendChar);
			//find if this character already inserted in the current branch
			int searchIndex = -1;
			for (int i = 0; i < nPtr->children.size(); ++i) {
				if (nPtr->children[i]->m_val == extendChar) {
					searchIndex = i;
					break;
				}
			}
			if (searchIndex == -1) {
				nPtr->children.push_back(new Node(extendChar, false));
				searchIndex = nPtr->children.size()-1;
			}
			//advance the branch
			nPtr = nPtr->children[searchIndex];
		}
		if (extendChar == '\n' || extendChar == '\r') { //if end of line
			nPtr->isEnd = true; //mark current node is the end of a word
			nPtr = head; //return to head
		}
	}

	return true;
}

bool StudentSpellCheck::spellCheck(std::string word, int max_suggestions, std::vector<std::string>& suggestions) {
	// TODO
	//check if word is in the dictionary
	if (privExist(word))
		return true;

	//SPELLING SUGGESTIONS
	//clear suggestion parameter
	while (!suggestions.empty()) suggestions.erase(suggestions.begin());

	//find suggestions
	for (int i = 0; i < word.size(); i++) {
		char temp = word[i];
		for (int j = 0; j < m_letters.size(); ++j) {
			if (word[i] == m_letters[j])
				continue;
			word[i] = m_letters[j];
			if (privExist(word)) {
				suggestions.push_back(word);
				if (suggestions.size() - 1 == max_suggestions)
					return false;
			}
		}
		word[i] = temp;
	}



	return false;
}

void StudentSpellCheck::spellCheckLine(const std::string& line, std::vector<SpellCheck::Position>& problems) {
	// TODO
	//clearn problems
	while (!problems.empty()) problems.erase(problems.begin());
	//DIVIDE
	int index = 0;
	while (index != line.size()) {
		unsigned char checkAlpha = line[index];
		if (isalpha(checkAlpha) || checkAlpha == '\'') {
			Position word;
			if (index == line.size() - 1) {
				word.start = word.end = index;
				problems.push_back(word);
				break;
			}
			
			int end = index+1;
			unsigned char extendChar = line[end];
			while (end!=line.size() && (isalpha(extendChar) || extendChar == '\'')) {
				end++;
				extendChar = line[end];
			}
			word.start = index;
			word.end = end - 1;
			problems.push_back(word);
			index = end;
		}
		else
			index++;
	}
	//CHECK & ERASE
	vector<Position>::iterator legitIterator = problems.begin();
	while (legitIterator != problems.end()) {
		int examineStart = legitIterator->start;
		int examineLength = legitIterator->end-examineStart+1;
		if (privExist(line.substr(examineStart, examineLength)))
			legitIterator = problems.erase(legitIterator);
		else
			legitIterator++;
	}

}


bool StudentSpellCheck::privExist(string word) {
	Node* nPtr = head;
	bool exist = false;
	for (int i = 0; i < word.size(); ++i) {
		//change character to lower case
		unsigned char extendChar = word[i];
		if(isalpha(extendChar))
			extendChar = tolower(extendChar);
		for (int j = 0; j < nPtr->children.size(); ++j) {
			if (nPtr->children[j]->m_val == extendChar) { //search for a match of the character
				//set this character is found
				exist = true;
				//advance the branch
				nPtr = nPtr->children[j];
				//still not the i character
				if (i != word.size() - 1) break;
				//if i is the end character and j isEnd true
				if (nPtr->isEnd && i == word.size() - 1) return true;
				else return false;
			}
		}
		if (!exist)
			return false;
		exist = false;
	}

	return false;
}

void StudentSpellCheck::clearTrie(Node* head) {
	if (head->children.size() == 0) {
		delete head;
		return;
	}
	vector<Node*>::iterator clearIterator = head->children.begin();
	while (clearIterator != head->children.end()) {
		clearTrie(*clearIterator);
		clearIterator++;
	}
}