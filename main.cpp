#include <algorithm>
#include <cctype>
#include <iostream>
#include <string>
#include <vector>
#include "web_scraping/web_scraping.h"
#include "html_parsing/html_parsing.h"
#include "sentiment_analysis/sentiment_analysis.h"

using namespace std;

// Function to replace spaces with '+' signs in the string
string replace_spaces_with_plus(const string &str) {
  string result = str;
  for (size_t i = 0; i < result.size(); ++i) {
    if (result[i] == ' ') {
      result[i] = '+';
    }
  }
  return result;
}

// Function to remove all spaces from the string
string remove_spaces(const string &str) {
  string result = str;
  result.erase(std::remove_if(result.begin(), result.end(),
                              [](unsigned char c) { return std::isspace(c); }),
               result.end());
  return result;
}

// Main function
int main() {
    string search_query = "search keyword";
  string formatted_query = replace_spaces_with_plus(search_query);
  vector<pair<string, string>> search_results =
      scrape_duckduckgo(formatted_query);

  if (!search_results.empty()) {
    for (size_t idx = 0; idx < search_results.size(); ++idx) {
      const auto &[title, url] = search_results[idx];
      cout << idx + 1 << ". " << title << endl;
      cout << "   URL: " << remove_spaces(url) << endl;
      string url_cleaned = remove_spaces(url);
      // Fetch the content of the web page using libcurl
      string page_content = fetch_web_page_content(url_cleaned);
     if (!page_content.empty()) {
        // Extract text from the HTML content
        string text_content = extract_text_from_html(page_content);

        // Perform sentiment analysis on the text content using Python
	std::optional <double> sentiment_score = analyze_sentiment(text_content);
	//std::optional<double>  sentiment_score;
	if (sentiment_score.has_value()) {
            cout << "   Sentiment Score: " << sentiment_score.value() << endl;

            // Classify the website based on sentiment score (dummy implementation)
            string classification = sentiment_score.value() >= 0.0 ? "Positive" : "Negative";
            cout << "   Website Classification: " << classification << endl;
        } else {
            cerr << "Error: Sentiment analysis failed." << endl;
        }
    } else {
        cerr << "Error: Unable to fetch page content from " << url_cleaned << endl;
    }
      // Sleep for 2 seconds to avoid sending too many requests too quickly
      //sleep(2);

      cout << endl;
    }
  } else {
    cerr << "No search results found for " << search_query << endl;
  }

 
    return 0;
}

