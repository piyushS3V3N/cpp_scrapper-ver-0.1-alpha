#include "sentiment_analysis.h"
#include <iostream>
#include <sstream>
#include <unordered_map>
#include <cpr/cpr.h>


// Function to perform sentiment analysis using VADER
double analyze_sentiment(const std::string& text) {
    // Download VADER lexicon if not already downloaded
    cpr::Response response = cpr::Get(cpr::Url{"https://raw.githubusercontent.com/cjhutto/vaderSentiment/master/vaderSentiment/vader_lexicon.txt"});
    if (response.status_code != 200) {
        std::cerr << "Error: Unable to download VADER lexicon." << std::endl;
        return -1.0;
    }

    // Parse the lexicon data
    std::istringstream iss(response.text);
    std::string line;
    std::unordered_map<std::string, double> lexicon;
    while (std::getline(iss, line)) {
        std::istringstream iss_line(line);
        std::string word;
        double score;
        if (iss_line >> word >> score) {
            lexicon[word] = score;
        }
    }

    // Tokenize the input text
    std::istringstream iss_text(text);
    std::string word;
    double total_score = 0.0;
    int num_tokens = 0;
    while (iss_text >> word) {
        // Remove punctuation (you can add more characters if needed)
        word.erase(std::remove_if(word.begin(), word.end(), [](char c) {
            return std::ispunct(static_cast<unsigned char>(c));
        }), word.end());

        // Convert to lowercase
        std::transform(word.begin(), word.end(), word.begin(), [](char c) {
            return std::tolower(static_cast<unsigned char>(c));
        });

        // Calculate the sentiment score
        auto it = lexicon.find(word);
        if (it != lexicon.end()) {
            total_score += it->second;
            num_tokens++;
        }
    }

    if (num_tokens > 0) {
        return total_score / num_tokens;
    }

    return -1.0;
}
