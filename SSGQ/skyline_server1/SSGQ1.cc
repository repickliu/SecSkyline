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

Socket::UDP sock1;
Socket::UDP sock2;
#define P0 "10.0.4.15" // current ip addr
#define P1 "10.0.4.15" // another computer

int *split(string msg) {
    int *a = new int[msg.length()];
    for (int i = 0; i < msg.length(); ++i) {
        a[i] = msg[i];
    }
    return a;
}

void init_server(){
    try {
        sock1.bind(5500);
        sock2.bind(5000);
    }
    catch (Socket::Exception &e) {
        cout << e.what() << endl;
    }
}

void sendMsg(string msg, string IP, int PartyNum, Socket::UDP& sock) {
    try {
//        Socket::UDP sock;
        int port;
        if(PartyNum == 1)
            port = 5000;
        else
            port = 5500;
        sock.send(IP, port, msg);
//        cout <<"sent"<< msg << endl;
//        sock.close();
    }
    catch (Socket::Exception &e) {
        cout << e.what() << endl;
    }
}

void recMessage(string &str, Socket::UDP& sock) {
    try {
        Socket::Datagram received = sock.receive();
        str = received.data;
    }
    catch (Socket::Exception &e) {
        cout << e.what() << endl;
    }
}

void synchronize(int PartyNum, Socket::UDP& sock){
    string received;
    if (PartyNum == 1){
        recMessage(ref(received), ref(sock));
        // cout << "recive msg from " << sock.get_port() << endl;
        sendMsg("start", P1, PartyNum, ref(sock));
    }else{
        sendMsg("start", P1, PartyNum, ref(sock));
        recMessage(ref(received), ref(sock));
        // cout << "recive msg from " << sock.get_port() << endl;
    }
}

void close(){
    sock1.close();
    sock2.close();
}

