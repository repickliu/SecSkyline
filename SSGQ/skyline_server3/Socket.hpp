/*
 * Socket.hpp
 * This file is part of VallauriSoft
 *
 * Copyright (C) 2012 - Comina, gnuze
 *
 * VallauriSoft is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * VallauriSoft is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with VallauriSoft. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef _SOCKET_HPP_
#define _SOCKET_HPP_

#include <iostream>
#include <sstream>
#include <exception>
#include <string>
#include <stdlib.h>
#include <vector>
#include <sstream>

#include <arpa/inet.h>

#define MAX_BUFFER 1024

using namespace std;

namespace Socket
{
    typedef int Socket;
    typedef string Ip;
    typedef unsigned int Port;
    typedef string Data;
    
    typedef struct
    {
        Ip ip;
        Port port;
    }Address;
    
    typedef struct
    {
        Address address;
        Data data;
    }Datagram;
    
    struct sockaddr_in* to_sockaddr(Address* a)
    {   struct sockaddr_in* ret;

        ret=(struct sockaddr_in*) malloc (sizeof(struct sockaddr_in));
        ret->sin_family = AF_INET;
        inet_aton(a->ip.c_str(),&(ret->sin_addr));
        ret->sin_port=htons(a->port);
            
        return ret;
    }

    Address* from_sockaddr(struct sockaddr_in* address)
    {   Address* ret;

        ret=(Address*)malloc(sizeof(Address));
        ret->ip = inet_ntoa(address->sin_addr);
        ret->port = ntohs(address->sin_port);
        
        return ret;
    }

    class Exception
    {
    private:
        string _message;
    public:
        Exception(string error) { this->_message = error; }
        virtual const char* what() { return this->_message.c_str(); }
    };

    class UDP
    {
    private:
        
        Socket _socket_id;
        bool _binded;
        int port;
        
    public:
        
        UDP(void);
        ~UDP(void);
        void close(void);
        void bind(Port port);
        void send(Ip ip, Port port, Data data);
        void send_vector(Ip ip, Port port, vector<int>& V);
        void send_matric(Ip ip, Port port, vector<vector<int>>& K);
        vector<int> receive_vector();
        vector<vector<int>> receive_matric();
        vector<vector<vector<int>>> receive_Result();
        Datagram receive();
        ssize_t send_all(const void *buffer, size_t length, int flags, const struct sockaddr *dest_addr, socklen_t dest_len, Ip ip, Port port);
        ssize_t recv_all(void *buffer, size_t length, int flags, struct sockaddr *src_addr, socklen_t *src_len);
        int get_port(){return port;}
        int get_sockId(){return _socket_id;}
    };

    UDP::UDP(void)
        {
            this->_socket_id = socket(AF_INET, SOCK_DGRAM, 0);   
            if (this->_socket_id == -1) throw Exception("[Constructor] Cannot create socket");           
            this->_binded = false;
        }

    UDP::~UDP(void)
        {
        }
        
    void UDP::close(void)
        {
            shutdown(this->_socket_id, SHUT_RDWR);
        }
        
    void UDP::bind(Port port)
        {
            struct sockaddr_in address;
            address.sin_family = AF_INET;
            address.sin_addr.s_addr=htonl(INADDR_ANY);
            address.sin_port=htons(port);
            this->port = port;
            
            if (this->_binded)
            {
                this->close();
                this->_socket_id = socket(AF_INET, SOCK_DGRAM, 0);
            }
            // ::bind() calls the function bind() from <arpa/inet.h> (outside the namespace)            
            if (::bind(this->_socket_id, (struct sockaddr*)&address, sizeof(struct sockaddr_in)) == -1)
            {
                stringstream error;
                error << "[listen_on_port] with [port=" << port << "] Cannot bind socket";
                throw Exception(error.str());
            }
            
            this->_binded = true;
        }
        
    void UDP::send(Ip ip, Port port, Data data)
        {
            struct sockaddr_in address;
            address.sin_family = AF_INET;
            address.sin_port = htons(port);
            inet_aton(ip.c_str(), &address.sin_addr);
            
            if (sendto(this->_socket_id, (void*)data.c_str(), data.length() + 1, 0, (struct sockaddr*)&address, sizeof(struct sockaddr_in)) == -1)
            {
                stringstream error;
                error << "[send] with [ip=" << ip << "] [port=" << port << "] [data=" << data << "] Cannot send";
                throw Exception(error.str());
            }

            
        }
    ssize_t UDP::send_all(const void *buffer, size_t length, int flags, const struct sockaddr *dest_addr, socklen_t dest_len, Ip ip, Port port) {
            const char *buf = static_cast<const char *>(buffer);
            
            size_t remaining = length;
            size_t max_sent = 10000;

            while (remaining > 0) {
                size_t sent = remaining > max_sent ? max_sent : remaining;
                ssize_t sented = sendto(this->_socket_id, buf, sent, flags, dest_addr, dest_len);
                if (sent == -1) {
                    cout << "[send] with [ip=" << ip << "] [port=" << port << "] [data=" << buffer << "] Cannot send" << endl;
                    return -1;
                }
                remaining -= sented;
                buf += sented;
                // cout << remaining << endl;
            }
            return length;
        }

    void UDP::send_vector(Ip ip, Port port, vector<int>& V)
        {
            struct sockaddr_in address;
            address.sin_family = AF_INET;
            address.sin_port = htons(port);
            inet_aton(ip.c_str(), &address.sin_addr);
            int dataSize = V.size();
            // cout << dataSize << endl;
            // cout << port << endl;
            // for(int i = 0; i < V.size(); ++i)
            //     cout << V[i] << " ";
            // cout << endl;
            //发送vector大小
            send_all(&dataSize, sizeof(dataSize), 0, (struct sockaddr*)&address, sizeof(struct sockaddr_in), ip, port);

            //分批发送vector数据
            send_all(V.data(), dataSize * sizeof(int), 0, (struct sockaddr*)&address, sizeof(struct sockaddr_in), ip, port);

            // string dataSizeToSend = to_string(dataSize);
            // if (sendto(this->_socket_id, (void*)dataSizeToSend.c_str(), dataSize + 1, 0, (struct sockaddr*)&address, sizeof(struct sockaddr_in)) == -1)
            // {
            //     stringstream error;
            //     error << "[send] with [ip=" << ip << "] [port=" << port << "] [data=" << dataSizeToSend << "] Cannot send";
            //     throw Exception(error.str());
            // }
            
            // if (sendto(this->_socket_id, (char*)V.data(), dataSize, 0, (struct sockaddr*)&address, sizeof(struct sockaddr_in)) == -1)
            // {
            //     stringstream error;
            //     error << "[send] with [ip=" << ip << "] [port=" << port << "] [data=" << V.data() << "] Cannot send";
            //     throw Exception(error.str());
            // }
        }
        
    void UDP::send_matric(Ip ip, Port port, vector<vector<int>>& K){
        struct sockaddr_in address;
        address.sin_family = AF_INET;
        address.sin_port = htons(port);
        inet_aton(ip.c_str(), &address.sin_addr);
        int line,col;
        line = K.size();
        col = line > 0 ? K[0].size() : 0;
        // cout << line * col << endl;
        vector<int> V(line * col);
        int t = 0;
        for(int i = 0; i < line; ++i)
            for(int j = 0; j < col; ++j)
                V[t++] = K[i][j];
        // cout << dataSize << endl;
        string dataSizeToSend;
        if(!line){
            dataSizeToSend = "null";
        }else
            dataSizeToSend = to_string(line) + " " + to_string(col);
        if (sendto(this->_socket_id, (void*)dataSizeToSend.c_str(), sizeof(dataSizeToSend) + 1, 0, (struct sockaddr*)&address, sizeof(struct sockaddr_in)) == -1)
        {
            stringstream error;
            error << "[send] with [ip=" << ip << "] [port=" << port << "] [data=" << dataSizeToSend << "] Cannot send";
            throw Exception(error.str());
        }
        if(line)
            send_all(V.data(), line * col * sizeof(int), 0, (struct sockaddr*)&address, sizeof(struct sockaddr_in), ip, port);
            // if (sendto(this->_socket_id, (char*)V.data(), line * col * sizeof(int), 0, (struct sockaddr*)&address, sizeof(struct sockaddr_in)) == -1)
            // {
            //     stringstream error;
            //     error << "[send] with [ip=" << ip << "] [port=" << port << "] [data=" << K.data() << "] Cannot send";
            //     throw Exception(error.str());
            // }
        cout << "end sendMatric" << endl;
    }

    Datagram UDP::receive()
        {
            int size = sizeof(struct sockaddr_in);
            char *buffer = (char*)malloc(sizeof(char) * MAX_BUFFER);
            struct sockaddr_in address;
            Datagram ret;
            
            if (recvfrom(this->_socket_id, (void*)buffer, MAX_BUFFER, 0, (struct sockaddr*)&address, (socklen_t*)&size) == -1) throw Exception("[receive] Cannot receive");
            
            ret.data = buffer;
            ret.address.ip = inet_ntoa(address.sin_addr);
            ret.address.port = ntohs(address.sin_port);
            
            free(buffer);
            
            return ret;
        }
    ssize_t UDP::recv_all(void *buffer, size_t length, int flags, struct sockaddr *src_addr, socklen_t *src_len) {
            char *buf = static_cast<char *>(buffer);
            size_t remaining = length;
            ssize_t received;

            while (remaining > 0) {
                received = recvfrom(this->_socket_id, buf, remaining, flags, src_addr, src_len);
                if (received == -1) {
                    throw Exception("[receive] Cannot receive");
                    return -1;
                }
                remaining -= received;
                buf += received;
            }

            return length;
        }

    vector<int> UDP::receive_vector()
        {
            int size = sizeof(struct sockaddr_in);
            char *buffer = (char*)malloc(sizeof(char) * MAX_BUFFER);
            struct sockaddr_in address;
            Datagram ret;
            int lenth;
            if (recvfrom(this->_socket_id, &lenth, MAX_BUFFER, 0, (struct sockaddr*)&address, (socklen_t*)&size) == -1) throw Exception("[receive] Cannot receive");
            
            // cout << lenth << endl;

            // 分配内存并接收实际数据
            std::vector<int> receivedData(lenth);
            // if (recvfrom(this->_socket_id, (char*)receivedData.data(), lenth * sizeof(int), 0, (struct sockaddr*)&address, (socklen_t*)&size) == -1) throw Exception("[receive] Cannot receive");
            recv_all((char*)receivedData.data(), lenth * sizeof(int), 0, (struct sockaddr*)&address, (socklen_t*)&size);
            cout << receivedData.size() << endl;
            for(int i = 0; i < receivedData.size(); ++i)
                cout << receivedData[i] << " ";
            cout << endl;
            return receivedData;
        }
    vector<vector<int>> UDP::receive_matric()
        {
            int size = sizeof(struct sockaddr_in);
            char *buffer = (char*)malloc(sizeof(char) * MAX_BUFFER);
            struct sockaddr_in address;
            Datagram ret;
            uint32_t receivedDataSize = ntohl(receivedDataSize);
            if (recvfrom(this->_socket_id, (void*) buffer, MAX_BUFFER, 0, (struct sockaddr*)&address, (socklen_t*)&size) == -1) throw Exception("[receive] Cannot receive");
            string l_c = string(buffer);
            cout << l_c << endl;
            if(l_c == "null"){
                vector<vector<int>> matrix;
                return matrix;
            }else{
                std::istringstream iss(l_c);
                int line,col;
                std::vector<int> tokens;
                string str;
                while (std::getline(iss, str, ' ')) {
                    tokens.push_back(stoi(str));
                }
                line = tokens[0];
                col = tokens[1];
                // cout << line << " " << col << endl;
                // 分配内存并接收实际数据
                std::vector<int> receivedData(line * col);
                vector<vector<int>> matrix(line, vector<int>(col));
                // cout << receivedData.size() << endl;
                // if (recvfrom(this->_socket_id, (char*)receivedData.data(), line * col * sizeof(int), 0, (struct sockaddr*)&address, (socklen_t*)&size) == -1) throw Exception("[receive] Cannot receive");
                recv_all((char*)receivedData.data(), line * col * sizeof(int), 0, (struct sockaddr*)&address, (socklen_t*)&size);
                cout << receivedData.size() << endl;
                for (int i = 0; i < line; ++i) {
                    for (int j = 0; j < col; ++j) {
                        matrix[i][j] = receivedData[i * col + j];
                    }
                }
                return matrix;
            }
        }
    vector<vector<vector<int>>> UDP::receive_Result()
        {
            int size = sizeof(struct sockaddr_in);
            char *buffer = (char*)malloc(sizeof(char) * MAX_BUFFER);
            struct sockaddr_in address;
            Datagram ret;
            uint32_t receivedDataSize = ntohl(receivedDataSize);
            if (recvfrom(this->_socket_id, (void*) buffer, MAX_BUFFER, 0, (struct sockaddr*)&address, (socklen_t*)&size) == -1) throw Exception("[receive] Cannot receive");
            string l_c = string(buffer);
            cout << l_c << endl;
            if(l_c == "null"){
                vector<vector<vector<int>>> matrix;
                return matrix;
            }else{
                std::istringstream iss(l_c);
                int line,col,m;
                std::vector<int> tokens;
                string str;
                while (std::getline(iss, str, ' ')) {
                    tokens.push_back(stoi(str));
                }
                line = tokens[0];
                col = tokens[1];
                m = tokens[2];
                // cout << line << " " << col << endl;
                // 分配内存并接收实际数据
                std::vector<int> receivedData(line * col * m);
                vector<vector<vector<int>>> matrix(line, vector<vector<int>>(col, vector<int>(m)));
                // cout << receivedData.size() << endl;
                // if (recvfrom(this->_socket_id, (char*)receivedData.data(), line * col * sizeof(int), 0, (struct sockaddr*)&address, (socklen_t*)&size) == -1) throw Exception("[receive] Cannot receive");
                recv_all((char*)receivedData.data(), line * col * sizeof(int), 0, (struct sockaddr*)&address, (socklen_t*)&size);
                cout << receivedData.size() << endl;
                for (int i = 0; i < line; ++i) {
                    for (int j = 0; j < col; ++j) {
                        for(int k = 0; k < m; ++k)
                            matrix[i][j][k] = receivedData[i * col * m + j * m + k];
                    }
                }
                return matrix;
            }
            
        }
}

#endif   // _SOCKET_HPP_

