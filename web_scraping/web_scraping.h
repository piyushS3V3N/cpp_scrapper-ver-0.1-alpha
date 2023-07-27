#pragma once

#include <string>
#include <vector>
#include <utility>

std::vector<std::pair<std::string, std::string>> scrape_duckduckgo(const std::string &query);
std::string fetch_web_page_content(const std::string &url);