int secMul( int u1, int v1, int PartyNum, Socket::UDP& sock) {

    // P1 和 P2 分别含有 u1 v1，u2 v2
    int alpha1 = (u1 - a), beta1 = (v1 - b);

    // 发送alpha1 beta1
        char msg[2];
        msg[0] = alpha1;
        msg[1] = beta1;
//        sendMsg(msg, P1);//将a0 b0 c0 发送  P1
    string received;

    thread *threads = new thread[2];
    threads[0] = thread(sendMsg, msg, P1, PartyNum, ref(sock));
    threads[1] = thread(recMessage,ref(received), ref(sock));
    for (int i = 0; i < 2; i++)
        threads[i].join();
    delete[] threads;

    int * share = split(received);
    int alpha2 = share[0], beta2 = share[1];
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

int secMul( int u1, int v1, int PartyNum) {
    // P1 和 P2 分别含有 u1 v1，u2 v2
    int alpha1 = (u1 - a), beta1 = (v1 - b);
    int alpha2, beta2;

    if(PartyNum == 1){
        msg_atob.push(alpha1);
        msg_atob.push(beta1);
        while(msg_btoa.empty());
        alpha2 = msg_btoa.front();
        msg_btoa.pop();
        while(msg_btoa.empty());
        beta2 = msg_btoa.front();
        msg_btoa.pop();
    }else{
        msg_btoa.push(alpha1);
        msg_btoa.push(beta1);
        while(msg_atob.empty());
        alpha2 = msg_atob.front();
        msg_atob.pop();
        while(msg_atob.empty());
        beta2 = msg_atob.front();
        msg_atob.pop();
    }
    int alpha = (alpha1+alpha2), beta = (beta1+beta2);
    return (c+b*alpha+a*beta+PartyNum*alpha*beta);
    //endregion
}

int secComp(int u1, int v1, int PartyNum) {
    int u, v;
    if(PartyNum == 1){
        msg_atob.push(u1);
        msg_atob.push(v1);
        while(msg_btoa.empty());
        u = msg_btoa.front();
        msg_btoa.pop();
        while(msg_btoa.empty());
        v = msg_btoa.front();
        msg_btoa.pop();
    }else{
        msg_btoa.push(u1);
        msg_btoa.push(v1);
        while(msg_atob.empty());
        u = msg_atob.front();
        msg_atob.pop();
        while(msg_atob.empty());
        v = msg_atob.front();
        msg_atob.pop();
    }
    u = u + u1;
    v = v + v1;
    int z;
    if(u < v)
        return PartyNum;
    else
        return 0;
}

int secComp( int u1, int v1, int PartyNum, Socket::UDP& sock) {
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
        d[i]=secMul((p[i]%2+2)%2,(q[i]%2+2)%2, PartyNum, ref(sock));
    }

    c[0]=d[0];
    for(int i =1; i < l-1;i++){
        d[i]=(((d[i]+PartyNum)%2)+2)%2;
        e[i]=((secMul(w[i],c[i-1], PartyNum, ref(sock))+PartyNum)%2+2)%2;
        c[i]=((secMul(e[i],d[i], PartyNum, ref(sock))+PartyNum)%2+2)%2;
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

vector<int> secMap(const vector<vector<int>>& g, const vector<int>& q, int PartyNum, Socket::UDP& sock){
    vector<int> t(q.size() + 1, 0);
    for(int i = 1; i <= q.size(); ++i){
        t[i] = g.back()[i - 1] - k * q[i - 1];
        t[i] = secMul(t[i], t[i], PartyNum, ref(sock));
        t[0] += t[i];
    }
    return t;
}

int SGD(vector<int>& ta, const vector<int>& tb, int PartyNum, Socket::UDP& sock){
    int m = ta.size();
    vector<int> c(m - 1, 0);
    for(int i = 1; i < m; ++i){
        c[i - 1] = PartyNum - secComp(tb[i], ta[i], PartyNum);
    }
    int e = c[0];
    for(int i = 1; i < m - 1; ++i)
        e = secMul(e, c[i], PartyNum, ref(sock));
    int x = ta[0], y = tb[0];
    int d = secComp(x, y, PartyNum);
    return secMul(e, d, PartyNum);
}

skyline_r SecGfetch(const vector<vector<int>>& T, const vector<vector<vector<int>>>& G, int PartyNum, Socket::UDP& sock){
    vector<int> ts(T[0]);
    vector<vector<int>> gs(G[0]);
    for(int i = 1; i < T.size() && i < G.size(); ++i){
        int alpha = secComp(T[i][0], ts[0], PartyNum);
        int alpha_ = PartyNum - alpha;
        for(int j = 0; j < ts.size(); ++j){
            ts[j] = secMul(alpha, T[i][j], PartyNum) + secMul(alpha_, ts[j], PartyNum);
        }
        for(int j = 0; j < k; ++j){
            for(int t = 0; t < m; ++t)
                gs[j][t] = secMul(alpha, G[i][j][t], PartyNum) + secMul(alpha_, gs[j][t], PartyNum, ref(sock));
        }
    }
    skyline_r sk(ts, gs);
    return sk;
}

vector<vector<int>> K_a;
vector<vector<int>> K_b;

void Gen_K(int PartyNum){
     while(msg_atoc.empty());
    while(msg_btoc.empty());
    vector<int> Va = msg_atoc.back();
    vector<int> Vb = msg_btoc.back();
    vector<int> V(Va.size());
    int t = 0;
    for(int i = 0; i < Va.size(); ++i){
        V[i] = Va[i] + Vb[i];
        if(V[i] == 1)
            t++;
    }
    vector<vector<int>> K(t, vector<int>(V.size(), 0));
    int j = 0;
    for(int i = 0; i < V.size() && j < t; ++i){
        if(V[i] == 1){
            K[j++][i] = 1;
        }
    }
    if(PartyNum == 1){
        K_a.clear();
        K_a.swap(K);
    }
    else{
        K_b.clear();
        K_b.swap(K);
    }
}

new_Tuple SecFilter(const vector<vector<int>>& T, const vector<vector<vector<int>>>& G, int PartyNum, skyline_r sk, Socket::UDP& sock){
    vector<vector<int>> T_n;
    vector<vector<vector<int>>> G_n;
    int flag = PartyNum;
    vector<int> V(T.size(), 0);
    int N = T.size();
    vector<int> t_sk = sk.t;
    vector<vector<int>> g_sk = sk.g;
    for(int i = 0; i < N; ++i){
        int theta = PartyNum - secComp(t_sk[0], T[i][0], PartyNum);
        int isFirstSky = secMul(theta, flag, PartyNum);
        // flag = secMul(theta, flag, PartyNum);
        flag = secMul(flag, PartyNum - isFirstSky, PartyNum);
        int alpha = PartyNum - SGD(t_sk, T[i], PartyNum, ref(sock));
        int alpha_ = alpha + isFirstSky - 2 * secMul(alpha, isFirstSky, PartyNum);
        V[i] = alpha_;
    }
    if(PartyNum == 1){
        msg_atoc.push(V);
    }else
        msg_btoc.push(V);
    Gen_K(PartyNum);
    vector<vector<int>> K_;
    if(PartyNum == 1)
        K_ = K_a;
    else
        K_ = K_b;
    for(int i = 0; i < K_.size(); ++i){
        vector<vector<int>> T_(N, vector<int>(T[0].size()));
        vector<vector<vector<int>>> G_(G.size(), vector<vector<int>>(G[0].size(), vector<int>(m, 0)));
        for(int j = 0; j < K_[i].size(); ++j){
            int k_ = K_[i][j];
            for(int j_ = 0; j_ < T[0].size(); ++j_){
                T_[j][j_] = secMul(k_, T[j][j_], PartyNum);
                // T_[j][j_] = k_ * T[j][j_];
            }
            for(int j_ = 0; j_ < G[0].size(); ++j_){
                for(int t_ = 0; t_ < m; ++t_){
                    int g = G[j][j_][t_];
                    G_[j][j_][t_] = secMul(k_, g, PartyNum);
                    // G_[j][j_][t_] = k_ * g;
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
    if(PartyNum == 1)
        msg_atoc.pop();
    else
        msg_btoc.pop();
    if(!K_.size()){
        T_n.resize(0);
        G_n.resize(0);
    }
    new_Tuple nt(T_n, G_n);
    // if(T_n.size() != n){
    //         cout << "pause" << endl;
    // } 
    return nt;
}

void SSGQ(const vector<vector<int>>& P, const vector<int>& q, int PartyNum, Socket::UDP& sock){
    vector<vector<vector<int>>> G = SecAgg(P);
    vector<vector<int>> T;
    vector<vector<vector<int>>> Gsk;
    for(int i = 0; i < G.size(); ++i){
        vector<int> t = secMap(G[i], q, PartyNum, ref(sock));
        T.push_back(t);
    }
    bool flag = true;
    if(!T.size())
        flag = false;
    new_Tuple nt;
    while(flag){
        skyline_r sk = SecGfetch(T, G, PartyNum, ref(sock));
        Gsk.push_back(sk.g);
        nt = SecFilter(T, G, PartyNum, sk, ref(sock));
        vector<vector<int>>().swap(T);
        vector<vector<vector<int>>>().swap(G);
        T = nt.T;
        G = nt.G;
        // cout << T.size() << endl;
        if(!T.size())
            flag = false;
    }
}

void server1()
{
    int PartyNum = 1;
    synchronize(PartyNum, ref(sock1));
    // clock_t startTime,endTime;
    // startTime = clock();
    SSGQ(P_a, q_a, PartyNum, ref(sock1));
    // endTime = clock();
}

void server0()
{
    int PartyNum = 0;
    synchronize(PartyNum, ref(sock2));
    clock_t startTime,endTime;
    startTime = clock();
    SSGQ(P_b, q_b, PartyNum, ref(sock2));
    endTime = clock();
    double duration = 1000.0 * (endTime - startTime) / CLOCKS_PER_SEC;
    cout << duration << endl;
}

vector<vector<int>> generateIndependent(int n, int m) {
    srand(time(0)); // 初始化随机数种子

    vector<vector<int>> data(n, vector<int>(m)); // 创建动态二维数组

    // 生成随机数填充数据集
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < m; j++) {
            data[i][j] = rand() % 100; // 生成0到99之间的随机整数
        }
    }

    return data;
}

// 生成相关数据集
vector<vector<int>> generateCorrelated(int n, int m) {
    srand(time(0)); // 初始化随机数种子

    vector<vector<int>> data(n, vector<int>(m)); // 创建动态二维数组

    // 生成随机数填充数据集
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < m; j++) {
            data[i][j] = rand() % 100; // 生成0到99之间的随机整数
        }
        for (int j = 1; j < m; j++) {
            data[i][j] += data[i][j - 1] + rand() % 10; // y与x具有正相关关系
        }
    }

    return data;
}

// 生成反相关数据集
vector<vector<int>> generateAntiCorrelated(int n, int m) {
    srand(time(0)); // 初始化随机数种子

    vector<vector<int>> data(n, vector<int>(m)); // 创建动态二维数组

    // 生成随机数填充数据集
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < m; j++) {
            data[i][j] = rand() % 100; // 生成0到99之间的随机整数
        }
        for (int j = 1; j < m; j++) {
            data[i][j] -= data[i][j - 1] + rand() % 10; // y与x具有负相关关系
        }
    }

    return data;
}

