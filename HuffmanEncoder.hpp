#ifndef _HUFFMAN_ENCODER_H_
#define _HUFFMAN_ENCODER_H_

#include "HuffmanTokenizer.hpp"
#include <string>
#include <queue>
#include <unordered_map>
#include <vector>
#include <iostream>
#include <fstream>

using namespace std;

template <typename Symbol>
class HuffmanEncoderNode {
public:
	HuffmanEncoderNode(Symbol symbol, int value): symbol(symbol), value(value), isLeaf(true) {}
	HuffmanEncoderNode(int value): value(value), isLeaf(false)  {}
	HuffmanEncoderNode() {} //needed for compare function

	HuffmanEncoderNode* right;
	HuffmanEncoderNode* left;
	Symbol symbol;
	int value;
	bool isLeaf;

    bool operator() (HuffmanEncoderNode<Symbol>* lhs, HuffmanEncoderNode<Symbol>* rhs) {
        return lhs->value > rhs->value;
    }
};


template <typename Symbol>
class HuffmanEncoder {
public:
	HuffmanEncoder(HuffmanTokenizer<Symbol>* tokenizer): tokenizer(tokenizer) {}

	void encode(string outputFile, string outputKey) {
		unordered_map<Symbol, int> frequencyHash;
		priority_queue<HuffmanEncoderNode<Symbol>*, std::vector<HuffmanEncoderNode<Symbol>*>, HuffmanEncoderNode<Symbol>> heap;
		unordered_map<Symbol, string> symbolToBitMapping;
		Symbol token;

		//build a map with a count of data
		frequencyHash = getTokenFrequencyStats();

		//build a priority queue with the occurance info
		auto hashIter = frequencyHash.begin();
		while(hashIter != frequencyHash.end()) {
			HuffmanEncoderNode<Symbol>* node = new HuffmanEncoderNode<Symbol>(hashIter->first, hashIter->second);
			heap.push(node);
			hashIter++;
		}

		//build a encoding tree
		int combinedValue;
		HuffmanEncoderNode<Symbol>* leftNode, *rightNode;
		while(heap.size() > 1) {
			leftNode = heap.top();
			combinedValue = leftNode->value;
			heap.pop();

			rightNode = heap.top();
			combinedValue += rightNode->value;
			heap.pop();

			HuffmanEncoderNode<Symbol>* node = new HuffmanEncoderNode<Symbol>(combinedValue);
			node->left = leftNode;
			node->right = rightNode;
			heap.push(node);
		}

		//save the encoding key to file
		string key = "";
		key = createKeyCoding(heap.top(), "", key);
		ofstream keyFile(outputKey);
		keyFile << key;
		keyFile.close();


		//create symbol --> bit mapping
		int lastColonLocation = 0;
		int nextColonLocation = key.find(":",0);
		int lastNewLineLocation = 0;
		int nextNewlineLocation = key.find("\n",0) + 1;
		Symbol symbolKey;
		string symbolValue;
		while(nextColonLocation != string::npos) {
			symbolValue = key.substr(lastNewLineLocation, nextColonLocation - lastNewLineLocation);
			symbolKey = tokenizer->deserialize(key.substr(nextColonLocation + 1, (nextNewlineLocation - nextColonLocation) - 2));

			symbolToBitMapping[symbolKey] = symbolValue;

			lastColonLocation = nextColonLocation;
			lastNewLineLocation = nextNewlineLocation;
			nextColonLocation = key.find(":", lastColonLocation + 1);
			nextNewlineLocation = key.find("\n", lastNewLineLocation) + 1;
		}


		//create the encoded file
		tokenizer->rewind();
		string encodedSymbols;
		while(tokenizer->hasNextToken()) {
			token = tokenizer->getNextToken();
			encodedSymbols += symbolToBitMapping.at(token); //using at so error will throw if symbol not mapped
		}
		ofstream encodedFile(outputFile);
		encodedFile << encodedSymbols;
		keyFile.close();

	}

	void decode(string decodeFile, string encodingKey) {

	}

private:
	string createKeyCoding(HuffmanEncoderNode<Symbol>* node, string path, string key) {
		if(node->isLeaf) {
			return path + ":" + tokenizer->serialize(node->symbol) + "\n";
		}

		key += createKeyCoding(node->left, path + "0", "");
		key += createKeyCoding(node->right, path + "1", "");

		return key;
	}

	/**
	* generates a unordered_map with symbols and their number of occurences
	* sideeffects: tokenizer is reset
	*/
	unordered_map<Symbol, int> getTokenFrequencyStats() {
		tokenizer->rewind(); //incase they used the tokenizer before calling this function
		unordered_map<Symbol, int> hash;
		Symbol token;

		while(tokenizer->hasNextToken()) {
			token = tokenizer->getNextToken();
			if(hash.count(token) == 0) {
				hash[token] = 1;
			} else {
				hash[token] += 1;
			}
		}

		tokenizer->rewind(); //leave the tokenizer in a reset state
		return hash;
	}


	HuffmanTokenizer<Symbol>* tokenizer;

};

#endif
