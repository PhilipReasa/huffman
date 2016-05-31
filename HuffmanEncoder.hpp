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
	//NOTE: the way we are doing leaf/non-leaf nodes is not robust, and can easily be improved

	//constructor to be used for leaf nodes
	HuffmanEncoderNode(Symbol symbol, int value): symbol(symbol), value(value), isLeaf(true) {}
	
	//constructor to be used for value only nodes
	HuffmanEncoderNode(int value): value(value), isLeaf(false)  {}

	//needed for compare function
	HuffmanEncoderNode() {} 

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

	/**
	* Provided a file to save compressed data to,
	* and a file to save the compression key, this function
	* will Huffman encode your file.
	*/
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
			//encode the tokens one by one
			token = tokenizer->getNextToken();
			encodedSymbols += symbolToBitMapping.at(token); //using "at" so error will throw if symbol not mapped
		}
		ofstream encodedFile(outputFile);
		encodedFile << encodedSymbols;
		encodedFile.close();
	}

	/**
	* Given a spot to save the decoded file, the location of the encoded file, 
	* and the encoding key file name, this function will decode the encoded file
	* and save the results.
	*/
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
		bitToSymbolMapping = deserializeFlattenedHuffmanTree(serializedFlattenedHuffmanTree);

		//decode the file
		ifstream encodedFile(encodedFileName);
		char bit;
		string key = "";
		string decodedString = "";

		encodedFile >> bit;
		while(bit == '1' || bit == '0') {
			//walk through the file one bit at a time.
			key += bit;
			if(bitToSymbolMapping.count(key)) {
				//we have found an encoding key
				decodedString += bitToSymbolMapping.at(key);
				key = "";
			}

			bit = '\0'; //so that our loop fails if we are out of chars in file
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
	* side-effects: tokenizer is reset
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

	/**
	* Where a good portion of the majic happens!
	* Takes the symbol->frequency map and build out an optimal
	* huffman encoding tree. So cool!
	*/
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

	/**
	* Root of a recursive function.
	*/
	void flattenHuffmanTree(HuffmanEncoderNode<Symbol>* node, unordered_map<Symbol, string>* symbolToBitMapping) {
		flattenHuffmanTree(node, "", symbolToBitMapping);
	}

	/**
	* Takes a tree, and turns it into a path->value map. The path is
	* discovered in a bepth first manner (although it doesn't really matter)
	*/
	void flattenHuffmanTree(HuffmanEncoderNode<Symbol>* node, string path, unordered_map<Symbol, string>* symbolToBitMapping) {
		if(node->isLeaf) {
			(*symbolToBitMapping)[node->symbol] = path;
			return;
		}

		flattenHuffmanTree(node->left, path + "0", symbolToBitMapping);
		flattenHuffmanTree(node->right, path + "1", symbolToBitMapping);
	}

	/**
	* Turns a flattened huffman tree into something that can be saved to disk
	*/
	string serializeFlattenedHuffmanTree(unordered_map<Symbol, string>* symbolToBitMapping) {
		string serialization = "";
		for(auto mappingIter = symbolToBitMapping->begin(); mappingIter != symbolToBitMapping->end(); mappingIter ++) {
			serialization += mappingIter->second + ":" + mappingIter->first + "\n";
		}

		return serialization;
	}

	/**
	* Turns a serialized flattened huffman tree in a more usable symbol based
	* flattened huffman tree
	*/
	unordered_map<string, Symbol> deserializeFlattenedHuffmanTree(vector<string> serialization) {
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
