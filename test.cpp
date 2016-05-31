#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include "CharHuffmanTokenizer.hpp"
#include "HuffmanEncoder.hpp"

using namespace std;


SCENARIO("CharHuffmanTokenizer Tests", "[tokenizer]") {
	GIVEN("A CharHuffmanTokenizer") {
		CharHuffmanTokenizer* tokenizer = new CharHuffmanTokenizer("samples/testAlphabet");

		WHEN("you call getNextToken") {
			char token = tokenizer->getNextToken();
			char token2 = tokenizer->getNextToken();

			THEN("you get the tokens in order") {
				REQUIRE(token == 'a');
				REQUIRE(token2 == 'b');
			}
		}

		WHEN("you have gone through all of the tokens") {

			for(int i = 0; i < 26; i ++ ) {
				tokenizer->getNextToken();
			}

			THEN("hasNextToken returns false") {
				REQUIRE(tokenizer->hasNextToken() == false);
			}

			THEN("hasNextToken returns false consitently") {
				REQUIRE(tokenizer->hasNextToken() == false);
				REQUIRE(tokenizer->hasNextToken() == false);
			}
		}

		WHEN("you rewind") {
			while(tokenizer->hasNextToken()) {
				tokenizer->getNextToken();
			}

			THEN("you get back at the start") {
				tokenizer->rewind();
				REQUIRE(tokenizer->hasNextToken() == true);
				REQUIRE(tokenizer->getNextToken() == 'a');
			}
		}

		WHEN("you serialize") {
			THEN("You get the correct string") {
				REQUIRE(tokenizer->serialize('a') == "a");
			}
		}

		WHEN("You deserialize") {
			THEN("You get the correct char") {
				REQUIRE(tokenizer->deserialize("a") == 'a');
			}
		}
	}
}
