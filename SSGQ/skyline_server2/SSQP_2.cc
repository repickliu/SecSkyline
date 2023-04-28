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
queue<int> msg_atob;
queue<int> msg_btoa;
queue<int> msg_atoc;
queue<int> msg_btoc;
queue<int> msg_ctob;
queue<int> msg_ctoa;
vector<int> V_a;
vector<int> V_b;
vector<vector<int>> K_;
int m;
int n;
int is_stop_a;
int is_stop_b;
vector<vector<int>> P_a;
vector<int> q_a;
vector<vector<int>> P_b;
vector<int> q_b;
std::string ipAddress = "127.0.0.1";

Socket::UDP sock1;
Socket::UDP sock2;
#define P0 "10.0.4.15" // current ip addr
#define P1 "10.0.4.15" // another computer

void init_server(){
    try {
        sock1.bind(6500);
        sock2.bind(6000);
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
            port = 6000;
        else
            port = 6500;
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

/**
 *
 * @param u1 u's share
 * @param v1 v's share
 * @return u>v ? 1'share : 0'share
 */
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

vector<vector<int>> secMap(vector<vector<int>>& P, vector<int>& q, int PartyNum, Socket::UDP& sock){
    vector<vector<int>> T(n, vector<int>(m));
    for(int i = 0; i < n; ++i){
        for(int j = 0; j < m; ++j){
            // int b = secComp(P[i][j], q[j], PartyNum, ref(sock));
            // int b = secComp(P[i][j], q[j], PartyNum);
            // int b_ = PartyNum - b;
            T[i][j] = secMul(P[i][j] - q[j], P[i][j] - q[j], PartyNum, ref(sock));
        }
    }
    return T;
}

sk_r SecFetch(vector<int>& s, vector<vector<int>>& T, vector<vector<int>>& P, int PartyNum, Socket::UDP& sock){
    int sMin = s[0];
    vector<int> t_ = T[0];
    vector<int> p_ = P[0];
    for(int i = 1; i < s.size(); ++i){
        // int phai = secComp(sMin, s[i], PartyNum, ref(sock));
        int phai = secComp(s[i], sMin, PartyNum);
        int phai_ = PartyNum - phai;
        sMin = secMul(phai, s[i], PartyNum, ref(sock)) + secMul(phai_, sMin, PartyNum, ref(sock));
        // cout << "sMin" << endl;
        for(int j = 0; j < t_.size(); ++j){
            t_[j] = secMul(phai, T[i][j],PartyNum, ref(sock)) + secMul(phai_, t_[j], PartyNum, ref(sock));
            // cout << "t_[j]" << endl;
        }
        for(int j = 0; j < p_.size(); ++j){
            p_[j] = secMul(phai, P[i][j], PartyNum, ref(sock)) + secMul(phai_, p_[j], PartyNum, ref(sock));
            // cout << "p_[j]" << endl;
        }
    }
    sk_r sk(t_, p_, sMin);
    return sk;
}

vector<int> SecFilt(vector<vector<int>>& T, vector<int>& t, int sMin, vector<int>& sum, int PartyNum, Socket::UDP& sock){
    int flag = 0;
    vector<int> s(sum.size());
    for(int i = 0; i < sum.size(); ++i){
        // int theta = PartyNum - secComp(sum[i], sMin, PartyNum, ref(sock));
        int theta = PartyNum - secComp(sMin, sum[i], PartyNum);
        int isFirstSky = secMul(PartyNum - flag, theta, PartyNum, ref(sock));
        flag = isFirstSky + flag - 2 * secMul(isFirstSky, flag, PartyNum, ref(sock));
        int th = 1;
        vector<int> th_s(m);
        for(int j = 0; j < m; ++j){
            // th_s[j] = PartyNum - secComp(t[j], T[i][j], PartyNum, ref(sock));
            th_s[j] = PartyNum - secComp(T[i][j], t[j], PartyNum);
        }
        for(int j = 0; j < m; ++j)
            th = secMul(th, th_s[j], PartyNum, ref(sock));
        int isDomi = secMul(th, PartyNum - theta, PartyNum, ref(sock));
        int fi = isFirstSky + isDomi - 2 * secMul(isFirstSky, isDomi, PartyNum, ref(sock));
        int vMax;
        if(PartyNum == 1)
            vMax = vMax_a;
        else
            vMax = vMax_b;
        s[i] = secMul(fi, vMax, PartyNum, ref(sock));
        s[i] += secMul(PartyNum - fi, sum[i], PartyNum, ref(sock));
    }
    return s;
}

void SSQP(vector<vector<int>>& P, vector<int>& q, int PartyNum, Socket::UDP& sock){
    int isStop = 0;
    vector<vector<int>> T = secMap(P, q, PartyNum, ref(sock));
    vector<int> sum(n, 0);
    for(int i = 0; i < n; ++i){
        for(int j = 0; j < m; ++j)
            sum[i] += T[i][j];
    }
    int vMax;
    if(PartyNum == 1)
        vMax = vMax_a;
    else
        vMax = vMax_b;
    while(!isStop){
        sk_r sk = SecFetch(sum, T, P, PartyNum, ref(sock));
        // cout << "SecFetch complete" << endl;
        sum = SecFilt(T, sk.t, sk.sMin, sum, PartyNum, ref(sock));
        // cout << "SecFilt complete" << endl;
        // isStop = PartyNum - secComp(vMax, sk.sMin, PartyNum, ref(sock));
        isStop = PartyNum - secComp(sk.sMin, vMax, PartyNum);
        if(PartyNum == 1){
            msg_atob.push(isStop);
            while(msg_btoa.empty()){
                // cout << "waite btoa" << endl;
            }
            isStop += msg_btoa.front();
            msg_btoa.pop();
        }
        else{
            msg_btoa.push(isStop);
            while(msg_atob.empty()){
                // cout << "waite atob" << endl;
            }
            isStop += msg_atob.front();
            msg_atob.pop();
        }
        // cout << isStop << endl;
    }
}

void server1()
{
    int PartyNum = 1;
    synchronize(PartyNum, ref(sock1));
    clock_t startTime,endTime;
    startTime = clock();
    SSQP(P_a, q_a, PartyNum, ref(sock1));
    // int t = 1000;
    // while(t--){
    //     secMul(1, 1, PartyNum, ref(sock1));
    // }
    endTime = clock();
    double duration = 1000.0 * (endTime - startTime) / CLOCKS_PER_SEC;
    cout << duration << endl;
}

void server0()
{
    int PartyNum = 0;
    synchronize(PartyNum, ref(sock2));
    // clock_t startTime,endTime;
    // startTime = clock();
    SSQP(P_b, q_b, PartyNum, ref(sock2));
    // endTime = clock();
    // cout << endTime << endl;
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
    srand((unsigned)time(NULL)); // 初始化随机数种子

    vector<vector<int>> arr(n, vector<int>(m)); // 创建动态二维数组

    // 生成随机数填充数组
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < m; j++) {
            arr[i][j] = rand() * rand() % 100; // 生成0到99之间的随机整数
        }
    }

    return arr;
}

vector<int> generateRandomArray(int m) {
    srand(time(0) * clock()); // 初始化随机数种子

    vector<int> arr(m); // 创建动态二维数组

    // 生成随机数填充数组
    for (int j = 0; j < m; j++) {
        arr[j] = rand()*(j + rand()) % 100; // 生成0到99之间的随机整数
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
    n = 9000;
    m = 2;
    vector<vector<int>> arr = generateIndependent(n, m);
    shareArray(arr, P_a, P_b);
    vector<int> q = generateRandomArray(m);
    shareArray(q, q_a, q_b);
    cout << "m=" << m << " || n=" << n <<" || time=";
}

int main(){
    init_data();
    init_server();
    thread th1(server1);
    thread th2(server0);
    th1.join();
    th2.join();
    return 0;
}