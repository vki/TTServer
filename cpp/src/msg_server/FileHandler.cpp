/*
 * FileHandler.cpp
 *
 *  Created on: 2013-12-17
 *      Author: ziteng@mogujie.com
 */

#include "FileHandler.h"
#include "MsgConn.h"
#include "RouteServConn.h"
#include "DBServConn.h"
#include "ImUser.h"
#include "AttachData.h"
#include "FileServConn.h"

CFileHandler* CFileHandler::s_handler_instance = NULL;

CFileHandler* CFileHandler::getInstance()
{
	if (!s_handler_instance) {
		s_handler_instance = new CFileHandler();
	}

	return s_handler_instance;
}

void CFileHandler::HandleClientFileRequest(CMsgConn* pMsgConn, CImPduClientFileRequest* pPdu)
{
	string from_id_url(pPdu->GetFromId(), pPdu->GetFromIdLen());
	uint32_t from_id = urltoid(from_id_url.c_str());
	string to_id_url(pPdu->GetToId(), pPdu->GetToIdLen());
	uint32_t to_id = urltoid(to_id_url.c_str());
	string file_name(pPdu->GetFileName(), pPdu->GetFileNameLen());
	uint32_t file_size = pPdu->GetFileSize();
	log("HandleClientFileRequest, %u->%u, fileName: %s\n", from_id, to_id, file_name.c_str());

    CPduAttachData attach(ATTACH_TYPE_HANDLE, pMsgConn->GetHandle(), 0, NULL, 0);

    CImUser* pUser = CImUserManager::GetInstance()->GetImUserById(to_id);
    if (pUser)
    {
        uint32_t client_type_flag = pUser->GetClientTypeFlag();
        //to_user has pc_client in this msg_server
        if ((client_type_flag & CLIENT_TYPE_FLAG_BOTH) == CLIENT_TYPE_FLAG_PC)
        {
            CFileServConn* pFileConn = get_random_file_serv_conn();
            if (pFileConn)
            {
                CImPduMsgFileTransferReq pdu(from_id, to_id, file_name.c_str(), file_size,
                                    FILE_TYPE_ONLINE, attach.GetLength(), attach.GetBuffer());
                pdu.SetReserved(pPdu->GetReserved());
                pFileConn->SendPdu(&pdu);
            }
            else
            {
                log("HandleClientFileRequest, no file server.\n");
                CImPduClientFileResponse pdu2(REFUSE_REASON_NO_FILE_SERVER, from_id_url.c_str(),
                                              to_id_url.c_str(), file_name.c_str(), NULL, NULL, 0);
                pdu2.SetReserved(pPdu->GetReserved());
                pMsgConn->SendPdu(&pdu2);
            }
            return;
        }
    }
    else
    {
        //no pc_client in this msg_server, check it from route_server
        CRouteServConn* pConn = get_route_serv_conn();
        CPduAttachData pduAttachData(ATTACH_TYPE_HANDLE_AND_PDU, pMsgConn->GetHandle(),
                                     pPdu->GetLength(), pPdu->GetBuffer());
        CImPduUserClientTypeRequest pdu3(to_id, pduAttachData.GetLength(), pduAttachData.GetBuffer());
        pdu3.SetReserved(pPdu->GetReserved());
        pConn->SendPdu(&pdu3);
    }
}

void CFileHandler::HandleClientFileHasOfflineReq(CMsgConn* pMsgConn, CImPduClientFileHasOfflineReq* pPdu)
{
	uint32_t req_user_id = pMsgConn->GetUserId();
	log("HandleClientFileHasOfflineReq, req_id=%u\n", req_user_id);

    CDbAttachData attach_data(ATTACH_TYPE_HANDLE, pMsgConn->GetHandle(), 0);
	CDBServConn* pDbConn = get_db_serv_conn();
	if (pDbConn) {
		CImPduFileHasOfflineReq pdu(req_user_id, attach_data.GetLength(), attach_data.GetBuffer());
		pdu.SetReserved(pPdu->GetReserved());
		pDbConn->SendPdu(&pdu);
	} else {
		log("warning no DB connection available\n");
		CImPduClientFileHasOfflineRes pdu;
		pdu.SetReserved(pPdu->GetReserved());
		pMsgConn->SendPdu(&pdu);
	}
}

