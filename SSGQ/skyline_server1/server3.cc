#include <iostream>
#include <cstring>
#include "Socket.hpp"
#include <thread>
#include <bits/stdc++.h>

using namespace std;
//#define party 3
#define P0 "10.0.4.15" // current ip addr
#define P1 "10.0.4.15" // another computer

Socket::UDP sock0;
Socket::UDP sock1;
Socket::UDP sock2;
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

void sendMsg(string msg, string IP, Socket::UDP& sock, int port) {
    try {
//        Socket::UDP sock;
        // int port;
        // if(PartyNum == PartyA)
        //     port = 6000;
        // else
        //     port = 6500;
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

void init(){
    try {
            sock1.bind(6000);
            sock2.bind(6500);
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

// 先运行partyA=1的机器
void synchronize(){
    string received1;
    string received2;
    recMessage(ref(received1), ref(sock1));
    recMessage(ref(received2), ref(sock2));
    cout << received1 << endl;
    cout << received2 << endl;
    sendMsg("start2_0",P1,ref(sock1), 5000);
    sendMsg("start2_1",P1,ref(sock2), 5500);
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
        cout << "------waite for V come------" << endl;
        recVector(ref(Va), ref(sock1));
        recVector(ref(Vb), ref(sock2));
        cout << "----------V is come---------" << endl;
        vector<int> V(Va.size());
        int t = 0;
        for(int i = 0; i < Va.size(); ++i){
            V[i] = Va[i] + Vb[i];
            if(V[i] == 1)
                t++;
        }
        // cout << t << endl;
        if(t == 0){
            vector<vector<int>> K;
            vector<vector<int>> K_a;
            cout << "pause1" << endl;
            sendMatric(K, P1, ref(sock1), 5500);
            sendMatric(K, P1, ref(sock2), 5000);
        }else{
            vector<vector<int>> K(t, vector<int>(V.size(), 0));
            int j = 0;
            for(int i = 0; i < V.size() && j < t; ++i){
                    if(V[i] == 1){
                        K[j++][i] = 1;
                    }
                }
            for(int i = 0; i < t; ++i){
                for(int j = 0; j < K[0].size(); ++j)
                    cout << K[i][j] << " ";
                cout << endl;
            }
            sendMatric(K, P1, ref(sock1), 5500);
            sendMatric(K, P1, ref(sock2), 5000);
        }
    }
}

int main(){
// 测试乘法
    PartyNum = 2;
    cout << "PartyNum"<< PartyNum << endl;

    init();
    synchronize();
    Gen_K();
    close();
    return 0;
}
