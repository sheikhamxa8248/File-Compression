//
// Author: Hamza Sheikh
// CS 251 project 6
// Util.h: This function contains the extra functions that
// implement the Huffman algorithm which helps with file
// compression.
//

#pragma once

#include <queue>
#include "hashmap.h"
#include "bitstream.h"

typedef hashmap hashmapF;
typedef unordered_map <int, string> hashmapE;

struct HuffmanNode {
    int character;
    int count;
    int order;
    HuffmanNode* zero;
    HuffmanNode* one;
};

// This class is used for ordering elements in the priority quueue
class prioritize {
    public: bool operator() (const pair<HuffmanNode*, int> &p1,
    const pair<HuffmanNode*, int> &p2) const {
        // if they have the same count its ordered by order of insert.
        if (p1.second == p2.second)
            return p1.first->order > p2.first->order;
        else
            return p1.second > p2.second;  
    }
};

// This function takes the order, count, and character and allocates
// memory for a new node and returns it.
HuffmanNode* makeNode(int character, int count, int order) {
    HuffmanNode* node = new HuffmanNode;
    node->character = character;
    node->count = count;
    node->order = order;
    node->zero = node->one = nullptr;
    return node;
}

//
// This method frees the memory allocated for the Huffman tree
// by traversing through each node and deleting it.
//
void freeTree(HuffmanNode* node) {
    if (node == nullptr)
            return;
    freeTree(node->zero);
    freeTree(node->one);
    delete node;
}

//
// This function build the frequency map.  If isFile is true, then it reads
// from filename.  If isFile is false, then it reads from a string filename.
//
//
void buildFrequencyMap(string filename, bool isFile, hashmapF &map) {
    int value = 0;
    if (isFile) {
        ifbitstream file(filename);
        char letter;
        while (file >> noskipws >> letter) {
            if (map.containsKey(letter)) {  // if char already in map
                value = map.get(letter);
                map.put((int)letter, value + 1);  // increments count by 1
            } else {
                map.put((int)letter, 1);
            }
        }
        map.put(PSEUDO_EOF, 1);  // 1 EOF added in the end
        file.close();
    } else {  // filename is a string
        size_t i;
        for (i = 0; i < filename.length(); i++) {
            if (map.containsKey(filename[i])) {  // if char already in map
                value = map.get(filename[i]);
                map.put((int)filename[i], value + 1);  // adds 1 to curr count
            } else {
                map.put((int)filename[i], 1);
            }
        }
        map.put(PSEUDO_EOF, 1);  // 1 EOF added in the end
    }
}

//
// This function builds an encoding tree from the frequency map.
//
HuffmanNode* buildEncodingTree(hashmapF &map) {
    priority_queue <
    pair<HuffmanNode*, int>,
    vector<pair<HuffmanNode*, int>>,
    prioritize> pq;
    int order = 0;
    // loops through the map and makes nodes for character and its count.
    // then adds the nodes to the priority queue
    for (auto &character : map.keys()) {
        int count = map.get(character);
        HuffmanNode* node = makeNode(character, count, order);
        pq.push(pair<HuffmanNode*, int>(node, count));
        order++;
    }
    HuffmanNode* root = nullptr;
    // Takes the first two nodes, makes a new node as their parent with
    // their combined counts. Keeps doing this until theirs only one node
    // in the queue which means we have a tree.
    while (pq.size() != 1) {
        pair<HuffmanNode*, int> pairOne = pq.top();
        pq.pop();
        pair<HuffmanNode*, int> pairTwo = pq.top();
        pq.pop();
        root = makeNode(NOT_A_CHAR, pairOne.second + pairTwo.second, order);
        root->zero = pairOne.first;
        root->one = pairTwo.first;
        pq.push(make_pair(root, pairOne.second + pairTwo.second));
        order++;
    }
    return root;
}

//
// Recursive helper function for building the encoding map.
//
void _buildEncodingMap(HuffmanNode* node, hashmapE &encodingMap, string str) {
    if (!node->zero && !node->one) {  // if leaf node
        // insert char and its encoding
        encodingMap.insert({(int)node->character, str});
        return;
    }
    if (node->zero) {
        str += "0";  // adds zero if their is a left child
        _buildEncodingMap(node->zero, encodingMap, str);
        // after it comes back from its left child and moves on to the right
        // child it doesn't need the 0 anymore in the string so we get rid of
        // that here
        str = str.substr(0, str.length() - 1);
    }
    if (node->one) {
        str += "1";  // adds one if their is a right child
        _buildEncodingMap(node->one, encodingMap, str);
    }
}

