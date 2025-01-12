#include "tokenizer.h"
#include <string>
#include <vector>
#include <sstream>
#include <cctype>
using namespace std;
vector<string> Tokenizer::tokenize(const string& input)
{
    vector<string> tokens;
    istringstream stream(input);
    string token;
    bool inQuotes = false;
    string currentToken;

    while (stream >> ws) { // Skip any leading whitespace
        char c = stream.peek();
        if (c == '\"') {
            // Handle quoted string
            stream.get(); // Consume the opening quote
            getline(stream, token, '\"'); // Read until closing quote
            tokens.push_back(token);
        }
        else {
            // Read the next word
            stream >> token;
            tokens.push_back(token);
        }
    }

    return tokens;
}



/*vector<string> Tokenizer::tokenize(const string& str) {
   vector<string> tokens;
    string token;
    bool inside_quotes = false;

    // Traverse each character in the string
    for (size_t i = 0; i < str.size(); ++i) {
        char current_char = str[i];

        // Check if we are inside quotes
        if (current_char == '"') {
            if (inside_quotes) {
                // Closing quote, push the token
                tokens.push_back(token);
                token.clear();
            }
            // Toggle the inside_quotes flag
            inside_quotes = !inside_quotes;
        }
        else if (inside_quotes) {
            // Collect characters inside quotes as one token
            token += current_char;
        }
        else if (current_char == ' ') {
            if (!token.empty()) {
                tokens.push_back(token);
                token.clear();
            }
        }
        else {
            // Collect regular characters
            token += current_char;
        }
    }

    // If there's any token left, push it
    if (!token.empty()) {
        tokens.push_back(token);
    }

    return tokens;
}*/