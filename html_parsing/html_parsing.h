#pragma once

#include <string>
#include <vector>
#include <utility>
#include <algorithm>
std::vector<std::pair<std::string, std::string>> parse_search_results(const std::string &html_content);
std::string extract_text_from_html(const std::string &html_content);

