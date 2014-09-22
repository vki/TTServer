/*
 * ClientConn.h
 *
 *  Created on: 2013-6-21
 *      Author: ziteng@mogujie.com
 */

#ifndef CLIENTCONN_H_
#define CLIENTCONN_H_

#include "imconn.h"

enum {
	CONN_TYPE_LOGIN_SERV = 1,
	CONN_TYPE_MSG_SERV
};

class ClientConn : public CImConn
{
public:
	ClientConn();
	virtual ~ClientConn();

	bool IsOpen() { return m_bOpen; }

	void Connect(const char* server_ip, uint16_t server_port, const char* user_name, const char* password, uint32_t conn_type);

	virtual void Close();

	virtual void OnConfirm();
	virtual void OnClose();
	virtual void OnTimer(uint64_t curr_tick);

	virtual void HandlePdu(CImPdu* pPdu);
private:
	void _HandleMsgServResponse(CImPduMsgServResponse* pPdu);
	void _HandleLoginResponse(CImPduLoginResponse* pPdu);
	void _HandleClientFriendList(CImPduClientFriendList* pPdu);
	void _HandleClientOnlineFriendList(CImPduClientOnlineFriendList* pPdu);
	void _HandleClientFriendNotify(CImPduClientFriendNotify* pPdu);
	void _HandleClientMsgData(CImPduClientMsgData* pPdu);
	void _HandleClientUnreadMsgCntResponse(CImPduClientUnreadMsgCntResponse* pPdu);
	void _HandleClientMsgListResponse(CImPduClientMsgListResponse* pPdu);
	void _HandleClientP2PCmdMsg(CImPduClientP2PCmdMsg* pPdu);
	void _HandleClientUserInfoResponse(CImPduClientUserInfoResponse* pPdu);
	void _HandleClientMsgDataAck(CImPduClientMsgDataAck* pPdu);
	void _HandleKickUser(CImPduKickUser* pPdu);
	void _HandleClientGroupListResponse(CImPduClientGroupListResponse* pPdu);
	void _HandleClientGroupUserListResponse(CImPduClientGroupUserListResponse* pPdu);
    void _HandleClientDepartmentResponse(CImPduClientDepartmentResponse* pPdu);
private:
    string      m_user_name;
    string      m_password;
	uint32_t	m_user_id;
	string		m_id_url;
	string		m_uuid;
	int			m_conn_type;
	bool 		m_bOpen;
};

void init_client_conn(bool display_pkt_info = true);
ClientConn* get_client_conn(uint32_t user_id);

#endif /* CLIENTCONN_H_ */
