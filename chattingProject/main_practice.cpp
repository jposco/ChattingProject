#pragma comment(lib, "ws2_32.lib") 

#include <WinSock2.h>
#include <iostream>
#include <string>
#include <thread>
#include <vector>
#include <string>
#include <mysql/jdbc.h>
#include <sstream>

std::string to_string(int value) {
    std::ostringstream os;
    os << value;
    return os.str();
}
using std::cout;
using std::cin;
using std::endl;
using std::string;
using std::vector;

#define MAX_SIZE 1024 //상수 선언
#define MAX_CLIENT 3 //최대 인원 3

const string server = "tcp://127.0.0.1:3306"; // 데이터베이스 주소
const string username = "root"; // 데이터베이스 사용자
const string password = "07wd2713"; // 데이터베이스 접속 비밀번호


struct SOCKET_INFO { //구조체 정의
    SOCKET sck=0; //ctrl + 클릭, unsigned int pointer형
    string user =""; //user  : 사람의 이름
};

vector<SOCKET_INFO> sck_list; //서버에 연결된 client를 저장할 변수.
SOCKET_INFO server_sock; //서브소켓의 정보를 저장할 정보.
int client_count = 0; //현재 접속된 클라이언트 수 카운트 용도.

//SQL
sql::mysql::MySQL_Driver* driver; // 추후 해제하지 않아도 Connector/C++가 자동으로 해제해 줌 
sql::Connection* con;
sql::PreparedStatement* pstmt;
sql::ResultSet* result;
sql::Statement* stmt;
void startSql(); //SQL실행 - creatTable;

//시스템구동
void mainMenu();

//채팅
void server_init(); //서버용 소켓을 만드는 함수
void add_client(); //accept 함수 실행되고 있을 예정
void send_msg(const char* msg); //send() 실행
void send_msg_notMe(const char* msg, int sender_idx);
void sendWhisper(int position, string sbuf, int idx);
void recv_msg(int idx); //recv() 실행
void del_client(int idx); //클라이언트와의 연결을 끊을 때
void print_clients();


