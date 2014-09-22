/*
 * ImPduFile.h
 *
 *  Created on: 2013-12-9
 *      Author: ziteng@mogujie.com
 */

#ifndef IMPDUFILE_H_
#define IMPDUFILE_H_

#include "ImPduBase.h"

#define PARTIAL_FILE_SIZE		4096

enum {
	FILE_TYPE_ONLINE 	= 1,
	FILE_TYPE_OFFLINE	= 2,
};

// to/from MsgServer
#define IM_PDU_TYPE_CLIENT_FILE_REQUEST					510
#define IM_PDU_TYPE_CLIENT_FILE_RESPONSE				511
#define IM_PDU_TYPE_CLIENT_FILE_NOTIFY                  512
#define IM_PDU_TYPE_CLIENT_FILE_HAS_OFFLINE_REQ			516
#define IM_PDU_TYPE_CLIENT_FILE_HAS_OFFLINE_RES			517
#define IM_PDU_TYPE_CLIENT_FILE_ADD_OFFLINE_REQ			518
#define IM_PDU_TYPE_CLIENT_FILE_DEL_OFFLINE_REQ			519
// server pdu
#define IM_PDU_TYPE_FILE_REQUEST					550
#define IM_PDU_TYPE_FILE_RESPONSE					551
#define IM_PDU_TYPE_FILE_NOTIFY                     552
#define IM_PDU_TYPE_FILE_SERVER_IP_REQUEST          553
#define IM_PDU_TYPE_FILE_SERVER_IP_RESPONSE         554

#define IM_PDU_TYPE_MSG_FILE_TRANSFER_REQ           565
#define IM_PDU_TYPE_MSG_FILE_TRANSFER_RSP           566

// to storage server
#define IM_PDU_TYPE_FILE_HAS_OFFLINE_REQ			561
#define IM_PDU_TYPE_FILE_HAS_OFFLINE_RES			562
#define IM_PDU_TYPE_FILE_ADD_OFFLINE_REQ			563
#define IM_PDU_TYPE_FILE_DEL_OFFLINE_REQ			564

typedef struct {
	uint32_t 	from_id_len;
	char*		from_id_url;
    uint32_t    task_id_len;
    char*       task_id;
    uint32_t    file_name_len;
    char*       file_name;
	uint32_t	file_size;
} client_offline_file_t;

typedef struct {
	uint32_t	from_id;
	uint32_t    task_id_len;
    char*       task_id;
    uint32_t    file_name_len;
    char*       file_name;
	uint32_t	file_size;
} offline_file_t;

//////// to/from FileServer//////

/////// to/from MsgServer ////////
class DLL_MODIFIER CImPduClientFileRequest : public CImPdu
{
public:
	CImPduClientFileRequest(uchar_t* buf, uint32_t len);
	CImPduClientFileRequest(const char* from_id, const char* to_id, const char* file_name, uint32_t file_size);
	virtual ~CImPduClientFileRequest() {}

	virtual uint16_t GetPduType() { return IM_PDU_TYPE_CLIENT_FILE_REQUEST; }

	uint32_t GetFromIdLen() { return m_from_id_len; }
	char* GetFromId() { return m_from_id; }
	uint32_t GetToIdLen() { return m_to_id_len; }
	char* GetToId() { return m_to_id; }
	uint32_t GetFileNameLen() { return m_file_name_len; }
	char* GetFileName() { return m_file_name; }
	uint32_t GetFileSize() { return m_file_size; }
private:
	uint32_t	m_from_id_len;
	char*		m_from_id;
	uint32_t	m_to_id_len;
	char* 		m_to_id;
	uint32_t	m_file_name_len;
	char* 		m_file_name;
	uint32_t	m_file_size;
};

class DLL_MODIFIER CImPduClientFileResponse : public CImPdu
{
public:
	CImPduClientFileResponse(uchar_t* buf, uint32_t len);
	CImPduClientFileResponse(uint32_t result, const char* from_id, const char* to_id,
                             const char* file_name, const char* task_id, const char* listen_ip,
                             uint16_t listen_port);
	virtual ~CImPduClientFileResponse() {}

	virtual uint16_t GetPduType() { return IM_PDU_TYPE_CLIENT_FILE_RESPONSE; }

