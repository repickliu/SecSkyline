#include <iostream>
#include <cstring>
#include "Socket.hpp"
#include <thread>
#include <chrono>
#include <bits/stdc++.h>

using namespace std;
//#define party 3
#define P0 "10.101.170.63" 
#define P1 "10.101.171.229" 

Socket::UDP sock0;
Socket::UDP sock1;
Socket::UDP sock2;
Socket::UDP sock3;
Socket::UDP sock4;
int l = 32;
// triplets are generated previous,so fixed it
int a = 1;
int b = 1;
int c = 2;
int c1 = a & b;
int PartyNum;
int PartyA =1;
int PartyB =0;
int cr = 5;
int m,n,k;

vector<vector<int>> P_a;
vector<int> q_a = {0,0};
vector<vector<int>> P_b;
vector<int> q_b = {0,0};

void sendMsg(string msg, string IP, Socket::UDP& sock, int port) {
    try {
        sock.send(IP, port, msg);
    }
    catch (Socket::Exception &e) {
        cout << e.what() << endl;
    }
}

void sendMatric(vector<vector<int>>& V, string IP, Socket::UDP& sock, int port) {
    try {
//        Socket::UDP sock;
        sock.send_matric(IP, port, V);
    }
    catch (Socket::Exception &e) {
        cout << e.what() << endl;
    }
}

void sendVector(vector<int>& V, string IP, int port, Socket::UDP& sock) {
    try {
//        Socket::UDP sock;
        sock.send_vector(IP, port, V);
    }
    catch (Socket::Exception &e) {
        cout << e.what() << endl;
    }
}

void init(){
    try {
            //port of SSGQ
            sock1.bind(6000);
            sock2.bind(6500);
            //port of SSQP
            sock3.bind(7000);
            sock4.bind(7500);
        }
    catch (Socket::Exception &e) {
        cout << e.what() << endl;
    }
}

