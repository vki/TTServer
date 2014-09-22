/*
 * ClientConn.cpp
 *
 *  Created on: 2013-6-21
 *      Author: ziteng@mogujie.com
 */

#include "ClientConn.h"

/*
 * message delay level:
 * 0 - [0, 100) millisecond
 * 1 - [100, 200) millisecond
 *  ...
 * 9 - [900, 1000) millisecond
 * 10 - >= 1 second
 */

#define MAX_MSG_DELAY_LEVEL	10

static ConnMap_t g_client_conn_map;
static UserMap_t g_client_user_map;

static bool g_dispaly_pkt_info = true;

static uint32_t g_msg_delay_cnt[MAX_MSG_DELAY_LEVEL + 1];
static uint32_t g_display_dealy_cnt = 0;

static void client_conn_timer_callback(void* callback_data, uint8_t msg, uint32_t handle, void* pParam)
{
	ConnMap_t::iterator it_old;
	ClientConn* pConn = NULL;
	uint64_t cur_time = get_tick_count();

	for (ConnMap_t::iterator it = g_client_conn_map.begin(); it != g_client_conn_map.end(); ) {
		it_old = it;
		it++;

		pConn = (ClientConn*)it_old->second;
		pConn->OnTimer(cur_time);
	}

	// only execute in pressure test
	if (!g_dispaly_pkt_info) {
		g_display_dealy_cnt++;
		if (g_display_dealy_cnt % 60 == 0) {
			for (int i = 0; i < MAX_MSG_DELAY_LEVEL + 1; i++) {
				log("level%d: %u\n", i, g_msg_delay_cnt[i]);
			}
		}
	}
}

void init_client_conn(bool display_pkt_info)
{
	g_dispaly_pkt_info = display_pkt_info;

	for (int i = 0; i < MAX_MSG_DELAY_LEVEL + 1; i++) {
		g_msg_delay_cnt[i] = 0;
	}

	netlib_register_timer(client_conn_timer_callback, NULL, 1000);
}

ClientConn* get_client_conn(uint32_t user_id)
{
	ClientConn* pConn = NULL;
	UserMap_t::iterator iter = g_client_user_map.find(user_id);
	if (iter != g_client_user_map.end()) {
		pConn = (ClientConn*)iter->second;
	}

	return pConn;
}

ClientConn::ClientConn()
{
	m_bOpen = false;
}

ClientConn::~ClientConn()
{

}

void ClientConn::Connect(const char* server_ip, uint16_t server_port, const char* user_name,
                         const char* password, uint32_t conn_type)
{
	m_conn_type = conn_type;
    m_user_name = string(user_name);
    m_password = string(password);
	//m_user_id = user_id;
	//m_uuid = uuid;
	//m_id_url = idtourl(user_id);

	m_handle = netlib_connect(server_ip, server_port, imconn_callback, (void*)&g_client_conn_map);

	if (m_handle != NETLIB_INVALID_HANDLE) {
		g_client_conn_map.insert(make_pair(m_handle, this));
	}

	if (m_conn_type == CONN_TYPE_MSG_SERV) {
		g_client_user_map.insert(make_pair(m_user_id, this));
	}
}

void ClientConn::Close()
{
	if (m_handle != NETLIB_INVALID_HANDLE) {
		netlib_close(m_handle);
		g_client_conn_map.erase(m_handle);
	}

	if (m_conn_type == CONN_TYPE_MSG_SERV) {
		g_client_user_map.erase(m_user_id);
	}

	ReleaseRef();
}

void ClientConn::OnConfirm()
{
	if (m_conn_type == CONN_TYPE_LOGIN_SERV) {
		CImPduMsgServRequest pdu;
		SendPdu(&pdu);
	} else {
		CImPduLoginRequest pdu(m_user_name.c_str(), m_password.c_str(), USER_STATUS_ONLINE, CLIENT_TYPE_WINDOWS,"TEST/1.0");
		SendPdu(&pdu);
	}
}

void ClientConn::OnClose()
{
	log("onclose from handle=%d\n", m_handle);
	Close();
}

void ClientConn::OnTimer(uint64_t curr_tick)
{
	if (curr_tick > m_last_send_tick + CLIENT_HEARTBEAT_INTERVAL) {
		CImPduHeartbeat pdu;
		SendPdu(&pdu);
	}

	if (curr_tick > m_last_recv_tick + CLIENT_TIMEOUT) {
		log("conn to msg_server timeout\n");
		Close();
	}
}

