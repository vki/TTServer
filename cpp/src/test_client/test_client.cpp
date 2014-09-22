/*
 * test_client.cpp
 *
 *  Created on: 2013-6-21
 *      Author: ziteng@mogujie.com
 */

#include "ClientConn.h"
#include "netlib.h"
#include "TokenValidator.h"

#define TEST_UUID	"6b509064f9f081b5d78b65b8c20ce8b5"
#define PROMPT		"im-client> "
#define MAX_LINE_LEN	1024

string g_cmd_string[10];
int g_cmd_num;
uint32_t g_msg_id = 0;
uint32_t g_user_id;
string g_user_id_url;
string g_uuid;
string g_user_name;
string g_user_password;
uint32_t g_seq_no = 0;

void split_cmd(char* buf)
{
	int len = strlen(buf);
	string element;

	g_cmd_num = 0;
	for (int i = 0; i < len; i++) {
		if (buf[i] == ' ' || buf[i] == '\t') {
			if (!element.empty()) {
				g_cmd_string[g_cmd_num++] = element;
				element.clear();
			}
		} else {
			element += buf[i];
		}
	}

	// put the last one
	if (!element.empty()) {
		g_cmd_string[g_cmd_num++] = element;
	}
}

void print_help()
{
	printf("Usage:\n");
	printf("connect serv_ip serv_port user_id\n");
	printf("close\n");
	printf("send msg_type user_id msg_content\n");
	printf("unread_msg_count\n");
	printf("unread_msg from_user_id\n");
	printf("history_msg from_user_id msg_offset msg_count\n");
	printf("msg_read_ack from_user_id\n");
	printf("get_user_info user_cnt user_1_id user_2_id ...\n");
	printf("p2p_msg to_user_id msg_data\n");
	printf("service_set key value\n");
	printf("service_request shop_id\n");
	printf("query user_id\n");
	printf("groups\n");
	printf("group_member group_id\n");
	printf("quit\n");
}

