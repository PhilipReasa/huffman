#ifndef _HUFFMAN_TOKENIZER_H_
#define _HUFFMAN_TOKENIZER_H_

#include <string>

using namespace std;

template <typename Symbol>
class HuffmanTokenizer {
public:
	/**
	* Get the next token.
	* Return the next token in the stream. Will only be caled if
	* hasNextToken returns true
	*/
	virtual Symbol getNextToken() = 0;

	/**
	* Does the stream have another token?
	* returns true if there is a token that can be retrieved
	* returns false otherwise
	*/
	virtual bool hasNextToken() = 0;

	/**
	* Resets the stream to it's original state
	* The nature of Huffman encoding requiers running over the data twice:
	* Once for symbol requency data
	* Once for the actuall encoding
	*/
	virtual void rewind() = 0;

	/**
	* Helper function to allow symbols to be saved to a file (for the key)
	* deserialize(serialize(x)) == x
	*/
	virtual string serialize(char in) = 0;

	/**
	* Helper function to allow symbols to be read from a string (for the key)
	* deserialize(serialize(x)) == x
	*/
	virtual Symbol deserialize(string in) = 0;

	virtual ~HuffmanTokenizer() {};
};

#endif
