#include "CharHuffmanTokenizer.hpp"
#include "HuffmanEncoder.hpp"

using namespace std;

int main() {
	CharHuffmanTokenizer* tokenizer = new CharHuffmanTokenizer("samples/input.txt");

	HuffmanEncoder<char>* encoder = new HuffmanEncoder<char>(tokenizer);

	encoder->encode("samples/encoded.txt","samples/key.txt");

	encoder->decode("samples/decoded.txt", "samples/encoded.txt", "samples/key.txt");

	delete tokenizer;
	delete encoder;
}