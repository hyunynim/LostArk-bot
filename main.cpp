#include "sleepy_discord/websocketpp_websocket.h"
#include<bits/stdc++.h>
#include<atlconv.h>
#include <experimental/filesystem>
#include <cpprest/http_client.h>
#include <cpprest/filestream.h>

using namespace std; 
using namespace std::experimental::filesystem;
using namespace utility;

typedef long long ll;
string ANSIToUTF8(const char* pszCode) {
	//https://snowbora.tistory.com/370
	int     nLength, nLength2;
	BSTR    bstrCode;
	char* pszUTFCode = NULL;

	nLength = MultiByteToWideChar(CP_ACP, 0, pszCode, lstrlen(pszCode), NULL, NULL);
	bstrCode = SysAllocStringLen(NULL, nLength);
	MultiByteToWideChar(CP_ACP, 0, pszCode, lstrlen(pszCode), bstrCode, nLength);

	nLength2 = WideCharToMultiByte(CP_UTF8, 0, bstrCode, -1, pszUTFCode, 0, NULL, NULL);
	pszUTFCode = (char*)malloc(nLength2 + 1);
	WideCharToMultiByte(CP_UTF8, 0, bstrCode, -1, pszUTFCode, nLength2, NULL, NULL);
	string res = pszUTFCode;
	delete pszUTFCode;
	return res;
}
string UTF8ToANSI(const char* pszCode){
	//https://snowbora.tistory.com/370
	BSTR    bstrWide;
	char* pszAnsi;
	int     nLength;

	nLength = MultiByteToWideChar(CP_UTF8, 0, pszCode, lstrlen(pszCode) + 1, NULL, NULL);
	bstrWide = SysAllocStringLen(NULL, nLength);

	MultiByteToWideChar(CP_UTF8, 0, pszCode, lstrlen(pszCode) + 1, bstrWide, nLength);

	nLength = WideCharToMultiByte(CP_ACP, 0, bstrWide, -1, NULL, 0, NULL, NULL);
	pszAnsi = new char[nLength];

	WideCharToMultiByte(CP_ACP, 0, bstrWide, -1, pszAnsi, nLength, NULL, NULL);
	SysFreeString(bstrWide);

	string res = pszAnsi;
	delete pszAnsi;
	return res;
}

wstring str2wstr(const string& _src){
	USES_CONVERSION;
	return wstring(A2W(_src.c_str()));
}
string wstr2str(const wstring& _src){
	USES_CONVERSION;
	return string(W2A(_src.c_str()));
}

ll GetLevelInfo(const string &name, const string &ID) {
	using namespace web;
	using namespace web::http;
	using namespace web::http::client;
	using namespace concurrency::streams;
	string_t fName = str2wstr(UTF8ToANSI(ID.c_str())) + U("glvl.html");
	string_t wName = str2wstr(UTF8ToANSI(name.c_str()));
	auto fileStream = std::make_shared<concurrency::streams::ostream>();
	
	pplx::task<void> requestTask = concurrency::streams::fstream::open_ostream(fName).then([=](concurrency::streams::ostream outFile)
		{
			*fileStream = outFile;

			http_client_config conf;
			conf.set_timeout(seconds(4));

			http_client client(U("http://lostark.game.onstove.com/Profile/Character/"));
			uri_builder builder(uri::encode_uri(wName));
			return client.request(methods::GET, builder.to_string());
		}).then([=](http_response response)
			{
				return response.body().read_to_end(fileStream->streambuf());
			}).then([=](size_t nVal)
				{
					return fileStream->close();
				});

			try
			{
				requestTask.wait();
			}
			catch (const std::exception& e)
			{
				printf("Error exception:%s\n", e.what());
			}
	locale::global(locale(".UTF-8"));
	wifstream f(fName);
	
	wstring wstr = L"";
	while(!f.eof()){
		getline(f, wstr);
		if (wstr.find(L"달성 아이템 레벨") != wstring::npos){
			auto p1 = wstr.substr(wstr.find(L"Lv.</small>") + 11);
			auto wLvl = p1.substr(0, p1.find(L"<small>"));
			string tmp = wstr2str(wLvl);
			ll res = 0;
			for (auto i : tmp)
				if (i != ',')
					res = res * 10 + (i - '0');
			f.close();
			system("del *.html");
			return res;
		}
	}
	f.close();
	system("del *.html");
	return 0;
}
vector<pair<int, string>> GetExpdInfo(const string& name, const string& ID) {
	using namespace web;
	using namespace web::http;
	using namespace web::http::client;
	using namespace concurrency::streams;
	string_t fName = str2wstr(UTF8ToANSI(ID.c_str())) + U("gexpd.html");
	string_t wName = str2wstr(UTF8ToANSI(name.c_str()));
	auto fileStream = std::make_shared<concurrency::streams::ostream>();

	pplx::task<void> requestTask = concurrency::streams::fstream::open_ostream(fName).then([=](concurrency::streams::ostream outFile)
		{
			*fileStream = outFile;

			http_client_config conf;
			conf.set_timeout(seconds(4));

			http_client client(U("http://lostark.game.onstove.com/Profile/Character/"));
			uri_builder builder(uri::encode_uri(wName));
			return client.request(methods::GET, builder.to_string());
		}).then([=](http_response response)
			{
				return response.body().read_to_end(fileStream->streambuf());
			}).then([=](size_t nVal)
				{
					return fileStream->close();
				});

			try
			{
				requestTask.wait();
			}
			catch (const std::exception& e)
			{
				printf("Error exception:%s\n", e.what());
			}
	locale::global(locale(".UTF-8"));
	wifstream f(fName);
	wstring wstr = L"";
	vector<pair<int, string>> res;
	while (!f.eof()) {
		getline(f, wstr);
		const wstring fStr = L"/Profile/Character/";
		auto idx = wstr.find(fStr);
		if (idx != wstring::npos) {
			idx += fStr.length();
			auto p1 = wstr.substr(idx);
			if (p1.find(L"'\">") == wstring::npos) continue;
			p1.pop_back(); p1.pop_back(); p1.pop_back();
			string uStr = ANSIToUTF8(wstr2str(p1).c_str());
			auto level = GetLevelInfo(uStr, "tmp");
			if(level >= 1325)
				res.push_back({ -level, uStr });
		}
	}
	f.close();
	system("del *.html");
	sort(res.begin(), res.end());
	res.erase(unique(res.begin(), res.end()), res.end());
	for (auto &i : res)
		i.first *= -1;
	return res;
}