void ClientConn::HandlePdu(CImPdu* pPdu)
{
	switch (pPdu->GetPduType()) {
	case IM_PDU_TYPE_HEARTBEAT:
		//printf("Heartbeat\n");
		return;
	case IM_PDU_TYPE_MSG_SERV_RESPONSE:
		_HandleMsgServResponse( (CImPduMsgServResponse*)pPdu );
		break;
	case IM_PDU_TYPE_LOGIN_RESPONSE:
		_HandleLoginResponse( (CImPduLoginResponse*)pPdu );
		break;
	case IM_PDU_TYPE_CLIENT_FRIEND_LIST:
		_HandleClientFriendList( (CImPduClientFriendList*)pPdu );
        break;

	case IM_PDU_TYPE_CLIENT_ONLINE_FRIEND_LIST:
		_HandleClientOnlineFriendList( (CImPduClientOnlineFriendList*)pPdu );
		break;
	case IM_PDU_TYPE_CLIENT_FRIEND_NOTIFY:
		_HandleClientFriendNotify( (CImPduClientFriendNotify*)pPdu );
		break;

	case IM_PDU_TYPE_CLIENT_MSG_DATA:
		_HandleClientMsgData( (CImPduClientMsgData*)pPdu );
		break;
	case IM_PDU_TYPE_CLIENT_UNREAD_MSG_CNT_RESPONSE:
		_HandleClientUnreadMsgCntResponse( (CImPduClientUnreadMsgCntResponse*)pPdu );
		break;
	case IM_PDU_TYPE_CLIENT_MSG_LIST_RESPONSE:
		_HandleClientMsgListResponse( (CImPduClientMsgListResponse*)pPdu );
		break;
	case IM_PDU_TYPE_CLIENT_P2P_CMD_MSG:
		_HandleClientP2PCmdMsg( (CImPduClientP2PCmdMsg*)pPdu );
		break;
	case IM_PDU_TYPE_CLIENT_USER_INFO_RESPONSE:
		_HandleClientUserInfoResponse( (CImPduClientUserInfoResponse*)pPdu );
		break;
	case IM_PDU_TYPE_CLIENT_MSG_DATA_ACK:
		_HandleClientMsgDataAck( (CImPduClientMsgDataAck*)pPdu );
		break;
	case IM_PDU_TYPE_KICK_USER:
		_HandleKickUser( (CImPduKickUser*)pPdu );
		break;

	case IM_PDU_TYPE_CLIENT_GROUP_LIST_RESPONSE:
		_HandleClientGroupListResponse((CImPduClientGroupListResponse*)pPdu);
		break;
	case IM_PDU_TYPE_CLIENT_GROUP_USER_LIST_RESPONSE:
		_HandleClientGroupUserListResponse((CImPduClientGroupUserListResponse*)pPdu);
		break;
    case IM_PDU_TYPE_CLIENT_DEPARTMENT_RESPONSE:
        _HandleClientDepartmentResponse((CImPduClientDepartmentResponse*)pPdu);
        break;
	default:
		log("wrong msg_type=%d\n", pPdu->GetPduType());
		break;
	}
}

void ClientConn::_HandleMsgServResponse(CImPduMsgServResponse* pPdu)
{
	uint32_t result = pPdu->GetResult();
	string ip(pPdu->GetIP2Addr(), pPdu->GetIP2Len());
	uint16_t port = pPdu->GetPort();

	if (g_dispaly_pkt_info) {
		printf("receive MsgServResponse, result=%d, domain_name=%s, port=%d\n", result, ip.c_str(), port);
	}

	if (result == 0) {
		ClientConn* pConn = new ClientConn();
		pConn->Connect(ip.c_str(), port, m_user_name.c_str(), m_password.c_str(), CONN_TYPE_MSG_SERV);
	}

	Close();
}

