/*
 * pressure_test.cpp
 *
 *  Created on: 2013-7-11
 *      Author: ziteng@mogujie.com
 */
#include "ClientConn.h"
#include "netlib.h"
#include "TokenValidator.h"
#include <stdio.h>

#define MAX_BUF_SIZE 4096
#define CONN_NUM_PER_TIMER	10
#define MAX_ID_RANGE	1000

#define AUTH_ENCRYPT_KEY "Mgj!@#123"

uchar_t g_tmp_buf[MAX_BUF_SIZE];
char g_line_buf[256];
char g_curl_cmd_buf[256];
char g_test_uuid[256];

string 	 g_server_ip;
uint16_t g_server_port;
uint32_t g_start_user_id;
uint32_t g_stop_user_id;
uint32_t g_request_id = 0;
uint32_t g_pkt_per_second;
uint32_t g_pkt_size;
uint32_t g_current_user_id;

uint32_t g_msg_user_id;

void pressure_test_callback(void* callback_data, uint8_t msg, uint32_t handle, void* pParam)
{
	char token[64];
	for (uint32_t i = 0; i < CONN_NUM_PER_TIMER && g_current_user_id <= g_stop_user_id; i++, g_current_user_id++) {
		genToken(g_current_user_id, 0, token);
		ClientConn* pConn = new ClientConn();
		pConn->Connect(g_server_ip.c_str(), g_server_port, "", "", CONN_TYPE_LOGIN_SERV);
		printf("connnect %s:%d, user_id=%u, token=%s\n", g_server_ip.c_str(), g_server_port,
				g_current_user_id, token);
	}

	if (g_current_user_id >= g_stop_user_id) {
		for (uint32_t i = 0; i < CONN_NUM_PER_TIMER && g_msg_user_id <= g_stop_user_id; i++, g_msg_user_id++) {
			string from_id_url = idtourl(g_msg_user_id);
			string to_id_url = idtourl(24361096);

			ClientConn* pConn = get_client_conn(g_msg_user_id);
			if (pConn && pConn->IsOpen()) {
				CImPduClientMsgData pdu(0, from_id_url.c_str(), to_id_url.c_str(), get_tick_count(), 1, 0,
						4, (uchar_t*)"test", 0, NULL);
				pConn->SendPdu(&pdu);
				printf("msg from %u->24361096\n", g_msg_user_id);
			}
		}
	}

	/*for (uint32_t i = 0; i < g_pkt_per_second / 10; i++) {
		// from_user_id and to_user_id is different by 1 or 2, this guarantee the send and receiver in the same PC
		// so we can calculate the time takes for the message packet from sender to receiver
		uint32_t from_user_id = rand() % (g_stop_user_id - g_start_user_id) + g_start_user_id;
		uint32_t to_user_id = 0;
		do {
			to_user_id = rand() % (g_stop_user_id - g_start_user_id) + g_start_user_id;
		} while (to_user_id == from_user_id);

		string from_id_url = idtourl(from_user_id);
		string to_id_url = idtourl(to_user_id);

		ClientConn* pConn = get_client_conn(from_user_id);
		if (pConn && pConn->IsOpen()) {
			CImPduClientMsgData pdu(0, from_id_url.c_str(), to_id_url.c_str(), get_tick_count(), 1, 0,
					g_pkt_size, g_tmp_buf, 0, NULL);
			pConn->SendPdu(&pdu);
		}
	}*/
}

int main(int argc, char* argv[])
{
	if (argc != 7) {
		printf("Usage: ./pressure_test server_ip server_port start_user_id stop_user_id pkt_per_second pkt_size\n");
		return -1;
	}

	for (int i = 0; i < MAX_BUF_SIZE; i += 8) {
		memcpy(g_tmp_buf + i, "test_msg", 8);
	}

	g_server_ip = argv[1];
	g_server_port = (uint16_t)atoi(argv[2]);
	g_start_user_id = (uint32_t)atoi(argv[3]);
	g_stop_user_id = (uint32_t)atoi(argv[4]);
	g_pkt_per_second = (uint32_t)atoi(argv[5]);
	g_pkt_size = (uint32_t)atoi(argv[6]);

	g_current_user_id = g_start_user_id;
	g_msg_user_id = g_start_user_id;

	if (g_pkt_size > MAX_BUF_SIZE) {
		printf("packet size too big\n");
		return -1;
	}

	signal(SIGPIPE, SIG_IGN);

	srand(time(NULL));

	int ret = netlib_init();

	if (ret == NETLIB_ERROR)
		return ret;

	init_client_conn(false);

	netlib_register_timer(pressure_test_callback, NULL, 100);

	netlib_eventloop();

	return 0;
}
