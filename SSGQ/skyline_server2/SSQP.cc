#include <bits/stdc++.h>
#include <thread>
#include "Socket.hpp"
#include <ctime>

using namespace std;

int a=1;
int b = 1;
int c = 2;
int l = 32;
int vMax_a = 35535;
int vMax_b = 30000;
int m;
int n;
vector<vector<int>> P_a;
vector<int> q_a;
vector<vector<int>> P_b;
vector<int> q_b;
// std::string ipAddress = "127.0.0.1";

int PartyNum;
Socket::UDP sock;
#define P0 "10.101.170.63" // current ip addr
#define P1 "10.101.171.229" // another computer
#define P2 "10.101.169.115" // another computer

void init_server(){
    try {
        if(PartyNum == 1)
            sock.bind(4500);
        else
            sock.bind(4000);
    }
    catch (Socket::Exception &e) {
        cout << e.what() << endl;
    }
}

void sendMsg(string msg, string IP, int port) {
    try {
        sock.send(IP, port, msg);
//        cout <<"sent"<< msg << endl;
//        sock.close();
    }
    catch (Socket::Exception &e) {
        cout << e.what() << endl;
    }
}

void sendVector(vector<int>& V, string IP, int port) {
    try {
//        Socket::UDP sock;
        sock.send_vector(IP, port, ref(V));
    }
    catch (Socket::Exception &e) {
        cout << e.what() << endl;
    }
}

void sendresult(vector<vector<int>>& sk, string IP, int port){
    try {
        sock.send_matric(IP, port, ref(sk));
    }
    catch (Socket::Exception &e) {
        cout << e.what() << endl;
    }
}

void recVector(vector<int>& V) {
    try {
        V = sock.receive_vector();
    }
    catch (Socket::Exception &e) {
        cout << e.what() << endl;
    }
}

void recMatric(vector<vector<int>>& V) {
    try {
        V = sock.receive_matric();
    }
    catch (Socket::Exception &e) {
        cout << e.what() << endl;
    }
}

void recMessage(string &str) {
    try {
        Socket::Datagram received = sock.receive();
        str = received.data;
    }
    catch (Socket::Exception &e) {
        cout << e.what() << endl;
    }
}

void synchronize(){
    if (PartyNum == 0){
        cout << "--------wait server3--------" << endl;
        string msg;
        recMessage(ref(msg));
        cout << msg << endl;
        sendMsg("SSQP_Server1_Start",P2,7000);
        recMatric(ref(P_a));
        recVector(ref(q_a));
        n = P_a.size();
        m = q_a.size();
        cout << "Server1 Data is Initialization :" << "---n=" << n << "---m=" << m << endl;
    }else{
        cout << "--------wait server3--------" << endl;
        string msg;
        recMessage(ref(msg));
        cout << msg << endl;
        sendMsg("SSQP_Server2_Start",P2,7500);
        recMatric(ref(P_b));
        recVector(ref(q_b));
        n = P_b.size();
        m = q_b.size();
        cout << "Server2 Data is Initialization :" << "---n=" << n << "---m=" << m << endl;
    }
}

void close(){
    sock.close();
}

struct sk_r{
    vector<int> t;
    vector<int> p;
    int sMin;
    sk_r(vector<int>& t_, vector<int>& p_, int smin){
        t = t_;
        p = p_;
        sMin = smin;
    }
};

int* zl2z2(int num){
    int *res = new int[l];
    for(int i = 0; i < l;i++){
        res[i]= num>>i&0x00000001;
    }
    return res;
}

int *split(string msg) {
    int *a = new int[msg.length()];
    for (int i = 0; i < msg.length(); ++i) {
        a[i] = msg[i];
    }
    return a;
}

int secMul( int u1, int v1, int port) {

    // P1 和 P2 分别含有 u1 v1，u2 v2
    int alpha1 = (u1 - a), beta1 = (v1 - b);

    vector<int> num(2);
    vector<int> received(2);
    num[0] = alpha1, num[1] = beta1;
    thread *threads = new thread[2];
    threads[0] = thread(sendVector, ref(num), P0, port);//Port1 == 5500, Port0 == 5000
    threads[1] = thread(recVector,ref(received));
    for (int i = 0; i < 2; i++)
        threads[i].join();
    delete[] threads;

    int alpha2 = received[0], beta2 = received[1];
    // cout << alpha2 << " " << beta2 << endl;
    int alpha = (alpha1+alpha2), beta = (beta1+beta2);
    return (c+a*alpha+b*beta+PartyNum*alpha*beta);
}

