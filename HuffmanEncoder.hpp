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
		HuffmanEncoderNode<Symbol>* huffmanTree;
		unordered_map<Symbol, string> symbolToBitMapping;
		string key;
		Symbol token;

		frequencyHash = getTokenFrequencyStats();

		huffmanTree = generateHuffmanTree(frequencyHash);

		flattenHuffmanTree(huffmanTree, &symbolToBitMapping);

		//save the encoding key to file
		key = serializeFlattenedHuffmanTree(&symbolToBitMapping);
		ofstream keyFile(outputKey);
		keyFile << key;
		keyFile.close();

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

	void decode(string decodeFileName, string encodedFileName, string encodingKeyName) {
		unordered_map<string, Symbol> bitToSymbolMapping;
		vector<string> serializedFlattenedHuffmanTree;
		string keyLine;

		//read in the key
		ifstream keyFile(encodingKeyName);
		while(keyFile >> keyLine) {
			serializedFlattenedHuffmanTree.push_back(keyLine);
		}
		keyFile.close();

		//deserialize the key
		bitToSymbolMapping = reverseDeserializeFlattenedHuffmanTree(serializedFlattenedHuffmanTree);

		//decode the file
		ifstream encodedFile(encodedFileName);
		char bit;
		encodedFile >> bit;
		string key = "";
		string decodedString = "";
		while(bit == '1' || bit == '0') {
			key += bit;
			if(bitToSymbolMapping.count(key)) {
				decodedString += bitToSymbolMapping.at(key);
				key = "";
			}

			bit = '\0';
			encodedFile >> bit;
		}

		//save the decoded file
		ofstream decodedFile(decodeFileName);
		decodedFile << decodedString;
		decodedFile.close();
	}

private:
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

	HuffmanEncoderNode<Symbol>* generateHuffmanTree(unordered_map<Symbol, int> frequencyHash) {
		priority_queue<HuffmanEncoderNode<Symbol>*, std::vector<HuffmanEncoderNode<Symbol>*>, HuffmanEncoderNode<Symbol>> heap;

		//build nodes and put into priority queue
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

		return heap.top();
	}

	void flattenHuffmanTree(HuffmanEncoderNode<Symbol>* node, unordered_map<Symbol, string>* symbolToBitMapping) {
		flattenHuffmanTree(node, "", symbolToBitMapping);
	}

	void flattenHuffmanTree(HuffmanEncoderNode<Symbol>* node, string path, unordered_map<Symbol, string>* symbolToBitMapping) {
		if(node->isLeaf) {
			(*symbolToBitMapping)[node->symbol] = path;
			return;
		}

		flattenHuffmanTree(node->left, path + "0", symbolToBitMapping);
		flattenHuffmanTree(node->right, path + "1", symbolToBitMapping);
	}

	string serializeFlattenedHuffmanTree(unordered_map<Symbol, string>* symbolToBitMapping) {
		string serialization = "";
		for(auto mappingIter = symbolToBitMapping->begin(); mappingIter != symbolToBitMapping->end(); mappingIter ++) {
			serialization += mappingIter->second + ":" + mappingIter->first + "\n";
		}

		return serialization;
	}

	unordered_map<string, Symbol> reverseDeserializeFlattenedHuffmanTree(vector<string> serialization) {
		unordered_map<string, Symbol> flattenedHuffmanTree;

		int colonLocation;
		string currentKeyValue;
		Symbol symbolKey;
		string symbolValue;
		for(auto it = serialization.begin(); it != serialization.end(); it++) {
			currentKeyValue = *it;
			colonLocation = currentKeyValue.find(":");
			symbolValue = currentKeyValue.substr(0, colonLocation);
			symbolKey = tokenizer->deserialize(currentKeyValue.substr(colonLocation + 1, currentKeyValue.size()));

			flattenedHuffmanTree[symbolValue] = symbolKey;
		}

		return flattenedHuffmanTree;
	}


	HuffmanTokenizer<Symbol>* tokenizer;

};

#endif
