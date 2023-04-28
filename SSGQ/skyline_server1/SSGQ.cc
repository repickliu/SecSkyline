#include <bits/stdc++.h>
#include "Socket.hpp"
#include <thread>
#include <ctime>

using namespace std;

int a=1;
int b = 1;
int c = 2;
int l = 32;
queue<int> msg_atob;
queue<int> msg_btoa;
queue<vector<int>> msg_atoc;
queue<vector<int>> msg_btoc;
queue<vector<vector<int>>> msg_ctob;
queue<vector<vector<int>>> msg_ctoa;
int k;
int m;
int n;

vector<vector<int>> P_a(n);
vector<int> q_a(m);
vector<vector<int>> P_b(n);
vector<int> q_b(m);

Socket::UDP sock;
Socket::UDP sock1;
Socket::UDP sock2;
#define P0 "10.101.170.63" // current ip addr
#define P1 "10.101.171.229" // another computer
#define P2 "10.101.169.115" // another computer
int PartyNum;


int *split(string msg) {
    int *a = new int[msg.length()];
    for (int i = 0; i < msg.length(); ++i) {
        a[i] = msg[i];
    }
    return a;
}

void init_server(){
    try {
        if(PartyNum == 0)
            sock.bind(5000);
        else
            sock.bind(5500);
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
        sock.send_vector(IP, port, V);
    }
    catch (Socket::Exception &e) {
        cout << e.what() << endl;
    }
}

void sendResult(vector<vector<vector<int>>>& Gsk, string IP, int port){
    try {
        sock.send_matric(IP, port, ref(Gsk));
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
        sendMsg("SSGQ_Server1_Start",P2,6000);
        recMatric(ref(P_a));
        recVector(ref(q_a));
        n = P_a.size();
        m = q_a.size();
        sendMsg("startP1", P0, 5000);
        string msg;
        recMessage(ref(msg));
        cout << msg << endl;
        cout << "Server1 Data is Initialization :" << "k=" << k << "---n=" << n << "---m=" << m << endl;
    }else{
        sendMsg("SSGQ_Server2_Start",P2,6500);
        recMatric(ref(P_b));
        recVector(ref(q_b));
        n = P_b.size();
        m = q_b.size();
        cout << "Server2 Data is Initialization :" << "k=" << k << "---n=" << n << "---m=" << m << endl;
    }
}

void close(){
    sock.close();
}

int secMul( int u1, int v1, int port) {

    // P1 和 P2 分别含有 u1 v1，u2 v2
    int alpha1 = (u1 - a), beta1 = (v1 - b);

    vector<int> num(2);
    vector<int> received(2);
    num[0] = alpha1, num[1] = beta1;
    thread *threads = new thread[2];
    threads[0] = thread(sendVector, ref(num), P1, port);//Port1 == 5500, Port0 == 5000
    threads[1] = thread(recVector,ref(received));
    for (int i = 0; i < 2; i++)
        threads[i].join();
    delete[] threads;

    int alpha2 = received[0], beta2 = received[1];
    // cout << alpha2 << " " << beta2 << endl;
    int alpha = (alpha1+alpha2), beta = (beta1+beta2);
    return (c+a*alpha+b*beta+PartyNum*alpha*beta);
}

struct skyline_r
{
    vector<int> t;
    vector<vector<int>> g;
    skyline_r(vector<int>& t_1, vector<vector<int>>& g_1){
        t = t_1;
        g = g_1;
    }
};

struct new_Tuple
{
    /* data */
    vector<vector<int>> T;
    vector<vector<vector<int>>> G;
    new_Tuple(){}
    new_Tuple(vector<vector<int>>& T_, vector<vector<vector<int>>>& G_){
        T = T_;
        G = G_;
    }
};


int* zl2z2(int num){
    int *res = new int[l];
    for(int i = 0; i < l;i++){
        res[i]= num>>i&0x00000001;
    }
    return res;
}

int secComp(int u, int v, int Port) {
    int a = u - v;
    string msg = to_string(a);
//        sendMsg(msg, P1);//将a0 b0 c0 发送  P1
    string received;

    thread *threads = new thread[2];
    threads[0] = thread(sendMsg, msg, P1, Port);
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
        d[i]=secMul((p[i]%2+2)%2,(q[i]%2+2)%2, 5500);
    }

    c[0]=d[0];
    for(int i =1; i < l-1;i++){
        d[i]=(((d[i]+PartyNum)%2)+2)%2;
        e[i]=((secMul(w[i],c[i-1], 5500)+PartyNum)%2+2)%2;
        c[i]=((secMul(e[i],d[i], 5500)+PartyNum)%2+2)%2;
    }
    return (w[l-1]+c[l-2]+2)%2;  // convert z2 to zl
}

