#include <sys/time.h>
#include <time.h>
#include <unistd.h>
#include <string>
#include <tuple>
#include <vector>

std::string makeLogDayRequestBody(const std::string& seq, const std::string& pos) {
    std::string res = "answers=%5B%220%22%5D&seq=";
    res += seq;
    res +=
        "&"
        "temperature=36.0&"
        "userId=&";
    res += pos;
    return res;
}
std::string makeLogDayCurlCMD(const std::string& req, const std::string& session) {
    std::string res = "curl -s ";
    res +=
        "-H \""
        "User-Agent: Mozilla/5.0 (Windows NT 6.1; WOW64) "
        "AppleWebKit/537.36 (KHTML, like Gecko) "
        "Chrome/53.0.2785.143 Safari/537.36 MicroMessenger/7.0.9.501 "
        "NetType/WIFI MiniProgramEnv/Windows WindowsWechat\" "
        "-H \"Referer: https://servicewechat.com/wxce6d08f781975d91/180/page-frame.html\" ";
    res += "-H \"JWSESSION: ";
    res += session;
    res += "\" ";
    res += "-d \"";
    res += req + "\" -X POST https://student.wozaixiaoyuan.com/heat/save.json";
    return res;
}
std::string makeLogCurlCMD(const std::string& session, const std::string& pos) {
    std::string res = "curl -s ";
    res +=
        "-H \""
        "User-Agent: Mozilla/5.0 (Windows NT 6.1; WOW64) "
        "AppleWebKit/537.36 (KHTML, like Gecko) "
        "Chrome/53.0.2785.143 Safari/537.36 MicroMessenger/7.0.9.501 "
        "NetType/WIFI MiniProgramEnv/Windows WindowsWechat\" "
        "-H \"Referer: https://servicewechat.com/wxce6d08f781975d91/180/page-frame.html\" ";
    res += "-H \"JWSESSION: ";
    res += session;
    res +=
        "\" "
        "-d \""
        "answers=%5B%220%22%5D&";
    res += pos;
    "\" -X POST https://student.wozaixiaoyuan.com/health/save.json";
    return res;
}
std::string makeFetchCurlCMD(const std::string& session) {
    std::string res = "curl -s ";
    res +=
        "-H \""
        "User-Agent: Mozilla/5.0 (Windows NT 6.1; WOW64) "
        "AppleWebKit/537.36 (KHTML, like Gecko) "
        "Chrome/53.0.2785.143 Safari/537.36 MicroMessenger/7.0.9.501 "
        "NetType/WIFI MiniProgramEnv/Windows WindowsWechat\" "
        "-H \"Referer: https://servicewechat.com/wxce6d08f781975d91/180/page-frame.html\" ";
    res += "-H \"JWSESSION: ";
    res += session;
    res +=
        "\" "
        "-X POST https://student.wozaixiaoyuan.com/heat/getTodayHeatList.json >> /dev/null";
    return res;
}
int getHour() {
    time_t now;
    time(&now);
    return localtime(&now)->tm_hour;
}
int getNowTask() {
    int h = getHour();
    if (h < 7) {
        return 0;
    } else if (h < 12) {
        return 1;
    } else if (h < 18) {
        return 2;
    } else {
        return 3;
    }
}
void dumpTime() {
    time_t now;
    time(&now);
    auto t = localtime(&now);
    printf("[%d-%d-%d %d:%d:%d]\n", t->tm_year, t->tm_mon, t->tm_mday, t->tm_hour, t->tm_min, t->tm_sec);
}
const char* tasks[] = {"0", "1", "2", "3", "4", "5", "6", "7", "8", "9"};
int main() {
    int lastTask = 0;

    //允许打卡多个数据
    //token和位置可通过fiddler抓包得到
    //方法是先打开fiddler，
    //然后打开电脑版微信，用我在校园打一次卡
    //在fiddler中查找heat/save.json
    std::vector<std::tuple<std::string, std::string> > arr;
    arr.push_back(std::make_tuple(
        std::string("你的token"),
        std::string("你的位置")));

    while (1) {
        int t = getNowTask();
        for (auto& it : arr) {
            auto & token = std::get<0>(it);
            auto & pos = std::get<1>(it);
            if (t == lastTask) {
                //自动循环
                std::string s = makeFetchCurlCMD(token);
                //printf("%s\n",s.c_str());
                system(s.c_str());
            } else {
                //打卡
                dumpTime();
                if (t == 0) {
                    std::string s = makeLogCurlCMD(token,pos);
                    //printf("%s\n",s.c_str());
                    system(s.c_str());
                } else {
                    std::string s = makeLogDayCurlCMD(makeLogDayRequestBody(tasks[t],pos), token);
                    //printf("%s\n",s.c_str());
                    system(s.c_str());
                }
                printf("\n");
            }
            sleep(20);
        }
        lastTask = t;
        sleep(600);
    }

    return 0;
}
