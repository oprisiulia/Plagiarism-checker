#ifndef PLAGIARISM_H
#define PLAGIARISM_H

#include <iostream>
#include <fstream>
#include <sstream>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <algorithm>
#include <string>
#include <iterator>
#include <cctype>

using namespace std;

class Plagiarism {
public:
    // Constructor: initializes file names and k-gram value for plagiarism detection.
    Plagiarism(const string& fileA, const string& fileB, int k_gram = 5);
    // Returns the plagiarism rate as a percentage.
    float get_rate();

private:
    unordered_map<string, vector<int>> hash_table;
    int k_gram;
    unordered_set<string> stopwords = { "the", "is", "at", "of", "on", "and", "a", "to" };

    string get_file(const string& filename);
    bool contains(const unordered_set<string>& set, const string& word);
    string preparation(const string& content);
    string stem(const string& word);
    void calc_hash(const string& content, const string& doc_type);
    float calc_plag_rate();
    vector<int> vector_intersect(const vector<int>& a, const vector<int>& b);
    string join(const vector<string>& vec, const string& delimiter);

    // Inner class for rolling hash computation using the Rabin-Karp algorithm.
    class RabinKarp {
    public:
        RabinKarp(const string& text, int k_gram);
        int hash() const;
        bool next_window();

    private:
        string text;
        string pattern;
        int k_gram;
        int hash_value;
        int base;
        int mod;
        int hash_compute(const string& s);
    };
};

#endif // PLAGIARISM_H