void close(){
    sock0.close();
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

void recVector(vector<int>& V, Socket::UDP& sock) {
    try {
        V = sock.receive_vector();
    }
    catch (Socket::Exception &e) {
        cout << e.what() << endl;
    }
}

void recResult(vector<vector<vector<int>>>& Gsk, Socket::UDP& sock) {
    try {
        Gsk = sock.receive_Result();
    }
    catch (Socket::Exception &e) {
        cout << e.what() << endl;
    }
}

void recResult(vector<vector<int>>& sk, Socket::UDP& sock) {
    try {
        sk = sock.receive_matric();
    }
    catch (Socket::Exception &e) {
        cout << e.what() << endl;
    }
}

// 先运行partyA=1的机器
void synchronize_SSGQ(){
    string received1;
    string received2;
    recMessage(ref(received1), ref(sock1));
    recMessage(ref(received2), ref(sock2));
    cout << received1 << endl;
    cout << received2 << endl;
    sendMatric(P_a, P0, ref(sock1), 5000);
    sendMatric(P_b, P1, ref(sock2), 5500);
    sendVector(q_a, P0, 5000, ref(sock1));
    sendVector(q_b, P1, 5500, ref(sock2));
    cout << "-------SSGQ Data Initialization-------" << endl;
}

void synchronize_SSQP(){
    string msg = "start SSQP";
    sendMsg(msg, P0, ref(sock1), 4000);
    sendMsg(msg, P1, ref(sock2), 4500);
    string received1;
    string received2;
    recMessage(ref(received1), ref(sock3));
    recMessage(ref(received2), ref(sock4));
    cout << received1 << endl;
    cout << received2 << endl;
    sendMatric(P_a, P0, ref(sock3), 4000);
    sendMatric(P_b, P1, ref(sock4), 4500);
    sendVector(q_a, P0, 4000, ref(sock3));
    sendVector(q_b, P1, 4500, ref(sock4));
    cout << "-------SSQP Data Initialization-------" << endl;
}

void Gen_K(){
//        sendMsg(msg, P1);//将a0 b0 c0 发送  P1
    // int l = V.size();
    // char* msg;
    // for(int i = 0; i < l; ++i){
    //     *(msg + i) = V[i];
    // }
    while(1){
        vector<int> Va;
        vector<int> Vb;
        cout << "-----------waite for V come-----------" << endl;
        recVector(ref(Va), ref(sock1));
        recVector(ref(Vb), ref(sock2));
        cout << "---------------V is come--------------" << endl;
        vector<int> V(Va.size());
        int t = 0;
        for(int i = 0; i < Va.size(); ++i){
            V[i] = Va[i] + Vb[i];
            if(V[i] == 1)
                t++;
        }
        cout << "V's size is " << t << endl;
        if(t == 0){
            vector<vector<int>> K;
            // cout << "V's size is 0" << endl;
            sendMatric(K, P0, ref(sock1), 5000);
            sendMatric(K, P1, ref(sock2), 5500);
            return;
        }else{
            vector<vector<int>> K(t, vector<int>(V.size(), 0));
            int j = 0;
            for(int i = 0; i < V.size() && j < t; ++i){
                    if(V[i] == 1){
                        K[j++][i] = 1;
                    }
                }
            // for(int i = 0; i < t; ++i){
            //     for(int j = 0; j < K[0].size(); ++j)
            //         cout << K[i][j] << " ";
            //     cout << endl;
            // }
            sendMatric(K, P0, ref(sock1), 5000);
            sendMatric(K, P1, ref(sock2), 5500);
        }
    }
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
void shareArray(vector<vector<int>> arr) {
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

void shareArray(vector<int>& arr) {
    int m = arr.size();

    // 随机生成n*m个分享值，填充到share1和share2中
    q_a = generateRandomArray(m);
    q_b = generateRandomArray(m);

    // 计算两个分享值的和，结果存储到share1中
    for (int j = 0; j < m; j++) {
        q_a[j] = arr[j] - q_b[j];
    }

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
void print_Result(vector<vector<vector<int>>>& Ga, vector<vector<vector<int>>>& Gb){
    vector<vector<vector<int>>> G(Ga.size(), vector<vector<int>>(Ga[0].size(), vector<int>(Ga[0][0].size())));
    for(int i = 0; i < Ga.size(); ++i)
        for(int j = 0; j < Ga[0].size(); ++j)
            for(int k = 0; k < Ga[0][0].size(); ++k)
                G[i][j][k] = Ga[i][j][k] + Gb[i][j][k];
    print_G(G);
}   

void init_data(int m, int n){
    vector<vector<int>> arr = generateIndependent(n, m);
    P_a.resize(n);
    P_b.resize(n);
    // q_a.resize(m);
    // q_b.resize(m);
    shareArray(arr);
    // vector<int> q = generateRandomArray(m);
    // shareArray(q);
    cout << "---------Data Generate sucess---------" << endl;
    cout << "---- n = " << n << " ---- m = " << m << " ----" << endl;
    // print_P(arr);
    // cout << endl;
    // cout << "-------------------P------------------" <<endl;
    // print_P(P_a);
    // cout << endl;
    // cout << "------------------P_a-----------------" <<endl;
    // print_P(P_b);
    // cout << endl;
    // cout << "------------------P_b-----------------" << endl;
    // print_p(q);
    // cout << endl;
    // cout << "-------------------q------------------" <<endl;
    // print_p(q_a);
    // cout << endl;
    // cout << "------------------q_a-----------------" << endl;
    // print_p(q_b);
    // cout << endl;
    // cout << "------------------q_b-----------------" << endl;
}

void test0(){
    int m = 2, n = 5, k = 1;
    init_data(m, n);
    for(auto p : P_a){
        for(auto a : p)
            cout << a << " ";
        cout << endl;
    }
    cout << "---------" << endl;
    for(auto p : P_b){
        for(auto a : p)
            cout << a << " ";
        cout << endl;
    }
    cout << "---------" << endl;
    for(auto q : q_a){
        cout << q << " ";
    }
    cout << endl;
    cout << "---------" << endl;
    for(auto q : q_b){
        cout << q << " ";
    }
    cout << endl;
    cout << "---------" << endl;
}

void getSockOpt(){
    int rcvbuf_size;
    socklen_t optlen = sizeof(rcvbuf_size);
    if (getsockopt(sock1.get_sockId(), SOL_SOCKET, SO_RCVBUF, &rcvbuf_size, &optlen) < 0) {
        // 错误处理
    }
    if (getsockopt(sock2.get_sockId(), SOL_SOCKET, SO_RCVBUF, &rcvbuf_size, &optlen) < 0) {
        // 错误处理
    }
    std::cout << "接收缓冲区大小: " << rcvbuf_size << " 字节" << std::endl;

    int sndbuf_size;
    optlen = sizeof(sndbuf_size);
    if (getsockopt(sock1.get_sockId(), SOL_SOCKET, SO_SNDBUF, &sndbuf_size, &optlen) < 0) {
        // 错误处理
    }
    std::cout << "发送缓冲区大小: " << sndbuf_size << " 字节" << std::endl;
}

void setOpt(){
    int buffer_size = 8388608;  // 8MB，您可以根据需要设置更大或更小的值
    if (setsockopt(sock1.get_sockId(), SOL_SOCKET, SO_RCVBUF, &buffer_size, sizeof(buffer_size)) < 0) {
        // 错误处理
    }
    if (setsockopt(sock2.get_sockId(), SOL_SOCKET, SO_RCVBUF, &buffer_size, sizeof(buffer_size)) < 0) {
        // 错误处理
    }
}

void test_SSGQ(){
    synchronize_SSGQ();  
    auto start = std::chrono::steady_clock::now();
    Gen_K();
    vector<vector<vector<int>>> Gska;
    vector<vector<vector<int>>> Gskb;
    recResult(ref(Gska), ref(sock1));
    recResult(ref(Gskb), ref(sock2));
    auto end = std::chrono::steady_clock::now();
    std::chrono::duration<double, std::milli> elapsed_milliseconds = end - start;
    cout << "The time is : " << elapsed_milliseconds.count() << " ms" << endl;
    cout << "The result is : ";
    print_Result(Gska, Gskb);
    cout << endl;
    cout << "-------------------------------" << endl;
}

void print_result(vector<vector<int>>& ska, vector<vector<int>>& skb){
    for(int i = 0; i < ska.size(); ++i)
        for(int j = 0; j < ska[0].size(); ++j)
            ska[i][j] += skb[i][j];
    print_P(ska);
}

void test_SSQP(){
    synchronize_SSQP();
    clock_t startTime,endTime;
    auto start = std::chrono::steady_clock::now(); 
    vector<vector<int>> ska;
    vector<vector<int>> skb;
    recResult(ref(ska), ref(sock3));
    recResult(ref(skb), ref(sock4));
    auto end = std::chrono::steady_clock::now();
    std::chrono::duration<double, std::milli> elapsed_milliseconds = end - start;
    cout << "The time is : " << elapsed_milliseconds.count() << " ms" << endl;
    cout << "result is: ";
    print_result(ska, skb);
    cout << endl;
    cout << "-------------------------------" << endl;
}

int main(int argc, char* argv[]){
// 测试乘法
    PartyNum = 2;
    cout << "PartyNum"<< PartyNum << endl;
    n = stoi((string(argv[1])));
    m = stoi((string(argv[2])));
    init_data(m, n);
    init();
    getSockOpt();
    setOpt();
    getSockOpt();
    test_SSGQ();
    // test_SSQP();
    // Gen_K();
    // test0();
    close();
    return 0;
}
