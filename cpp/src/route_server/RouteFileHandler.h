/*
 * RouteFileHandler.h
 *
 *  Created on: 2013-12-18
 *      Author: ziteng@mogujie.com
 */

#ifndef ROUTEFILEHANDLER_H_
#define ROUTEFILEHANDLER_H_

#include "impdu.h"

class CRouteConn;
/*
class CRouteFileHandler
{
public:
	virtual ~CRouteFileHandler() {}
	static CRouteFileHandler* getInstance();

	void HandleFileRequest(CImPduFileRequest* pPdu, CRouteConn* pFromConn);
	void HandleFileResponse(CImPduFileResponse* pPdu);
	void HandleFileRecvReady(CImPduFileRecvReady* pPdu);
	void HandleFileAbout(CImPduFileAbort* pPdu);
	void HandleFileUploadOfflineNotify(CImPduFileUploadOfflineNotify* pPdu);
	void HandleFileDownloadOfflineNotify(CImPduFileDownloadOfflineNotify* pPdu);

private:
	CRouteFileHandler() {}

private:
	static CRouteFileHandler* s_handler_instance;
};
*/

#endif /* ROUTEFILEHANDLER_H_ */