/**
 *
 * @param u1 u's share
 * @param v1 v's share
 * @return u>v ? 1'share : 0'share
 */
int secComp( int u1, int v1) {
    int z = v1-u1;
    // convert to bit string 0放置低位 31放置高位（符号位）
    int * res = zl2z2(z);


    int* w = new int[l];
    int* p = new int[l];
    int* q = new int[l];
    int* d = new int[l];
    int* c = new int[l];
    int* e = new int[l];
    for(int i = 0; i < l ;i++){
        w[i]=res[i];
        if (PartyNum == 0){
            p[i]=res[i];
            q[i]=0;
        }else{
            p[i]=0;
            q[i]=res[i];
        }
        d[i]=secMul((p[i]%2+2)%2,(q[i]%2+2)%2, 4000);
    }

    c[0]=d[0];
    for(int i =1; i < l-1;i++){
        d[i]=(((d[i]+PartyNum)%2)+2)%2;
        e[i]=((secMul(w[i],c[i-1], 4000)+PartyNum)%2+2)%2;
        c[i]=((secMul(e[i],d[i], 4000)+PartyNum)%2+2)%2;
    }
    return (w[l-1]+c[l-2]+2)%2;  // convert z2 to zl
}

int secComp(int u, int v, int Port) {
    int a = u - v;
    string msg = to_string(a);
//        sendMsg(msg, P1);//将a0 b0 c0 发送  P1
    string received;

    thread *threads = new thread[2];
    threads[0] = thread(sendMsg, msg, P0, Port);
    threads[1] = thread(recMessage,ref(received));
    for (int i = 0; i < 2; i++)
        threads[i].join();
    delete[] threads;
    int value = stoi(received);
    int c = value + a;
    if(c < 0)
        return PartyNum;
    else
        return 0;
}

vector<vector<int>> secMap(vector<vector<int>>& P, vector<int>& q){
    vector<vector<int>> T(n, vector<int>(m));
    // cout << P.size() << " " << q.size() << endl;
    for(int i = 0; i < n; ++i){
        for(int j = 0; j < m; ++j){
            // int b = secComp(P[i][j], q[j], PartyNum, ref(sock));
            // int b = secComp(P[i][j], q[j], PartyNum);
            // int b_ = PartyNum - b;
            T[i][j] = secMul(P[i][j] - q[j], P[i][j] - q[j], 4000);
        }
    }
    return T;
}

sk_r SecFetch(vector<int>& s, vector<vector<int>>& T, vector<vector<int>>& P){
    int sMin = s[0];
    vector<int> t_ = T[0];
    vector<int> p_ = P[0];
    for(int i = 1; i < s.size(); ++i){
        // int phai = secComp(sMin, s[i], PartyNum, ref(sock));
        int phai = secComp(s[i], sMin, 4000);
        int phai_ = PartyNum - phai;
        sMin = secMul(phai, s[i], 4000) + secMul(phai_, sMin, 4000);
        // cout << "sMin" << endl;
        for(int j = 0; j < t_.size(); ++j){
            t_[j] = secMul(phai, T[i][j], 4000) + secMul(phai_, t_[j], 4000);
            // cout << "t_[j]" << endl;
        }
        for(int j = 0; j < p_.size(); ++j){
            p_[j] = secMul(phai, P[i][j], 4000) + secMul(phai_, p_[j], 4000);
            // cout << "p_[j]" << endl;
        }
    }
    sk_r sk(t_, p_, sMin);
    return sk;
}