    uint32_t GetResult() { return m_result; }
	uint32_t GetFromIdLen() { return m_from_id_len; }
	char* GetFromId() { return m_from_id; }
	uint32_t GetToIdLen() { return m_to_id_len; }
	char* GetToId() { return m_to_id; }
	uint32_t GetFileNameLen() { return m_file_name_len; }
	char* GetFileName() { return m_file_name; }
    uint32_t GetTaskIdLen() { return m_task_id_len; }
    char* GetTaskId() { return m_task_id; }
	uint32_t GetListenIpLen() { return m_listen_ip_len; }
	char* GetListenIp() { return m_listen_ip; }
	uint16_t GetListenPort() { return m_listen_port; }
private:
    uint32_t    m_result;
	uint32_t	m_from_id_len;
	char*		m_from_id;
	uint32_t	m_to_id_len;
	char* 		m_to_id;
	uint32_t	m_file_name_len;
	char* 		m_file_name;
    uint32_t    m_task_id_len;
    char*       m_task_id;
	uint32_t	m_listen_ip_len;
	char*		m_listen_ip;
	uint16_t	m_listen_port;
};

class DLL_MODIFIER CImPduClientFileNotify : public CImPdu
{
public:
    CImPduClientFileNotify(uchar_t* buf, uint32_t len);
    CImPduClientFileNotify(const char* from_id, const char* to_id, const char* file_name,
                           uint32_t file_size, const char* task_id, const char* listen_ip,
                           uint16_t listen_port);
    virtual ~CImPduClientFileNotify() {}
    
    virtual uint16_t GetPduType() { return IM_PDU_TYPE_CLIENT_FILE_NOTIFY; }
    uint32_t GetFromIdLen() { return m_from_id_len; }
	char* GetFromId() { return m_from_id; }
	uint32_t GetToIdLen() { return m_to_id_len; }
	char* GetToId() { return m_to_id; }
	uint32_t GetFileNameLen() { return m_file_name_len; }
	char* GetFileName() { return m_file_name; }
    uint32_t GetFileSize() { return m_file_size; }
    uint32_t GetTaskIdLen() { return m_task_id_len; }
    char* GetTaskId() { return m_task_id; }
	uint32_t GetListenIpLen() { return m_listen_ip_len; }
	char* GetListenIp() { return m_listen_ip; }
	uint16_t GetListenPort() { return m_listen_port; }
private:
    uint32_t	m_from_id_len;
	char*		m_from_id;
	uint32_t	m_to_id_len;
	char* 		m_to_id;
	uint32_t	m_file_name_len;
	char* 		m_file_name;
    uint32_t    m_file_size;
    uint32_t    m_task_id_len;
    char*       m_task_id;
	uint32_t	m_listen_ip_len;
	char*		m_listen_ip;
	uint16_t	m_listen_port;
};

class DLL_MODIFIER CImPduClientFileHasOfflineReq : public CImPdu
{
public:
	CImPduClientFileHasOfflineReq(uchar_t* buf, uint32_t len);
	CImPduClientFileHasOfflineReq();
	virtual ~CImPduClientFileHasOfflineReq() {}

	virtual uint16_t GetPduType() { return IM_PDU_TYPE_CLIENT_FILE_HAS_OFFLINE_REQ; }
};

class DLL_MODIFIER CImPduClientFileHasOfflineRes : public CImPdu
{
public:
	CImPduClientFileHasOfflineRes(uchar_t* buf, uint32_t len);
	CImPduClientFileHasOfflineRes(uint32_t file_cnt = 0, offline_file_t* serv_file_list = NULL,
                                   const list<ip_addr_t>* ip_addr_list = NULL);
	virtual ~CImPduClientFileHasOfflineRes();

	virtual uint16_t GetPduType() { return IM_PDU_TYPE_CLIENT_FILE_HAS_OFFLINE_RES; }
	uint32_t GetFileCnt() { return m_file_cnt; }
	client_offline_file_t* GetFileList() { return m_file_list; }
    uint32_t GetIPAddrCnt() { return m_ip_addr_cnt; }
    ip_addr_t* GetIPAddrList() { return m_ip_addr_list; }
private:
	uint32_t 				m_file_cnt;
	client_offline_file_t* 	m_file_list;
    uint32_t                m_ip_addr_cnt;
    ip_addr_t*              m_ip_addr_list;
};

class DLL_MODIFIER CImPduClientFileAddOfflineReq : public CImPdu
{
public:
	CImPduClientFileAddOfflineReq(uchar_t* buf, uint32_t len);
	CImPduClientFileAddOfflineReq(const char* from_id, const char* to_id, const char* task_id, const char* file_name, uint32_t file_size);
	virtual ~CImPduClientFileAddOfflineReq() {}

	virtual uint16_t GetPduType() { return IM_PDU_TYPE_CLIENT_FILE_ADD_OFFLINE_REQ; }
	uint32_t GetFromIdLen() { return m_from_id_len; }
	char* GetFromId() { return m_from_id; }
	uint32_t GetToIdLen() { return m_to_id_len; }
	char* GetToId() { return m_to_id; }
    uint32_t GetTaskIdLen() { return m_task_id_len; }
    char*   GetTaskId() { return m_task_id; }
	uint32_t GetFileNameLen() { return m_file_name_len; }
	char* GetFileName() { return m_file_name; }
	uint32_t GetFileSize() { return m_file_size; }
private:
	uint32_t	m_from_id_len;
	char* 		m_from_id;
	uint32_t	m_to_id_len;
	char* 		m_to_id;
    uint32_t    m_task_id_len;
    char*       m_task_id;
    uint32_t    m_file_name_len;
    char*       m_file_name;
    uint32_t	m_file_size;
    
};

