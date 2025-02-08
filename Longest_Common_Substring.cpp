#include <iostream>
#include <vector>
#include <algorithm>

void toUpper(std::string& s) {
    for (char& c : s) c = toupper(c);
}

bool isAlpha(const std::string& s) {
    return std::all_of(s.begin(), s.end(), ::isalpha);
}

std::string findLCS(const std::string& s1, const std::string& s2, int& len) {
    int n = s1.size();
    std::vector<std::vector<int>> dp(n + 1, std::vector<int>(n + 1, 0));
    len = 0;
    int endIdx = 0;

    for (int i = 1; i <= n; ++i) {
        for (int j = 1; j <= n; ++j) {
            if (s1[i - 1] == s2[j - 1]) {
                dp[i][j] = dp[i - 1][j - 1] + 1;
                if (dp[i][j] > len) {
                    len = dp[i][j];
                    endIdx = i;
                }
            }
        }
    }
    
    std::cout << "LCS Table:\n";
    for (const auto& row : dp) {
        for (int cell : row) std::cout << cell << " ";
        std::cout << "\n";
    }

    return s1.substr(endIdx - len, len);
}

int main() {
    std::string s1, s2;
    while (true) {
        std::cout << "Enter first string (or 'quit' to exit): ";
        std::cin >> s1;
        if (s1 == "quit") break;
        
        std::cout << "Enter second string (or 'quit' to exit): ";
        std::cin >> s2;
        if (s2 == "quit") break;

        if (!isAlpha(s1) || !isAlpha(s2)) {
            std::cout << "Error: Only alphabetic characters allowed.\n";
            continue;
        }

        toUpper(s1);
        toUpper(s2);
        
        int lcsLen;
        std::string lcs = findLCS(s1, s2, lcsLen);
        
        std::cout << "Longest Common Substring: " << lcs << "\n";
        std::cout << "Length: " << lcsLen << "\n";
    }
    return 0;
}
