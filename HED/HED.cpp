// HED.cpp: 콘솔 응용 프로그램의 진입점을 정의합니다.
//
#include "stdafx.h"
#include <string>
#include <vector>
#include <fstream>
#include "Node.h"
#include <iostream>
#include <time.h>

//for Global Use
#define BUF_SIZE 100//Incase make it global
#define NUM_ASCII 256//English and Extended size of ASCII
int charFreq[NUM_ASCII];//char freq[97] = 10; means 'a's frequency is 10
char* symCode[NUM_ASCII];//Array to save Huffman Code for each Word.
string document = "";
double runtime = 0;
double instant = 0;

vector<Node*> heap;
int lastHeapIdx = 0;
char codebuf[100];//to save huffman encoded codes to temp char.
int codeBufIdx = -1;


/*----------------------------------------------------------------------*/
/*
IDEA#01
Priority Queue = Min Heap -> Huffman Tree

Make a Heap in Array -> [1]based Array to count left, right child
left = 2*parentindex;
right = 2*parentindex + 1;

Node* heap[];

need Last Index to defin size of heap array.


IDEA#2
Node Object:
class Node{
	char label;
	int freq;
	Node* left;
	Node* right;
};
*/
/*----------------------------------------------------------------------*/
using namespace std;

//
//Function Declarlation
//
void addToHeap(Node* cur);
Node* deleteFromHeap();
void traverse(Node* cur, char c);
void performEncoding(char *fName);
int countNonZeroFreqChar();
void getCharFreq();
void printHeap(Node* cur);
int twoToTen(int input);

//argc: argument count
//ex: huff.exe -e alice.txt, argc==>3
//argv: argument variable
//argv[0]="huff.exe" , argv[1]="-e", argv[2]="alice.txt"

/*--------------------------------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------------------------------*/
/*----------------------------------------------main------------------------------------------------------*/
/*--------------------------------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------------------------------*/
int main(int argc, char* argv[]) {
	cout.precision(3);
	if (argv[1] && strstr(argv[1], ".txt")!=NULL) {
		// Encoding
		cout << "***HED ENCODER, CS301 CLASS REFERENCE IMPLEMENTATION***" << endl;
		performEncoding(argv[1]);
		cout << "TOTAL TIME: " << runtime << " sec"<< endl;
	}
	else {
		printf("Usage: HED.exe fileToEndoce.txt\n");
		return -1;//end the program.
	}
	return 0;
}
/*--------------------------------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------------------------------*/
/*----------------------------------------end of the main-------------------------------------------------*/
/*--------------------------------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------------------------------*/




