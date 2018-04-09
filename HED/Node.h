#pragma once
using namespace std;

class Node {
public:
	char label;
	int freq;
	Node* left;
	Node* right;

	Node(char l, int f, Node* left, Node* right);//Define Constructor
	~Node();//Define Destructor
};