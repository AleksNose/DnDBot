#include <cpprest/http_client.h>
#include <cpprest/filestream.h>
#include <iostream>

using namespace utility;
using namespace web;
using namespace web::http;
using namespace web::http::client;

class ApiClient
{
	public:
		json::value createErrorResponse(const http_exception& exception);
		std::map<string_t, json::value> getResultList(http_client& client, const string_t& path_query_fragment);
};