void Cnk(const vector<vector<int>>& P, int n, int k, vector<vector<vector<int>>>& G, vector<vector<int>>& g, int t){
    if(k == 0){
        vector<int> g0(m);
        for(int j = 0; j < m; ++j)
            for(int i = 0; i < g.size(); ++i)
                g0[j] += g[i][j];
        G.push_back(g);
        G.back().push_back(g0);
        return;
    }
    for(int i = t; i < n - k + 1; ++i){
        g.push_back(P[i]);
        Cnk(P, n, k - 1, G, g, i + 1);
        g.pop_back();
    }
}

vector<vector<vector<int>>> SecAgg(const vector<vector<int>>& P){
    vector<vector<vector<int>>> G;
    vector<vector<int>> g;
    Cnk(P, n, k, G, g, 0);
    return G;
}

vector<int> secMap(const vector<vector<int>>& g, const vector<int>& q){
    vector<int> t(q.size() + 1, 0);
    for(int i = 1; i <= q.size(); ++i){
        t[i] = g.back()[i - 1] - k * q[i - 1];
        t[i] = secMul(t[i], t[i], 5500);
        t[0] += t[i];
    }
    return t;
}

int SGD(vector<int>& ta, const vector<int>& tb){
    int m = ta.size();
    vector<int> c(m - 1, 0);
    for(int i = 1; i < m; ++i){
        c[i - 1] = PartyNum - secComp(tb[i], ta[i], 5500);
    }
    int e = c[0];
    for(int i = 1; i < m - 1; ++i)
        e = secMul(e, c[i], 5500);
    int x = ta[0], y = tb[0];
    int d = secComp(x, y, 5500);
    return secMul(e, d, 5500);
}

skyline_r SecGfetch(const vector<vector<int>>& T, const vector<vector<vector<int>>>& G){
    vector<int> ts(T[0]);
    vector<vector<int>> gs(G[0]);
    for(int i = 1; i < T.size() && i < G.size(); ++i){
        int alpha = secComp(T[i][0], ts[0], 5500);
        int alpha_ = PartyNum - alpha;
        for(int j = 0; j < ts.size(); ++j){
            ts[j] = secMul(alpha, T[i][j], 5500) + secMul(alpha_, ts[j], 5500);
        }
        for(int j = 0; j < k; ++j){
            for(int t = 0; t < m; ++t)
                gs[j][t] = secMul(alpha, G[i][j][t], 5500) + secMul(alpha_, gs[j][t], 5500);
        }
    }
    skyline_r sk(ts, gs);
    return sk;
}

vector<vector<int>> K_a;
vector<vector<int>> K_b;

vector<vector<int>> Gen_K(int Port, vector<int>& V){
    sendVector(V, P2, Port);
    vector<vector<int>> K;
    cout << "-----------waite for K-----------" << endl;
    recMatric(ref(K));
    return K;
}

new_Tuple SecFilter(const vector<vector<int>>& T, const vector<vector<vector<int>>>& G, skyline_r& sk){
    vector<vector<int>> T_n;
    vector<vector<vector<int>>> G_n;
    int flag = PartyNum;
    vector<int> V(T.size(), 0);
    int N = T.size();
    vector<int> t_sk = sk.t;
    vector<vector<int>> g_sk = sk.g;
    for(int i = 0; i < N; ++i){
        int theta = PartyNum - secComp(t_sk[0], T[i][0], 5500);
        int isFirstSky = secMul(theta, flag, 5500);
        // flag = secMul(theta, flag, PartyNum);
        flag = secMul(flag, PartyNum - isFirstSky, 5500);
        int alpha = PartyNum - SGD(t_sk, T[i]);
        int alpha_ = alpha + isFirstSky - 2 * secMul(alpha, isFirstSky, 5500);
        V[i] = alpha_;
    }
    cout << "-----------waite for V-----------" << endl;
    vector<vector<int>> K_ = Gen_K(6000, V);
    for(int i = 0; i < K_.size(); ++i){
        vector<vector<int>> T_(N, vector<int>(T[0].size()));
        vector<vector<vector<int>>> G_(G.size(), vector<vector<int>>(G[0].size(), vector<int>(m, 0)));
        for(int j = 0; j < K_[i].size(); ++j){
            int k_ = K_[i][j];
            for(int j_ = 0; j_ < T[0].size(); ++j_){
                // T_[j][j_] = secMul(k_, T[j][j_], PartyNum);
                T_[j][j_] = k_ * T[j][j_];
            }
            for(int j_ = 0; j_ < G[0].size(); ++j_){
                for(int t_ = 0; t_ < m; ++t_){
                    int g = G[j][j_][t_];
                    // G_[j][j_][t_] = secMul(k_, g, PartyNum);
                    G_[j][j_][t_] = k_ * g;
                }
            }
        }
        vector<int> t(T_[0].size(), 0);
        vector<vector<int>> g(G_[0].size(), vector<int>(m, 0));
        for(int j_ = 0; j_ < T_[0].size(); ++j_){
            for(int i_ = 0; i_ < T_.size(); ++i_)
                t[j_] += T_[i_][j_];
        }
        for(int j_ = 0; j_ < G_[0].size(); ++j_){
            for(int i_ = 0; i_ < G_.size(); ++i_){
                for(int t_ = 0; t_ < m; ++t_){
                    g[j_][t_] += G_[i_][j_][t_];
                }
            }
        }
        T_n.push_back(t);
        G_n.push_back(g);
    }
    if(!K_.size()){
        T_n.resize(0);
        G_n.resize(0);
    }
    new_Tuple nt(T_n, G_n);
    // if(T_n.size() != n){
    //         cout << "pause" << endl;
    // } 
    cout << T_n.size() << endl;
    return nt;
}

