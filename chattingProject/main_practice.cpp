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

#define MAX_SIZE 1024 //��� ����
#define MAX_CLIENT 3 //�ִ� �ο� 3

const string server = "tcp://127.0.0.1:3306"; // �����ͺ��̽� �ּ�
const string username = "root"; // �����ͺ��̽� �����
const string password = "07wd2713"; // �����ͺ��̽� ���� ��й�ȣ


struct SOCKET_INFO { //����ü ����
    SOCKET sck=0; //ctrl + Ŭ��, unsigned int pointer��
    string user =""; //user  : ����� �̸�
};

vector<SOCKET_INFO> sck_list; //������ ����� client�� ������ ����.
SOCKET_INFO server_sock; //��������� ������ ������ ����.
int client_count = 0; //���� ���ӵ� Ŭ���̾�Ʈ �� ī��Ʈ �뵵.

//SQL
sql::mysql::MySQL_Driver* driver; // ���� �������� �ʾƵ� Connector/C++�� �ڵ����� ������ �� 
sql::Connection* con;
sql::PreparedStatement* pstmt;
sql::ResultSet* result;
sql::Statement* stmt;
void startSql(); //SQL���� - creatTable;

//�ý��۱���
void mainMenu();

//ä��
void server_init(); //������ ������ ����� �Լ�
void add_client(); //accept �Լ� ����ǰ� ���� ����
void send_msg(const char* msg); //send() ����
void send_msg_notMe(const char* msg, int sender_idx);
void recv_msg(int idx); //recv() ����
void del_client(int idx); //Ŭ���̾�Ʈ���� ������ ���� ��
void print_clients();


int main()
{
    system("color 06");
    startSql();
    mainMenu();
    WSADATA wsa;

    int code = WSAStartup(MAKEWORD(2, 2), &wsa);
    if (!code) {
        server_init(); //���� ����

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
        cout << "���α׷� ����. (Error code : " << code << ")";
    }
    WSACleanup();
    delete result;
    delete pstmt;
    delete con;
    delete stmt;

    return 0;
}

void server_init() //������ ���� Ȱ��ȭ
{
    server_sock.sck = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);

    SOCKADDR_IN server_addr = {};
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(7777); //123.0.0.1:7777 �߿� ------:7777�� ����
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY); //123.0.0.1:----�� �����Ѵ�.

    bind(server_sock.sck, (sockaddr*)&server_addr, sizeof(server_addr));

    listen(server_sock.sck, SOMAXCONN);

    server_sock.user = "server";
}
void send_msg_notMe(const char* msg, int sender_idx)
{
    for (int i = 0; i < client_count; i++) {
        if (i != sender_idx) {
            send(sck_list[i].sck, msg, MAX_SIZE, 0);
        }
    }
}

void send_msg(const char* msg)
{

    for (int i = 0; i < client_count; i++) {
            send(sck_list[i].sck, msg, MAX_SIZE, 0);
        
    }
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

    string msg = "��" + new_client.user + " ���� �����߽��ϴ�.";
    cout << msg << endl;
    sck_list.push_back(new_client);
    print_clients();

    std::thread th(recv_msg, client_count);
    th.detach();
    client_count++;

    cout << "������ ������ �� : " << client_count << "��" << endl;
    send_msg(msg.c_str());
}

void recv_msg(int idx) {
    char buf[MAX_SIZE] = { };
    string msg = "";

    while (1) {
        ZeroMemory(&buf, MAX_SIZE);//0�� �ʱ�ȭ

        if (recv(sck_list[idx].sck, buf, MAX_SIZE, 0) > 0) {
        
            if(string(buf) == "/exit")
            {
                msg = "��" + sck_list[idx].user + " ���� �����߽��ϴ�.";
                cout << msg << endl;
                send_msg(msg.c_str());
                del_client(idx);
                return;
            }
            else if (string(buf) == "/DM")
            {

            }

            else {

                pstmt = con->prepareStatement("INSERT INTO chatting(chatname, time, recv) VALUE(?, NOW(),  ?)");
                pstmt->setString(1, sck_list[idx].user);
                pstmt->setString(2, buf);
                pstmt->execute();

                pstmt = con->prepareStatement("SELECT chatname, time, recv FROM chatting ORDER BY time DESC LIMIT 1");
                result = pstmt->executeQuery();
                if (result->next())
                {
                    string chatname = result->getString(1);
                    string time = result->getString(2);
                    string recv = result->getString(3);
                    msg = "----------------------------------------------------------------\n";
                    msg += "������ ��� : " + chatname + "\t\t" + "������ �ð� : " + time + "\n";
                    msg += "������ : " + recv + "\n";
                    msg += "---------------------------------------------------------------\n";
                    cout << msg << endl;
                    send_msg_notMe(msg.c_str(), idx);
                }
            }
        }
    }
}

void del_client(int idx) {
    std::thread th(add_client);
    closesocket(sck_list[idx].sck);
    client_count--;
    cout << "������ ������ �� : " << client_count << "��" << endl;
    sck_list.erase(sck_list.begin() + idx);
    th.join();
}
void mainMenu()
{
    cout << "\n\n";
    cout << " "; cout << "********************************** \n";
    cout << " "; cout << "*                                * \n";
    cout << " "; cout << "*  *******    *     *     *  *   * \n";
    cout << " "; cout << "*     *      * *    *     * *    * \n";
    cout << " "; cout << "*     *     *****   *     **     * \n";
    cout << " "; cout << "*     *    *     *  *     * *    * \n";
    cout << " "; cout << "*     *   *       * ***** *  *   * \n";
    cout << " "; cout << "*                                * \n";
    cout << " "; cout << "*                                * \n";
    cout << " "; cout << "*                                * \n";
    cout << " "; cout << "*                                * \n";
    cout << " "; cout << "*            SERVER ON           * \n";
    cout << " "; cout << "*                                * \n";
    cout << " "; cout << "*                                * \n";
    cout << " "; cout << "*                                * \n";
    cout << " "; cout << "*                                * \n";
    cout << " "; cout << "********************************** \n\n";
}
void startSql()
{
    // MySQL Connector/C++ �ʱ�ȭ

    try {
        driver = sql::mysql::get_mysql_driver_instance();
        con = driver->connect(server, username, password);
    }
    catch (sql::SQLException& e) {
        cout << "Could not connect to server. Error message: " << e.what() << endl;
        exit(1);
    }

    // �����ͺ��̽� ����                                                                
    con->setSchema("project1");

    // DB �ѱ� ������ ���� ����                                                             
    stmt = con->createStatement();
    stmt->execute("set names euckr");
    if (stmt) { delete stmt; stmt = nullptr; }
}
void print_clients() {
    cout << "������ ���� : ";
    for (auto& client : sck_list) {
        cout << client.user << " ";
    }
    cout << endl;
}