void exec_cmd()
{
	if (g_cmd_num == 0) {
		return;
	}

	if (strcmp(g_cmd_string[0].c_str(), "connect") == 0) {
		if (g_cmd_num != 5) {
			print_help();
			return;
		}

		const char* server_ip = g_cmd_string[1].c_str();
		uint16_t server_port = atoi(g_cmd_string[2].c_str());
        g_user_name = g_cmd_string[3].c_str();
        g_user_password = g_cmd_string[3].c_str();
		//g_user_id_url = g_cmd_string[3].c_str();
		//g_user_id = urltoid(g_user_id_url.c_str());
		//char token[64];
		//genToken(g_user_id, 0, token);

		ClientConn* pConn = new ClientConn();
		pConn->Connect(server_ip, server_port , g_user_name.c_str(), g_user_password.c_str(),
                       CONN_TYPE_LOGIN_SERV);
	} else if (strcmp(g_cmd_string[0].c_str(), "close") == 0) {
		ClientConn* pConn = get_client_conn(g_user_id);
		if (pConn) {
			pConn->Close();
		}
	} else if (strcmp(g_cmd_string[0].c_str(), "send") == 0) {
		if (g_cmd_num != 4) {
			print_help();
			return;
		}

		uint8_t msg_type = atoi(g_cmd_string[1].c_str());
		uint32_t to_user_id = atoi(g_cmd_string[2].c_str());
		const char* msg_content = g_cmd_string[3].c_str();
		uint32_t msg_len = strlen(msg_content);
		string from_id_url = idtourl(g_user_id);
		string to_id_url = idtourl(to_user_id);

		CImPduClientMsgData pdu(++g_seq_no,  from_id_url.c_str(), to_id_url.c_str(), get_tick_count(),
				msg_type, 0, msg_len, (uchar_t*)msg_content, 0, NULL);
		ClientConn* pConn = get_client_conn(g_user_id);
		if (pConn) {
			pConn->SendPdu(&pdu);
		}
	} else if (strcmp(g_cmd_string[0].c_str(), "unread_msg_count") == 0) {
		ClientConn* pConn = get_client_conn(g_user_id);
		if (pConn) {
			CImPduClientUnreadMsgCntRequest pdu;
			pConn->SendPdu(&pdu);
		}
	} else if (strcmp(g_cmd_string[0].c_str(), "unread_msg") == 0) {
		if (g_cmd_num != 2) {
			print_help();
			return;
		}

		ClientConn* pConn = get_client_conn(g_user_id);
		if (pConn) {
			uint32_t from_user_id = atoi(g_cmd_string[1].c_str());
			string from_id_url = idtourl(from_user_id);
			CImPduClientUnreadMsgRequest pdu(from_id_url.c_str());
			pConn->SendPdu(&pdu);
		}
	} else if (strcmp(g_cmd_string[0].c_str(), "history_msg") == 0) {
		if (g_cmd_num != 4) {
			print_help();
			return;
		}

		ClientConn* pConn = get_client_conn(g_user_id);
		if (pConn) {
			uint32_t from_user_id = atoi(g_cmd_string[1].c_str());
			string from_id_url = idtourl(from_user_id);
			uint32_t msg_offset = atoi(g_cmd_string[2].c_str());
			uint32_t msg_count = atoi(g_cmd_string[3].c_str());
			CImPduClientHistoryMsgRequest pdu(from_id_url.c_str(), msg_offset, msg_count);
			pConn->SendPdu(&pdu);
		}
	} else if (strcmp(g_cmd_string[0].c_str(), "msg_read_ack") == 0) {
		if (g_cmd_num != 2) {
			print_help();
			return;
		}

		ClientConn* pConn = get_client_conn(g_user_id);
		if (pConn) {
			uint32_t from_user_id = atoi(g_cmd_string[1].c_str());
			string from_id_url = idtourl(from_user_id);
			CImPduClientMsgReadAck pdu(from_id_url.c_str());
			pConn->SendPdu(&pdu);
		}
	} else if (strcmp(g_cmd_string[0].c_str(), "get_user_info") == 0) {
		ClientConn* pConn = get_client_conn(g_user_id);
		if (pConn) {
			uint32_t user_cnt = 1;
			client_id_t client_id_list;
			client_id_list.id_url = (char*)g_user_id_url.c_str();

			CImPduClientUserInfoRequest pdu(user_cnt, &client_id_list);
			pConn->SendPdu(&pdu);
		}
	} else if (strcmp(g_cmd_string[0].c_str(), "p2p_msg") == 0) {
		if (g_cmd_num != 3) {
			print_help();
			return;
		}

		ClientConn* pConn = get_client_conn(g_user_id);
		if (pConn) {
			uint32_t to_user_id = atoi(g_cmd_string[1].c_str());
			string to_id_url = idtourl(to_user_id);
			CImPduClientP2PCmdMsg pdu(++g_seq_no, g_user_id_url.c_str(), to_id_url.c_str(), g_cmd_string[2].length(), (uchar_t*)g_cmd_string[2].data());
			pConn->SendPdu(&pdu);
		}
	} else if (strcmp(g_cmd_string[0].c_str(), "service_staff_set") == 0) {
		if (g_cmd_num != 3) {
			print_help();
			return;
		}

		ClientConn* pConn = get_client_conn(g_user_id);
		if (pConn) {
			uint32_t key = atoi(g_cmd_string[1].c_str());
			uint32_t value = atoi(g_cmd_string[2].c_str());

			CImPduClientServiceSetting pdu(key, value);
			pConn->SendPdu(&pdu);
		}
	} else if (strcmp(g_cmd_string[0].c_str(), "query") == 0) {
		uint32_t user_id = atoi(g_cmd_string[1].c_str());
		ClientConn* pConn = get_client_conn(g_user_id);
		if (pConn) {
			char* id_url = idtourl(user_id);
			CImPduClientUserStatusRequest pdu(id_url);
			pConn->SendPdu(&pdu);
		}
	} else if (strcmp(g_cmd_string[0].c_str(), "groups") == 0) {
		ClientConn* pConn = get_client_conn(g_user_id);
		if (pConn) {
			CImPduClientGroupListRequest pdu(CID_GROUP_LIST_REQUEST);
			pConn->SendPdu(&pdu);
		}
	} else if (strcmp(g_cmd_string[0].c_str(), "group_member") == 0) {
		if (g_cmd_num != 2) {
			print_help();
			return;
		}

		ClientConn* pConn = get_client_conn(g_user_id);
		if (pConn) {
			uint32_t group_id = atoi(g_cmd_string[1].c_str());
			char* group_id_url = idtourl(group_id);
			CImPduClientGroupUserListRequest pdu(group_id_url);
			pConn->SendPdu(&pdu);
		}
	} else if (strcmp(g_cmd_string[0].c_str(), "quit") == 0) {
		exit(0);
	} else {
		print_help();
	}
}


class CmdThread : public CThread
{
public:
	void OnThreadRun()
	{
		while (true)
		{
			fprintf(stderr, "%s", PROMPT);	// print to error will not buffer the printed message

			if (fgets(m_buf, MAX_LINE_LEN - 1, stdin) == NULL)
			{
				fprintf(stderr, "fgets failed: %d\n", errno);
				continue;
			}

			m_buf[strlen(m_buf) - 1] = '\0';	// remove newline character

			split_cmd(m_buf);

			exec_cmd();
		}
	}
private:
	char	m_buf[MAX_LINE_LEN];
};

CmdThread g_cmd_thread;

int main(int argc, char* argv[])
{
	g_cmd_thread.StartThread();

	signal(SIGPIPE, SIG_IGN);

	int ret = netlib_init();

	if (ret == NETLIB_ERROR)
		return ret;

	init_client_conn();

	netlib_eventloop();

	return 0;
}
