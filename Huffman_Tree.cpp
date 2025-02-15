#include <iostream>
#include <queue>
#include <unordered_map>
#include <vector>
#include <sstream>
#include <algorithm>
#include <limits>
using namespace std;

struct HuffmanTreeNode {
    char character;
    int frequency;
    HuffmanTreeNode *left, *right;
    
    HuffmanTreeNode(char c, int f) : character(c), frequency(f), left(nullptr), right(nullptr) {}
};

struct CompareNodes {
    bool operator()(HuffmanTreeNode* a, HuffmanTreeNode* b) {
        return a->frequency > b->frequency;
    }
};

void generateHuffmanCodes(HuffmanTreeNode* root, string code, unordered_map<char, string>& huffmanMap) {
    if (!root) return;
    if (!root->left && !root->right) {
        huffmanMap[root->character] = code;
    }
    generateHuffmanCodes(root->left, code + "0", huffmanMap);
    generateHuffmanCodes(root->right, code + "1", huffmanMap);
}

unordered_map<char, string> buildHuffmanTree(const string& symbols, const vector<int>& frequencies) {
    priority_queue<HuffmanTreeNode*, vector<HuffmanTreeNode*>, CompareNodes> minHeap;
    for (size_t i = 0; i < symbols.size(); ++i) {
        minHeap.push(new HuffmanTreeNode(symbols[i], frequencies[i]));
    }
    while (minHeap.size() > 1) {
        HuffmanTreeNode *left = minHeap.top(); minHeap.pop();
        HuffmanTreeNode *right = minHeap.top(); minHeap.pop();
        HuffmanTreeNode *parent = new HuffmanTreeNode('#', left->frequency + right->frequency);
        parent->left = left;
        parent->right = right;
        minHeap.push(parent);
    }
    unordered_map<char, string> huffmanMap;
    generateHuffmanCodes(minHeap.top(), "", huffmanMap);
    return huffmanMap;
}

bool isValidCharacterSet(const string& input) {
    return all_of(input.begin(), input.end(), [](char ch) { return isalpha(ch); });
}

bool isValidFrequencyInput(const string& input, vector<int>& frequencyList, size_t expectedSize) {
    frequencyList.clear();
    stringstream freqStream(input);
    int freq;
    while (freqStream >> freq) {
        frequencyList.push_back(freq);
    }
    return frequencyList.size() == expectedSize;
}

int main() {
    string characterSet;
    vector<int> frequencies;
    size_t numSymbols;
    
    while (true) {
        cout << "Enter string example abcdef (or type 'exit' to quit): ";
        cin >> characterSet;
        if (characterSet == "exit" || characterSet == "quit") return 0;
        if (isValidCharacterSet(characterSet)) break;
        cout << "Invalid input. Please enter alphabetic characters only." << endl;
    }
    
    numSymbols = characterSet.size();
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    
    while (true) {
        string freqInput;
        cout << "Enter the frequency example 5, 9, 12, 13, 16, 45 (or type 'exit' to quit): ";
        getline(cin, freqInput);
        if (freqInput == "exit" || freqInput == "quit") return 0;
        if (isValidFrequencyInput(freqInput, frequencies, numSymbols)) break;
        cout << "Invalid input. you may entered string or  Please re-enter frequencies as space-separated integers." << endl;
    }
    
    unordered_map<char, string> huffmanCodes = buildHuffmanTree(characterSet, frequencies);
    cout << "Huffman Codes in Preorder Traversal:\n";
    priority_queue<HuffmanTreeNode*, vector<HuffmanTreeNode*>, CompareNodes> minHeap;
    for (size_t i = 0; i < characterSet.size(); ++i) {
        minHeap.push(new HuffmanTreeNode(characterSet[i], frequencies[i]));
    }
    while (minHeap.size() > 1) {
        HuffmanTreeNode *left = minHeap.top(); minHeap.pop();
        HuffmanTreeNode *right = minHeap.top(); minHeap.pop();
        HuffmanTreeNode *parent = new HuffmanTreeNode('#', left->frequency + right->frequency);
        parent->left = left;
        parent->right = right;
        minHeap.push(parent);
    }
    HuffmanTreeNode* root = minHeap.top();
    for (const auto& pair : huffmanCodes) {
        cout << "  " << pair.first << " : " << pair.second << endl;
    }
    return 0;
}
