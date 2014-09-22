/*
 * RouteFileHandler.cpp
 *
 *  Created on: 2013-12-18
 *      Author: ziteng@mogujie.com
 */

#include "RouteFileHandler.h"
#include "RouteConn.h"

/*
CRouteFileHandler* CRouteFileHandler::s_handler_instance = NULL;

CRouteFileHandler* CRouteFileHandler::getInstance()
{
	if (!s_handler_instance) {
		s_handler_instance = new CRouteFileHandler();
	}

	return s_handler_instance;
}

void CRouteFileHandler::HandleFileRequest(CImPduFileRequest* pPdu, CRouteConn* pFromConn)
{
	uint32_t from_id = pPdu->GetFromId();
	uint32_t to_id = pPdu->GetToId();
	string file_name(pPdu->GetFileName(), pPdu->GetFileNameLen());
	log("HandleFileRequest, %u->%u, fileName: %s\n", from_id, to_id, file_name.c_str());

	CRouteConn* pToConn = get_route_conn(to_id);
	if (pToConn) {
		pToConn->SendPdu(pPdu);
	}
}

void CRouteFileHandler::HandleFileResponse(CImPduFileResponse* pPdu)
{
	uint32_t from_id = pPdu->GetFromId();
	uint32_t to_id = pPdu->GetToId();
	string file_name(pPdu->GetFileName(), pPdu->GetFileNameLen());
	log("HandleFileResponse, %u->%u, fileName: %s\n", from_id, to_id, file_name.c_str());

	CRouteConn* pConn = get_route_conn(to_id);
	if (pConn) {
		pConn->SendPdu(pPdu);
	}
}

void CRouteFileHandler::HandleFileRecvReady(CImPduFileRecvReady* pPdu)
{
	uint32_t from_id = pPdu->GetFromId();
	uint32_t to_id = pPdu->GetToId();
	log("HandleFileRecvReady, %u->%u\n", from_id, to_id);

	CRouteConn* pConn = get_route_conn(to_id);
	if (pConn) {
		pConn->SendPdu(pPdu);
	}
}

void CRouteFileHandler::HandleFileAbout(CImPduFileAbort* pPdu)
{
	uint32_t from_id = pPdu->GetFromId();
	uint32_t to_id = pPdu->GetToId();
	log("HandleFileAbort, %u->%u\n", from_id, to_id);

	CRouteConn* pConn = get_route_conn(to_id);
	if (pConn) {
		pConn->SendPdu(pPdu);
	}
}

void CRouteFileHandler::HandleFileUploadOfflineNotify(CImPduFileUploadOfflineNotify* pPdu)
{
	uint32_t from_id = pPdu->GetFromId();
	uint32_t to_id = pPdu->GetToId();
	log("HandleFileUploadOfflineNotify, %u->%u\n", from_id, to_id);

	CRouteConn* pConn = get_route_conn(to_id);
	if (pConn) {
		pConn->SendPdu(pPdu);
	}
}

void CRouteFileHandler::HandleFileDownloadOfflineNotify(CImPduFileDownloadOfflineNotify* pPdu)
{
	uint32_t from_id = pPdu->GetFromId();
	uint32_t to_id = pPdu->GetToId();
	log("HandleFileDownloadOfflineNotify, %u->%u\n", from_id, to_id);

	CRouteConn* pConn = get_route_conn(to_id);
	if (pConn) {
		pConn->SendPdu(pPdu);
	}
}
 */
