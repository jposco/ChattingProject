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
void recv_msg(int idx); //recv() 실행
void del_client(int idx); //클라이언트와의 연결을 끊을 때

int main()
{
    startSql();
    mainMenu();
    WSADATA wsa;
    int code = WSAStartup(MAKEWORD(2, 2), &wsa);

    if (!code) {
        server_init(); //서버 연결

        std::thread th1[MAX_CLIENT];

        for (int i = 0; i < MAX_CLIENT; i++) {
            th1[i] = std::thread(add_client);
            //클라이언트를 받을 수 있는 상태를 만들어줌 accept
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

    return 0;
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

void mainMenu()
{
    cout << "\n\n";
    cout << "\t"; cout << "********************************** \n";
    cout << "\t"; cout << "*                                * \n";
    cout << "\t"; cout << "*  *******    *     *     *  *   * \n";
    cout << "\t"; cout << "*     *      * *    *     * *    * \n";
    cout << "\t"; cout << "*     *     *****   *     **     * \n";
    cout << "\t"; cout << "*     *    *     *  *     * *    * \n";
    cout << "\t"; cout << "*     *   *       * ***** *  *   * \n";
    cout << "\t"; cout << "*                                * \n";
    cout << "\t"; cout << "*                                * \n";
    cout << "\t"; cout << "*                                * \n";
    cout << "\t"; cout << "*                                * \n";
    cout << "\t"; cout << "*            SERVER ON           * \n";
    cout << "\t"; cout << "*                                * \n";
    cout << "\t"; cout << "*                                * \n";
    cout << "\t"; cout << "*                                * \n";
    cout << "\t"; cout << "*                                * \n";
    cout << "\t"; cout << "********************************** \n\n";
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
    char buf[MAX_SIZE] = { }; //메시지의 최대길이를 설정해준다. 1024

    ZeroMemory(&addr, addrsize); //addr을 0x00으로 초기화

    SOCKET_INFO new_client = {};

    new_client.sck = accept(server_sock.sck, (sockaddr*)&addr, &addrsize);

    recv(new_client.sck, buf, MAX_SIZE, 0); 
    new_client.user = string(buf);

    string msg = "[공지] " + new_client.user + " 님이 입장했습니다.";
    //정제철(제주삼다수)님이 입장했습니다.
    cout << msg << endl;
    sck_list.push_back(new_client);
    cout << "sock" << sck_list[0].sck << endl;
    std::thread th(recv_msg, client_count);
    client_count++;

    cout << "[공지] 현재 접속자 수 : " << client_count << "명" << endl;
    send_msg(msg.c_str());

    th.join();
}

void send_msg(const char* msg) {
    for (int i = 0; i < client_count; i++) {
        send(sck_list[i].sck, msg, MAX_SIZE, 0);
    }
}
void handle_input(int idx, const char* buf) {
    if (strcmp(buf, "/exit") == 0) {
        string msg = "client " + to_string(idx) + " has left the chat room";
        cout << msg << endl;
        send_msg(msg.c_str());
        del_client(idx);
    }
    else {
        // handle normal message
    }
}
void recv_msg(int idx) {
    char buf[MAX_SIZE] = { };
    string msg = "";

    while (1) {
        ZeroMemory(&buf, MAX_SIZE);//0로 초기화
        cout << "sockkkkk : " << sck_list[idx].sck;

        if (recv(sck_list[idx].sck, buf, MAX_SIZE, 0) > 0) {
            cout << "aa : " << buf;
            handle_input(idx, buf);//대기. 메세지가 들어오면 0보다 커진다
            msg = sck_list[idx].user + " : " + buf;
            cout << msg << endl;
            send_msg(msg.c_str());

            pstmt = con->prepareStatement("INSERT INTO chatting(chatname, time, recv) VALUE(?, NOW(),  ?)");
            pstmt->setString(1, sck_list[idx].user);
            pstmt->setString(2, buf);
            pstmt->execute();
        }

        else {
            cout << "bb : ";
            msg = "[공지] " + sck_list[idx].user + " 님이 퇴장했습니다.";
            cout << msg << endl;
            send_msg(msg.c_str());
            del_client(idx);
            return;
        }
    }
}

void del_client(int idx) {
    closesocket(sck_list[idx].sck);
    sck_list.clear();
    client_count--;
}


//pstmt = con->prepareStatement("INSERT INTO inventory(name, quantity) VALUES(?,?)"); // INSERT
////prepareStatement 나중에 ?에 값 입력

//pstmt->setString(1, "banana");
//pstmt->setInt(2, 150);
//pstmt->execute();
//cout << "One row inserted." << endl;

//pstmt->setString(1, "orange");
//pstmt->setInt(2, 154);
//pstmt->execute();
//cout << "One row inserted." << endl;

//pstmt->setString(1, "사과");
//pstmt->setInt(2, 100);
//pstmt->execute();
//cout << "One row inserted." << endl;

////select  
//pstmt = con->prepareStatement("SELECT * FROM inventory;");
//result = pstmt->executeQuery();

////update
//pstmt = con->prepareStatement("UPDATE inventory SET quantity = ? WHERE name = ?");
//pstmt->setInt(1, 200);
//pstmt->setString(2, "banana");
//pstmt->executeQuery();
//printf("Row updated\n");

////delete
//pstmt = con->prepareStatement("DELETE FROM inventory WHERE name = ?");
//pstmt->setString(1, "orange");
//result = pstmt->executeQuery();
//printf("Row deleted\n");

// MySQL Connector/C++ 정리
//delete result;
//delete pstmt;
//delete con;
//system("pause");