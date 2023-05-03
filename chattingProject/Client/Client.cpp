#pragma comment(lib, "ws2_32.lib")

#include <WinSock2.h> //Winsock 헤더파일 include. WSADATA 들어있음.ㄴ
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

void mainMenu()
{
    cout << "\n";
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
    cout << "\t"; cout << "*       1.로그인                 * \n";
    cout << "\t"; cout << "*                                * \n";
    cout << "\t"; cout << "*       2. ID 찾기               * \n";
    cout << "\t"; cout << "*                                * \n";
    cout << "\t"; cout << "*       3. PW 찾기               * \n";
    cout << "\t"; cout << "*                                * \n";
    cout << "\t"; cout << "*       4. 회원가입              * \n";
    cout << "\t"; cout << "*                                * \n";
    cout << "\t"; cout << "*       0. 종료                  * \n";
    cout << "\t"; cout << "*                                * \n";
    cout << "\t"; cout << "********************************** \n\n";
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

    void login()
    {
        cout << "id";
        cin >> id;
        this->id = id;
        cout << "pw";
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
                cout << name << "님 환영합니다." << endl;
            }
            else cout << "해당하는 정보가 없습니다." << endl;
        }
        else cout << "해당하는 정보가 없습니다." << endl;

        cout << "로그인 완료." << endl;
    }
    void searchId()
    {
        cout << "name";
        cin >> name;
        cout << "phone";
        cin >> phone;
        cout << "birth";
        cin >> birth;

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
                cout << name << "님의 아이디는 " << db_id << "입니다." << endl;
            }
            else  cout << "해당하는 정보가 없습니다." << endl;
        }
        else cout << "해당하는 정보가 없습니다." << endl;
    }
    void searchPw()
    {
        cout << "id";
        cin >> id;
        cout << "name";
        cin >> name;
        cout << "phone";
        cin >> phone;
        cout << "birth";
        cin >> birth;

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
                cout << "new_pw";
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
                        new_pw.push_back(ch);
                        cout << '*'; // 별표로 대체하여 출력
                    }
                }
                cout << endl;
                string renew_pw = "";
                cout << "renew_pw";
                char rech = ' ';
                while (rech != 13) { // Enter 키를 누르면 입력 종료
                    rech = _getch();
                    if (rech == 13) break; // Enter 키를 누르면 입력 종료
                    if (ch == 8) { // Backspace 키인 경우
                        if (!pw.empty()) { // 입력된 문자가 있으면
                            pw.pop_back(); // 마지막 문자를 삭제
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
                    printf("New PW updated\n");
                }
                else cout << "비밀번호가 다릅니다." << endl;
            }
            else  cout << "해당하는 정보가 없습니다." << endl;
        }
        else cout << "해당하는 정보가 없습니다." << endl;
    }
    void crateUser()
    {
        while (1)
        {
            cout << "id";
            cin >> id;
            pstmt = con->prepareStatement("SELECT id FROM user WHERE id=?");
            pstmt->setString(1, id);
            result = pstmt->executeQuery();

            if (result->next())
            {
                string db_id = result->getString(1);
                if (db_id == id) {
                    cout << "중복된 id가 있습니다." << endl;
                }
                else {
                    cout << "중복체크 완료." << endl;
                }
            }
        }
        while (1)
        {
            cout << "pw";
            char ch = ' ';
            while (ch != 13)
            { // Enter 키를 누르면 입력 종료
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
            string re_pw = "";
            cout << "renew_pw";
            char rech = ' ';
            while (rech != 13)
            { // Enter 키를 누르면 입력 종료
                rech = _getch();
                if (rech == 13) break; // Enter 키를 누르면 입력 종료
                if (ch == 8) { // Backspace 키인 경우
                    if (!re_pw.empty()) { // 입력된 문자가 있으면
                        re_pw.pop_back(); // 마지막 문자를 삭제
                        cout << "\b \b"; // 커서 위치를 왼쪽으로 이동시켜 공백을 출력한 뒤, 다시 커서 위치를 원래대로 이동시킴
                    }
                }
                else {
                    re_pw.push_back(rech);
                    cout << '*'; // 별표로 대체하여 출력
                }
            }
            if (pw == re_pw)
            {
                break;
            }
            else cout << "비밀번호가 다릅니다." << endl;
        }
        cout << "name";
        cin >> name;
        cout << "phone";
        cin >> phone;
        cout << "birth 8자리입력";
        cin >> birth;

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
        cout << "회원가입 완료." << endl;
    }
    void myProfile()
    {
        pstmt = con->prepareStatement("SELECT name, status, song, birth, phone FROM user WHERE id = ?;");
        pstmt->setString(1, id);
        result = pstmt->executeQuery();

        while (result->next())
        {
            cout << "이름 : " << result->getString("name") << endl;
            if (result->getString("status") == "") 
            {
                cout << "상메 : 없음" << endl;
            }
            else {
                cout << "상메 : " << result->getString("status") << endl;
            }
            if (result->getString("song") == "")
            {
                cout << "노래 : 없음" << endl;
            }
            else {
                cout << "노래 : " << result->getString("status") << endl;
            }
            cout << "생일 : " << result->getString("birth") << endl;
            cout << "번호 : " << result->getString("phone") << endl;
        }
    }
    void updateStatus()
    {
        cout << "status";
        cin >> status;
        pstmt = con->prepareStatement("UPDATE user SET status = ? WHERE id = ?");
        pstmt->setString(1, status);
        pstmt->setString(2, id);
        pstmt->executeQuery();
        printf("Status updated\n");
    }
    void updateSong()
    {
        cout << "song";
        cin >> song;
        pstmt = con->prepareStatement("UPDATE user SET song = ? WHERE id = ?");
        pstmt->setString(1, song);
        pstmt->setString(2, id);
        pstmt->executeQuery();
        printf("Song updated\n");
    }
    void friends()
    {
        pstmt = con->prepareStatement("SELECT name, status, song, birth, phone FROM user WHERE id != ?;");
        pstmt->setString(1, id);
        result = pstmt->executeQuery();

        while (result->next())
        {
            cout << "이름 : " << result->getString("name") << endl;
            // status 값이 NULL인 경우 "없음"으로 출력
            if (result->wasNull()) {
                cout << "상메 : 없음" << endl;
            }
            else {
                cout << "상메 : " << result->getString("status") << endl;
            }
            // song 값이 NULL인 경우 "없음"으로 출력
            if (result->wasNull()) {
                cout << "음악 : 없음" << endl;
            }
            else {
                cout << "음악 : " << result->getString("song") << endl;
            }
            cout << "생일 : " << result->getString("birth") << endl;
            cout << "번호 : " << result->getString("phone") << endl;
        }
    }
    void searchBirth()
    {
        string startDay, endDay;
        cout << "startDay";
        cin >> startDay;// 0303
        cout << "endDay";//0505
        cin >> endDay;
        pstmt = con->prepareStatement("SELECT name, birth FROM user \
            WHERE DATE_FORMAT(birth, '%m%d') BETWEEN ? AND ? \
            AND id != ?;");
        pstmt->setString(1, startDay);
        pstmt->setString(2, endDay);
        pstmt->setString(3, id);
        result = pstmt->executeQuery();

        while (result->next())
        {
            cout << "이름 : " << result->getString("name") << endl;
            cout << "생일 : " << result->getString("birth") << endl;
        }
    }
    void modifyPw()
    {
        cout << "기존 pw";
        string in_pw = "";
        char ch = ' ';
        while (ch != 13) { // Enter 키를 누르면 입력 종료
            ch = _getch();
            if (ch == 13) break; // Enter 키를 누르면 입력 종료
            in_pw.push_back(ch);
            cout << '*'; // 별표로 대체하여 출력
        }
        cout << endl;

        pstmt = con->prepareStatement("SELECT pw FROM user WHERE id=?");
        pstmt->setString(1, id);
        result = pstmt->executeQuery();
        if (result->next())
        { // 쿼리 결과가 있다면
            string db_pw = result->getString(1); // 데이터베이스에서 가져온 pw
            if (db_pw == in_pw)
            {
                string new_pw = "";
                cout << "new_pw";
                char ch = ' ';
                while (ch != 13)
                { // Enter 키를 누르면 입력 종료
                    ch = _getch();
                    if (ch == 13) break; // Enter 키를 누르면 입력 종료
                    if (ch == 8) { // Backspace 키인 경우
                        if (!pw.empty()) { // 입력된 문자가 있으면
                            pw.pop_back(); // 마지막 문자를 삭제
                            cout << "\b \b"; // 커서 위치를 왼쪽으로 이동시켜 공백을 출력한 뒤, 다시 커서 위치를 원래대로 이동시킴
                        }
                    }
                    else {
                        new_pw.push_back(ch);
                        cout << '*'; // 별표로 대체하여 출력
                    }
                }
                string renew_pw = "";
                cout << "renew_pw";
                char rech = ' ';
                while (rech != 13)
                { // Enter 키를 누르면 입력 종료
                    rech = _getch();
                    if (rech == 13) break; // Enter 키를 누르면 입력 종료
                    if (ch == 8) { // Backspace 키인 경우
                        if (!pw.empty()) { // 입력된 문자가 있으면
                            pw.pop_back(); // 마지막 문자를 삭제
                            cout << "\b \b"; // 커서 위치를 왼쪽으로 이동시켜 공백을 출력한 뒤, 다시 커서 위치를 원래대로 이동시킴
                        }
                    }
                    else {
                        renew_pw.push_back(rech);
                        cout << '*'; // 별표로 대체하여 출력
                    }
                }
                if (new_pw == renew_pw)
                {
                    pstmt = con->prepareStatement("UPDATE user SET pw = ? WHERE id = ?");
                    pstmt->setString(1, new_pw);
                    pstmt->setString(2, id);
                    pstmt->executeQuery();
                    printf("New PW updated\n");
                }
                else cout << "비밀번호가 다릅니다." << endl;
            }
        }
    }
    void deleteUser()
    {
        string pw = "";
        cout << "기존 pw";
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

        pstmt = con->prepareStatement("SELECT pw FROM user \
            WHERE id=?");
        pstmt->setString(1, id);
        result = pstmt->executeQuery();

        if (result->next())
        { // 쿼리 결과가 있다면
            string db_pw = result->getString(1); // 데이터베이스에서 가져온 PW

            if (db_pw == pw)
            {
                cout << "정말 삭제하시겠습니까? 삭제한 이후엔 되돌릴 수 없습니다. 계속하기:1, 그만두기:2";
                char check;
                cin >> check;
                while (1)
                {
                    if (check == '1')
                    {
                        pstmt = con->prepareStatement("DELETE FROM user WHERE id = ?");
                        pstmt->setString(1, id);
                        result = pstmt->executeQuery();
                        cout << "삭제완료." << endl;
                    }
                    else if (check == '2') cout << "돌아갑니다." << endl;
                    else cout << "잘못된 입력입니다." << endl;
                }
            }
            else cout << "비밀번호 불일치." << endl;
        }
        else cout << "해당하는 정보가 없습니다." << endl;
    }
};

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

