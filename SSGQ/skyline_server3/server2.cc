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

void sendMsg(string msg, string IP, Socket::UDP& sock) {
    try {
//        Socket::UDP sock;
        int port;
        if(PartyNum == 1)
            port = 6000;
        else
            port = 6500;
        sock.send(IP, port, msg);
    }
    catch (Socket::Exception &e) {
        cout << e.what() << endl;
    }
}

void sendVector(vector<int>& V, string IP, Socket::UDP& sock) {
    try {
//        Socket::UDP sock;
        int port;
        if(PartyNum == PartyA)
            port = 6000;
        else
            port = 6500;
        sock.send_vector(IP, port, V);
    }
    catch (Socket::Exception &e) {
        cout << e.what() << endl;
    }
}

void recMatric(vector<vector<int>>& V, Socket::UDP& sock) {
    try {
        V = sock.receive_matric();
    }
    catch (Socket::Exception &e) {
        cout << e.what() << endl;
    }
}

void init(){
    try {
        int port;
        if(PartyNum == PartyA)
            sock0.bind(5500);
        else if(PartyNum == PartyB)
            sock0.bind(5000);
        else{
            sock1.bind(6000);
            sock2.bind(6500);
        }
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

// /***
//  * 将接受到的字符串拆成int数组返回
//  * @param msg 需要拆分的字符串
//  * @return 返回拆分的int数组
//  */
// int *split(string msg) {
//     int *a = new int[msg.length()];
//     for (int i = 0; i < msg.length(); ++i) {
//         a[i] = msg[i];
//     }
//     return a;
// }

// int secBMul(int u1, int v1) {
//     // P1 和 P2 分别含有 u1 v1，u2 v2
//     int alpha1 = (u1 ^ a) % 2, beta1 = (v1 ^ b) % 2;

//     // 发送alpha1 beta1
//         char msg[2];
//         msg[0] = alpha1;
//         msg[1] = beta1;
// //        sendMsg(msg, P1);//将a0 b0 c0 发送  P1
//     string received;

//     thread *threads = new thread[2];
//     threads[0] = thread(sendMsg, msg, P1);
//     threads[1] = thread(recMessage,ref(received));
//     for (int i = 0; i < 2; i++)
//         threads[i].join();
//     delete[] threads;

//     //region 发送alpha2 beta2
// //    msg = recMsg();
//     int * share = split(received);
//     int alpha2 = share[0], beta2 = share[1];
//     int alpha = (alpha1 ^ alpha2) % 2, beta = (beta1 ^ beta2) % 2;
//     return (c ^ (b & alpha) ^ (a & beta) ^ (PartyNum * (alpha & beta))) % 2;
//     //endregion
// }

// int secMul( int u1, int v1) {

//     // P1 和 P2 分别含有 u1 v1，u2 v2
//     int alpha1 = (u1 - a), beta1 = (v1 - b);

//     // 发送alpha1 beta1
//         char msg[2];
//         msg[0] = alpha1;
//         msg[1] = beta1;
// //        sendMsg(msg, P1);//将a0 b0 c0 发送  P1
//     string received;

//     thread *threads = new thread[2];
//     threads[0] = thread(sendMsg, msg, P1);
//     threads[1] = thread(recMessage,ref(received));
//     for (int i = 0; i < 2; i++)
//         threads[i].join();
//     delete[] threads;

//     //region 发送alpha2 beta2
// //    msg = recMsg();
//     int * share = split(received);
//     int alpha2 = share[0], beta2 = share[1];
//     int alpha = (alpha1+alpha2), beta = (beta1+beta2);
//     return (c+b*alpha+a*beta+PartyNum*alpha*beta);
//     //endregion

// }

// int* zl2z2(int num){
//     int *res = new int[l];
//     for(int i = 0; i < l;i++){
//         res[i]= num>>i&0x00000001;
//     }
//     return res;
// }

// int z22zl(int num){
//     cout << endl <<"num:" << num << endl;
//     int t1;int t2;
//     if (PartyNum == PartyA){
//         t1=num;
//         t2 =0;
//     }else{
//         t1=0;
//         t2=num;
//     }
//     return t1+t2-2*secMul(t1,t2);
// }

// /**
//  *
//  * @param u1 u's share
//  * @param v1 v's share
//  * @return u>v ? 1'share : 0'share
//  */
// int secComp( int u1, int v1) {
//     int z = v1-u1;
//     // convert to bit string 0放置低位 31放置高位（符号位）
//     int * res = zl2z2(z);


//     int* w = new int[l];
//     int* p = new int[l];
//     int* q = new int[l];
//     int* d = new int[l];
//     int* c = new int[l];
//     int* e = new int[l];
//     for(int i = 0; i < l ;i++){
//         w[i]=res[i];
//         if (PartyNum == PartyB){
//             p[i]=res[i];
//             q[i]=0;
//         }else{
//             p[i]=0;
//             q[i]=res[i];
//         }
//         d[i]=secMul((p[i]%2+2)%2,(q[i]%2+2)%2);
//     }

//     c[0]=d[0];
//     for(int i =1; i < l-1;i++){
//         d[i]=(((d[i]+PartyNum)%2)+2)%2;
//         e[i]=((secMul(w[i],c[i-1])+PartyNum)%2+2)%2;
//         c[i]=((secMul(e[i],d[i])+PartyNum)%2+2)%2;
//     }
//     return (w[l-1]+c[l-2]+2)%2;  // convert z2 to zl
// }

// // ssed
// int euclideanDistance(int x,int y){
//     return secMul(x-y,x-y);
// }

// int sBranch(int m1,int m2,int c1,int c2){
//     int z = secComp(m1,m2);
//     int p = PartyNum-z;
//     cout << "p: " << p << "\t z: " << z << endl;
//     int tmp = secMul(p,c1);
//     int tmp1 = secMul(z,c2);

//     cout << "secMul(p,c1): " << tmp << "\t secMul(z,c2): " << tmp1 << endl;
//     return tmp+tmp1;
// }

// // sfindmin
// int findMin(int *nums,int len){
//     int min=nums[0];
//     for(int i = 0; i < len;i++){
//         min = sBranch(min,nums[i],min,nums[i]);
//     }
// }

// vector<vector<int>> Gen_K(int PartyNum, vector<int>& V, Socket::UDP& sock){
// //        sendMsg(msg, P1);//将a0 b0 c0 发送  P1
//     int l = V.size();
//     char* msg;
//     for(int i = 0; i < l; ++i){
//         *(msg + i) = V[i];
//     }
//     string received;
//     thread *threads = new thread[1];
//     threads[0] = thread(sendMsg, msg, P1, PartyNum, ref(sock));
//     // threads[1] = thread(recMessage,ref(received), ref(sock));
//     threads[0].join();
//     delete[] threads;
//     int * share = split(received);
// }

void Gen_K(){
//        sendMsg(msg, P1);//将a0 b0 c0 发送  P1
    vector<int> V = {0,0,0,0,-1,0};
    int l = V.size();
    sendVector(V, P1, ref(sock0));
    vector<vector<int>> K;
    recMatric(ref(K), ref(sock0));
    for (const auto& row : K) {
        for (const auto& elem : row) {
            std::cout << elem << ' ';
        }
        std::cout << std::endl;
    }
}


// 先运行partyA=2的机器,在运行1，在运行0
void synchronize(){
    string received;
    if (PartyNum == PartyA){
        sendMsg("start1",P1,ref(sock0));
        recMessage(ref(received), ref(sock0));
        cout << received << endl;
    }else if(PartyNum == PartyB){
        sendMsg("start2",P1,ref(sock0));
        recMessage(ref(received), ref(sock0));
        cout << received << endl;
    }else{
        string received1;
        string received2;
        recMessage(ref(received1), ref(sock1));
        recMessage(ref(received1), ref(sock2));
        cout << received1 << endl;
        cout << received2 << endl;
        sendMsg("start31",P1,ref(sock1));
        sendMsg("start32",P1,ref(sock2));
    }
}

int main(int argc, char* argv[]){
// 测试乘法
    PartyNum = atoi(argv[1]);
    cout << "PartyNum"<< PartyNum << endl;

    init();
    synchronize();
    Gen_K();
    close();
    return 0;
}