class DLL_MODIFIER CImPduClientFileDelOfflineReq : public CImPdu
{
public:
	CImPduClientFileDelOfflineReq(uchar_t* buf, uint32_t len);
	CImPduClientFileDelOfflineReq(const char* from_id, const char* to_id, const char* task_id);
	virtual ~CImPduClientFileDelOfflineReq() {}

	virtual uint16_t GetPduType() { return IM_PDU_TYPE_CLIENT_FILE_DEL_OFFLINE_REQ; }
	uint32_t GetFromIdLen() { return m_from_id_len; }
	char* GetFromId() { return m_from_id; }
	uint32_t GetToIdLen() { return m_to_id_len; }
	char* GetToId() { return m_to_id; }
	uint32_t GetTaskIdLen() { return m_task_id_len; }
    char* GetTaskId() { return m_task_id; }
private:
	uint32_t	m_from_id_len;
	char* 		m_from_id;
	uint32_t	m_to_id_len;
	char* 		m_to_id;
    uint32_t    m_task_id_len;
    char*       m_task_id;
};

////// for server pdu ////
#ifndef WIN32
class CImPduFileServerIPReq : public CImPdu
{
public:
    CImPduFileServerIPReq(uchar_t* buf, uint32_t len);
    CImPduFileServerIPReq();
    virtual ~CImPduFileServerIPReq(){}
    
    virtual uint16_t GetPduType() { return IM_PDU_TYPE_FILE_SERVER_IP_REQUEST; }
};

class CImPduFileServerIPRsp : public CImPdu
{
public:
    CImPduFileServerIPRsp(uchar_t* buf, uint32_t len);
    CImPduFileServerIPRsp(list<ip_addr_t>* ip_list);
    virtual ~CImPduFileServerIPRsp();
    
    virtual uint16_t GetPduType() { return IM_PDU_TYPE_FILE_SERVER_IP_RESPONSE; }
    
    uint32_t GetIPCnt() { return m_ip_cnt; }
    ip_addr_t* GetIPList() { return m_ip_list; }
private:
    uint32_t    m_ip_cnt;
    ip_addr_t*  m_ip_list;
};

class CImPduFileNotify : public CImPdu
{
public:
    CImPduFileNotify(uchar_t* buf, uint32_t len);
    CImPduFileNotify(uint32_t from_id, uint32_t to_id, const char* file_name,
                           uint32_t file_size, const char* task_id, const char* listen_ip,
                           uint16_t listen_port);
    virtual ~CImPduFileNotify() {}
    
    virtual uint16_t GetPduType() { return IM_PDU_TYPE_FILE_NOTIFY; }
	uint32_t GetFromId() { return m_from_id; }
	uint32_t GetToId() { return m_to_id; }
	uint32_t GetFileNameLen() { return m_file_name_len; }
	char* GetFileName() { return m_file_name; }
    uint32_t GetFileSize() { return m_file_size; }
    uint32_t GetTaskIdLen() { return m_task_id_len; }
    char* GetTaskId() { return m_task_id; }
	uint32_t GetListenIpLen() { return m_listen_ip_len; }
	char* GetListenIp() { return m_listen_ip; }
	uint16_t GetListenPort() { return m_listen_port; }
private:
    uint32_t	m_from_id;
	uint32_t	m_to_id;
	uint32_t	m_file_name_len;
	char* 		m_file_name;
    uint32_t    m_file_size;
    uint32_t    m_task_id_len;
    char*       m_task_id;
	uint32_t	m_listen_ip_len;
	char*		m_listen_ip;
	uint16_t	m_listen_port;
};


class CImPduMsgFileTransferReq : public CImPdu
{
public:
    CImPduMsgFileTransferReq(uchar_t* buf, uint32_t len);
    CImPduMsgFileTransferReq(uint32_t from_id, uint32_t to_id, const char* file_name, uint32_t file_length, uint32_t mode, uint32_t attach_len, uchar_t* attach_data);
    virtual ~CImPduMsgFileTransferReq() {}
    
public:
    virtual uint16_t GetPduType() { return IM_PDU_TYPE_MSG_FILE_TRANSFER_REQ; }
	uint32_t GetFromId() { return m_from_id; }
	uint32_t GetToId() { return m_to_id; }
    uint32_t GetFileNameLen() { return m_file_name_len; }
    char* GetFileName() { return m_file_name; }
	uint32_t GetFileLength() { return m_file_length; }
    uint32_t GetTransferMode() { return m_transfer_mode; }
    uint32_t GetAttachLen() { return m_attach_len; }
    uchar_t* GetAttachData() { return m_attach_data; }
private:
    uint32_t    m_from_id;
	uint32_t 	m_to_id;
    uint32_t    m_file_name_len;
    char*       m_file_name;
    uint32_t    m_file_length;
    uint32_t    m_transfer_mode;
    uint32_t    m_attach_len;
    uchar_t*    m_attach_data;
};

