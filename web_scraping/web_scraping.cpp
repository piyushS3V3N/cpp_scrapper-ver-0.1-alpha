#include "web_scraping.h"
#include <curl/curl.h>
#include <iostream>
#include <vector>
#include <cstring>
#include <libxml/HTMLparser.h>
#include <libxml/xpath.h>
#include <string>
struct MemoryStruct {
    char* memory;
    size_t size;
};

static size_t WriteMemoryCallback(void* contents, size_t size, size_t nmemb, void* userp) {
    size_t realsize = size * nmemb;
    struct MemoryStruct* mem = static_cast<struct MemoryStruct*>(userp);

    mem->memory = static_cast<char*>(realloc(mem->memory, mem->size + realsize + 1));
    if (mem->memory == nullptr) {
        std::cerr << "Out of memory!" << std::endl;
        return 0;
    }

    memcpy(&(mem->memory[mem->size]), contents, realsize);
    mem->size += realsize;
    mem->memory[mem->size] = 0;

    return realsize;
}

// Function to fetch web page content using libcurl with gzip decompression
std::string fetch_web_page_content(const std::string &url) {
	std::string page_content;
  CURL *curl = curl_easy_init();

  if (curl) {
    struct MemoryStruct chunk;
    chunk.memory = (char *)malloc(1);
    chunk.size = 0;

    // Set the User-Agent and Accept-Encoding headers correctly
    struct curl_slist *headers = nullptr;
    headers = curl_slist_append(headers,
                                "User-Agent: Mozilla/5.0 (Windows NT 10.0; "
                                "Win64; x64) AppleWebKit/537.36 (KHTML, like "
                                "Gecko) Chrome/58.0.3029.110 Safari/537.3");
    headers = curl_slist_append(headers, "Accept-Encoding: gzip");
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&chunk);
    curl_easy_setopt(curl, CURLOPT_ACCEPT_ENCODING,
                     "gzip"); // Enable gzip decompression

    CURLcode res = curl_easy_perform(curl);
    if (res == CURLE_OK) {
      page_content = std::string(chunk.memory, chunk.size);
    } else {
	    std::cerr << "Error: Unable to fetch page content from " << url << " - Error Code: " << res << std::endl;
    }

    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);
    free(chunk.memory);
  } else {
	  std::cerr << "Error: Unable to initialize CURL." << std::endl;
  }

  return page_content;
}

std::vector<std::pair<std::string, std::string>> parse_search_results(const std::string& html_content){
	std::vector<std::pair<std::string, std::string>> results;

  htmlDocPtr doc =
      htmlReadMemory(html_content.c_str(), html_content.size(), nullptr,
                     nullptr, HTML_PARSE_NOWARNING | HTML_PARSE_NOERROR);
  if (doc) {
    xmlXPathContextPtr xpathCtx = xmlXPathNewContext(doc);
    if (xpathCtx) {
      // XPath expression to select the titles
      xmlXPathObjectPtr xpathTitleObj = xmlXPathEvalExpression(
          (const xmlChar *)"//a[@class='result__a']", xpathCtx);
      if (xpathTitleObj) {
        xmlNodeSetPtr titleNodes = xpathTitleObj->nodesetval;
        // XPath expression to select the URLs
        xmlXPathObjectPtr xpathUrlObj = xmlXPathEvalExpression(
            (const xmlChar *)"//a[@class='result__url']", xpathCtx);
        xmlNodeSetPtr urlNodes = xpathUrlObj->nodesetval;

        // Combine the results (title and URL)
        for (int i = 0; i < titleNodes->nodeNr && i < urlNodes->nodeNr; ++i) {
          xmlChar *title = xmlNodeGetContent(titleNodes->nodeTab[i]);
          xmlChar *url = xmlNodeGetContent(urlNodes->nodeTab[i]);
          if (title && url) {
            results.emplace_back((const char *)title, (const char *)url);
            xmlFree(title);
            xmlFree(url);
          }
        }

        xmlXPathFreeObject(xpathTitleObj);
        xmlXPathFreeObject(xpathUrlObj);
      }
      xmlXPathFreeContext(xpathCtx);
    }
    xmlFreeDoc(doc);
  }

  return results;
}

std::vector<std::pair<std::string, std::string>> scrape_duckduckgo(const std::string& query) {
    std::string url = "https://duckduckgo.com/html/?q=" + query;
    CURL* curl = curl_easy_init();
    std::vector<std::pair<std::string, std::string>> results;

    if (curl) {
        struct MemoryStruct chunk;
        chunk.memory = static_cast<char*>(malloc(1));
        chunk.size = 0;

        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, static_cast<void*>(&chunk));

        CURLcode res = curl_easy_perform(curl);
        if (res == CURLE_OK) {
            std::string body(chunk.memory, chunk.size);
            curl_easy_cleanup(curl);

            results = parse_search_results(body);
        } else {
            std::cerr << "Error: Unable to fetch search results for " << query << std::endl;
        }

        free(chunk.memory);
    }

    return results;
}

