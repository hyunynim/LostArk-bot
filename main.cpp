#include"sleepy_discord/websocketpp_websocket.h"
#include<bits/stdc++.h>
#include<atlconv.h>
#include<experimental/filesystem>
#include<cpprest/http_client.h>
#include<cpprest/filestream.h>

using namespace std; 
using namespace std::experimental::filesystem;
using namespace utility;

typedef long long ll;

char tmpChar[1010];
random_device rd;
mt19937 gen(rd());

typedef uniform_int_distribution<int> ud;
typedef uniform_int_distribution<ll> ud2;

map<string, string> expd;
const string dataRoot = "./data/";
const string expdRoot = dataRoot + "expd/";


string ANSIToUTF8(const char* pszCode);
string UTF8ToANSI(const char* pszCode);
wstring str2wstr(const string& _src);
string wstr2str(const wstring& _src);
vector<string> split(string input, char delimiter);
string num2per(ll num);
ll str2ll(const string& str);
int str2int(const string& str);
string ll2str(ll num);
ll GetLevelInfo(const string& name, const string& ID);
vector<pair<int, string>> GetExpdInfo(const string& name, const string& ID);
string GetMariShopInfo(ll crystal);
vector<string> LoadFileList(const string root);
struct RAID {
	string name;
	int lb, ub, gold, group;
};
vector<RAID> weeklyList, dailyList;
void InitTodoList();
class EXPEDITION {
	class CHARACTER {
		string name;
		int level;
		vector<pair<RAID, bool>> daily, weekly;
	public:
		CHARACTER() : name(""), level(0) {}
		CHARACTER(const string& name, const int lvl) : name(name), level(lvl) {
			for (auto it = dailyList.rbegin(); it != dailyList.rend(); ++it) {
				if (((*it).lb == -1 || (*it).lb <= lvl) && (daily.empty() || daily.back().first.group != (*it).group))
					daily.push_back({ *it, 0 });
			}
			for (auto it = weeklyList.rbegin(); it != weeklyList.rend(); ++it) {
				if (((*it).lb == -1 || (*it).lb <= lvl) && (weekly.empty() || weekly.back().first.group != (*it).group))
					weekly.push_back({ *it, 0 });
			}
			reverse(daily.begin(), daily.end());
			reverse(weekly.begin(), weekly.end());
		}
		string GetName() { return name; }
		int GetLevel() { return level; }
		void SetName(const string& name) { this->name = name; }
		void SetLevel() { this->level = level; }
		friend class EXPEDITION;
	};
	string id;
	vector<CHARACTER> charList;
public:
	EXPEDITION() :id("") { }
	EXPEDITION(const string& id) : id(id) {}
	void AddCharacter(const string &name, const int lvl) {
		charList.push_back(CHARACTER(name, lvl));
	}
	string GetCharacterWeeklyInfo(const string& name) {
		string res = u8"";
		for (auto it : charList) {
			if (it.name == name) {
				for (auto w : it.weekly) {
					res += u8"(";
					res += (w.second ? u8"O" : u8"X");
					res += u8") " + w.first.name + u8"\n";
				}
				break;
			}
		}
		return res;
	}
	string GetCharacterDailyInfo(const string& name) {
		string res = u8"";
		for (auto it : charList) {
			if (it.name == name) {
				for (auto w : it.daily) {
					res += u8"(";
					res += (w.second ? u8"O" : u8"X");
					res += u8") " + w.first.name + u8"\n";
				}
			}
			break;
		}
		return res;
	}
};