void ClientConn::_HandleLoginResponse(CImPduLoginResponse* pPdu)
{
	if (g_dispaly_pkt_info) {
		printf("receive LoginResponse, result=%d\n", pPdu->GetResult());
	}

	if (pPdu->GetResult() == 0) {
		m_bOpen = true;
		if (pPdu->GetNicknameLen() != 0) {
			string nickname(pPdu->GetNickname(), pPdu->GetNicknameLen());
			if (g_dispaly_pkt_info)
				printf("nickname: %s, n", nickname.c_str());
		}
        
        m_id_url = string(pPdu->GetUserIdUrl(), pPdu->GetUserIdUrllen());
        m_user_id = urltoid(m_id_url.c_str());
		string nickname(pPdu->GetNickname(), pPdu->GetNicknameLen());
		string avatar_url(pPdu->GetAvatarUrl(), pPdu->GetAvatarLen());
        string title(pPdu->GetTitle(), pPdu->GetTitleLen());
        string position(pPdu->GetPosition(), pPdu->GetPositionLen());
        uint32_t RoleStatus = pPdu->GetRoleStatus();
        uint32_t Sex = pPdu->GetSex();
        string DepartId = string(pPdu->GetDepartIdUrl(), pPdu->GetDepartIdUrlLen());
        uint32_t job_num = pPdu->GetJobNum();
        string Telphone = string(pPdu->GetTelphone(), pPdu->GetTelphoneLen());
        string email = string(pPdu->GetEmail(), pPdu->GetEmailLen());
        string token = string(pPdu->GetToken(), pPdu->GetTokenLen());
		if (g_dispaly_pkt_info)
			printf("user_id: %d, nick_name: %s, avatar_url: %s, title: %s, position: %s, \
                   role_status: %d, sex: %d, departid: %d, job_num: %d, tel: %s, email: %s, \
                   token: %s", m_user_id,
                   nickname.c_str(), avatar_url.c_str(), title.c_str(), position.c_str(), RoleStatus,
                   Sex, urltoid(DepartId.c_str()), job_num, Telphone.c_str(), email.c_str(),
                   token.c_str());

		CImPduClientBuddyListRequest pdu(0);
		SendPdu(&pdu);
        
        CImPduClientDepartmentRequest pdu2;
        SendPdu(&pdu2);
        
        CImPduClientAllUserRequest pdu3;
        SendPdu(&pdu3);
	}
}

void ClientConn::_HandleClientFriendList(CImPduClientFriendList* pPdu)
{
	if (g_dispaly_pkt_info) {
		uint32_t friend_cnt = pPdu->GetFriendCnt();
		client_user_info_t* friend_list = pPdu->GetFriendList();

		printf("receive friend list response: \n");
		for (uint32_t i = 0; i < friend_cnt; i++) {
			string friend_id_url(friend_list[i].id_url, friend_list[i].id_len);
			//string name_url(friend_list[i].name, friend_list[i].name_len);
			//string avatar_url(friend_list[i].avatar_url, friend_list[i].avatar_len);

			printf("\t %s, updated: %d.\n", friend_id_url.c_str(), friend_list[i].user_updated);
		}
		printf("\n");
	}
}

void ClientConn::_HandleClientOnlineFriendList(CImPduClientOnlineFriendList* pPdu)
{
	if (g_dispaly_pkt_info) {
		uint32_t friend_cnt = pPdu->GetFriendCnt();
		client_stat_t* friend_stat_list = pPdu->GetFriendStatList();

		printf("receive online friend list, list_type=%u: \n", pPdu->GetListType());
		for (uint32_t i = 0; i < friend_cnt; i++) {
			string friend_id_url(friend_stat_list[i].id_url, friend_stat_list[i].id_len);
			uint32_t friend_id = urltoid(friend_id_url.c_str());
			uint32_t friend_status = friend_stat_list[i].status;

			printf("id: %u, status: %u | ", friend_id, friend_status);
		}
		printf("\n");
	}
}

void ClientConn::_HandleClientFriendNotify(CImPduClientFriendNotify* pPdu)
{
	if (g_dispaly_pkt_info) {
		string id_url(pPdu->GetIdUrl(), pPdu->GetIdLen());
		uint32_t id = urltoid(id_url.c_str());
		uint32_t status = pPdu->GetStatus();

		printf("friend user_id=%u, status=%u\n", id, status);
	}
}

