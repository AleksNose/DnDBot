#include "ApiClient.h"

json::value createErrorResponse(const http_exception& exception) {
	json::value error_response;
	string_t str_exception = utility::conversions::to_string_t(exception.what());

	error_response[U("error")] = json::value::string(U("Internal Server Error"));
	error_response[U("message")] = json::value::string(str_exception);

	return error_response;
}

json::value responseHandleGet(http_client& client, const string_t& path_query_fragment) {
	try {
		pplx::task<json::value> response_task = client.request(methods::GET, path_query_fragment)
			.then([](http_response response) {
			if (response.status_code() == status_codes::OK) {
				return response.extract_json();
			}

			return pplx::task_from_result(json::value());
				}).then([](pplx::task<json::value> previousTask) {
					try {
						return previousTask.get();
					}
					catch (const http_exception& exception) {
						return createErrorResponse(exception);
					}
				});

			return response_task.get();
	}
	catch (const std::exception& exc) {
		return createErrorResponse(http_exception(exc.what()));
	}
}

std::map<string_t, json::value> getResultList(http_client& client, const string_t& path_query_fragment) {

	std::map<string_t, json::value> result;
	json::value response = responseHandleGet(client, path_query_fragment);
	json::array arrays = response[U("results")].as_array();

	for (int i = 0; i < arrays.size(); ++i) {
		string_t key = (arrays[i].at(U("index"))).as_string();
		result[key] = arrays[i];

		std::wcout << arrays[i].serialize() << std::endl;
	}

	return result;
}

int main() {
	http_client client(U("https://www.dnd5eapi.co/"));

	auto response = getResultList(client, U("api/spells"));

	return 0;
}