/*--------------------------------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------------------------------*/
/*----------------------------------------encoding function-----------------------------------------------*/
/*--------------------------------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------------------------------*/
void performEncoding(char *fName) {
	FILE* fin;
	char buf[BUF_SIZE];

	fin = fopen(fName, "rt");
	//memset(charFreq, 0, NUM_ASCII * sizeof(int)); //set carFreq array's elements to zero
	if (fin == 0) {
		cout << "Unable to open, end the program" << endl;
		return;
	}
	cout << "Generating frequency data...";
	clock_t fS = clock();
	while (fgets(buf, BUF_SIZE, fin) != 0) {
		int len = strlen(buf);
		for (int i = 0; i < len; i++) {
			charFreq[(int)buf[i]]++;
		}
		document += buf;
	}
	clock_t eS = clock();
	long msec = eS - fS;
	instant = (double)msec / 1000;
	runtime += instant;
	cout << fixed << instant << " sec" << endl;
	
	cout << "Building the Huffman tree...";
	fS = clock();
	//for debugging
	//getCharFreq();
	int size = countNonZeroFreqChar();
	heap.resize(size + 1);//[1] based vector.
	for (Node* n : heap) {
		n = NULL;
	}
	//for debugging
	/*for (Node* n : heap) {
		if(n == NULL){
			cout << "NULL added to a heap vector!!" << endl;
		}
	}*/

	//Make MinHeap=Priority Queue
	for (int i = 0; i < NUM_ASCII; i++) {
		if (charFreq[i] > 0) {
			Node *tmp = new Node((char)i, charFreq[i], NULL, NULL);
			//cout << tmp->label << " added!!" << tmp->freq << endl;
			addToHeap(tmp);
		}
	}
	fclose(fin);
	

	//Debugging Purpose
	/*
	cout << "Root Node's" << endl;
	cout << "label: " << heap[1]->label << endl;
	cout << "freq: " << heap[1]->freq << endl;
	
	Node* test = deleteFromHeap();

	cout << "Root Node's" << endl;
	cout << "label: " << heap[1]->label << endl;
	cout << "freq: " << heap[1]->freq << endl;
	*/
	

	//Making Huffman tree
	Node* first = NULL;
	Node* second = NULL;

	while (1) {
		first = deleteFromHeap();
		//cout << "looping" << endl;
		second = deleteFromHeap();
		//cout << "looping" << endl;

		if (second == NULL) {
			//cout << "HF Tree Done" << endl;
			break;
		}

		Node* newOne = new Node(NULL, first->freq + second->freq, first, second);
		addToHeap(newOne);
		//cout << "add done" << endl;
	}
	eS = clock();
	msec = eS - fS;
	instant = (double)msec / 1000;
	runtime += instant;
	cout << fixed << instant << " sec" << endl;

	//printHeap(first);

	
	//memset(symCode, NULL, NUM_ASCII * sizeof(int));//Initialize Huffman Code Table
	//Wirte Huffman Code to buffer;
	cout << "Generating the Huffman codes...";
	fS = clock();
	traverse(first->left, '0');
	traverse(first->right, '1');
	eS = clock();
	msec = eS - fS;
	instant = (double)msec / 1000;
	runtime += instant;
	cout << fixed << instant << " sec" << endl;

	int numOfSym = 0;//number of char's which has frequencies
	//symCode Debugging(print prompt)
	for (char* c : symCode) {
		if (c != 0) {
			//cout << "Saved Huffman Code: " << c << endl;
			numOfSym++;
		}
	}

	//writing file

	//1. naming file.
	char outputFileName[100];
	char* period = strchr(fName, (int)'.');
	strncpy(outputFileName, fName, (int)(period - fName));//ex alice.txt get->alice
	outputFileName[(int)(period - fName)] = 0; //outputFileName = "alice" + 0
	strcat(outputFileName, ".zip301");//merging string

	//cout << outputFileName << endl;//filename.encoded

	//2. making/saving file.
	//final project rubirc version of output...still need some help from bro.draper.
	ofstream fout;
	fout.open(outputFileName);
	
	if (fout.is_open()) {
		//fwrite(&numOfSym, sizeof(numOfSym), 1, fout);//all counted number of symbol
		char wirteBuf[BUF_SIZE];
		int curBitPos = 0;//to make a size of bit(when to save)
		int rsize = 0;
		char huffBuf[BUF_SIZE];
		cout << "Encoding the document...";
		fS = clock();
		for (int i = 0; i < NUM_ASCII; i++) {
			if (symCode[i] != 0) {
				//code + ' ' + char
				fout << symCode[i];
				fout << " ";
				if ((char)i == ' ') {
					fout << "space";
				}
				else if ((char)i == '\n') {
					fout << "newline";
				}
				else if ((char)i == '\r') {
					fout << "return";
				}
				else if ((char)i == '\t') {
					fout << "tap";
				}
				else {
					fout << (char)i;
				}
				fout << endl;
			}
		}
		fout << "*****" << endl;
		int len = document.length();
		string codeDoc = "";
		eS = clock();
		msec = eS - fS;
		instant = (double)msec / 1000;
		runtime += instant;
		cout << fixed << instant << " sec" << endl;


		cout << "Writing output file (" << outputFileName << ".zip301)...";
		fS = clock();
		for (int i = 0; i < len; i++) {
			//char* tempCode = symCode[(int)document[j]];
			//cout << "Writing to the file " << tempCode << " which converted from '" << document[j] << "' " << endl;
			//fout << tempCode;
			codeDoc += symCode[(int)document[i]];
		}
		len = codeDoc.length();
		fout << len << endl;
		string binary = "";
		int decimal = 0;
		int bIdx = 0;
		unsigned char writeBuf;
		for (int i = 0; i < len; i++) {
			if (bIdx < 8) {
				binary += codeDoc[i];
				//cout << "Adding " << codeDoc[i] << "to Binary code. index: " << i << endl;
				bIdx++;
			}
			else {
				decimal = twoToTen(stoi(binary));
				//cout << "Writing " << decimal << " as a unsigned char* to the File" << endl;
				writeBuf = decimal;
				fout.put(writeBuf);
				binary = "";
				decimal = 0;
				bIdx = 0;
				i--;
			}
		}
		string sig = "";
		if (binary.length() > 0) {
			for(int i = bIdx; i < 8; i++){
				sig += '0';
				//cout << "Adding " << 0 << "to Significant code. index: out" << endl;
				bIdx++;
			}
		}
		sig += binary;
		decimal = twoToTen(stoi(sig));
		//cout << "Writing " << decimal << " as a unsigned char* to the File" << endl;
		writeBuf = decimal;
		fout.put(writeBuf);
		binary = "";
		decimal = 0;
		bIdx = 0;
	}
	eS = clock();
	msec = eS - fS;
	instant = (double)msec / 1000;
	runtime += instant;
	cout << fixed << instant << " sec" << endl;
}
/*--------------------------------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------------------------------*/
/*----------------------------------------encoding function end-------------------------------------------*/
/*--------------------------------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------------------------------*/




