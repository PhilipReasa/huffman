# Huffman coding

Someone mentioned huffman encoding in passing one day, and I though to myself "hmmm, that would be a fun compression algo to write myself" - and so this project was born.

## Tests

The CharHuffmanTokenizer is fully unit tested in test.cpp
I have not written tests for the Overall encoder, but you can just run it to see if it is working (I would like to write tests for it though!)

## Things to note: 

* You  don't want to use this; I'm confident there are better tested and more robust libraries already in existence. Also, this actually doesn't compress data (see next point)
* I wrote this as a fun learning excercise, and one of the things I learned is that writting *bits* to a file in c++ is *very* hard. for that reason, I output ascii 0's and 1's (no actuall compression is happening)
* There are probably a lot of things done here that are not done the "standard way". I read up on the actual theory around Huffman, but as far as implementing it - this is my own creation. The best way to learn is to do it yourself! Then you can see how other people did things better or worse.