class CImPduMsgFileTransferRsp : public CImPdu
{
public:
    CImPduMsgFileTransferRsp(uchar_t* buf, uint32_t len);
    CImPduMsgFileTransferRsp(uint32_t from_id, uint32_t to_id, const char* file_name,
        uint32_t file_size, const char* file_svr_ip, uint32_t file_svr_port, const char* task_id,
                             uint32_t attach_len, uchar_t* attach_data);
    virtual ~CImPduMsgFileTransferRsp() {}
    
public:
    virtual uint16_t GetPduType() { return IM_PDU_TYPE_MSG_FILE_TRANSFER_RSP; }
    uint32_t GetResult() { return m_result; }
    uint32_t GetFromId() { return m_from_id; }
    uint32_t GetToId() { return m_to_id; }
    uint32_t GetFileNameLength() { return m_file_name_len; }
    char* GetFileName() { return m_file_name; }
    uint32_t GetFileLength() { return m_file_length; }
	uint32_t GetFileServerIpLength() { return m_file_server_ip_len; }
	char* GetFileServerIp() { return m_file_server_ip; }
	uint32_t GetFileServerPort() { return m_file_server_port; }
	char* GetTaskId() { return m_task_id; }
    uint32_t GetTaskIdLen() { return m_task_id_len; }
    uint32_t GetAttachLen() { return m_attach_len; }
    uchar_t* GetAttachData() { return m_attach_data; }
    
private:
    uint32_t    m_result; // 0 OK
    uint32_t    m_from_id;
    uint32_t    m_to_id;
    uint32_t    m_file_name_len;
    char*       m_file_name;
    uint32_t    m_file_length;
    uint32_t    m_file_server_ip_len; // 0 failed
    char*       m_file_server_ip;     // null failed
    uint32_t    m_file_server_port;   // 0 failed
    uint32_t    m_task_id_len;        // 0 failed
    char*       m_task_id;            // null failed
    uint32_t    m_attach_len;         // 0 failed
    uchar_t*    m_attach_data;        // null failed
};

// to storage server
class CImPduFileHasOfflineReq : public CImPdu
{
public:
	CImPduFileHasOfflineReq(uint32_t req_user_id, uint32_t attach_len, uchar_t* attach_data);
	virtual ~CImPduFileHasOfflineReq() {}

	virtual uint16_t GetPduType() { return IM_PDU_TYPE_FILE_HAS_OFFLINE_REQ; }
};

class CImPduFileHasOfflineRes : public CImPdu
{
public:
	CImPduFileHasOfflineRes(uchar_t* buf, uint32_t len);
	virtual ~CImPduFileHasOfflineRes();

	virtual uint16_t GetPduType() { return IM_PDU_TYPE_FILE_HAS_OFFLINE_RES; }
	uint32_t GetReqUserId() { return m_req_user_id; }
	uint32_t GetFileCnt() { return m_file_cnt; }
	offline_file_t* GetFileList() { return m_file_list; }
    uint32_t GetAttachLen() { return m_attach_len; }
    uchar_t* GetAttachData() { return m_attach_data; }
private:
	uint32_t			m_req_user_id;
	uint32_t 			m_file_cnt;
	offline_file_t* 	m_file_list;
    uint32_t            m_attach_len;
    uchar_t*            m_attach_data;
};

class CImPduFileAddOfflineReq : public CImPdu
{
public:
	CImPduFileAddOfflineReq(uint32_t req_user_id, uint32_t from_id, uint32_t to_id,
                        const char* task_id, const char* file_name, uint32_t file_size);
	virtual ~CImPduFileAddOfflineReq() {}

	virtual uint16_t GetPduType() { return IM_PDU_TYPE_FILE_ADD_OFFLINE_REQ; }
};

class CImPduFileDelOfflineReq : public CImPdu
{
public:
	CImPduFileDelOfflineReq(uint32_t req_user_id, uint32_t from_id, uint32_t to_id, const char* task_id);
	virtual ~CImPduFileDelOfflineReq() {}

	virtual uint16_t GetPduType() { return IM_PDU_TYPE_FILE_DEL_OFFLINE_REQ; }
};



#endif	// WIN32

#endif /* IMPDUFILE_H_ */