int main()
{
    system("color 06");
    startSql();
    mainMenu();
    WSADATA wsa;

    int code = WSAStartup(MAKEWORD(2, 2), &wsa);
    if (!code) {
        server_init(); //서버 연결

        std::thread th1[MAX_CLIENT];

        for (int i = 0; i < MAX_CLIENT; i++) {
            th1[i] = std::thread(add_client);
        }

        while (1) {
            string text, msg = "";

            std::getline(cin, text);
            const char* buf = text.c_str();
            msg = server_sock.user + " : " + buf;
            send_msg(msg.c_str());
        }

        for (int i = 0; i < MAX_CLIENT; i++) {
            th1[i].join();
        }

        closesocket(server_sock.sck);
    }
    else {
        cout << "프로그램 종료. (Error code : " << code << ")";
    }
    WSACleanup();
    delete result;
    delete pstmt;
    delete con;
    delete stmt;

    return 0;
}
void mainMenu(){
    cout << "\n";
    cout << " "; cout << "*************************************************\n";      
    cout << " "; cout << "*                                               *\n";
    cout << " "; cout << "*       *******      *       *       *  *       *\n";
    cout << " "; cout << "*          *        * *      *       * *        *\n";
    cout << " "; cout << "*          *       *****     *       **         *\n";
    cout << " "; cout << "*          *      *     *    *       * *        *\n";
    cout << " "; cout << "*          *     *       *   *****   *  *       *\n";
    cout << " "; cout << "*                                               *\n";
    cout << " "; cout << "*                                               *\n";
    cout << " "; cout << "*                                               *\n";
    cout << " "; cout << "*                                               *\n";
    cout << " "; cout << "*                                               *\n";
    cout << " "; cout << "*                 < SERVER ON >                 *\n";
    cout << " "; cout << "*                                               *\n";
    cout << " "; cout << "*                                               *\n";
    cout << " "; cout << "*                                               *\n";
    cout << " "; cout << "*                                               *\n";
    cout << " "; cout << "*                                               *\n";
    cout << " "; cout << "*                                               *\n";
    cout << " "; cout << "*                                               *\n";
    cout << " "; cout << "*                                               *\n";
    cout << " "; cout << "*                                               *\n";
    cout << " "; cout << "*************************************************\n\n";
}
void startSql()
{
    // MySQL Connector/C++ 초기화

    try {
        driver = sql::mysql::get_mysql_driver_instance();
        con = driver->connect(server, username, password);
    }
    catch (sql::SQLException& e) {
        cout << "Could not connect to server. Error message: " << e.what() << endl;
        exit(1);
    }

    // 데이터베이스 선택                                                                
    con->setSchema("project1");

    // DB 한글 저장을 위한 셋팅                                                             
    stmt = con->createStatement();
    stmt->execute("set names euckr");
    if (stmt) { delete stmt; stmt = nullptr; }
}
void server_init() //서버측 소켓 활성화
{
    server_sock.sck = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);

    SOCKADDR_IN server_addr = {};
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(7777); //123.0.0.1:7777 중에 ------:7777을 정의
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY); //123.0.0.1:----을 정의한다.

    bind(server_sock.sck, (sockaddr*)&server_addr, sizeof(server_addr));

    listen(server_sock.sck, SOMAXCONN);

    server_sock.user = "server";
}
void add_client() {
    SOCKADDR_IN addr = {};
    int addrsize = sizeof(addr);
    char buf[MAX_SIZE] = { };

    ZeroMemory(&addr, addrsize);

    SOCKET_INFO new_client = {};

    new_client.sck = accept(server_sock.sck, (sockaddr*)&addr, &addrsize);
    recv(new_client.sck, buf, MAX_SIZE, 0);
    new_client.user = string(buf);

    string msg = "▶" + new_client.user + " 님이 입장했습니다.";
    cout << msg << endl;
    sck_list.push_back(new_client);
    print_clients();

    std::thread th(recv_msg, client_count);
    th.detach();
    client_count++;

    cout << "▷현재 접속자 수 : " << client_count << "명" << endl;
    send_msg(msg.c_str());
}
void send_msg(const char* msg)
{
    for (int i = 0; i < client_count; i++)
    {
        send(sck_list[i].sck, msg, MAX_SIZE, 0);
    }
}
void send_msg_notMe(const char* msg, int sender_idx)
{
    for (int i = 0; i < client_count; i++) {
        if (i != sender_idx) {
            send(sck_list[i].sck, msg, MAX_SIZE, 0);
        }
    }
}
void sendWhisper(int position, string sbuf, int idx)
{
    int cur_position = position + 1;
    position = sbuf.find(" ", cur_position);
    int len = position - cur_position;
    string receiver = sbuf.substr(cur_position, len);
    cur_position = position + 1;
    string dm = sbuf.substr(cur_position);
    string msg = "※귓속말 도착 [" + sck_list[idx].user + "] : " + dm;
    for (int i = 0; i < client_count; i++)
    {
        if (receiver.compare(sck_list[i].user) == 0)
            send(sck_list[i].sck, msg.c_str(), MAX_SIZE, 0);
    }
}
void recv_msg(int idx) {
    char buf[MAX_SIZE] = { };
    string msg = "";

    while (1) {
        ZeroMemory(&buf, MAX_SIZE);

        if (recv(sck_list[idx].sck, buf, MAX_SIZE, 0) > 0) {
            string whisper(buf);
            int position = whisper.find(" ", 0);
            int message = position - 0;
            string flag = whisper.substr(0, message);
            if(string(buf) == "/종료")
            {
                msg = "▶" + sck_list[idx].user + " 님이 퇴장했습니다.";
                cout << msg << endl;
                send_msg(msg.c_str());
                del_client(idx);
                return;
            }
            else if (flag.compare("/귓말") == 0)
            {
                sendWhisper(position, whisper, idx);
            }
            else {

                pstmt = con->prepareStatement("INSERT INTO chatting(chatname, time, recv) VALUE(?, NOW(),  ?)");
                pstmt->setString(1, sck_list[idx].user);
                pstmt->setString(2, whisper);
                pstmt->execute();

                pstmt = con->prepareStatement("SELECT chatname, time, recv FROM chatting ORDER BY time DESC LIMIT 1");
                result = pstmt->executeQuery();
                if (result->next())
                {
                    string chatname = result->getString(1);
                    string time = result->getString(2);
                    string recv = result->getString(3);
                    msg += "--------------------------------------------------";
                    msg += "\n▷보낸사람: " + chatname + "  " + "▷보낸시간: " + time + "\n";
                    msg += "▷내용 : " + recv + "\n";
                    msg += "--------------------------------------------------\n";
                    cout << msg << endl;
                    send_msg_notMe(msg.c_str(), idx);
                }
            }
        }
        else { //그렇지 않을 경우 퇴장에 대한 신호로 생각하여 퇴장 메시지 전송
            msg = "[공지] " + sck_list[idx].user + " 님이 퇴장했습니다.";
            cout << msg << endl;
            send_msg(msg.c_str());
            del_client(idx); // 클라이언트 삭제
            return;
        }
    }
} 
void del_client(int idx) {
    std::thread th(add_client);
    closesocket(sck_list[idx].sck);
    client_count--;
    cout << "▷현재 접속자 수 : " << client_count << "명" << endl;
    sck_list.erase(sck_list.begin() + idx);
    th.join();
}
void print_clients() {
    cout << "▷현재 접속 : ";
    for (auto& client : sck_list) {
        cout << client.user << " ";
    }
    cout << endl;
}
//
//#pragma comment(lib, "ws2_32.lib") 
//
//#include <WinSock2.h>
//#include <string>
//#include <iostream>
//#include <thread>
//#include <vector>
//
//#define MAX_SIZE 1024
//#define MAX_CLIENT 3
//
//
//using std::cout;
//using std::cin;
//using std::endl;
//using std::string;
//
//struct SOCKET_INFO {
//    SOCKET sck;
//    string user;
//};
//
//std::vector<SOCKET_INFO> sck_list;
//SOCKET_INFO server_sock;
//int client_count = 0;
//
//void server_init();
//void add_client();
//void send_msg(const char* msg);
//void recv_msg(int idx);
//void del_client(int idx);
//
//int main() {
//    WSADATA wsa;
//
//    // Winsock를 초기화하는 함수. MAKEWORD(2, 2)는 Winsock의 2.2 버전을 사용하겠다는 의미.
//    // 실행에 성공하면 0을, 실패하면 그 이외의 값을 반환.
//    // 0을 반환했다는 것은 Winsock을 사용할 준비가 되었다는 의미.
//    int code = WSAStartup(MAKEWORD(2, 2), &wsa);
//
//    if (!code) {
//        server_init();
//
//        std::thread th1[MAX_CLIENT];
//        for (int i = 0; i < MAX_CLIENT; i++) {
//            th1[i] = std::thread(add_client);
//        }
//
//        while (1) {
//            string text, msg = "";
//
//            std::getline(cin, text);
//            const char* buf = text.c_str();
//            msg = server_sock.user + " : " + buf;
//            send_msg(msg.c_str());
//        }
//
//        for (int i = 0; i < MAX_CLIENT; i++) {
//            th1[i].join();
//        }
//
//        closesocket(server_sock.sck);
//    }
//    else {
//        cout << "프로그램 종료. (Error code : " << code << ")";
//    }
//
//    WSACleanup();
//
//    return 0;
//}
//
//void server_init() {
//    server_sock.sck = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
//
//    SOCKADDR_IN server_addr = {};
//    server_addr.sin_family = AF_INET;
//    server_addr.sin_port = htons(7777);
//    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
//
//    bind(server_sock.sck, (sockaddr*)&server_addr, sizeof(server_addr));
//    listen(server_sock.sck, SOMAXCONN);
//
//    server_sock.user = "server";
//
//    cout << "Server On" << endl;
//}
//
//void add_client() {
//    SOCKADDR_IN addr = {};
//    int addrsize = sizeof(addr);
//    char buf[MAX_SIZE] = { };
//
//    ZeroMemory(&addr, addrsize);
//
//    SOCKET_INFO new_client = {};
//
//    new_client.sck = accept(server_sock.sck, (sockaddr*)&addr, &addrsize);
//    recv(new_client.sck, buf, MAX_SIZE, 0);
//    new_client.user = string(buf);
//
//    string msg = "[공지] " + new_client.user + " 님이 입장했습니다.";
//    cout << msg << endl;
//    sck_list.push_back(new_client);
//
//    std::thread th(recv_msg, client_count);
//
//    client_count++;
//    cout << "[공지] 현재 접속자 수 : " << client_count << "명" << endl;
//    send_msg(msg.c_str());
//
//    th.join();
//}
//
//void send_msg(const char* msg) {
//    for (int i = 0; i < client_count; i++) {
//        send(sck_list[i].sck, msg, MAX_SIZE, 0);
//    }
//}
//
//void recv_msg(int idx) {
//    char buf[MAX_SIZE] = { };
//    string msg = "";
//
//    while (1) {
//        ZeroMemory(&buf, MAX_SIZE);
//        if (recv(sck_list[idx].sck, buf, MAX_SIZE, 0) > 0) {
//            msg = sck_list[idx].user + " : " + buf;
//            cout << msg << endl;
//            send_msg(msg.c_str());
//        }
//        else {
//            msg = "[공지] " + sck_list[idx].user + " 님이 퇴장했습니다.";
//            cout << msg << endl;
//            send_msg(msg.c_str());
//            del_client(idx);
//            return;
//        }
//    }
//}
//
//void del_client(int idx) {
//    closesocket(sck_list[idx].sck);
//    client_count--;
//}