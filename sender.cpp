#include<iostream>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<unistd.h>
#include<cstring>  // for memset
#include<opencv2/opencv.hpp>
#include<fstream>
#include<filesystem>

// using namespace std;
int main()
{
    int sockfd;
    sockfd=socket(AF_INET,SOCK_DGRAM,0);
    if(sockfd<0)
    {
        std::cout<<"socket creation failed"<<std::endl;
        return 0;

    }
    struct  sockaddr_in destaddr;
    std::memset(&destaddr,0,sizeof(destaddr));
    destaddr.sin_family=AF_INET;
    destaddr.sin_port=htons(8080);
    if (inet_pton(AF_INET, "127.0.0.1", &destaddr.sin_addr) <= 0) {
    perror("inet_pton failed");
    return 1;
    }

    cv::VideoCapture cap("song.mp4");
    if (!cap.isOpened()) {
        std::cerr << "Error opening video file" << std::endl;
        return -1;
    }

    // 2. Create a directory to save frames
    std::string outputDir = "frames";
    // fs::create_directory(outputDir);

    // 3. Frame processing loop
    cv::Mat frame;
    int frameCount = 0;

    while (cap.read(frame)) {  // Reads next frame into `frame`
        std::string filename = outputDir + "/frame_" + std::to_string(frameCount) + ".jpg";
        cv::imwrite(filename, frame);  // Save frame as image
        std::cout << "Saved " << filename << std::endl;
         std::cout<<"before sending data..."<<std::endl;
        std::vector<uchar> encoded;
        std::vector<int> params = {cv::IMWRITE_JPEG_QUALITY, 80};  // quality 0-100
        cv::imencode(".jpg", frame, encoded, params);

        // Send the size of encoded data first
        uint32_t data_size = htonl(encoded.size());  // convert to network byte order
        sendto(sockfd, &data_size, sizeof(data_size), 0, (struct sockaddr*)&destaddr, sizeof(destaddr));

        // Then send the encoded data bytes
        sendto(sockfd, encoded.data(), encoded.size(), 0, (struct sockaddr*)&destaddr, sizeof(destaddr));


        // Wait a bit for ~30fps
        frameCount++;
         cv::waitKey(33);


        // Optional: break after N frames for testing
        // if (frameCount > 100) break;
    }

    cap.release();
    std::cout << "Finished extracting frames." << std::endl;




































    // std::ifstream file("song.mp4",std::ios::binary);
    //     if(!file)
    //     {
    //         std::cout<<"file not found"<<std::endl;
    //         return 0;
    //     }
    
    // const int bufferSize=64000;
    // char buffer[bufferSize];
    // while(file.read(buffer,bufferSize))
    // {
    //     int bytesRead=file.gcount();
    //     std::cout<<"before sending data..."<<std::endl;
    //     int bytesSent=sendto(sockfd,buffer,bytesRead,0,(struct sockaddr*)&destaddr,sizeof(destaddr));
    //     std::cout<<"after sending data..."<<std::endl;

    //     if(bytesSent<0)
    //     {
    //         std::cout<<"error in sending data"<<std::endl;
    //         return 0;
    //     }
    //     sleep(2);
    //     std::cout<<"sent "<<bytesSent<<" bytes"<<std::endl;
    // }
    // if(file.eof())
    // {
    //     std::cout<<"file sent successfully"<<std::endl;
    // }
    // else
    // {
    //     std::cout<<"error in sending file"<<std::endl;
    // }
    // file.close();
    close(sockfd);
    std::cout<<"socket closed"<<std::endl;
    std::cout<<"file closed"<<std::endl;



    return 0;

}