void ClientConn::_HandleClientMsgData(CImPduClientMsgData* pPdu)
{
	uint32_t msg_seq_no = pPdu->GetSeqNo();
	uint32_t msg_delay_time = get_tick_count() - pPdu->GetCreateTime();
	uint32_t level = msg_delay_time / 100;
	if (level >= MAX_MSG_DELAY_LEVEL) {
		++g_msg_delay_cnt[MAX_MSG_DELAY_LEVEL];
	} else {
		++g_msg_delay_cnt[level];
	}

	CImPduClientMsgDataAck pdu(msg_seq_no, pPdu->GetFromIdLen(), pPdu->GetFromIdUrl());
	SendPdu(&pdu);

	if (g_dispaly_pkt_info) {
		string from_id(pPdu->GetFromIdUrl(), pPdu->GetFromIdLen());
		string to_id(pPdu->GetToIdUrl(), pPdu->GetToIdLen());
		string msg_content((char*)pPdu->GetMsgData(), pPdu->GetMsgLen());
		printf("msg data from %d to %d, seq_no=%u, msg_delay_time=%u, content: %s\n",
				urltoid(from_id.c_str()), urltoid(to_id.c_str()), msg_seq_no, msg_delay_time, msg_content.c_str());
	}
}

void ClientConn::_HandleClientUnreadMsgCntResponse(CImPduClientUnreadMsgCntResponse* pPdu)
{
	if (g_dispaly_pkt_info) {
		unread_info_t* user_unread_list = pPdu->GetUnreadList();
		uint32_t user_unread_cnt = pPdu->GetUnreadCnt();

		printf("_HandleUnreadMsgCountResponse, cnt=%d\n", user_unread_cnt);
		for (uint32_t i = 0; i < user_unread_cnt; i++) {
			string from_id_url(user_unread_list[i].id_url, user_unread_list[i].id_len);
			uint32_t from_id = urltoid(from_id_url.c_str());

			printf("from_user_id=%d, unread_cnt=%d\n", from_id, user_unread_list[i].unread_cnt);
		}
	}
}

void ClientConn::_HandleClientMsgListResponse(CImPduClientMsgListResponse* pPdu)
{
	if (g_dispaly_pkt_info) {
		string display_id_url(pPdu->GetDisplayId(), pPdu->GetDisplayIdLen());
		uint32_t display_id = urltoid(display_id_url.c_str());
		uint32_t msg_cnt = pPdu->GetMsgCnt();
		client_msg_t* msg_list = pPdu->GetMsgList();

		printf("_HandleMsgListResponse, to_user_id=%u, msg_cnt=%u\n", display_id, msg_cnt);
		for (uint32_t i = 0; i < msg_cnt; i++) {
			string msg_content((char*)msg_list[i].msg_content, msg_list[i].msg_len);
			printf("create_time=%u, type=%d, msg: %s\n", msg_list[i].create_time, msg_list[i].msg_type,
					msg_content.c_str());
		}
	}
}

void ClientConn::_HandleClientP2PCmdMsg(CImPduClientP2PCmdMsg* pPdu)
{
	if (g_dispaly_pkt_info) {
		string from_id_url(pPdu->GetFromId(), pPdu->GetFromIdLen());
		string to_id_url(pPdu->GetToId(), pPdu->GetToIdLen());
		uint32_t from_user_id = urltoid(from_id_url.c_str());
		uint32_t to_user_id = urltoid(to_id_url.c_str());
		string msg((char*)pPdu->GetCmdMsgData(), pPdu->GetCmdMsgLen());
		uint32_t seq_no = pPdu->GetSeqNo();

		printf("_HandleClientP2PCmdMsg,seq_no=%u, %u->%u, msg: %s\n", seq_no, from_user_id, to_user_id, msg.c_str());
	}
}


void ClientConn::_HandleClientUserInfoResponse(CImPduClientUserInfoResponse* pPdu)
{
	if (g_dispaly_pkt_info) {
		uint32_t user_cnt = pPdu->GetUserCnt();
		client_user_info_t* user_info_list = pPdu->GetUserInfoList();

		printf("_HandleClientUserInfoResponse, user_cnt=%u\n", user_cnt);
		for (uint32_t i = 0; i < user_cnt; i++) {
			string id_url(user_info_list[i].id_url, user_info_list[i].id_len);
			string name(user_info_list[i].name, user_info_list[i].name_len);
			uint32_t user_id = urltoid(id_url.c_str());
            string nickname(user_info_list[i].nick_name, user_info_list[i].nick_name_len);
            string avatar_url(user_info_list[i].avatar_url, user_info_list[i].avatar_len);
            string title(user_info_list[i].title, user_info_list[i].title_len);
            string position(user_info_list[i].position, user_info_list[i].position_len);
            uint32_t RoleStatus = user_info_list[i].role_status;
            uint32_t Sex = user_info_list[i].sex;
            string DepartId(user_info_list[i].depart_id_url, user_info_list[i].depart_id_len);
            uint32_t job_num = user_info_list[i].job_num;
            string Telphone(user_info_list[i].telphone, user_info_list[i].telphone_len);
            string email(user_info_list[i].email, user_info_list[i].email_len);
			printf("user%u info: use_id=%u, name=%s, nick_name=%s, avatar_url=%s, title=%s, position\
                   =%s, RoleStatus=%d, sex=%d, departid=%d, job_num=%d, telphone=%s, email=%s.\n",
                   i + 1, user_id, name.c_str(), nickname.c_str(), avatar_url.c_str(), title.c_str(),
                   position.c_str(), RoleStatus, Sex, urltoid(DepartId.c_str()), job_num,
                   Telphone.c_str(), email.c_str());
		}
	}
}

