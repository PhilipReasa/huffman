#ifndef _CHAR_HUFFMAN_TOKENIZER_H_
#define _CHAR_HUFFMAN_TOKENIZER_H_

#include "HuffmanTokenizer.hpp"
#include <string>
#include <iostream>
#include <fstream>

using namespace std;

class CharHuffmanTokenizer: public HuffmanTokenizer<char> {
public:
	CharHuffmanTokenizer(string inputFileName): inputFileName(inputFileName) {
		inputFile.open(inputFileName);
	}

	~CharHuffmanTokenizer() {
		inputFile.close();
	}

	char getNextToken() {
		char nextToken;
		inputFile >> nextToken;
		return nextToken;
	}

	bool hasNextToken() {
		return !inputFile.eof();
	}

	void rewind() {
		inputFile.close();
		inputFile.open(inputFileName);
	}

	string serialize(char toSerialize) {
		return string(1, toSerialize);
	}

	char deserialize(string toDeserialize) {
		return toDeserialize[0];
	}

private:
	string inputFileName;
	ifstream inputFile;
};

#endif