//
// This function builds the encoding map from an encoding tree.
//
hashmapE buildEncodingMap(HuffmanNode* tree) {
    hashmapE encodingMap;
    if (!tree)  // if nullptr return empty encodingMap
        return encodingMap;
    string str = "";
    _buildEncodingMap(tree, encodingMap, str);  // helper function called
    return encodingMap;
}

//
// This function encodes the data in the input stream into the output stream
// using the encodingMap.  This function calculates the number of bits
// written to the output stream and sets result to the size parameter, which is
// passed by reference.  This function also returns a string representation of
// the output file, which is particularly useful for testing.
//
string encode(ifstream& input, hashmapE &encodingMap, ofbitstream& output, int &size, bool makeFile) {
    string str = "";
    char character;
    while (input.get(character)) {
        str += encodingMap[character];  // add encodings of each char to str
    }
    str += encodingMap[PSEUDO_EOF];
    if (makeFile) {  // if we have to make a file
        for (auto &bite: str) {
            // loop through str which has the encodings and write it into
            // the output file one by one.
            output.writeBit(bite - '0');
        }   
    }
    size += str.length();  // adds the number of bits written to size
    return str;
}

//
// This function decodes the input stream and writes the result to the output
// stream using the encodingTree.  This function also returns a string
// representation of the output file, which is particularly useful for testing.
//
string decode(ifbitstream &input, HuffmanNode* encodingTree, ofstream &output) {
    string str = "";
    HuffmanNode* tmp = encodingTree;
    int bit = 0;
    while (bit != PSEUDO_EOF) {
        bit = input.readBit();  // keeps on getting the chars
        if (bit == 1)
            tmp = tmp->one;
        if (bit == 0)
            tmp = tmp->zero;
        if (!tmp->one && !tmp->zero) {  //.if leaf node
            if (tmp->character == PSEUDO_EOF)  // if EOF then break
                break;
            // otherwise add the char to str and output
            str += (char)tmp->character;
            output << (char)tmp->character;
            // move tmp back up to the root
            tmp = encodingTree;
            continue;
        }
    }
    return str;
}

//
// This function completes the entire compression process.  Given a file,
// filename, this function (1) builds a frequency map; (2) builds an encoding
// tree; (3) builds an encoding map; (4) encodes the file (don't forget to
// include the frequency map in the header of the output file).  This function
// should create a compressed file named (filename + ".huf") and should also
// return a string version of the bit pattern.
//
string compress(string filename) {
    hashmapF frequencyMap;
    HuffmanNode* encodingTree = nullptr;
    hashmapE encodingMap;
    buildFrequencyMap(filename, true, frequencyMap);
    encodingTree = buildEncodingTree(frequencyMap);
    encodingMap = buildEncodingMap(encodingTree);
    ofbitstream output(filename + ".huf");
    ifstream input(filename);
    stringstream stream;
    stream << frequencyMap;
    output << frequencyMap;
    int size = 0;
    string compressedString = encode(input, encodingMap, output, size, true);
    output.close();
    freeTree(encodingTree);
    return compressedString;
}

//
// This function completes the entire decompression process.  Given the file,
// filename (which should end with ".huf"), (1) extract the header and build
// the frequency map; (2) build an encoding tree from the frequency map; (3)
// using the encoding tree to decode the file.  This function should create a
// compressed file using the following convention.
// If filename = "example.txt.huf", then the uncompressed file should be named
// "example_unc.txt".  The function should return a string version of the
// uncompressed file.  Note this function should reverse what the compress
// function did.
//
string decompress(string filename) {
    size_t pos = filename.find(".huf");
    if ((int)pos >= 0) {
        filename = filename.substr(0, pos);
    }
    pos = filename.find(".");
    string ext = filename.substr(pos, filename.length() - pos);
    filename = filename.substr(0, pos);
    ifbitstream input(filename + ext + ".huf");  // opens this file for reading
    ofstream output(filename + "_unc" + ext);  // creates this file for output
    hashmapF header;
    input >> header;  // makes the frequency map using the >> operator
    HuffmanNode* encodingTree = buildEncodingTree(header);
    string decodeStr  = decode(input, encodingTree, output);
    output.close();
    freeTree(encodingTree);
    return decodeStr;
}
