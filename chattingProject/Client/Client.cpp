#pragma comment(lib, "ws2_32.lib")

#include <WinSock2.h> //Winsock 헤더파일 include. WSADATA 들어있음.
#include <WS2tcpip.h>
#include <string>
#include <sstream>
#include <iostream>
#include <thread>
#include <mysql/jdbc.h>
#include <conio.h> 

using std::cout;
using std::cin; 
using std::endl;
using std::string;

#define MAX_SIZE 1024

const string server = "tcp://127.0.0.1:3306"; // 데이터베이스 주소
const string username = "root"; // 데이터베이스 사용자
const string password = "07wd2713"; // 데이터베이스 접속 비밀번호

// MySQL Connector/C++ 초기화
sql::mysql::MySQL_Driver* driver; // 추후 해제하지 않아도 Connector/C++가 자동으로 해제해 줌 
sql::Connection* con; //연결
sql::PreparedStatement* pstmt; //나중에 값 입력
sql::ResultSet* result;
sql::Statement* stmt;

SOCKET client_sock;
string my_nick;

void startMenu()
{
    cout << "\n";
    cout << " "; cout << "********************************* \n";
    cout << " "; cout << "*                               * \n";
    cout << " "; cout << "*  *******    *     *     *  *  * \n";
    cout << " "; cout << "*     *      * *    *     * *   * \n";
    cout << " "; cout << "*     *     *****   *     **    * \n";
    cout << " "; cout << "*     *    *     *  *     * *   * \n";
    cout << " "; cout << "*     *   *       * ***** *  *  * \n";
    cout << " "; cout << "*                               * \n";
    cout << " "; cout << "*                               * \n";
    cout << " "; cout << "*                               * \n";
    cout << " "; cout << "*                               * \n";
    cout << " "; cout << "*      1.로그인                 * \n";
    cout << " "; cout << "*                               * \n";
    cout << " "; cout << "*      2. ID 찾기               * \n";
    cout << " "; cout << "*                               * \n";
    cout << " "; cout << "*      3. PW 찾기               * \n";
    cout << " "; cout << "*                               * \n";
    cout << " "; cout << "*      4. 회원가입              * \n";
    cout << " "; cout << "*                               * \n";
    cout << " "; cout << "*      0. 종료                  * \n";
    cout << " "; cout << "*                               * \n";
    cout << " "; cout << "*                               * \n";
    cout << " "; cout << "********************************* \n\n";
}
void login()
{
    system("cls");
    cout << "\n";
    cout << " "; cout << "********************************* \n";
    cout << " "; cout << "*                               * \n";
    cout << " "; cout << "*  *******    *     *     *  *  * \n";
    cout << " "; cout << "*     *      * *    *     * *   * \n";
    cout << " "; cout << "*     *     *****   *     **    * \n";
    cout << " "; cout << "*     *    *     *  *     * *   * \n";
    cout << " "; cout << "*     *   *       * ***** *  *  * \n";
    cout << " "; cout << "*                               * \n";
    cout << " "; cout << "*                               * \n";
    cout << " "; cout << "*                               * \n";
    cout << " "; cout << "*                               * \n";
    cout << " "; cout << "*      < 로그인 하기 >          * \n";
    cout << " "; cout << "*                               * \n";
    cout << " "; cout << "*      >> 아이디 입력           * \n";
    cout << " "; cout << "*                               * \n";
    cout << " "; cout << "*      >> 비밀번호 입력         * \n";
    cout << " "; cout << "*                               * \n";
    cout << " "; cout << "*      >> 실패시 메인화면       * \n";
    cout << " "; cout << "*                               * \n";
    cout << " "; cout << "*                               * \n";
    cout << " "; cout << "*                               * \n";
    cout << " "; cout << "*                               * \n";
    cout << " "; cout << "********************************* \n\n";
}
void searchId()
{
    system("cls");
    cout << "\n";
    cout << " "; cout << "********************************* \n";
    cout << " "; cout << "*                               * \n";
    cout << " "; cout << "*  *******    *     *     *  *  * \n";
    cout << " "; cout << "*     *      * *    *     * *   * \n";
    cout << " "; cout << "*     *     *****   *     **    * \n";
    cout << " "; cout << "*     *    *     *  *     * *   * \n";
    cout << " "; cout << "*     *   *       * ***** *  *  * \n";
    cout << " "; cout << "*                               * \n";
    cout << " "; cout << "*                               * \n";
    cout << " "; cout << "*                               * \n";
    cout << " "; cout << "*                               * \n";
    cout << " "; cout << "*      < 아이디 찾기 >          * \n";
    cout << " "; cout << "*                               * \n";
    cout << " "; cout << "*      >> 이름 입력             * \n";
    cout << " "; cout << "*                               * \n";
    cout << " "; cout << "*      >> 번호 입력             * \n";
    cout << " "; cout << "*                               * \n";
    cout << " "; cout << "*      >> 생년월일(8자리) 입력  * \n";
    cout << " "; cout << "*                               * \n";
    cout << " "; cout << "*      >> 완료시 메인화면       * \n";
    cout << " "; cout << "*                               * \n";
    cout << " "; cout << "*                               * \n";
    cout << " "; cout << "********************************* \n\n";
}
void searchPw()
{
    system("cls");
    cout << "\n";
    cout << " "; cout << "********************************* \n";
    cout << " "; cout << "*                               * \n";
    cout << " "; cout << "*  *******    *     *     *  *  * \n";
    cout << " "; cout << "*     *      * *    *     * *   * \n";
    cout << " "; cout << "*     *     *****   *     **    * \n";
    cout << " "; cout << "*     *    *     *  *     * *   * \n";
    cout << " "; cout << "*     *   *       * ***** *  *  * \n";
    cout << " "; cout << "*                               * \n";
    cout << " "; cout << "*                               * \n";
    cout << " "; cout << "*                               * \n";
    cout << " "; cout << "*                               * \n";
    cout << " "; cout << "*      < 비밀번호 찾기 >        * \n";
    cout << " "; cout << "*                               * \n";
    cout << " "; cout << "*      >> 아이디 입력           * \n";
    cout << " "; cout << "*                               * \n";
    cout << " "; cout << "*      >> 이름 입력             * \n";
    cout << " "; cout << "*                               * \n";
    cout << " "; cout << "*      >> 전화번호 입력         * \n";
    cout << " "; cout << "*                               * \n";
    cout << " "; cout << "*      >> 생년월일(8자리) 입력  * \n";
    cout << " "; cout << "*                               * \n";
    cout << " "; cout << "*      >> 완료시 메인화면       * \n";
    cout << " "; cout << "*                               * \n";
    cout << " "; cout << "********************************* \n\n";
}
void createUser()
{
    system("cls");
    cout << "\n";
    cout << " "; cout << "********************************* \n";
    cout << " "; cout << "*                               * \n";
    cout << " "; cout << "*  *******    *     *     *  *  * \n";
    cout << " "; cout << "*     *      * *    *     * *   * \n";
    cout << " "; cout << "*     *     *****   *     **    * \n";
    cout << " "; cout << "*     *    *     *  *     * *   * \n";
    cout << " "; cout << "*     *   *       * ***** *  *  * \n";
    cout << " "; cout << "*                               * \n";
    cout << " "; cout << "*                               * \n";
    cout << " "; cout << "*                               * \n";
    cout << " "; cout << "*                               * \n";
    cout << " "; cout << "*      < 회원가입 하기 >        * \n";
    cout << " "; cout << "*                               * \n";
    cout << " "; cout << "*      >> 아이디 입력           * \n";
    cout << " "; cout << "*                               * \n";
    cout << " "; cout << "*      >> 비밀번호 입력         * \n";
    cout << " "; cout << "*                               * \n";
    cout << " "; cout << "*      >> 이름 입력             * \n";
    cout << " "; cout << "*                               * \n";
    cout << " "; cout << "*      >> 전화번호 입력         * \n";
    cout << " "; cout << "*                               * \n";
    cout << " "; cout << "*      >> 생년월일(8자리) 입력  * \n";
    cout << " "; cout << "*                               * \n";
    cout << " "; cout << "*      >> 완료시 메인화면       * \n";
    cout << " "; cout << "*                               * \n";
    cout << " "; cout << "********************************* \n\n";
}
void mainMenu()
{
    system("cls");
    cout << "\n";
    cout << " "; cout << "********************************* \n";
    cout << " "; cout << "*                               * \n";
    cout << " "; cout << "*  *******    *     *     *  *  * \n";
    cout << " "; cout << "*     *      * *    *     * *   * \n";
    cout << " "; cout << "*     *     *****   *     **    * \n";
    cout << " "; cout << "*     *    *     *  *     * *   * \n";
    cout << " "; cout << "*     *   *       * ***** *  *  * \n";
    cout << " "; cout << "*                               * \n";
    cout << " "; cout << "*                               * \n";
    cout << " "; cout << "*    < 현재 상태 : 접속 중 >    * \n";
    cout << " "; cout << "*                               * \n";
    cout << " "; cout << "*      1. 내 정보               * \n";
    cout << " "; cout << "*                               * \n";
    cout << " "; cout << "*      2. 친구                  * \n";
    cout << " "; cout << "*                               * \n";
    cout << " "; cout << "*      3. 대화방                * \n";
    cout << " "; cout << "*                               * \n";
    cout << " "; cout << "*      4. 설정                  * \n";
    cout << " "; cout << "*                               * \n";
    cout << " "; cout << "*      0. 종료                  * \n";
    cout << " "; cout << "*                               * \n";
    cout << " "; cout << "*                               * \n";
    cout << " "; cout << "********************************* \n\n";
}
void myMenu()
{
    system("cls");
    cout << "\n";
    cout << " "; cout << "********************************* \n";
    cout << " "; cout << "*                               * \n";
    cout << " "; cout << "*  *******    *     *     *  *  * \n";
    cout << " "; cout << "*     *      * *    *     * *   * \n";
    cout << " "; cout << "*     *     *****   *     **    * \n";
    cout << " "; cout << "*     *    *     *  *     * *   * \n";
    cout << " "; cout << "*     *   *       * ***** *  *  * \n";
    cout << " "; cout << "*                               * \n";
    cout << " "; cout << "*                               * \n";
    cout << " "; cout << "*                               * \n";
    cout << " "; cout << "*                               * \n";
    cout << " "; cout << "*      < 내 정보 >              * \n";
    cout << " "; cout << "*                               * \n";
    cout << " "; cout << "*      1. 내 프로필             * \n";
    cout << " "; cout << "*                               * \n";
    cout << " "; cout << "*      2. 상메 설정/수정        * \n";
    cout << " "; cout << "*                               * \n";
    cout << " "; cout << "*      3. BGM 설정/수정         * \n";
    cout << " "; cout << "*                               * \n";
    cout << " "; cout << "*      0. 뒤로가기              * \n";
    cout << " "; cout << "*                               * \n";
    cout << " "; cout << "*                               * \n";
    cout << " "; cout << "********************************* \n\n";
}
void friends()
{
    system("cls");
    cout << "\n";
    cout << " "; cout << "********************************* \n";
    cout << " "; cout << "*                               * \n";
    cout << " "; cout << "*  *******    *     *     *  *  * \n";
    cout << " "; cout << "*     *      * *    *     * *   * \n";
    cout << " "; cout << "*     *     *****   *     **    * \n";
    cout << " "; cout << "*     *    *     *  *     * *   * \n";
    cout << " "; cout << "*     *   *       * ***** *  *  * \n";
    cout << " "; cout << "*                               * \n";
    cout << " "; cout << "*                               * \n";
    cout << " "; cout << "*                               * \n";
    cout << " "; cout << "*                               * \n";
    cout << " "; cout << "*      < 친구 >                 * \n";
    cout << " "; cout << "*                               * \n";
    cout << " "; cout << "*      1. 친구 목록 보기        * \n";
    cout << " "; cout << "*                               * \n";
    cout << " "; cout << "*      2. 친구 생일 검색        * \n";
    cout << " "; cout << "*                               * \n";
    cout << " "; cout << "*      >> 월일 ~ 월일 검색      * \n";
    cout << " "; cout << "*                               * \n";
    cout << " "; cout << "*      0. 뒤로가기              * \n";
    cout << " "; cout << "*                               * \n";
    cout << " "; cout << "*                               * \n";
    cout << " "; cout << "********************************* \n\n";
}
void chatting()
{
    system("cls");
    cout << "\n";
    cout << " "; cout << "********************************* \n";
    cout << " "; cout << "*                               * \n";
    cout << " "; cout << "*  *******    *     *     *  *  * \n";
    cout << " "; cout << "*     *      * *    *     * *   * \n";
    cout << " "; cout << "*     *     *****   *     **    * \n";
    cout << " "; cout << "*     *    *     *  *     * *   * \n";
    cout << " "; cout << "*     *   *       * ***** *  *  * \n";
    cout << " "; cout << "*                               * \n";
    cout << " "; cout << "*                               * \n";
    cout << " "; cout << "*                               * \n";
    cout << " "; cout << "*                               * \n";
    cout << " "; cout << "*      < 채팅 >                 * \n";
    cout << " "; cout << "*                               * \n";
    cout << " "; cout << "*      1. 채팅방 입장           * \n";
    cout << " "; cout << "*                               * \n";
    cout << " "; cout << "*      >> /exit 입력시 퇴장     * \n";
    cout << " "; cout << "*                               * \n";
    cout << " "; cout << "*      2. 채팅 내용 검색        * \n";
    cout << " "; cout << "*                               * \n";
    cout << " "; cout << "*      0. 뒤로가기              * \n";
    cout << " "; cout << "*                               * \n";
    cout << " "; cout << "*                               * \n";
    cout << " "; cout << "********************************* \n\n";
}
void setting()
{
    system("cls");
    cout << "\n";
    cout << " "; cout << "********************************* \n";
    cout << " "; cout << "*                               * \n";
    cout << " "; cout << "*  *******    *     *     *  *  * \n";
    cout << " "; cout << "*     *      * *    *     * *   * \n";
    cout << " "; cout << "*     *     *****   *     **    * \n";
    cout << " "; cout << "*     *    *     *  *     * *   * \n";
    cout << " "; cout << "*     *   *       * ***** *  *  * \n";
    cout << " "; cout << "*                               * \n";
    cout << " "; cout << "*                               * \n";
    cout << " "; cout << "*                               * \n";
    cout << " "; cout << "*                               * \n";
    cout << " "; cout << "*      < 설정 >                 * \n";
    cout << " "; cout << "*                               * \n";
    cout << " "; cout << "*      1. 비밀번호 변경         * \n";
    cout << " "; cout << "*                               * \n";
    cout << " "; cout << "*      2. 회원 탈퇴             * \n";
    cout << " "; cout << "*                               * \n";
    cout << " "; cout << "*      0. 뒤로가기              * \n";
    cout << " "; cout << "*                               * \n";
    cout << " "; cout << "*                               * \n";
    cout << " "; cout << "*                               * \n";
    cout << " "; cout << "*                               * \n";
    cout << " "; cout << "********************************* \n\n";
}
int chat_recv() {
    char buf[MAX_SIZE] = { };
    string msg;

    while (1) {
        ZeroMemory(&buf, MAX_SIZE);
        if (recv(client_sock, buf, MAX_SIZE, 0) > 0) {
            msg = buf;
            //닉네임 : 메시지
            std::stringstream ss(msg);  // 문자열을 스트림화
            string user;
            ss >> user; // 스트림을 통해, 문자열을 공백 분리해 변수에 할당
            //띄어쓰기를 기준으로 닉네임 추출
            if (user != my_nick) cout << buf << endl; // 내 닉네임과 대조한 후 내가 보낸 게 아닐 경우(상대방)에만 출력하도록.
        }
        else {
            cout << "Server Off" << endl;
            return -1;
        }
    }
}