/*--------------------------------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------------------------------*/
/*----------------------------------------encoding sub functions------------------------------------------*/
/*--------------------------------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------------------------------*/
/*int tenToTwo(int input) {

}*/

int twoToTen(int input) {
	int result = 0, mul = 1;
	while (input > 0) {
		if (input % 2) {
			result += mul;
		}
		mul *= 2;
		input /= 10;
	}
	return result;
}
//Writing Huffman Code
void traverse(Node* cur, char c) {
	codeBufIdx++;
	codebuf[codeBufIdx] = c;
	codebuf[codeBufIdx + 1] = 0;

	if (cur->left == NULL && cur->right == NULL) {//if this current node is leaf
		//cout << cur->label <<": "<< codebuf << endl;//debugging
		//Save Huffman Code to symCode
		char* hufCode = (char *)malloc(strlen(codebuf) + 1);
		strcpy(hufCode, codebuf);
		//cout << "copied: " << hufCode << endl;//works well
		symCode[(int)cur->label] = hufCode;
		//cout << "saved: " << symCode[(int)cur->label] << endl;//working
	}
	else {
		//go through all Huffman Tree
		traverse(cur->left, '0');
		traverse(cur->right, '1');
	}
	codebuf[codeBufIdx] = 0;
	codeBufIdx--;
	return;
}

void printHeap(Node* cur) {
	cout << cur << " | " << cur->label << " | " << cur->freq << " | " << cur->left << " | " << cur->right << endl;
	if (cur->left != NULL) {
		printHeap(cur->left);
	}
	if (cur->right != NULL) {
		printHeap(cur->right);
	}
}

//add Node to heap
void addToHeap(Node* cur) {
	lastHeapIdx++;
	heap[lastHeapIdx] = cur;

	int currentIdx = lastHeapIdx;
	int parentIdx = lastHeapIdx / 2;

	//to make minheap;
	while (parentIdx >= 1) {
		if (heap[parentIdx]->freq > heap[currentIdx]->freq) {
			//exchange Parent Node and Current Node
			Node* temp = heap[parentIdx];
			heap[parentIdx] = heap[currentIdx];
			heap[currentIdx] = temp;

			currentIdx = parentIdx;
			parentIdx = currentIdx/2;
		}
		else {
			break;
		}
	}
}

//get root value from heap
Node* deleteFromHeap() {
	if (lastHeapIdx <= 0) {
		//cout << "Heap is Empty" << endl;
		return 0;
	}

	//cout << "start deleting" << endl;
	Node* reValue = heap[1];
	//cout << reValue->label << endl;
	heap[1] = heap[lastHeapIdx];
	//cout << heap[lastHeapIdx]->label << endl;
	lastHeapIdx--;

	int parent = 1;
	int left = 2 * parent;
	int right = left + 1;
	//cout << heap[parent]->label << heap[left]->label << heap[right]->label << endl;

	while (1) {
		if (left > lastHeapIdx) {//no need to compare
			break;
		}
		else if(right > lastHeapIdx){//have only left child
			if (heap[left]->freq < heap[parent]->freq) {
				//cout << "start exchange" << endl;
				//exchange
				Node* temp = heap[left];
				heap[left] = heap[parent];
				heap[parent] = temp;

				parent = left;
				left = 2 * parent;
				right = left + 1;
				//cout << heap[parent]->label << heap[left]->label << heap[right]->label << endl;
			}
			else {
				break;
			}
		}
		else {//have both left and right
			int smaller;
			if (heap[left]->freq <= heap[right]->freq) {
				smaller = left;
			}
			else {
				smaller = right;
			}
			if (heap[smaller]->freq < heap[parent]->freq) {
				//exchange
				//cout << "start exchange" << endl;
				Node* temp = heap[smaller];
				heap[smaller] = heap[parent];
				heap[parent] = temp;

				parent = smaller;
				left = 2 * parent;
				right = left + 1;
				//cout << heap[parent]->label << heap[left]->label << heap[right]->label << endl;
			}
			else {
				break;
			}
		}
	}
	return reValue;
}

//use of getting size of heap.
int countNonZeroFreqChar() {
	int cnt = 0;
	for (int i = 0; i < NUM_ASCII; i++) {
		if (charFreq[i] > 0) {
			cnt++;
		}
	}

	return cnt;
}

//Debugging purpose only(check if reading file and counting char is working)
void getCharFreq() {
	//charFreq
	//O(n)
	for (int i = 0; i < NUM_ASCII; i++) {
		//get index values for each char only if it has frequency.
		if (charFreq[i] > 0) {
			cout << "Char " << (char)i << ": " << charFreq[i] << endl;
		}
	}
}
/*--------------------------------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------------------------------*/
/*--------------------------------------encoding sub functions end----------------------------------------*/
/*--------------------------------------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------------------------------------*/

