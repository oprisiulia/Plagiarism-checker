#include <string>
#include <algorithm>
#include <cmath>
#include <iostream>
#include <fstream>
#include <sstream>
#include <unordered_set>
#include <vector>
#include <stdexcept>
#include <unordered_map>

#include "plagiarism.h"

using namespace std;

Plagiarism::Plagiarism(const string& fileA, const string& fileB, int k_gram)
    : k_gram(k_gram) {
    // Calculate hash values for both files
    calc_hash(get_file(fileA), "a");
    calc_hash(get_file(fileB), "b");
}

float Plagiarism::get_rate() {
    return calc_plag_rate();
}

std::string Plagiarism::get_file(const string& filename) {
    ifstream file(filename);
    if (!file.is_open()) {
        throw runtime_error("Could not open file: " + filename);
    }
    stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

bool Plagiarism::contains(const unordered_set<std::string>& set, const string& word) {
    return set.find(word) != set.end();
}

std::string Plagiarism::preparation(const string& content) {
    istringstream iss(content);
    vector<std::string> tokens;
    string token;
    // Tokenize the content word by word
    while (iss >> token) {
        // Remove punctuation characters
        token.erase(std::remove_if(token.begin(), token.end(), ::ispunct), token.end());
        // Convert all characters to lowercase
        transform(token.begin(), token.end(), token.begin(), ::tolower);
        // Check for stopwords
        if (!contains(stopwords, token)) {
            tokens.push_back(stem(token));
        }
    }
    // Concatenate words without any separator
    return join(tokens, "");
}

std::string Plagiarism::stem(const std::string& word) {
    std::string stemmed = word;

    if (stemmed.length() > 4) {
        if (stemmed.substr(stemmed.length() - 3) == "ing") {
            stemmed = stemmed.substr(0, stemmed.length() - 3);
        }
        else if (stemmed.substr(stemmed.length() - 2) == "ed") {
            stemmed = stemmed.substr(0, stemmed.length() - 2);
        }
        else if (stemmed.substr(stemmed.length() - 2) == "es") {
            stemmed = stemmed.substr(0, stemmed.length() - 2);
        }
        else if (stemmed.back() == 's' && stemmed.length() > 1) {
            stemmed.pop_back();
        }
    }

    return stemmed;
}

void Plagiarism::calc_hash(const string& content, const string& doc_type) {
    string text = preparation(content);
    RabinKarp text_rolling(text, k_gram);
    for (size_t i = 0; i <= text.size() - k_gram + 1; ++i) {
        hash_table[doc_type].push_back(text_rolling.hash());
        if (!text_rolling.next_window()) {
            break;
        }
    }
}

float Plagiarism::calc_plag_rate() {
    size_t hashes_a = hash_table["a"].size();
    size_t hashes_b = hash_table["b"].size();
    auto intersect = vector_intersect(hash_table["a"], hash_table["b"]);
    size_t size_hash = intersect.size();

    // Plagiarism rate formula: P = (2 * size_hash / (hashes_a + hashes_b)) * 100%
    float p = static_cast<float>(2 * size_hash) / static_cast<float>(hashes_a + hashes_b);
    return p * 100;
}

std::vector<int> Plagiarism::vector_intersect(const vector<int>& a, const vector<int>& b) {
    unordered_set<int> set_a(a.begin(), a.end());
    vector<int> intersection;
    for (int val : b) {
        if (set_a.find(val) != set_a.end()) {
            intersection.push_back(val);
        }
    }
    return intersection;
}

std::string Plagiarism::join(const vector<std::string>& vec, const string& delimiter) {
    ostringstream oss;
    for (auto it = vec.begin(); it != vec.end(); ++it) {
        if (it != vec.begin()) {
            oss << delimiter;
        }
        oss << *it;
    }
    return oss.str();
}

Plagiarism::RabinKarp::RabinKarp(const string& text, int k_gram)
    : text(text), k_gram(k_gram), base(256), mod(101) {
    pattern = text.substr(0, k_gram);
    hash_value = hash_compute(pattern);
}

int Plagiarism::RabinKarp::hash() const {
    return hash_value;
}

int Plagiarism::RabinKarp::hash_compute(const string& s) {
    int hash = 0;
    for (char c : s) {
        hash = (hash * base + c) % mod;
    }
    return hash;
}

bool Plagiarism::RabinKarp::next_window() {
    if (text.size() <= k_gram) {
        return false;
    }

    // Remove the influence of the first character
    int old_first_char = pattern[0];
    pattern = pattern.substr(1) + text[k_gram];

    // Calculate base^(k_gram-1)
    int power = static_cast<int>(pow(base, k_gram - 1));

    // Update the hash value using rolling hash formula
    hash_value = (hash_value - old_first_char * power % mod + mod) % mod;
    hash_value = (hash_value * base + text[k_gram]) % mod;

    k_gram++;

    return true;
}