random_device rd;
mt19937 gen(rd());

typedef uniform_int_distribution<int> ud;
typedef uniform_int_distribution<ll> ud2;

map<string, string> expd;
const string expdRoot = "./data/expd/";
class MyClientClass : public SleepyDiscord::DiscordClient {
	vector<string> LoadFileList(const string root) {
		vector<string> res;
		for (const auto& entry : directory_iterator(root)) {
			auto cur = entry.path().native();
			string path(cur.begin(), cur.end());
			if (is_directory(entry.path())) {
				auto tmp = LoadFileList(path);
				for (auto i : tmp)
					res.push_back(i);
			}
			else
				res.push_back(path);
		}
		return res;
	}

public:
	using SleepyDiscord::DiscordClient::DiscordClient;
	void MessageSend(SleepyDiscord::Message & message, string str) {
		sendMessage(message.channelID, str);
	}
	bool learningFlag = 0;
	string learningAuth = "";
	void onMessage(SleepyDiscord::Message msg) override {
		if (msg.author.bot) return;
		if (msg.startsWith("!")) {
			auto content = msg.content.substr(1);

			auto res = split(content, u8' ');

			if (res[0] == u8"하위")
				sendMessage(msg.channelID, u8"헬로 " + msg.author.username + u8" 마더뻐-커 ");
			else if (res[0] == u8"강화") {
				auto num = str2ll(res[1]);
				if (num == -1)
					sendMessage(msg.channelID, u8"!강화 확률(1% = 10, 0.1% = 1)");
				else {
					string per = num2per(num);
					sendMessage(msg.channelID, u8"강화 확률 = " + per);
					ud2 randomNumber(1, 1000);
					if (randomNumber(gen) <= num)
						sendMessage(msg.channelID, u8"아; 이게 성공하네");
					else
						sendMessage(msg.channelID, u8"77777777777777ㅓㅓㅓㅓㅓㅓㅓㅓㅓㅓ억");
				}
			}
			else if (res[0] == u8"툴쓰칼잽이") sendMessage(msg.channelID, u8"김연수");
			else if (res[0] == u8"주사위") {
				if (res.size() == 1) {
					ud randomNumber(1, 100);
					auto res = ll2str(randomNumber(gen));
					sendMessage(msg.channelID, u8"주사위 결과는 " + res + u8"(1-100)");
				}
			}
			else if (res[0] == u8"입장") {
				printf("%s\n", msg.member.nick.c_str());
				//client.connectToVoiceChannel(, "584340930237169684");
			}
			else if (res[0] == u8"원정대등록") {
				if (res.size() != 2) {
					sendMessage(msg.channelID, u8"!원정대등록 [원정대에 포함된 캐릭터 중 하나의 캐릭터 이름]");
				}
				else {
					string rt = expdRoot + msg.author.ID.string();
					FILE* fp = fopen((rt + "/cList.txt").c_str(), "r");
					if (fp == NULL) {
						v1::path p(rt.c_str());
						v1::create_directory(p);

						fp = fopen((rt + "/cList.txt").c_str(), "w");

						string rep = u8"```등록되지 않은 원정대입니다. 원정대 등록을 시작합니다.\n원정대에 포함되어 있는 캐릭터 중, 1325 이상의 캐릭터만 등록됩니다.\n원정대 정보는 디스코드 계정의 고유 ID를 통해 저장됩니다.```";
						sendMessage(msg.channelID, rep);
						
						auto expd = GetExpdInfo(res[1], msg.author.ID.string());
						rep = u8"```\n";
						for (auto i : expd) {
							rep += i.second + u8"(Lv." + ANSIToUTF8(ll2str(i.first).c_str()) + u8")\n";
							fprintf(fp, "%s\n", i.second.c_str());
							FILE * cfp = fopen((rt + "/" + i.second + ".txt").c_str(), "w");
							fprintf(cfp, "%lld\n", i.first);
							fclose(cfp);
						}
						rep += u8"총 " + ll2str(expd.size()) + u8"개의 캐릭터를 등록하였습니다.```";
						sendMessage(msg.channelID, rep);
						fclose(fp);
					}
					else {
						fclose(fp);
						sendMessage(msg.channelID, u8"이미 등록된 원정대입니다.");
					}
				}
			}
			else if (res[0] == u8"캐릭터추가") {
				if (res.size() != 2) {
					sendMessage(msg.channelID, u8"!캐릭터추가 [추가할 캐릭터 이름]");
				}
				else {
					string rt = expdRoot + msg.author.ID.string();
					FILE* fp = fopen((rt + "/cList.txt").c_str(), "r");
					if (fp == NULL) {
						sendMessage(msg.channelID, u8"등록되지 않은 원정대(!원정대등록)");
					}
					else {
						fp = fopen((rt + "/" + res[1] + ".txt").c_str(), "r");
						if (fp) {
							sendMessage(msg.channelID, u8"이미 등록된 캐릭터");
							fclose(fp);
						}
						else {
							fp = fopen((rt + "/" + res[1] + ".txt").c_str(), "w");
							auto lvl = GetLevelInfo(res[1], msg.author.ID.string());
							fprintf(fp, "%lld\n", lvl);
							fclose(fp);

							fp = fopen((rt + "/cList.txt").c_str(), "a");
							fprintf(fp, "%s\n", res[1].c_str());
							fclose(fp);
							sendMessage(msg.channelID, res[1] + u8"(" + ll2str(lvl) + u8") 캐릭터 등록 완료");
						}
					}
					if (fp != NULL) fclose(fp);
				}
			}
			else if (res[0] == u8"test") {
				
			}
		}
	}
	vector<string> split(string input, char delimiter) {
		vector<string> answer;
		stringstream ss(input);
		string temp;

		while (getline(ss, temp, delimiter)) {
			answer.push_back(temp);
		}

		return answer;
	}
	void InitExpedition() {

	}
	string num2per(ll num) {
		string res = "";
		bool flag = 0;
		while (num) {
			if (!flag) res.push_back('%');
			res.push_back(num % 10 + '0');
			if (!flag) {
				res.push_back('.');
				flag = 1;
			}
			num /= 10;
		}
		reverse(res.begin(), res.end());
		return res;
	}
	ll str2ll(const string& str) {
		ll res = 0;
		for (int i = 0; i < str.size(); ++i) {
			if ('0' <= str[i] && str[i] <= '9')
				res = res * 10 + (str[i] - '0');
			else return -1;
		}
		return res;
	}
	string ll2str(ll num) {
		string res = "";
		while (num) {
			res.push_back(num % 10 + '0');
			num /= 10;
		}
		reverse(res.begin(), res.end());
		return res;
	}
};

void Test() {
	auto res = GetExpdInfo(u8"안홍자", u8"541542135431522");
	sort(res.begin(), res.end());
	res.erase(unique(res.begin(), res.end()), res.end());
	for (auto i : res) {
		i.first *= -1;
		cout << i.first << i.second << endl;
	}
	system("pause");
}
int main() {
	//Test(); return 0;
	setlocale(LC_ALL, "ko_KR.utf8");
	srand(time(0)); 

	FILE* fp = fopen("token.txt", "r");
	char token[123]; fscanf(fp, "%s", token);
	fclose(fp);

	MyClientClass client(token, 2);
	client.updateStatus(u8"하아아아앙");
	client.run();
}
