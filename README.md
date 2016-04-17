#Huffman coding
Someone mentioned huffman encoding in passing one day, and I though to myself "hmmm, that would be a fun compression algo to write myself" - and so this project was born.

Things to note: 
* You  don't want to use this; I'm confident there are better tested and more robust libraries already in existence. Also, this actually doesn't compress data (see next point)
* I wrote this as a fun learning excercise, and one of the things I learned is that writting *bits* to a file in c++ is *very* hard. for that reason, I output ascii 0's and 1's
* There are probably a lot of things done here that are not done the "standard way". I read up on the actual theory around Huffman, but as far as implementing it - this is my own creation. The best way to learn is to do it yourself, and then see how other people did things better or worse. You have to do it yourself first though!
