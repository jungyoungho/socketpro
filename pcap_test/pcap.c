#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pcap.h>    
#include <errno.h>
#include <sys/socket.h>
#include <net/ethernet.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>
#include <netinet/in.h>
#include <arpa/inet.h>

struct ip *iph;
struct tcphdr *tcph;


void view(u_char * , const struct pcap_pkthdr  *,  const u_char *);
int main(int argc, char **argv)
{
    char *dev;   
    char *net;
    char *mask;
    int ret;        
    char errbuf[PCAP_ERRBUF_SIZE];
    bpf_u_int32 netp; 
    bpf_u_int32 maskp; 
    struct in_addr net_addr, mask_addr;
    struct bpf_program fcode;
    pcap_t *pcd;
     
    if(!(dev = pcap_lookupdev(errbuf)))
    {
        printf("%s\n",errbuf);
        return 0;
    }
    ret = pcap_lookupnet(dev,&netp,&maskp,errbuf);
    if(ret < 0) 
    {
        printf("%s\n",errbuf);
        return 0;
    } 
  
    if(ret == -1)
    {
        printf("%s\n",errbuf);
        return 0;
    }

    printf("DEV: %s\n",dev);
    net_addr.s_addr = netp;
    net = inet_ntoa(net_addr);
    printf("NET address : %s\n", net);

    mask_addr.s_addr = maskp;
    mask = inet_ntoa(mask_addr);
    printf("NET mask : %s\n", mask);
    printf("====================================\n");
   
    pcd = pcap_open_live(dev, BUFSIZ,0 , -1, errbuf);
    if (pcd == NULL)
    {
        printf("%s\n", errbuf);
        return 0;
    }    
   
    if (pcap_loop(pcd,0,view,0))
    {
    perror(errbuf);
    return 0;
    }

	pcap_close(pcd);
	return 0;
}

void view(u_char *user, const struct pcap_pkthdr *pkthdr, const u_char *packet)
   {
    int len = 0;
    unsigned short ether_type;    
    int ch =0;
    int length=pkthdr->len;
    struct ether_header *ep;
    ep = (struct ether_header *)packet;  

        //MAC 주소 받아오기
        printf("Src MAC:%02x:%02x:%02x:%02x:%02x:%02x\n"
               "Drc MAC:%02x:%02x:%02x:%02x:%02x:%02x\n",
                ep->ether_shost[0],
                ep->ether_shost[1],
                ep->ether_shost[2],
                ep->ether_shost[3],
                ep->ether_shost[4],
                ep->ether_shost[5],
                ep->ether_dhost[0],
                ep->ether_dhost[1],
                ep->ether_dhost[2],
                ep->ether_dhost[3],
                ep->ether_dhost[4],
                ep->ether_dhost[5]);

    ether_type = ntohs(ep->ether_type);
    if (ether_type == ETHERTYPE_IP)
    {
        //IP정보 받아오기  
        packet += sizeof(struct ether_header);
        iph = (struct ip *)packet;
        printf("IP 패킷\n");
        printf("Version     : %d\n", iph->ip_v);
        printf("Header Len  : %d\n", iph->ip_hl);
        printf("Ident       : %d\n", ntohs(iph->ip_id));
        printf("TTL         : %d\n", iph->ip_ttl);   
        printf("Src Address : %s\n", inet_ntoa(iph->ip_src));
        printf("Dst Address : %s\n", inet_ntoa(iph->ip_dst));       

   
        if (iph->ip_p == IPPROTO_TCP)
        {
            tcph = (struct tcphdr *)(packet + iph->ip_hl * 4);
            printf("Src Port : %d\n" , ntohs(tcph->source));
            printf("Dst Port : %d\n" , ntohs(tcph->dest));
        }
        //패킷정보
        printf("Packet\n");
        while(length--)
        {  
           printf("%02x ", *(packet++)); 
           if ((++ch % 16) == 0) 
                printf("\n");
        }
    }
        else
    {
        printf("NO packet\n");
    }
    printf("\n\n"); 
    printf("===============================================\n");
}    