void ClientConn::_HandleClientMsgDataAck(CImPduClientMsgDataAck* pPdu)
{
	if (g_dispaly_pkt_info) {
		printf("_HandleClientMsgDataAck, seq=%u\n", pPdu->GetSeqNo());
	}
}

void ClientConn::_HandleKickUser(CImPduKickUser* pPdu)
{
	uint32_t user_id = pPdu->GetUserId();
	uint32_t reason = pPdu->GetReason();
	printf("_HandleKickUser, user_id=%u, reason=%u\n", user_id, reason);
}

void ClientConn::_HandleClientGroupListResponse(CImPduClientGroupListResponse* pPdu)
{
	uint32_t group_cnt = pPdu->GetGroupCnt();
	client_group_info_t* group_list = pPdu->GetGroupList();
	printf("_HandleClientGroupListResp, group_cnt=%u\n", group_cnt);

	for (uint32_t i = 0; i < group_cnt; i++) {
		string group_id_url(group_list[i].group_id, group_list[i].group_id_len);
		string group_name(group_list[i].group_name, group_list[i].group_name_len);
		uint32_t group_id = urltoid(group_id_url.c_str());
		printf("group_id=%u, group_name=%s\n", group_id, group_name.c_str());
	}

}

void ClientConn::_HandleClientGroupUserListResponse(CImPduClientGroupUserListResponse* pPdu)
{
	string group_id_url(pPdu->GetGroupId(), pPdu->GetGroupIdLen());
	uint32_t group_id = urltoid(group_id_url.c_str());
	uint32_t result = pPdu->GetResult();

	printf("_HandleClientGroupUserListResp, group_id=%u, result=%u\n", group_id, result);
	if (!result) {
		string group_name(pPdu->GetGroupName(), pPdu->GetGroupNameLen());
		string group_avatar(pPdu->GetGroupAvatar(), pPdu->GetGroupAvatarLen());

		uint32_t user_cnt = pPdu->GetUserCnt();
		user_id_t* user_list = pPdu->GetUserList();
		for (uint32_t i = 0; i < user_cnt; i++) {
			string user_id_url(user_list[i].id_url, user_list[i].id_len);
			uint32_t user_id = urltoid(user_id_url.c_str());
			printf("user_id=%u\n", user_id);
		}
	}
}

void ClientConn::_HandleClientDepartmentResponse(CImPduClientDepartmentResponse *pPdu)
{
    client_department_info_t* departlist = pPdu->GetDepartmentList();
    uint32_t departcnt = pPdu->GetDepartCnt();
    for (uint32_t i = 0; i < departcnt; i++) {
        string departid_url(departlist[i].depart_id_url,
                        departlist[i].depart_id_url_len);
        string title(departlist[i].title, departlist[i].title_len);
        string description(departlist[i].description, departlist[i].description_len);
        string parent_depart_id_url(departlist[i].parent_depart_id_url,
                                    departlist[i].parent_depart_id_url_len);
        string leader(departlist[i].leader, departlist[i].leader_len);
        uint32_t status = departlist[i].status;
        printf("_HandleClientDepartmentResponse, %d, departid=%d, title=%s, description=%s, \
               parent_depart_id=%d, leader=%s, status=%d.", i+1, urltoid(departid_url.c_str()),
                title.c_str(), description.c_str(), urltoid(parent_depart_id_url.c_str()),
                                                            leader.c_str(), status);
        
    }
}
