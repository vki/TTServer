/*
 * file_server.cpp
 *
 *  Created on: 2013-12-9
 *      Author: ziteng@mogujie.com
 */

#include "FileConn.h"
#include "netlib.h"
#include "ConfigFileReader.h"
#include "version.h"

void file_serv_callback(void* callback_data, uint8_t msg, uint32_t handle, void* pParam)
{
	if (msg == NETLIB_MSG_CONNECT) {
		CFileConn* pConn = new CFileConn();
		pConn->OnConnect(handle);
	} else {
		log("!!!error msg: %d\n", msg);
	}
}

int main(int argc, char* argv[])
{
	if ((argc == 2) && (strcmp(argv[1], "-v") == 0)) {
		printf("Server Version: FileServer/%s\n", VERSION);
		printf("Server Build: %s %s\n", __DATE__, __TIME__);
		return 0;
	}

	signal(SIGPIPE, SIG_IGN);

	CConfigFileReader config_file("fileserver.conf");

	char* listen_ip = config_file.GetConfigName("ListenIP");
	char* str_listen_port = config_file.GetConfigName("ListenPort");

	if (!listen_ip || !str_listen_port) {
		log("config item missing, exit...\n");
		return -1;
	}

	uint16_t listen_port = atoi(str_listen_port);

	init_file_conn();

	int ret = netlib_init();

	if (ret == NETLIB_ERROR)
		return ret;

	CStrExplode listen_ip_list(listen_ip, ';');
	for (uint32_t i = 0; i < listen_ip_list.GetItemCnt(); i++) {
		ret = netlib_listen(listen_ip_list.GetItem(i), listen_port, file_serv_callback, NULL);
		if (ret == NETLIB_ERROR)
			return ret;
	}

	printf("server start listen on %s:%d\n", listen_ip, listen_port);
	printf("now enter the event loop...\n");

	netlib_eventloop();

	return 0;
}