vector<EXPEDITION> expdList(1);
map<string, int> expdComp;
set<string> scheduleList;
vector<pair<string, vector<string>>> scheduleListVector;
void InitExpdList() {

}
class MyClientClass : public SleepyDiscord::DiscordClient {
private:
	string learningAuth = "";
	string rep = u8""; 
	const string startString = u8"!";
	bool scheduleOpened = 0;
public:
	using SleepyDiscord::DiscordClient::DiscordClient;
	void MessageSend(SleepyDiscord::Message & message, string str) {
		sendMessage(message.channelID, str);
	}
	bool learningFlag = 0;
	void onMessage(SleepyDiscord::Message msg) override {
		if (msg.author.bot) return;

		//PM test
		//SleepyDiscord::Channel pmChannel = createDirectMessageChannel(msg.author.ID);
		//createDirectMessageChannel(msg.author.ID);
		//sendMessage(pmChannel.ID, "Test");

		if (msg.startsWith(startString)) {
			if (msg.length() == 1) return;
			if (msg.channelID.string() != u8"884882368056414289") {
				sendMessage(msg.channelID, u8"????????? ????????? ??????????????????.");
				
				deleteMessage(msg.channelID, msg.ID);
				return;
			}

			auto content = msg.content.substr(1);

			auto res = split(content, u8' ');
			if (res[0] == u8"??????") sendMessage(msg.channelID, u8"?????? " + msg.author.username + u8" ?????????-??? ");
			else if(res[0] == u8"??????") {
				auto num = str2ll(res[1]);
				if (num == -1)
					sendMessage(msg.channelID, u8"!?????? ??????(1% = 10, 0.1% = 1)");
				else {
					string per = num2per(num);
					sendMessage(msg.channelID, u8"?????? ?????? = " + per);
					ud2 randomNumber(1, 1000);
					if (randomNumber(gen) <= num)
						sendMessage(msg.channelID, u8"???; ?????? ????????????");
					else
						sendMessage(msg.channelID, u8"77777777777777?????????????????????????????????");
				}
			}
			else if(res[0] == u8"???????????????") sendMessage(msg.channelID, u8"?????????");
			else if(res[0] == u8"?????????") {
				if (res.size() == 1) {
					ud randomNumber(1, 100);
					auto res = ll2str(randomNumber(gen));
					sendMessage(msg.channelID, u8"????????? ????????? " + res + u8"(1-100)");
				}
			}
			else if(res[0] == u8"???????????????") {
				if (res.size() != 2) {
					sendMessage(msg.channelID, u8"!??????????????? [???????????? ????????? ????????? ??? ????????? ????????? ??????]");
				}
				else {
					string rt = expdRoot + msg.author.ID.string();
					FILE* fp = fopen((rt + "/cList.txt").c_str(), "r");
					if (fp == NULL) {
						v1::path p(rt.c_str());
						v1::create_directory(p);

						fp = fopen((rt + "/cList.txt").c_str(), "w");

						rep = u8"```???????????? ?????? ??????????????????. ????????? ????????? ???????????????.\n???????????? ???????????? ?????? ????????? ???, 1325 ????????? ???????????? ???????????????.\n????????? ????????? ???????????? ????????? ?????? ID??? ?????? ???????????????.```";
						sendMessage(msg.channelID, rep);
						
						auto expd = GetExpdInfo(res[1], msg.author.ID.string());
						rep = u8"```\n";
						EXPEDITION expdTmp(msg.author.ID.string());
						for (auto i : expd) {
							expdTmp.AddCharacter(i.second, i.first);
							rep += i.second + u8"(Lv." + ANSIToUTF8(ll2str(i.first).c_str()) + u8")\n";
							fprintf(fp, "%s\n", i.second.c_str());
							FILE * cfp = fopen((rt + "/" + i.second + ".txt").c_str(), "w");
							fprintf(cfp, "%lld\n", i.first);
							fclose(cfp);
						}
						expdComp[msg.author.ID.string()] = expdList.size();
						expdList.push_back(expdTmp);
						rep += u8"??? " + ll2str(expd.size()) + u8"?????? ???????????? ?????????????????????.```";
						sendMessage(msg.channelID, rep);
						fclose(fp);
					}
					else {
						fclose(fp);
						sendMessage(msg.channelID, u8"?????? ????????? ??????????????????.");
					}
				}
			}
			else if(res[0] == u8"???????????????") {
				if (res.size() != 2) {
					sendMessage(msg.channelID, u8"!??????????????? [????????? ????????? ??????]");
				}
				else {
					string rt = expdRoot + msg.author.ID.string();
					FILE* fp = fopen((rt + "/cList.txt").c_str(), "r");
					if (fp == NULL) {
						sendMessage(msg.channelID, u8"???????????? ?????? ?????????(!???????????????)");
					}
					else {
						fp = fopen((rt + "/" + res[1] + ".txt").c_str(), "r");
						if (fp) {
							sendMessage(msg.channelID, u8"?????? ????????? ?????????");
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
							sendMessage(msg.channelID, res[1] + u8"(" + ll2str(lvl) + u8") ????????? ?????? ??????");
						}
					}
					if (fp != NULL) fclose(fp);
				}
			}
			else if(res[0] == u8"??????") {
				if (res.size() != 2) {
					sendMessage(msg.channelID, u8"!?????? [???????????? ??????]");
					return;
				}
				auto rep = ANSIToUTF8(GetMariShopInfo(str2ll(res[1])).c_str());
				sendMessage(msg.channelID, rep);
			}
			else if(res[0] == u8"test") {
				rep = u8"```\n?????? ????????? ???????????? ?????? ??????/?????? ?????? ??????????????????.\n\n";
				for (auto it : weeklyList)
					rep += it.name + u8"\n";

				for (auto it : dailyList)
					rep += it.name + u8"\n";

				rep += u8"```";
				sendMessage(msg.channelID, rep);
			}
			else if(res[0] == u8"?????????"){
				if (res.size() != 3) {
					sendMessage(msg.channelID, u8"!????????? [????????? ???] [??????, ??????]");
					return;
				}
				if (expdComp[msg.author.ID.string()] == 0) {
					sendMessage(msg.channelID, u8"???????????? ?????? ??????????????????.(!???????????????)");
					return;
				}
				rep = u8"```\n";
				if (res[2] == u8"??????") {
					rep += res[1] + u8"???????????? ?????? ????????? ???????????????.\n";
					rep += expdList[expdComp[msg.author.ID.string()]].GetCharacterDailyInfo(res[1]);
				}
				else if (res[2] == u8"??????") {
					rep += res[1] + u8"???????????? ?????? ????????? ???????????????.\n";
					rep += expdList[expdComp[msg.author.ID.string()]].GetCharacterWeeklyInfo(res[1]);
				}
				else {
					sendMessage(msg.channelID, u8"!????????? [????????? ???] [??????, ??????]");
					return;
				}
				rep += u8"```";
				sendMessage(msg.channelID, rep);
			}
			else if(res[0] == u8"?????????") {
				ud randomNumber(1, 10000);
				if (randomNumber(gen) <= 1000) sendMessage(msg.channelID, u8"???; ?????? ??????;;");
				else sendMessage(msg.channelID, u8"77777777777777?????????????????????????????????");
			}
			else if(res[0] == u8"????????????") {
				if (res.size() != 5) {
					rep = u8"```\n";
					rep += startString + u8"???????????? [??????] [?????????] [??????] [??????]\n";
					rep += u8"ex) !???????????? ?????? ?????? ??? 21:15\n";
					rep += u8"    !???????????? ??????????????? ??????(1-4) ??? 21:00\n";
					rep += u8"    !???????????? ??????????????? 1-6 ?????? ??????\n";
					rep += u8"[??????], [?????????], [??????], [??????]?????? ????????? ?????? ???????????????.\n????????? ???????????? ???????????? ????????? ???????????????.\n";
					rep += u8"????????? ????????? ?????? " + startString + u8"???????????? ???????????? ??????????????????. ????????? ???????????? ????????? ????????? ??? ????????????.";
					rep += u8"```";
				}
				else {
					//????????? ?????? ??????
					//?????? ID ??????, ID ?????? ???????????? ?????? ??????
					if (msg.author.ID.string() != u8"343740791065280512") 
						rep = u8"????????? ?????? ????????? ????????????";
					else {
						rep = u8"????????? ?????????????????????.";
						string tmp = u8"";
						for (int i = 1; i < res.size(); ++i)
							tmp += res[i] + u8" ";
						scheduleList.insert(tmp);
					}
				}
				sendMessage(msg.channelID, rep);
			}
			else if(res[0] == u8"????????????") {
				rep = u8"```\n";
				rep += u8"???????????? ????????? ?????? ???????????????.\n";
				if (!scheduleOpened) {
					for (auto i : scheduleList)
						rep += i + u8"\n";
					rep += u8"?????? ????????? ???????????? ??????????????? ????????? ????????? ??? ????????????.\n";
				}
				else {
					int num = 1;
					for (auto i : scheduleListVector)
						rep += ll2str(num++) + u8" " + i.first + u8"(" + (i.second.empty() ? u8"0" : ll2str(i.second.size())) + u8"???)\n";
					rep += startString + u8"???????????? [??????]??? ?????? ????????? ??? ????????????.\n";
				}
				rep += u8"```";
				sendMessage(msg.channelID, rep);
			}
			else if(res[0] == u8"????????????") {

				if (msg.author.ID.string() != u8"343740791065280512") rep = u8"????????? ?????? ????????? ????????????";
				else if (scheduleOpened) {
					rep = u8"?????? ????????? ??????????????????.";
				}
				else {
					scheduleOpened = 1;
					//Need to add mention everyone
					rep = u8"????????? ?????????????????????. ???????????? ????????? ???????????? ??? ????????????.";
					scheduleListVector.clear();
					for (auto it : scheduleList)
						scheduleListVector.push_back({ it, vector<string>() });
				}
				sendMessage(msg.channelID, rep);
			}
			else if (res[0] == u8"????????????") {
				if (res.size() != 3) {
					rep = startString + u8"???????????? [????????????] [??????]\n";
					rep += startString + u8"???????????? ???????????? ????????? ?????????????????? ????????????.\n";
					if(!scheduleOpened)
						rep += u8"?????? ????????? ???????????? ??????????????? ????????? ????????? ??? ????????????.\n";
					else
						rep += u8"?????? ????????? ??????????????????.\n";
				}
				else {
					int idx = str2ll(res[2]) - 1;
					if (idx < 0 || idx >= scheduleList.size()) 
						rep = "????????? ????????? ????????? ??????????????????.";
					else if (find(scheduleListVector[idx].second.begin(), scheduleListVector[idx].second.begin(), res[1]) != scheduleListVector[idx].second.end()) {
						rep = "?????? ????????? ??????????????????.";
					}
					else {
						rep = res[1] + u8"(" + ll2str(GetLevelInfo(res[1], msg.author.ID.string())) + u8") ???????????? " + scheduleListVector[idx].first + u8"??? ?????????????????????.";
						scheduleListVector[idx].second.push_back(res[1]);
					}
				}
				sendMessage(msg.channelID, rep);
			}
		}
	}
};
void Test() {
	EXPEDITION e;
	system("pause");
}
int main() {
	//Test(); return 0;
	setlocale(LC_ALL, "ko_KR.utf8");
	srand(time(0)); 
	InitTodoList();
	FILE* fp = fopen("token.txt", "r");
	char token[123]; fscanf(fp, "%s", token);
	fclose(fp);

	MyClientClass client(token, 2);
	client.updateStatus(u8"???????????????");
	client.run();
}


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
string UTF8ToANSI(const char* pszCode) {
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

wstring str2wstr(const string& _src) {
	USES_CONVERSION;
	return wstring(A2W(_src.c_str()));
}
string wstr2str(const wstring& _src) {
	USES_CONVERSION;
	return string(W2A(_src.c_str()));
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
int str2int(const string& str) {
	int res = 0;
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
ll GetLevelInfo(const string& name, const string& ID) {
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
			while (!f.eof()) {
				getline(f, wstr);
				if (wstr.find(L"?????? ????????? ??????") != wstring::npos) {
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
					if (level >= 1325)
						res.push_back({ -level, uStr });
				}
			}
			f.close();
			system("del *.html");
			sort(res.begin(), res.end());
			res.erase(unique(res.begin(), res.end()), res.end());
			for (auto& i : res)
				i.first *= -1;
			return res;
}
string GetMariShopInfo(ll crystal) {
	using namespace web;
	using namespace web::http;
	using namespace web::http::client;
	using namespace concurrency::streams;
	string_t fName = L"mari.html";
	auto fileStream = std::make_shared<concurrency::streams::ostream>();

	pplx::task<void> requestTask = concurrency::streams::fstream::open_ostream(fName).then([=](concurrency::streams::ostream outFile)
		{
			*fileStream = outFile;

			http_client_config conf;
			conf.set_timeout(seconds(4));

			http_client client(U("https://lostark.game.onstove.com/Shop"));
			return client.request(methods::GET);
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
			bool flag = 0;
			ll amount = 0, len = 0;
			vector<string> strArr;
			string strTmp;
			while (!f.eof()) {
				getline(f, wstr);
				const wstring fStr = L"<span class=\"item-name\">";
				const wstring fStr2 = L"<span class=\"amount\" data-format=\"currency\">";

				auto idx = wstr.find(fStr);
				auto idx2 = wstr.find(fStr2);

				if (idx != wstring::npos) {
					idx += fStr.length();
					auto p1 = wstr.substr(idx);
					wstring wstr2 = L"</span>";
					if (p1.find(wstr2) == wstring::npos) continue;
					amount = 0;
					for (int i = 0; i < wstr2.size(); ++i) p1.pop_back();
					string item = wstr2str(p1);
					strTmp = "- " + item + " (1?????? ";
					item.pop_back();
					string tmp = "";
					while (item.size() && item.back() != '[') {
						if ('0' <= item.back() && item.back() <= '9')
							tmp.push_back(item.back());
						item.pop_back();
					}
					std::reverse(tmp.begin(), tmp.end());
					amount = str2ll(tmp);
					flag = 1;
				}
				if (flag && idx2 != wstring::npos) {
					idx2 += fStr2.length();
					auto p1 = wstr.substr(idx2);
					wstring wstr2 = L"</span>";
					if (p1.find(wstr2) == wstring::npos) continue;
					for (int i = 0; i < wstr2.size(); ++i) p1.pop_back();
					ll price = str2ll(wstr2str(p1));
					sprintf(tmpChar, "%.2lf", (((double)crystal / 95.0) * (double)price / (double)amount));
					string strPrice = tmpChar;
					strTmp += strPrice + "??????)";
					strArr.push_back(strTmp);
					flag = 0;
				}
			}
			const int align = 70;
			const int itemAmountPerTime = 6;
			string res = "```?????? ???????????? ???????????? ?????? ????????? ???????????????. ???????????? ???????????? ????????? ???????????? ????????? ???????????? ???????????????.\n\n";
			len = 0;
			for (int i = 0; i < strArr.size() / 2; ++i) {
				if (i == 6 || i == 12) {
					for (int j = 0; j < len; ++j)
						res.push_back('=');
					res += "=\n";
				}
				res += strArr[i];
				for (int j = 0; j < 70 - strArr[i].length(); ++j)
					res.push_back(' ');
				res += strArr[i + itemAmountPerTime * 3] + "\n";
				len = max((unsigned long long)len, 70 + strArr[i + itemAmountPerTime * 3].length());
			}
			res += "```";
			f.close();
			system("del *.html");
			return res;
}
void InitTodoList() {
	if (weeklyList.size() && dailyList.size()) return;

	FILE* fp = fopen((dataRoot + "weekly.csv").c_str(), "r");

	//?????? ??????(?????? ??????)
	while (fgets(tmpChar, 1010, fp)) {
		string cur = tmpChar;
		if (cur.back() == '\n') cur.pop_back();
		auto strList = split(cur, ',');
		//??????, ??????, ??????, ??????, ??????
		weeklyList.push_back({ ANSIToUTF8(strList[0].c_str()), str2int(strList[1]), str2int(strList[2]), str2int(strList[3]), str2int(strList[4]) });
	}
	fclose(fp);

	fp = fopen((dataRoot + "daily.csv").c_str(), "r");
	//?????? ??????(????????? ??????)
	while (fgets(tmpChar, 1010, fp)) {
		string cur = tmpChar;
		if (cur.back() == '\n') cur.pop_back();
		auto strList = split(cur, ',');
		dailyList.push_back({ ANSIToUTF8(strList[0].c_str()), str2int(strList[1]), -1, 0, str2int(strList[2]) });
	}
	fclose(fp);
}
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
