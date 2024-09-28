#include "ApiClient.h"
#include "sleepy_discord/sleepy_discord.h"
#include <random>

struct SquareWave : public SleepyDiscord::AudioVectorSource
{
	using SleepyDiscord::AudioVectorSource::AudioVectorSource;
	void read(
		SleepyDiscord::AudioTransmissionDetails &details,
		SleepyDiscord::AudioVectorSource::Container &target) override
	{
		for (SleepyDiscord::AudioSample &sample : target)
		{
			sample = (++sampleOffset / 100) % 2 ? volume : -1 * volume;
		}
	}
	std::size_t sampleOffset = 0;
	int volume = 2000;
	int halfSquareWaveLength = 100;
};

struct Noise : public SleepyDiscord::AudioVectorSource
{
	Noise() : SleepyDiscord::AudioVectorSource(),
			  engine(randomDevice()),
			  distribution(-1.0, 1.0)
	{
	}
	void read(
		SleepyDiscord::AudioTransmissionDetails &details,
		SleepyDiscord::AudioVectorSource::Container &target) override
	{
		for (SleepyDiscord::AudioSample &sample : target)
		{
			sample = static_cast<SleepyDiscord::AudioSample>(
				distribution(engine) * volume);
		}
	}
	std::random_device randomDevice;
	std::mt19937 engine;
	std::uniform_real_distribution<> distribution;
	double volume = 2000.0;
};

struct RawPCMAudioFile : public SleepyDiscord::AudioPointerSource
{
	RawPCMAudioFile(std::string fileName)
	{
		//open file
		const std::size_t fileSize = 0;
		if (fileSize == static_cast<std::size_t>(-1))
			return;

		//read file
		music.reserve(fileSize / sizeof(SleepyDiscord::AudioSample));
		musicLength = music.size();
		progress = 0;
	}
	void read(
		SleepyDiscord::AudioTransmissionDetails &details,
		SleepyDiscord::AudioSample *&buffer, std::size_t &length) override
	{
		constexpr std::size_t proposedLength = SleepyDiscord::AudioTransmissionDetails::proposedLength();
		buffer = &music[progress];
		//if song isn't over, read proposedLength amount of samples
		//else use a length of 0 to stop listening
		length = proposedLength < (musicLength - progress) ? proposedLength : 0;
		progress += proposedLength;
	}
	std::size_t progress = 0;
	std::vector<SleepyDiscord::AudioSample> music;
	std::size_t musicLength = 0;
};

template<class _OnReadyCallback>
class VoiceEvents : public SleepyDiscord::BaseVoiceEventHandler
{
public:
	using OnReadyCallback = _OnReadyCallback;
	VoiceEvents(OnReadyCallback& theOnReadyCallback) 
		: onReadyCallback(theOnReadyCallback) {}

	void onReady(SleepyDiscord::VoiceConnection &connection) override {
		onReadyCallback(connection);
	}

	void onEndSpeaking(SleepyDiscord::VoiceConnection &connection) override {
		connection.disconnect();
	}

private:
	OnReadyCallback onReadyCallback;
};

class MyClientClass : public SleepyDiscord::DiscordClient {
public:
	using SleepyDiscord::DiscordClient::DiscordClient;
	void onMessage(SleepyDiscord::Message message) override {
		if (message.startsWith("whcg hello"))
			sendMessage(message.channelID, "Hello " + message.author.username);
	}
};

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
	MyClientClass dcClient("token", SleepyDiscord::USER_CONTROLED_THREADS);
	dcClient.setIntents(SleepyDiscord::Intent::SERVER_MESSAGES);
	dcClient.run();

	http_client client(U("https://www.dnd5eapi.co/"));

	auto response = getResultList(client, U("api/spells"));

	return 0;
}