int main()
{
    mainMenu();
    SQL sql;
    sql.login();
    //sql.searchId();
    sql.searchPw();
    //sql.crateUser();//회원 가입
    sql.myProfile(); //내 정보
    //sql.updateStatus(); //상메 설정
    //sql.updateSong(); //노래 설정
    //sql.friends(); //내 친구
    //sql.searchBirth(); //생일 검색
    //sql.modifyPw(); //비번 변경
    //sql.deleteUser(); //회원 탈퇴
    //system("cls");
    //system("pause");

    WSADATA wsa;
    int code = WSAStartup(MAKEWORD(2, 2), &wsa);

    if (!code) {
        cout << "login on";
        client_sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);

        SOCKADDR_IN client_addr = {};
        client_addr.sin_family = AF_INET;
        client_addr.sin_port = htons(7777);
        InetPton(AF_INET, TEXT("127.0.0.1"), &client_addr.sin_addr);

        while (1) {
            if (!connect(client_sock, (SOCKADDR*)&client_addr, sizeof(client_addr))) {
                cout << "Server Connect" << endl;
                send(client_sock, my_nick.c_str(), my_nick.length(), 0);
                break;
            }
            cout << "Connecting..." << endl;
        }

        std::thread th2(chat_recv);

        while (1) {
            string text;
            std::getline(cin, text);
            const char* buffer = text.c_str(); // string형을 char* 타입으로 변환
            send(client_sock, buffer, strlen(buffer), 0);
        }
        th2.join();
        closesocket(client_sock);
    }

    WSACleanup();

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