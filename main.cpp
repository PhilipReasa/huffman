#include "CharHuffmanTokenizer.hpp"
#include "HuffmanEncoder.hpp"

using namespace std;

int main() {
	CharHuffmanTokenizer* tokenizer = new CharHuffmanTokenizer("input.txt");

	HuffmanEncoder<char>* encoder = new HuffmanEncoder<char>(tokenizer);

	encoder->encode("encoded.txt","key.txt");

	encoder->decode("decoded.txt", "encoded.txt", "key.txt");

	delete tokenizer;
	delete encoder;
}