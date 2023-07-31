#include "html_parsing.h"
#include <libxml/HTMLparser.h>
#include <libxml/xpath.h>

std::vector<std::pair<std::string, std::string>> parse_search_results(const std::string& html_content) {
    std::vector<std::pair<std::string, std::string>> results;

    htmlDocPtr doc = htmlReadMemory(html_content.c_str(), html_content.size(), nullptr,
        nullptr, HTML_PARSE_NOWARNING | HTML_PARSE_NOERROR);
    if (doc) {
        xmlXPathContextPtr xpathCtx = xmlXPathNewContext(doc);
        if (xpathCtx) {
            // XPath expression to select the titles
            xmlXPathObjectPtr xpathTitleObj = xmlXPathEvalExpression(
                (const xmlChar*)"//a[@class='result__a']", xpathCtx);
            if (xpathTitleObj) {
                xmlNodeSetPtr titleNodes = xpathTitleObj->nodesetval;
                // XPath expression to select the URLs
                xmlXPathObjectPtr xpathUrlObj = xmlXPathEvalExpression(
                    (const xmlChar*)"//a[@class='result__url']", xpathCtx);
                xmlNodeSetPtr urlNodes = xpathUrlObj->nodesetval;

                // Combine the results (title and URL)
                for (int i = 0; i < titleNodes->nodeNr && i < urlNodes->nodeNr; ++i) {
                    xmlChar* title = xmlNodeGetContent(titleNodes->nodeTab[i]);
                    xmlChar* url = xmlNodeGetContent(urlNodes->nodeTab[i]);
                    if (title && url) {
                        results.emplace_back((const char*)title, (const char*)url);
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

std::string extract_text_from_node(xmlNode* node){

	std::string text;
  xmlNode *cur_node = nullptr;

  for (cur_node = node; cur_node; cur_node = cur_node->next) {
    if (cur_node->type == XML_TEXT_NODE) {
      text += reinterpret_cast<const char *>(cur_node->content);
    } else if (cur_node->type == XML_ELEMENT_NODE) {
      if (cur_node->name &&
          xmlStrcasecmp(cur_node->name, (const xmlChar *)"script") == 0) {
        // Skip script tags
        continue;
      }
      if (cur_node->name &&
          xmlStrcasecmp(cur_node->name, (const xmlChar *)"style") == 0) {
        // Skip style tags
        continue;
      }
      text += extract_text_from_node(cur_node->children);
    }
  }

  return text;
}

std::string extract_text_from_html(const std::string& html_content) {
    std::string text;
    htmlDocPtr doc = htmlReadMemory(html_content.c_str(), html_content.size(), nullptr,
        nullptr, HTML_PARSE_NOERROR | HTML_PARSE_NOBLANKS);
    if (doc) {
        text = extract_text_from_node(xmlDocGetRootElement(doc));
        xmlFreeDoc(doc);
    }

    // Remove extra whitespaces and newline characters
    text.erase(std::unique(text.begin(), text.end(),
        [](char a, char b) { return isspace(a) && isspace(b); }), text.end());

    return text;
}

