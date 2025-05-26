#include <iostream>
#include <fstream>
#include <cstring>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include<opencv2/opencv.hpp>
#include<fstream>
#include<filesystem>

int main() {
    // 1. Create UDP socket
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        perror("socket creation failed");
        return 1;
    }

    // 2. Bind socket to port
    struct sockaddr_in recv_addr;
    memset(&recv_addr, 0, sizeof(recv_addr));
    recv_addr.sin_family = AF_INET;
    recv_addr.sin_addr.s_addr = INADDR_ANY;   // listen on all interfaces
    recv_addr.sin_port = htons(8080);        // must match sender port

    if (bind(sockfd, (struct sockaddr*)&recv_addr, sizeof(recv_addr)) < 0) {
        perror("bind failed");
        close(sockfd);
        return 1;
    }
    else{
        std::cout << "Socket bound to port 8080\n";
    }
    
    while (true) {
        uint32_t net_data_size;
        ssize_t len = recvfrom(sockfd, &net_data_size, sizeof(net_data_size), 0, NULL, NULL);
        if (len <= 0) continue;

        uint32_t data_size = ntohl(net_data_size);

        std::vector<uchar> buffer(data_size);
        ssize_t recv_len = 0;
        while (recv_len < data_size) {
            ssize_t chunk = recvfrom(sockfd, buffer.data() + recv_len, data_size - recv_len, 0, NULL, NULL);
            if (chunk <= 0) break;
            recv_len += chunk;
        }

        if (recv_len != data_size) {
            std::cerr << "Failed to receive full frame data\n";
            continue;
        }

        // Decode JPEG bytes to OpenCV Mat
        cv::Mat frame = cv::imdecode(buffer, cv::IMREAD_COLOR);
        if (frame.empty()) {
            std::cerr << "Failed to decode frame\n";
            continue;
        }

        // Show frame
        cv::imshow("Received Frame", frame);
        if (cv::waitKey(1) == 27) break;  // exit on ESC key
    }










    // 3. Open output file to write received data
    // std::ofstream outfile("received_song.mp4", std::ios::binary);
    // if (!outfile) {
    //     std::cerr << "Cannot open output file\n";
    //     close(sockfd);
    //     return 1;
    // }
    //  struct timeval tv;
    // tv.tv_sec = 5;  // 5 seconds timeout
    // tv.tv_usec = 0;
    // setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));

    // const size_t bufferSize = 64000;
    // char buffer[bufferSize];
    // struct sockaddr_in sender_addr;
    // socklen_t addr_len = sizeof(sender_addr);

    // std::cout << "Waiting for data...\n";
    // ssize_t recv_len;
    // 4. Receive loop


    // while ((recv_len = recvfrom(sockfd, buffer, bufferSize, 0,
    //                            (struct sockaddr*)&sender_addr, &addr_len)) > 0){
    //     outfile.write(buffer, recv_len);
    //     std::cout << "Received " << recv_len << " bytes\n";
    //     std::cout<<recv_len<<std::endl;

    //     // Optional: Add condition to break loop (e.g., special packet or timeout)
    // }
    // std::cout<<"here"<<std::endl;

    // if (recv_len < 0) {
    //     perror("recvfrom failed");
    // } else {
    //     std::cout << "Reception finished\n";
    // }

    // outfile.close();
    close(sockfd);
    return 0;
}
