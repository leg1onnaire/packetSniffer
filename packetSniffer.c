#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/if_ether.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>
#include <netinet/udp.h>
#include <arpa/inet.h>
#include <net/ethernet.h> // Added for struct ether_header
#include <netinet/tcp.h>  // Added for struct tcphdr



#define BUFFER_SIZE 1514
int sockfd;
FILE *logFile;


void *packetReceiver(void *arg){
    int sockfd = *((int*)arg);
    struct ether_header eth_header;
    unsigned char buffer [BUFFER_SIZE];

    while(1){
        ssize_t bytes_received = recv(sockfd,buffer,BUFFER_SIZE,0);
        if(bytes_received < 0){
            perror("Error receiving packet ");
            continue;
        }
        memcpy(&eth_header,buffer,sizeof(struct ether_header));

        fprintf(logFile,"Received an ethernet frame\n");
        fprintf(logFile,"Source MAC : %02X:%02X:%02X:%02X:%02X:%02X \n",
        eth_header.ether_shost[0],eth_header.ether_shost[1],
        eth_header.ether_shost[2],eth_header.ether_shost[3],
        eth_header.ether_shost[4],eth_header.ether_shost[5]);
        fprintf(logFile,"Destination MAC : %02X:%02X:%02X:%02X:%02X:%02X \n",
        eth_header.ether_dhost[0],eth_header.ether_dhost[1],
        eth_header.ether_dhost[2],eth_header.ether_dhost[3],
        eth_header.ether_dhost[4],eth_header.ether_dhost[5]);
        fprintf(logFile,"received an ethernet frame\n");


        //fprintf(logFile,"Received an ethernet frame\n");
       
    }
    return NULL;
}



void *protocolExtractor(void *arg){
    int sockfd = *((int *)arg);
    unsigned char buffer[BUFFER_SIZE];

    while(1){
        ssize_t bytes_received = recv(sockfd,buffer,BUFFER_SIZE,0);
        if(bytes_received < 0){
            perror("Error receiving packet ");
            continue;
        }
        struct iphdr *ip_header = (struct iphdr *)(buffer +sizeof(struct ether_header));

        struct in_addr src_ip,dest_ip;
        src_ip.s_addr = ip_header->saddr;
        dest_ip.s_addr = ip_header->daddr;

        char src_ip_str[INET_ADDRSTRLEN];
        char dest_ip_str[INET_ADDRSTRLEN];

        inet_ntop(AF_INET,&(src_ip.s_addr),src_ip_str,INET_ADDRSTRLEN);
        inet_ntop(AF_INET,&(dest_ip.s_addr),dest_ip_str,INET_ADDRSTRLEN);

        
        int ip_packet_size = ntohs(ip_header->tot_len);
        
        if(ip_header->protocol == IPPROTO_TCP){
            struct tcphdr *tcp_header = (struct tcphdr *)(buffer +sizeof(struct ether_header)+ip_header->ihl *4);
            //printf("Source IP is : %s, Destination Ip is :%s\n",inet_ntoa(*(struct in_addr*)&src_ip),inet_ntoa(*(struct in_addr*)&dest_ip));
            //printf("Source IP is : %s, Destination Ip is :%s\n",inet_ntoa(src_ip),inet_ntoa(dest_ip));
            fprintf(logFile,"Source IP is : %08X, Destination Ip is :%08X\n",ip_header->saddr,ip_header->daddr);
            fprintf(logFile,"Source IP is : %s, Destination Ip is :%s\n",src_ip_str,dest_ip_str);
            fprintf(logFile,"TCP Source Port:%d,Destination Port :%d,Packet Size :%d bytes \n",ntohs(tcp_header->source),ntohs(tcp_header->dest),ip_packet_size);
        }
        else if(ip_header->protocol == IPPROTO_UDP) {
            struct udphdr *udp_header = (struct udphdr *)(buffer +sizeof(struct ether_header)+ip_header->ihl *4);
            //printf("Source IP is : %s, Destination Ip is :%s\n",inet_ntoa(*(struct in_addr*)&src_ip),inet_ntoa(*(struct in_addr*)&dest_ip));
            //printf("Source IP is : %s, Destination Ip is :%s\n",inet_ntoa(src_ip),inet_ntoa(dest_ip));
            fprintf(logFile,"Source IP is : %08X, Destination Ip is :%08X\n",ip_header->saddr,ip_header->daddr);
            fprintf(logFile,"Source IP is : %s, Destination Ip is :%s\n",src_ip_str,dest_ip_str);
            fprintf(logFile,"UDP Source Port :%d,Destination Port : %d,Packet Size :%d bytes \n",ntohs(udp_header->source),ntohs(udp_header->dest),ip_packet_size);            
        }
        else{
         fprintf(logFile,"IP Protocol : %d,Packet Size:%d bytes \n",ip_header ->protocol,ip_packet_size);
        fprintf(logFile,"IP Protocol : %d,Packet Size:%d bytes \n",ip_header ->protocol,ip_packet_size);
        fprintf(logFile,"Source IP is : %s, Destination Ip is :%s\n",src_ip_str,dest_ip_str);

        }

       
    }
    return NULL;
}
int main(){
    
    pthread_t ethernetThread,protocolThread;
    sockfd = socket(AF_PACKET,SOCK_RAW,htons(ETH_P_ALL));

    int choice;
    printf("Select an option:\n");
    printf("1.Run ethernet \n");
    printf("2.Run protocol \n");
    printf("Enter your choice :");
    scanf("%d",&choice);

    logFile =fopen("logfileMac.txt","wb");
    if(logFile == NULL){
        perror("error openning log file ");
        close(sockfd);
        return 1;
    }

     
    
    if(sockfd < 0){
        perror("error creating socket ");
        return 1;        
    }
    if(choice == 1){
         pthread_create(&ethernetThread,NULL,packetReceiver,&sockfd);
          pthread_join(ethernetThread,NULL);
          
         
    }
   if(choice == 2){
     pthread_create(&protocolThread,NULL,protocolExtractor,&sockfd);
     pthread_join(protocolThread,NULL);

   }
   else {
    printf("invalid choice \n");
    close(sockfd);
    return 1;
   }
     pthread_create(&ethernetThread,NULL,packetReceiver,&sockfd);
     pthread_create(&protocolThread,NULL,protocolExtractor,&sockfd);
     
    
    

    pthread_join(ethernetThread,NULL);
    pthread_join(protocolThread,NULL);

    fclose(logFile);
    close(sockfd);

    return 0;

}
