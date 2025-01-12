#include <iostream>
#include <vector>
#include <unordered_map>
#include <sstream>
#include <limits>
#include <regex>

using namespace std;

pair<int, int> twoSum(const vector<int>& arr, int target) {
    unordered_map<int, int> lookup;
    for (int i = 0; i < arr.size(); ++i) {
        int complement = target - arr[i];
        if (lookup.find(complement) != lookup.end()) {
            return {lookup[complement], i};
        }
        lookup[arr[i]] = i;
    }
    return {-1, -1}; 
}

// Improved regex to allow both space and comma as separators
bool isValidInput(const string& input) {
    regex pattern(R"(^\s*-?\d+(\s*[, ]\s*-?\d+)*\s*$)"); 
    return regex_match(input, pattern);
}

vector<int> parseInput(const string& input) {
    vector<int> result;
    string cleanedInput;
    
    // Replace commas with spaces to standardize input
    for (char c : input) {
        if (c == ',') {
            cleanedInput += ' ';
        } else {
            cleanedInput += c;
        }
    }

    stringstream ss(cleanedInput);
    int num;
    
    while (ss >> num) {
        result.push_back(num);
    }
    
    return result;
}

int main() {
    string input;
    int target;

    cout << "Enter numbers (space/comma separated): ";
    getline(cin, input);

    if (!isValidInput(input)) {
        cout << "Invalid input. Retry." << endl;
        return 1;
    }

    vector<int> nums = parseInput(input);

    if (nums.size() < 2 || nums.size() > 10000) {
        cout << "Error: The number of elements must be between 2 and 10^4. Retry." << endl;
        return 1;
    }

    cout << "Enter target: ";
    if (!(cin >> target)) {  
        cout << "Invalid target input. Please enter a valid integer." << endl;
        return 1;
    }

    if (target < -1000000000 || target > 1000000000) {  
        cout << "Error: Target must be in the range -10^9 to 10^9. Retry." << endl;
        return 1;
    }

    pair<int, int> result = twoSum(nums, target);
    
    if (result.first != -1) {
        cout << "[" << result.first << ", " << result.second << "]" << endl;
    } else {
        cout << "No valid pair found." << endl;
    }

    return 0;
}
