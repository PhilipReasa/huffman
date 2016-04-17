#ifndef _HUFFMAN_TOKENIZER_H_
#define _HUFFMAN_TOKENIZER_H_

#include <string>

using namespace std;

template <typename Symbol>
class HuffmanTokenizer {
public:
	virtual Symbol getNextToken() = 0;
	virtual bool hasNextToken() = 0;
	virtual void rewind() = 0;
	virtual string serialize(char in) = 0;
	virtual Symbol deserialize(string in) = 0;

	virtual ~HuffmanTokenizer() {};
};

#endif