vector<int> SecFilt(vector<vector<int>>& T, vector<int>& t, int sMin, vector<int>& sum){
    int flag = 0;
    vector<int> s(sum.size());
    for(int i = 0; i < sum.size(); ++i){
        // int theta = PartyNum - secComp(sum[i], sMin, PartyNum, ref(sock));
        int theta = PartyNum - secComp(sMin, sum[i], 4000);
        int isFirstSky = secMul(PartyNum - flag, theta, 4000);
        flag = isFirstSky + flag - 2 * secMul(isFirstSky, flag, 4000);
        int th = 1;
        vector<int> th_s(m);
        for(int j = 0; j < m; ++j){
            // th_s[j] = PartyNum - secComp(t[j], T[i][j], PartyNum, ref(sock));
            th_s[j] = PartyNum - secComp(T[i][j], t[j], 4000);
        }
        for(int j = 0; j < m; ++j)
            th = secMul(th, th_s[j], 4000);
        // cout << "th = " << th << endl;
        int isDomi = secMul(th, PartyNum - theta, 4000);
        int fi = isFirstSky + isDomi - 2 * secMul(isFirstSky, isDomi, 4000);
        int vMax;
        if(PartyNum == 1)
            vMax = vMax_a;
        else
            vMax = vMax_b;
        s[i] = secMul(fi, vMax, 4000);
        s[i] += secMul(PartyNum - fi, sum[i], 4000);
        // cout << "fi=" << fi << " isDomi=" << isDomi;
    }
    cout << endl;
    return s;
}

vector<vector<int>> SSQP(vector<vector<int>>& P, vector<int>& q){
    int isStop = 0;
    // cout << "begin SecMap" << endl;
    vector<vector<int>> T = secMap(P, q);
    vector<int> sum(n, 0);
    for(int i = 0; i < n; ++i){
        for(int j = 0; j < m; ++j)
            sum[i] += T[i][j];
    }
    cout << "SecMap complete" << endl;
    int vMax;
    if(PartyNum == 1)
        vMax = vMax_a;
    else
        vMax = vMax_b;
    vector<vector<int>> Skr;
    while(!isStop){
        cout << "Begin SecFetch" << endl;
        sk_r sk = SecFetch(sum, T, P);
        cout << "SecFetch complete" << endl;
        sum = SecFilt(T, sk.t, sk.sMin, sum);
        cout << "SecFilt complete" << endl;
        // isStop = PartyNum - secComp(vMax, sk.sMin, PartyNum, ref(sock));
        isStop = PartyNum - secComp(sk.sMin, vMax, 4000);
        string msg = to_string(isStop);
        sendMsg(msg, P0, 4000);
        recMessage(ref(msg));
        isStop += stoi(msg);
        if(!isStop)
            Skr.push_back(sk.p);
        // cout << isStop << endl;
    }
    return Skr;
}

void print_p(vector<int>& p){
    cout << "(";
    for(int i = 0; i < p.size(); ++i){
        cout << p[i];
        if(i != p.size() - 1)
            cout << ",";
    }
    cout << ")";
}

void print_P(vector<vector<int>>& P){
    cout << "{";
    for(int i = 0; i < P.size(); ++i){
        print_p(P[i]);
    }
    cout << "}";
}

void getSockOpt(){
    int rcvbuf_size;
    socklen_t optlen = sizeof(rcvbuf_size);
    if (getsockopt(sock.get_sockId(), SOL_SOCKET, SO_RCVBUF, &rcvbuf_size, &optlen) < 0) {
        // 错误处理
    }
    std::cout << "接收缓冲区大小: " << rcvbuf_size << " 字节" << std::endl;

    int sndbuf_size;
    optlen = sizeof(sndbuf_size);
    if (getsockopt(sock.get_sockId(), SOL_SOCKET, SO_SNDBUF, &sndbuf_size, &optlen) < 0) {
        // 错误处理
    }
    std::cout << "发送缓冲区大小: " << sndbuf_size << " 字节" << std::endl;
}

void setOpt(){
    int buffer_size = 8388608;  // 8MB，您可以根据需要设置更大或更小的值
    if (setsockopt(sock.get_sockId(), SOL_SOCKET, SO_RCVBUF, &buffer_size, sizeof(buffer_size)) < 0) {
        // 错误处理
    }
}

void test_SSQP(){
    synchronize();
    // print_P(P_a);
    cout << P_b.size() << endl;
    cout << "--------------------------------" << endl;
    print_p(q_b);
    cout << endl;
    cout << "--------------------------------" << endl;
    vector<vector<int>> skr = SSQP(P_b, q_b);
    cout << endl;
    sendresult(skr, P2, 7500);
    cout << endl;
}

int main(){
    PartyNum = 1;
    cout << "Party" << PartyNum << endl;
    init_server();
    getSockOpt();
    setOpt();
    getSockOpt();
    test_SSQP();
    close();
    return 0;
}