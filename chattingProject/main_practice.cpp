#pragma comment(lib, "ws2_32.lib") 

#include <WinSock2.h>
#include <iostream>
#include <string>
#include <thread>
#include <vector>
#include <mysql/jdbc.h>

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
void recv_msg(int idx); //recv() ����
void del_client(int idx); //Ŭ���̾�Ʈ���� ������ ���� ��

int main()
{
    startSql();

    WSADATA wsa;
    int code = WSAStartup(MAKEWORD(2, 2), &wsa);

    if (!code) {
        server_init(); //���� ����

        std::thread th1[MAX_CLIENT];

        for (int i = 0; i < MAX_CLIENT; i++) {
            th1[i] = std::thread(add_client);
            //Ŭ���̾�Ʈ�� ���� �� �ִ� ���¸� ������� accept
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
   
    return 0;
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

void server_init() //������ ���� Ȱ��ȭ
{
    server_sock.sck = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    //���� ������ Ư���� �� �ִ� int�� ���ڸ� ����.
    //server_sock�� sck -> SOCKET_INFO sck, user

    SOCKADDR_IN server_addr = {};
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(7777); //123.0.0.1:7777 �߿� ------:7777�� ����
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY); //123.0.0.1:----�� �����Ѵ�.

    bind(server_sock.sck, (sockaddr*)&server_addr, sizeof(server_addr));
    //server_sock.sck, �ּҸ� �Ҵ��ϰ� ���� socket
    //server_adrr�� �ڷ��� SOCKADDR_IN�� sockaddr*������ ��ȯ

    listen(server_sock.sck, SOMAXCONN);

    server_sock.user = "server";
}

void add_client() {
    //Ŭ���̾�Ʈ�� ������ �����ϸ� ������ ���ο� ������ �����ȴ�.
    //�� ������ �ּҸ� ���� ���� => addr�̴�.
    SOCKADDR_IN addr = {};
    int addrsize = sizeof(addr);
    char buf[MAX_SIZE] = { }; //�޽����� �ִ���̸� �������ش�. 1024

    ZeroMemory(&addr, addrsize); //addr�� 0x00���� �ʱ�ȭ

    SOCKET_INFO new_client = {};
    //sck, user : Ŭ���̾�Ʈ�� ���� ������ ����, �ؿ��� sck_list�� �߰���.

    new_client.sck = accept(server_sock.sck, (sockaddr*)&addr, &addrsize);
    //���� �ּ�, �ּ��� ����
    //Ŭ���̾�Ʈ ����ŭ accept���� connect()
    recv(new_client.sck, buf, MAX_SIZE, 0); //Ŭ���̾�Ʈ connect(), send()
    //Ŭ���̾�Ʈ ������ �ٷ� user �̸��� ��Ƽ� send�� ��. recv()�� �ޱ� ����
    new_client.user = string(buf); //buf�� ����ڰ� �Է��� �̸��� ����ִ�.
    //user�� char������ ������ ������ string������ ����ȯ ���༭ ����.

    string msg = "[����] " + new_client.user + " ���� �����߽��ϴ�.";
    //����ö(���ֻ�ټ�)���� �����߽��ϴ�.
    cout << msg << endl;
    sck_list.push_back(new_client); //sck_list�� �߰���.
    //[ {1234, jechul}, { 1234, jechul }, { 1234, jechul } ]

    std::thread th(recv_msg, client_count);//join�� ������ �������� ��⸦ �Ѵ�.
    //�������ʵ��� ����Ű������ thread�� ����Ѵ�.
    //��� ������ Ŭ���̾�Ʈ�� �����ε� ��� �޽����� ���� �� �ֵ��� recv���� ���
    client_count++;

    cout << "[����] ���� ������ �� : " << client_count << "��" << endl;
    //������ : ���Ҷ�, ����ö
    send_msg(msg.c_str());

    th.join();
}

void send_msg(const char* msg) {
    for (int i = 0; i < client_count; i++) {
        send(sck_list[i].sck, msg, MAX_SIZE, 0);
        //���� ������ ��� Ŭ���̾�Ʈ���� send�޽��� �Ѵ�
    }
}

void recv_msg(int idx) {
    char buf[MAX_SIZE] = { };
    string msg = "";

    while (1) {
        ZeroMemory(&buf, MAX_SIZE);//0�� �ʱ�ȭ
        if (recv(sck_list[idx].sck, buf, MAX_SIZE, 0) > 0) {//���. �޼����� ������ 0���� Ŀ����
            msg = sck_list[idx].user + " : " + buf;
            cout << msg << endl;
            send_msg(msg.c_str());

            pstmt = con->prepareStatement("INSERT INTO chatting(chatname, time, recv) VALUE(?, NOW(),  ?)");
            pstmt->setString(1, sck_list[idx].user);
            pstmt->setString(2, buf);
            pstmt->execute();
        }

        else {
            msg = "[����] " + sck_list[idx].user + " ���� �����߽��ϴ�.";
            cout << msg << endl;
            send_msg(msg.c_str());
            del_client(idx);
            return;
        }
    }
}

void del_client(int idx) {
    closesocket(sck_list[idx].sck);
    client_count--;
}


//pstmt = con->prepareStatement("INSERT INTO inventory(name, quantity) VALUES(?,?)"); // INSERT
////prepareStatement ���߿� ?�� �� �Է�

//pstmt->setString(1, "banana");
//pstmt->setInt(2, 150);
//pstmt->execute();
//cout << "One row inserted." << endl;

//pstmt->setString(1, "orange");
//pstmt->setInt(2, 154);
//pstmt->execute();
//cout << "One row inserted." << endl;

//pstmt->setString(1, "���");
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

// MySQL Connector/C++ ����
//delete result;
//delete pstmt;
//delete con;
//system("pause");