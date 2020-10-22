#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>

#include <linux/net.h>
#include <net/sock.h>
#include <linux/tcp.h>
#include <linux/in.h>
#include <asm/uaccess.h>
#include <linux/socket.h>
#include <linux/slab.h>
#include <net/bluetooth/bluetooth.h>
#include <net/bluetooth/rfcomm.h>

MODULE_LICENSE("Dual BSD/GPL");

struct socket *conn_socket = NULL;

int tcp_client_receive(struct socket *sock, char *str, unsigned long flags){
	struct msghdr msg;
	struct kvec vec;
	int len;
	int max_size = 50;

	msg.msg_name = 0;
	msg.msg_namelen = 0;

	msg.msg_control = NULL;
	msg.msg_controllen = 0;
	msg.msg_flags = flags;

	vec.iov_len = max_size;
	vec.iov_base = str;

read_again:
	len = kernel_recvmsg(sock,&msg,&vec,max_size,max_size,flags);

	if(len == -EAGAIN || len == -ERESTARTSYS){
		pr_info("*** mtp | error while reading: %d | tcp_client_receive *** \n",len);
		goto read_again;
	}

	pr_info(" *** mtp | the server says: %s | tcp_client_receive *** \n",str);
	return len;
}

void recv_from_arduino(void){
	struct sockaddr_rc saddr;
	int len = 49;
	char response[len + 1];
	int ret = -1;
	int cnt = 0;

	DECLARE_WAIT_QUEUE_HEAD(recv_wait);

	ret = sock_create(AF_BLUETOOTH,SOCK_STREAM,BTPROTO_RFCOMM,&conn_socket);
	if(ret < 0){
		pr_info(" *** mtp | Error: %d while creating first socket. | setup_connection *** \n", ret);
		goto err;
	}

	memset(&saddr,0,sizeof(saddr));
	saddr.rc_family = AF_BLUETOOTH;
	saddr.rc_channel = 1;
	
	saddr.rc_bdaddr.b[0] = 66;
	saddr.rc_bdaddr.b[1] = 6;
	saddr.rc_bdaddr.b[2] = 24;
	saddr.rc_bdaddr.b[3] = 4;
	saddr.rc_bdaddr.b[4] = 23;
	saddr.rc_bdaddr.b[5] = 32;

	ret = conn_socket->ops->connect(conn_socket, (struct sockaddr *)&saddr, sizeof(saddr), O_RDWR);
	if(ret && (ret != -EINPROGRESS)){
		pr_info(" *** mtp | Error: %d while connecting using conn socket. | setup_connection *** \n",ret);
		goto err;
	}

	while(cnt < 30){
		cnt++;
		pr_info("count num %d \n",cnt);
		wait_event_timeout(recv_wait,!skb_queue_empty(&conn_socket->sk->sk_receive_queue),5*HZ);
		if(!skb_queue_empty(&conn_socket->sk->sk_receive_queue)){
			memset(&response,0,len+1);
			tcp_client_receive(conn_socket,response,MSG_DONTWAIT);
		}
err:
	return;
}

int __init network_client_init(void){
	pr_info(" ** mtp | network client init | network_client_init *** \n");
	recv_from_arduino();
	return 0;
}

void __exit network_client_exit(void){
	int len = 49;
	char response[len + 1];

	DECLARE_WAIT_QUEUE_HEAD(exit_wait);
	wait_event_timeout(exit_wait,!skb_queue_empty(&conn_socket->sk->sk_receive_queue),5*HZ);
	if(!skb_queue_empty(&conn_socket->sk->sk_receive_queue)){
		memset(&response,0,len+1);
		tcp_client_receive(conn_socket,response,MSG_DONTWAIT);
	}

	if(conn_socket != NULL){
		sock_release(conn_socket);
	}
	pr_info(" *** mtp | network client exiting | network_client_exit *** \n");
}

module_init(network_client_init);
module_exit(network_client_exit);