class SQL
{
private:
    string id, pw, name, phone, status, birth, song = "";
public:
    SQL()
    {
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

    int login()
    {
        cout << ">>아이디 : ";
        cin >> id;
        this->id = id;
        cout << ">>비밀번호를 입력해주세요 : ";
        char ch = ' ';
        while (ch != 13) { // Enter 키를 누르면 입력 종료
            ch = _getch();
            if (ch == 13) break; // Enter 키를 누르면 입력 종료
            if (ch == 8) { // Backspace 키인 경우
                if (!pw.empty()) { // 입력된 문자가 있으면
                    pw.pop_back(); // 마지막 문자를 삭제
                    cout << "\b \b"; // 커서 위치를 왼쪽으로 이동시켜 공백을 출력한 뒤, 다시 커서 위치를 원래대로 이동시킴
                }
            }
            else {
                pw.push_back(ch);
                cout << '*'; // 별표로 대체하여 출력
            }
        }
        cout << endl;

        pstmt = con->prepareStatement("SELECT id, pw, name FROM user \
            WHERE id=?");
        pstmt->setString(1, id);
        result = pstmt->executeQuery();

        if (result->next())
        { // 쿼리 결과가 있다면
            string db_id = result->getString(1); // 데이터베이스에서 가져온 ID
            string db_pw = result->getString(2); // 데이터베이스에서 가져온 PW

            if (db_id == id && db_pw == pw)
            {
                name = result->getString(3);
                this->name = name;
                cout << "▶" << name << "님 환영합니다." << endl;
                return 1;
            }
            else cout << "▶해당하는 정보가 없습니다." << endl;
        }
        else cout << "▶해당하는 정보가 없습니다." << endl;
        pw.clear(); //재로그인을 위해 pw값을 비워둔다.
    }
    void searchId()
    {
        cout << ">>이름 : ";
        cin >> name;
        cout << ">>전화번호 : ";
        cin >> phone;
        while (true)
        {
            cout << ">>생년월일 : ";
            cin >> birth;
            if (birth.length() != 8) {
                cout << "▶생년월일은 8자리로 입력해주세요." << endl;
                continue;
            }
            break;
        }


        string year = birth.substr(0, 4);
        string month = birth.substr(4, 2);
        string day = birth.substr(6, 2);
        string DATE = year + "-" + month + "-" + day;

        pstmt = con->prepareStatement("SELECT id, name, phone, birth FROM user WHERE phone=?");
        pstmt->setString(1, phone);
        result = pstmt->executeQuery();

        if (result->next())
        {
            string db_id = result->getString(1);
            string db_name = result->getString(2);
            string db_phone = result->getString(3);
            string db_birth = result->getString(4);

            if (db_name == name && db_phone == phone && db_birth == DATE)
            {
                cout << "▶" << name << "님의 아이디는 " << db_id << "입니다." << endl;
            }
            else  cout << "▶해당하는 정보가 없습니다." << endl;
        }
        else cout << "▶해당하는 정보가 없습니다." << endl;
    }
    void searchPw()
    {
        cout << ">>아이디 : ";
        cin >> id;
        cout << ">>이름 : ";
        cin >> name;
        cout << ">>전화번호 : ";
        cin >> phone;
        while (true)
        {
            cout << ">>생년월일 : ";
            cin >> birth;
            if (birth.length() != 8) {
                cout << "▶생년월일은 8자리로 입력해주세요." << endl;
                continue;
            }
            break;
        }

        string year = birth.substr(0, 4);
        string month = birth.substr(4, 2);
        string day = birth.substr(6, 2);
        string DATE = year + "-" + month + "-" + day;

        pstmt = con->prepareStatement("SELECT id, name, phone, birth FROM user WHERE id=?");
        pstmt->setString(1, id);
        result = pstmt->executeQuery();

        if (result->next())
        {
            string db_id = result->getString(1);
            string db_name = result->getString(2);
            string db_phone = result->getString(3);
            string db_birth = result->getString(4);

            if (db_id == id && db_name == name && db_phone == phone && db_birth == DATE)
            {
                string new_pw = "";
                cout << ">>새로운 비밀번호를 입력해주세요 : ";
                char ch = ' ';
                while (ch != 13) { // Enter 키를 누르면 입력 종료
                    ch = _getch();
                    if (ch == 13) break; // Enter 키를 누르면 입력 종료
                    if (ch == 8) { // Backspace 키인 경우
                        if (!new_pw.empty()) { // 입력된 문자가 있으면
                            new_pw.pop_back(); // 마지막 문자를 삭제
                            cout << "\b \b"; // 커서 위치를 왼쪽으로 이동시켜 공백을 출력한 뒤, 다시 커서 위치를 원래대로 이동시킴
                        }
                    }
                    else {
                        new_pw.push_back(ch);
                        cout << '*'; // 별표로 대체하여 출력
                    }
                }
                cout << endl;
                while (1) {
                    string renew_pw = "";
                    cout << endl << ">>다시 한번 입력해주세요. : ";
                    char rech = ' ';
                    while (rech != 13) { // Enter 키를 누르면 입력 종료
                        rech = _getch();

                        if (rech == 13) break; // Enter 키를 누르면 입력 종료
                        if (ch == 8) { // Backspace 키인 경우
                            if (!renew_pw.empty()) { // 입력된 문자가 있으면
                                renew_pw.pop_back(); // 마지막 문자를 삭제
                                cout << "\b \b"; // 커서 위치를 왼쪽으로 이동시켜 공백을 출력한 뒤, 다시 커서 위치를 원래대로 이동시킴
                            }
                        }
                        else {
                            renew_pw.push_back(rech);
                            cout << '*'; // 별표로 대체하여 출력
                        }
                    }
                    cout << endl;

                    if (new_pw == renew_pw)
                    {
                        pstmt = con->prepareStatement("UPDATE user SET pw = ? WHERE id = ?");
                        pstmt->setString(1, new_pw);
                        pstmt->setString(2, id);
                        pstmt->executeQuery();
                        printf("▶새로운 비밀번호로 변경되었습니다.\n");
                        break;
                    }
                    else cout << "▶비밀번호가 다릅니다." << endl;
                }
            }
            else  cout << "▶해당하는 정보가 없습니다." << endl;
        }
        else cout << "▶해당하는 정보가 없습니다." << endl;
    }
    void crateUser()
    {
        while (1)
        {
            cout << ">>아이디 : ";
            cin >> id;
            pstmt = con->prepareStatement("SELECT id FROM user WHERE id=?");
            pstmt->setString(1, id);
            result = pstmt->executeQuery();

            if (result->next())
            {
                string db_id = result->getString(1);
                if (db_id == id) {
                    cout << "▶중복된 아이디가 있습니다." << endl;
                    continue;
                }
            }
            else {
                cout << "▶중복체크 완료." << endl;
                break;
            }
        }
        while (1)
        {
            string new_pw = "";
            cout << " >> 새로운 비밀번호를 입력해주세요 : ";
            char ch = ' ';
            while (ch != 13) { // Enter 키를 누르면 입력 종료
                ch = _getch();
                if (ch == 13) break; // Enter 키를 누르면 입력 종료
                if (ch == 8) { // Backspace 키인 경우
                    if (!new_pw.empty()) { // 입력된 문자가 있으면
                        new_pw.pop_back(); // 마지막 문자를 삭제
                        cout << "\b \b"; // 커서 위치를 왼쪽으로 이동시켜 공백을 출력한 뒤, 다시 커서 위치를 원래대로 이동시킴
                    }
                }
                else {
                    new_pw.push_back(ch);
                    cout << '*'; // 별표로 대체하여 출력
                }
            }
            cout << endl;
            string renew_pw = "";
            cout << endl << ">>다시 한번 입력해주세요. : ";
            char rech = ' ';
            while (rech != 13) { // Enter 키를 누르면 입력 종료
                rech = _getch();
                if (rech == 13) break; // Enter 키를 누르면 입력 종료
                if (rech == 8) { // Backspace 키인 경우
                    if (!renew_pw.empty()) { // 입력된 문자가 있으면
                        renew_pw.pop_back(); // 마지막 문자를 삭제
                        cout << "\b \b"; // 커서 위치를 왼쪽으로 이동시켜 공백을 출력한 뒤, 다시 커서 위치를 원래대로 이동시킴
                    }
                }
                else {
                    renew_pw.push_back(rech);
                    cout << '*'; // 별표로 대체하여 출력
                }
            }
            cout << endl;
            if (new_pw == renew_pw)
            {
                break;
            }
            else cout << "▶비밀번호가 다릅니다." << endl;
        }
        cout << ">>이름 : ";
        cin >> name;
        cout << ">>전호번호 : ";
        cin >> phone;
        while (true)
        {
            cout << ">>생년월일 : ";
            cin >> birth;
            if (birth.length() != 8) {
                cout << "▶생년월일은 8자리로 입력해주세요." << endl;
                continue;
            }
            break;
        }

        string year = birth.substr(0, 4);
        string month = birth.substr(4, 2);
        string day = birth.substr(6, 2);
        string DATE = year + "-" + month + "-" + day;

        pstmt = con->prepareStatement("INSERT INTO user(id, pw, name, phone, birth) VALUE(?, ?, ?, ?, ?)");
        pstmt->setString(1, id);
        pstmt->setString(2, pw);
        pstmt->setString(3, name);
        pstmt->setString(4, phone);
        pstmt->setString(5, DATE);
        pstmt->execute();
        cout << "▶회원가입이 완료되었습니다." << endl;
    }
    void myProfile()
    {
        pstmt = con->prepareStatement("SELECT name, status, song, birth, phone FROM user WHERE id = ?;");
        pstmt->setString(1, id);
        result = pstmt->executeQuery();

        while (result->next())
        {
            cout << "-----------------------------" << endl;
            cout << "▷이름 : " << result->getString("name") << endl;
            if (result->getString("status") == "")
            {
                cout << "▷상메 : 없음" << endl;
            }
            else {
                cout << "▷상메 : " << result->getString("status") << endl;
            }
            if (result->getString("song") == "")
            {
                cout << "▷노래 : 없음" << endl;
            }
            else {
                cout << "▷노래 : " << result->getString("song") << endl;
            }
            cout << "▷생일 : " << result->getString("birth") << endl;
            cout << "▷번호 : " << result->getString("phone") << endl;
            cout << "-----------------------------" << endl;
        }
    }
    void updateStatus()
    {
        cout << ">>상태메시지 입력 : ";
        cin.ignore();
        getline(cin, status);
        pstmt = con->prepareStatement("UPDATE user SET status = ? WHERE id = ?");
        pstmt->setString(1, status);
        pstmt->setString(2, id);
        pstmt->executeQuery();
        printf("▶업데이트 되었습니다.\n");
    }
    void updateSong()
    {
        cout << ">>노래 입력 : ";
        cin.ignore();
        getline(cin, song);
        pstmt = con->prepareStatement("UPDATE user SET song = ? WHERE id = ?");
        pstmt->setString(1, song);
        pstmt->setString(2, id);
        pstmt->executeQuery();
        printf("▶업데이트 되었습니다.\n");
    }
    void friends()
    {
        pstmt = con->prepareStatement("SELECT name, status, song, birth, phone FROM user WHERE id != ?;");
        pstmt->setString(1, id);
        result = pstmt->executeQuery();

        while (result->next())
        {
            cout << "-----------------------------" << endl;
            cout << "▷이름 : " << result->getString("name") << endl;
            if (result->getString("status") == "")
            {
                cout << "▷상메 : 없음" << endl;
            }
            else {
                cout << "▷상메 : " << result->getString("status") << endl;
            }
            if (result->getString("song") == "")
            {
                cout << "▷노래 : 없음" << endl;
            }
            else {
                cout << "▷노래 : " << result->getString("status") << endl;
            }
            cout << "▷생일 : " << result->getString("birth") << endl;
            cout << "▷번호 : " << result->getString("phone") << endl;

        }
    }
    void searchMessage()
    {
        string content;
        cout << ">>메시지 검색 : ";
        cin >> content;
        pstmt = con->prepareStatement("SELECT chatname, time, recv FROM chatting\
                               WHERE recv LIKE ?");
        pstmt->setString(1, "%" + content + "%");
        result = pstmt->executeQuery();

        if (!result->next()) {
            cout << "검색 결과가 없습니다." << endl;
        }
        else {
            while (true) {
                string chatname = result->getString(1);
                string time = result->getString(2);
                string recv = result->getString(3);
                cout << "------------------------------------------------------------" << endl;
                cout << "▷보낸 사람 : " << chatname << "\t▷보낸 시간 : " << time << endl;
                cout << "▷" << recv << endl;
                if (!result->next()) break;
            }
        }

    }
    void searchBirth()
    {
        string startDay, endDay;
        cout << "검색 시작월일 4자리를 입력해주세요.(ex.0201) : ";
        cin >> startDay;// 0303
        cout << "검색 종료월일 4자리를 입력해주세요.(ex.0405) : ";
        cin >> endDay; //0505
        pstmt = con->prepareStatement("SELECT name, birth, phone FROM user \
        WHERE DATE_FORMAT(birth, '%m%d') BETWEEN ? AND ? \
        AND id != ?;");
        pstmt->setString(1, startDay);
        pstmt->setString(2, endDay);
        pstmt->setString(3, id);
        result = pstmt->executeQuery();

        if (!result->next()) {
            cout << "검색 결과가 없습니다." << endl;
        }
        else {
            while (true) {
                cout << "-----------------------------------------------" << endl;
                cout << "▷이름 : " << result->getString("name") << endl;
                cout << "▷생일 : " << result->getString("birth") << endl;
                cout << "▷번호 : " << result->getString("phone") << endl;
                if (!result->next()) break; // 더이상 결과가 없으면 while문을 빠져나옵니다.

            }
        }
    }
    void modifyPw()
    {
        cout << ">>기존 비밀번호를 입력해주세요. : ";
        string existPw = "";
        char ch = ' ';
        while (ch != 13) { // Enter 키를 누르면 입력 종료
            ch = _getch();
            if (ch == 13) break; // Enter 키를 누르면 입력 종료
            if (ch == 8) { // Backspace 키인 경우
                if (!existPw.empty()) { // 입력된 문자가 있으면
                    existPw.pop_back(); // 마지막 문자를 삭제
                    cout << "\b \b"; // 커서 위치를 왼쪽으로 이동시켜 공백을 출력한 뒤, 다시 커서 위치를 원래대로 이동시킴
                }
            }
            else {
                existPw.push_back(ch);
                cout << '*'; // 별표로 대체하여 출력
            }
        }
        cout << endl;

        pstmt = con->prepareStatement("SELECT pw FROM user WHERE id=?");
        pstmt->setString(1, id);
        result = pstmt->executeQuery();
        if (result->next())
        { // 쿼리 결과가 있다면
            string db_pw = result->getString(1); // 데이터베이스에서 가져온 pw
            if (db_pw == existPw)
            {
                string new_pw = "";
                cout << ">>새로운 비밀번호를 입력해주세요 : ";
                char ch = ' ';
                while (ch != 13)
                { // Enter 키를 누르면 입력 종료
                    ch = _getch();
                    if (ch == 13) break; // Enter 키를 누르면 입력 종료
                    if (ch == 8) { // Backspace 키인 경우
                        if (!new_pw.empty()) { // 입력된 문자가 있으면
                            new_pw.pop_back(); // 마지막 문자를 삭제
                            cout << "\b \b"; // 커서 위치를 왼쪽으로 이동시켜 공백을 출력한 뒤, 다시 커서 위치를 원래대로 이동시킴
                        }
                    }
                    else {
                        new_pw.push_back(ch);
                        cout << '*'; // 별표로 대체하여 출력
                    }
                }
                cout << endl;
                while (1)
                {
                    string renew_pw = "";
                    cout << endl << ">>다시 한번 입력해주세요. : ";
                    char rech = ' ';
                    while (rech != 13) { // Enter 키를 누르면 입력 종료
                        rech = _getch();

                        if (rech == 13) break; // Enter 키를 누르면 입력 종료
                        if (ch == 8) { // Backspace 키인 경우
                            if (!renew_pw.empty()) { // 입력된 문자가 있으면
                                renew_pw.pop_back(); // 마지막 문자를 삭제
                                cout << "\b \b"; // 커서 위치를 왼쪽으로 이동시켜 공백을 출력한 뒤, 다시 커서 위치를 원래대로 이동시킴
                            }
                        }
                        else {
                            renew_pw.push_back(rech);
                            cout << '*'; // 별표로 대체하여 출력
                        }
                    }
                    cout << endl;

                    if (new_pw == renew_pw)
                    {
                        pstmt = con->prepareStatement("UPDATE user SET pw = ? WHERE id = ?");
                        pstmt->setString(1, new_pw);
                        pstmt->setString(2, id);
                        pstmt->executeQuery();
                        printf("▶새로운 비밀번호 설정이 완료되었습니다.\n");
                        break;
                    }
                    else cout << "▶비밀번호가 다릅니다." << endl;
                }
            }
        }
        else  cout << "▶비밀번호가 다릅니다." << endl;

    }
    void deleteUser()
    {
        cout << ">>기존 비밀번호를 입력해주세요. : ";
        string existPw = "";
        char ch = ' ';
        while (ch != 13) { // Enter 키를 누르면 입력 종료
            ch = _getch();
            if (ch == 13) break; // Enter 키를 누르면 입력 종료
            if (ch == 8) { // Backspace 키인 경우
                if (!existPw.empty()) { // 입력된 문자가 있으면
                    existPw.pop_back(); // 마지막 문자를 삭제
                    cout << "\b \b"; // 커서 위치를 왼쪽으로 이동시켜 공백을 출력한 뒤, 다시 커서 위치를 원래대로 이동시킴
                }
            }
            else {
                existPw.push_back(ch);
                cout << '*'; // 별표로 대체하여 출력
            }
        }
        cout << endl;

        pstmt = con->prepareStatement("SELECT pw FROM user \
            WHERE id=?");
        pstmt->setString(1, id);
        result = pstmt->executeQuery();

        if (result->next())
        { // 쿼리 결과가 있다면
            string db_pw = result->getString(1); // 데이터베이스에서 가져온 PW
            if (db_pw == existPw)
            {
                cout << "정말 삭제하시겠습니까? 삭제한 이후엔 되돌릴 수 없습니다. 1. 계속하기, 2. 그만두기 : ";
                char lastCheck;
                cin >> lastCheck;
                while (1)
                {
                    if (lastCheck == '1')
                    {
                        pstmt = con->prepareStatement("DELETE FROM user WHERE id = ?");
                        pstmt->setString(1, id);
                        result = pstmt->executeQuery();
                        cout << "▶그동안 감사했습니다. 또 이용해주세요." << endl;
                    }
                    else if (lastCheck == '2')
                    {
                        cout << "잘 생각하셨습니다.." << endl;
                        break;
                    }
                    else cout << "▶잘못된 입력입니다." << endl;
                }
            }
            else cout << "▶비밀번호가 다릅니다." << endl;
        }
        else cout << "▶비밀번호가 다릅니다." << endl;
    }
    string getName()
    {
        string getName = "";
        pstmt = con->prepareStatement("SELECT name FROM user \
            WHERE id=?");
        pstmt->setString(1, id);
        result = pstmt->executeQuery();
        if (result->next())
        {
            getName = result->getString(1);
        }
        return getName;
    }
};

int main()
{
    SQL sql;

    //시작화면 구현부
    bool loginSuccess = false;
    while (!loginSuccess)
    {
        startMenu(); //시작 화면
        char startIn=0;
        cout << "▶ ";
        cin >> startIn;
        switch (startIn)
        {
        case '1': //로그인
            login();
            if (sql.login() == 1) { 
                loginSuccess = true;
                    break; }
            continue;
        case '2': //아이디 찾기
            searchId();
            sql.searchId();
            continue;

        case '3': //비밀번호찾기
            searchPw();
            sql.searchPw();
            continue;

        case '4': //회원가입
            createUser();
            sql.crateUser();
            continue;
        case '0':
            return -1;
        default:
            cout << "▶잘못된 입력입니다. 다시 입력해주세요." << endl;
            continue;
        }

    }

    //메인화면 구현부
    while (1)
    {
        mainMenu(); //메인 화면
        int mainIn = 0;
        cout << "▶ ";
        cin >> mainIn;
        //내정보 구현부
        if (mainIn == 1)
        {
            myMenu();
            bool backButton = false;
            while (!backButton)
            {
                char informationIn = 0;
                cout << "▶ ";
                cin >> informationIn;
                switch (informationIn)
                {
                case '1':
                    sql.myProfile();
                    break;
                case '2':
                    sql.updateStatus(); //상메 설정
                    continue;
                case '3':
                    sql.updateSong(); //노래 설정
                    continue;
                case '0':
                    backButton = true;
                    break;
                default:
                    cout << "▶잘못된 입력입니다. 다시 입력해주세요." << endl;
                    continue;
                }
            }
        }
        //친구 구현부
        else if (mainIn == 2)
        {
            friends(); //친구화면
            bool backButton = false;
            while (!backButton)
            {
                char friendsIn = 0;
                cout << "▶ ";
                cin >> friendsIn;
                switch (friendsIn)
                {
                case '1':
                    sql.friends(); //친구 목록
                    continue;
                case '2':
                    sql.searchBirth(); //생일 검색
                    continue;
                case '0':
                    backButton = true;
                    break;
                default:
                    cout << "▶잘못된 입력입니다. 다시 입력해주세요." << endl;
                    continue;
                }
            }
        }
        //채팅방 구현부
        else if (mainIn == 3)
        {
            chatting(); //친구화면
            bool backButton = false;
            int code = 0; // 초기화 진행
            while (!backButton)
            {
                char chattingIn = 0;
                cout << "▶ ";
                cin >> chattingIn;
                switch (chattingIn)
                {
                case '1':
                    WSADATA wsa;
                    code = WSAStartup(MAKEWORD(2, 2), &wsa); // 변수에 초기화 진행
                    if (!code)
                    {
                        cout << "< 채팅방에 입장합니다. >" << endl;
                        my_nick = sql.getName();
                        client_sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);

                        SOCKADDR_IN client_addr = {};
                        client_addr.sin_family = AF_INET;
                        client_addr.sin_port = htons(7777);
                        InetPton(AF_INET, TEXT("127.0.0.1"), &client_addr.sin_addr);

                        while (1)
                        {
                            if (!connect(client_sock, (SOCKADDR*)&client_addr, sizeof(client_addr))) {
                                cout << "※지인을 사칭하여 금전을 요구할 수 있으니, 도용이 의심 된다면 대화를 중단해주시기 바랍니다." << endl;
                                cout << "nick " <<  my_nick;
                                send(client_sock, my_nick.c_str(), my_nick.length(), 0);
                                break;
                            }
                            cout << "Connecting..." << endl;
                        }

                        std::thread th2(chat_recv);

                        while (1)
                        {
                            string text;
                            std::getline(cin, text);
                            const char* buffer = text.c_str(); // string형을 char* 타입으로 변환
                            send(client_sock, buffer, strlen(buffer), 0);
                            if (text == "/exit") {
                                backButton = true;
                                break;
                            }
                        }
                        th2.join();
                        closesocket(client_sock);
                    }
                    WSACleanup();
                    break;
                case '2':
                    sql.searchMessage();
                    continue;
                case '0':
                    backButton = true;
                    break;
                default:
                    cout << "▶잘못된 입력입니다. 다시 입력해주세요." << endl;
                    continue;
                }
            }
        }
        //설정부 구현
        else if (mainIn == 4)
        {
            setting(); //설정화면
            bool backButton = false;
            while (!backButton)
            {
                char settingIn = 0;
                cout << "▶ ";
                cin >> settingIn;
                switch (settingIn)
                {
                case '1':
                    sql.modifyPw(); //비밀번호 변경
                    continue;
                case '2':
                    sql.deleteUser(); //회원 탈퇴
                    backButton = true;
                    return 0;
                case '0':
                    backButton = true;
                    break;
                default:
                    cout << "▶잘못된 입력입니다. 다시 입력해주세요." << endl;
                }
            }
        }
        else cout << "▶잘못된 입력입니다. 다시 입력해주세요." << endl;
    }    
    delete result;
    delete pstmt;
    delete con;
    delete stmt;

    return 0;
}

//<getch함수>
//사용자가 입력한 키보드 문자를 즉시 가져오는 함수입니다.
//이 함수는 키를 누르면 바로 그 값을 반환하며, 입력받은 키를 화면에 출력하지 않습니다.
//비밀번호와 같은 보안성이 필요한 정보를 입력받을 때 사용하면 유용합니다
//ch 변수에 13을 대입한 이유는, 13은 Enter 키를 나타내기 때문입니다.
//while 문은 Enter 키를 누르기 전까지 계속해서 사용자의 입력을 받습니다.
//getch() 함수는 콘솔에서 사용자가 입력한 값을 읽어들입니다.
//사용자가 Enter 키를 누르면, while 문이 종료됩니다.
//
//비밀번호 입력 시, 입력된 문자열을 별표로 대체하기 위해,
//입력된 문자열을 pw 변수에 push_back() 함수로 저장합니다.
//push_back() 함수는 문자열의 맨 뒤에 문자를 추가합니다.
//
//그리고, cout << '' 를 통해, 입력된 문자열을 별표()로 대체하여 출력합니다.
//이렇게 되면, 사용자가 입력한 비밀번호는 별표로 대체되어 보안성이 높아집니다.