vector<vector<int>> generateRandomArray(int n, int m) {
    srand(time(0)); // 初始化随机数种子

    vector<vector<int>> arr(n, vector<int>(m)); // 创建动态二维数组

    // 生成随机数填充数组
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < m; j++) {
            arr[i][j] = rand() % 100; // 生成0到99之间的随机整数
        }
    }

    return arr;
}

vector<int> generateRandomArray(int m) {
    srand(time(0) * clock()); // 初始化随机数种子

    vector<int> arr(m); // 创建动态二维数组

    // 生成随机数填充数组
    for (int j = 0; j < m; j++) {
        arr[j] = rand() % 100; // 生成0到99之间的随机整数
    }

    return arr;
}

// 将一个n*m维度数组采用2-out-of-2的加法秘密共享来分解为两个分享值
void shareArray(vector<vector<int>> arr, vector<vector<int>>& share1, vector<vector<int>>& share2) {
    int n = arr.size();
    int m = arr[0].size();

    // 随机生成n*m个分享值，填充到share1和share2中
    P_a = generateRandomArray(n, m);
    P_b = generateRandomArray(n, m);

    // 计算两个分享值的和，结果存储到share1中
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < m; j++) {
            P_a[i][j] = arr[i][j] - P_b[i][j];
        }
    }
}

void shareArray(vector<int> arr, vector<int>& share1, vector<int>& share2) {
    int m = arr.size();

    // 随机生成n*m个分享值，填充到share1和share2中
    share1 = generateRandomArray(m);
    share2 = generateRandomArray(m);

    // 计算两个分享值的和，结果存储到share1中
    for (int j = 0; j < m; j++) {
        share1[j] = arr[j] - share2[j];
    }

}

void init_data(){
    m = 2;
    k = 1;
    n = 1000;
    vector<vector<int>> arr = generateCorrelated(n, m);
    shareArray(arr, P_a, P_b);
    vector<int> q = generateRandomArray(m);
    shareArray(q, q_a, q_b);
    cout << "m=" << m << " || n=" << n << " || k=" << k << " || time=";
}

int main(){
    init_server();
    init_data();
    thread th1(server1);
    thread th2(server0);
    th1.join();
    th2.join();
    // cout << z1 << " " << z2 << endl;
    return 0;
}