vector<vector<vector<int>>> SSGQ(const vector<vector<int>>& P, const vector<int>& q){
    vector<vector<vector<int>>> G = SecAgg(P);
    vector<vector<int>> T;
    vector<vector<vector<int>>> Gsk;
    for(int i = 0; i < G.size(); ++i){
        vector<int> t = secMap(G[i], q);
        T.push_back(t);
    }
    bool flag = true;
    if(!T.size())
        flag = false;
    new_Tuple nt;
    while(flag){
        cout << "-----------waite for SecGfetch-----------" << endl;
        skyline_r sk = SecGfetch(T, G);
        Gsk.push_back(sk.g);
        cout << "-----------waite for SecFilter-----------" << endl;
        nt = SecFilter(T, G, sk);
        cout << "-----------for next search-----------" << endl;
        if(!nt.T.size())
            flag = false;
        else{
            T = nt.T;
            G = nt.G;
        }
    }
    return Gsk;
}

void init_data(int m1, int n1, int k1){
    m = m1;
    k = k1;
    n = n1;
    // shareArray(arr, P_a, P_b);
    // vector<int> q = generateRandomArray(m);
    // shareArray(q, q_a, q_b);
    // cout << "m=" << m << " || n=" << n << " || k=" << k << " || time=";
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

void print_G(vector<vector<vector<int>>>& G){
    if(!G.size()){
        cout << "G is null" << endl;
        return;
    }
    cout << "{";
    for(int i = 0; i < G.size(); ++i){
        print_P(G[i]);
    }
    cout << "}";
}

void test1(){
    vector<int> q = {0, 0};
    vector<vector<int>> P = {{2,1}, {2,4}, {1,3}};
    init_data(2, 3, 1);
    synchronize();
    vector<vector<vector<int>>> G = SecAgg(P);
    print_G(G);
    cout << endl;
    vector<vector<int>> T;
    for(int i = 0; i < G.size(); ++i){
        vector<int> t = secMap(G[i], q);
        T.push_back(t);
    }
    print_P(T);
    cout << endl;
    skyline_r sk = SecGfetch(T, G);
    vector<int> ts = sk.t;
    vector<vector<int>> gs = sk.g;
    print_p(ts);
    cout << endl;
    new_Tuple nt = SecFilter(T, G, sk);
    T = nt.T;
    G = nt.G;
    print_G(G);
    cout << endl;
    print_P(T);
    cout << endl;
}

void test2(){
    vector<int> q = {0, 0};
    vector<vector<int>> P = {{-1,0}, {-2,0}, {1,1}};
    print_P(P);
    cout << endl;
    init_data(2, 3, 1);
    synchronize();
    vector<vector<vector<int>>> gsk = SSGQ(P, q);
    print_G(gsk);
    cout << endl;
}

void test_SSGQ(){
    // k = 1;
    synchronize();
    // print_P(P_a);
    cout << P_a.size();
    cout << endl;
    cout << "--------------------------------" << endl;
    print_p(q_a);
    cout << endl;
    cout << "--------------------------------" << endl;
    vector<vector<vector<int>>> gsk = SSGQ(P_a, q_a);
    print_G(gsk);
    cout << endl;
    sendResult(gsk, P2, 6000);
    cout << endl;
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

int main(int argc, char* argv[]){
    k = stoi(string(argv[1]));
    PartyNum = 0;
    cout << "Party" << PartyNum << endl;
    cout << "k = " << k << endl;
    init_server();
    getSockOpt();
    setOpt();
    getSockOpt();
    test_SSGQ();
    close();
    return 0;
}