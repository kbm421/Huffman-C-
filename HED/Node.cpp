#include "Node.h"
#include "stdafx.h"

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

Node::Node(char l, int f, Node* le, Node* ri) {
	label = l;
	freq = f;
	left = le;
	right = ri;
}

Node::~Node() {

}