void CFileHandler::HandleClientFileAddOfflineReq(CImPduClientFileAddOfflineReq* pPdu)
{
	string from_id_url(pPdu->GetFromId(), pPdu->GetFromIdLen());
	string to_id_url(pPdu->GetToId(), pPdu->GetToIdLen());
    string task_id(pPdu->GetTaskId(), pPdu->GetTaskIdLen());
	string file_name(pPdu->GetFileName(), pPdu->GetFileNameLen());
	uint32_t file_size = pPdu->GetFileSize();
	uint32_t from_id = urltoid(from_id_url.c_str());
	uint32_t to_id = urltoid(to_id_url.c_str());
	log("HandleClientFileAddOfflineReq, %u->%u, task_id: %s, file_name: %s, size: %u \n",
			from_id, to_id, task_id.c_str(), file_name.c_str(), file_size);

	CDBServConn* pDbConn = get_db_serv_conn();
	if (pDbConn) {
		CImPduFileAddOfflineReq pdu(from_id, from_id, to_id, task_id.c_str(), file_name.c_str(), file_size);
		pDbConn->SendPdu(&pdu);
	}
}

void CFileHandler::HandleClientFileDelOfflineReq(CImPduClientFileDelOfflineReq* pPdu)
{
	string from_id_url(pPdu->GetFromId(), pPdu->GetFromIdLen());
	string to_id_url(pPdu->GetToId(), pPdu->GetToIdLen());
	uint32_t from_id = urltoid(from_id_url.c_str());
	uint32_t to_id = urltoid(to_id_url.c_str());
    string task_id(pPdu->GetTaskId(), pPdu->GetTaskIdLen());
	log("HandleClientFileDelOfflineReq, %u->%u, task_id=%s\n", from_id, to_id, task_id.c_str());

	CDBServConn* pDbConn = get_db_serv_conn();
	if (pDbConn) {
		CImPduFileDelOfflineReq pdu(to_id, from_id, to_id, task_id.c_str());
		pDbConn->SendPdu(&pdu);
	}
}

/////////// for server
void CFileHandler::HandleFileHasOfflineRes(CImPduFileHasOfflineRes* pPdu)
{
	uint32_t req_user_id = pPdu->GetReqUserId();
	uint32_t file_cnt = pPdu->GetFileCnt();
	offline_file_t* file_list = pPdu->GetFileList();
    CDbAttachData attach(pPdu->GetAttachData(), pPdu->GetAttachLen());
	log("HandleFileHasOfflineRes, req_id=%u, file_cnt=%u\n", req_user_id, file_cnt);

	CMsgConn* pConn = CImUserManager::GetInstance()->GetMsgConnByHandle(req_user_id,
                                                                        attach.GetHandle());
    CFileServConn* pFileConn = get_random_file_serv_conn();
    const list<ip_addr_t>* ip_list = NULL;
    if (pFileConn)
    {
        ip_list = pFileConn->GetFileServerIPList();
    }
    else
    {
        log("HandleFileHasOfflineRes, no file server.\n");
    }
    if (pConn) {
        
        CImPduClientFileHasOfflineRes pdu(file_cnt, file_list, ip_list);
        pdu.SetReserved(pPdu->GetReserved());
        pConn->SendPdu(&pdu);
    }
}


void CFileHandler::HandleFileNotify(CImPduFileNotify *pPdu)
{
    uint32_t from_user_id = pPdu->GetFromId();
    uint32_t to_user_id = pPdu->GetToId();
    string file_name(pPdu->GetFileName(), pPdu->GetFileNameLen());
    uint32_t file_size = pPdu->GetFileSize();
    string task_id(pPdu->GetTaskId(), pPdu->GetTaskIdLen());
    string listen_ip(pPdu->GetListenIp(), pPdu->GetListenIpLen());
    uint32_t listen_port = pPdu->GetListenPort();
    log("HandleFileNotify, from_id: %u, to_id: %u, file_name: %s, task_id: %s, file_server_ip: %s,\
        file_server_port: %u.\n", from_user_id, to_user_id, file_name.c_str(), task_id.c_str(),
        listen_ip.c_str(), listen_port);
    CImUser* pUser = CImUserManager::GetInstance()->GetImUserById(to_user_id);
    if (pUser) {
        CImPduClientFileNotify pdu(idtourl(from_user_id), idtourl(to_user_id), file_name.c_str(),
                                   file_size, task_id.c_str(), listen_ip.c_str(), listen_port);
        pUser->BroadcastPdu(&pdu);
    }
}

