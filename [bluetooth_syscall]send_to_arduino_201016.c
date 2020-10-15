#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>

#include <linux/delay.h>
#include <linux/timer.h>
#include <linux/net.h>
#include <net/sock.h>
#include <linux/tcp.h>
#include <linux/in.h>
#include <asm/uaccess.h>
#include <linux/socket.h>
#include <linux/slab.h>
#include <net/bluetooth/bluetooth.h>
#include <net/bluetooth/rfcomm.h>

static struct socket *conn_socket = NULL;

int tcp_client_send(struct socket *sock, const char *buf, const size_t length, unsigned long flags){
	struct msghdr msg;
	struct kvec vec;
	int len,written = 0, left = length;
	
	mm_segment_t oldmm;

	msg.msg_name = 0;
	msg.msg_namelen = 0;
	
	msg.msg_control = NULL;
	msg.msg_controllen = 0;
	msg.msg_flags = flags;

	// for using system call in kernel area
	oldmm = get_fs();
	set_fs(KERNEL_DS);

repeat_send:
	
	vec.iov_len = left;
	vec.iov_base = (char*)buf + written;

	len = kernel_sendmsg(sock,&msg,&vec,left,left);
	if((len == -ERESTARTSYS) || (!(flags * MSG_DONTWAIT) && (len == -EAGAIN))){
		pr_info("*** mtp | error while sending: %d | tcp_client_send *** \n",len);
		goto repeat_send;
	}
	if(len > 0){
		written += len;
		left -= len;
		if(left)
			goto repeat_send;
	}
	set_fs(oldmm);
	return written ? written:len;
}

void sys_send_to_arduino(void){
	struct sockaddr_rc saddr;
	int len = 49;
	char response[len + 1];
	char reply[len + 1];
	int ret = -1;

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
		pr_info(" *** mtp | Error: ^d while connecting using conn socket. | setup_connection *** \n",ret);
		goto err;
	}

	do{
		ssleep(1);
		memset(&reply,0,len + 1);
		strcat(reply,"HOLA");
		tcp_client_send(conn_socket,reply,strlen(reply), MSG_DONTWAIT);
	}while(1);
err:
	return;
}














































	





































	

	



































