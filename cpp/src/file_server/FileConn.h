/*
 * FileConn.h
 *
 *  Created on: 2013-12-9
 *      Author: ziteng@mogujie.com
 */

#ifndef FILECONN_H_
#define FILECONN_H_

#include "imconn.h"
#include "impdu.h"

typedef struct {
	FILE* 		fp;
	string		file_path;
	string		save_path;
	string 		peer_user_id;
	uint32_t	file_size;
	uint32_t	transfer_size;
} file_stat_t;

typedef map<string, file_stat_t*> FileMap_t;

class CFileConn : public CImConn
{
public:
	CFileConn();
	virtual ~CFileConn();

	virtual void Close();

	void OnConnect(net_handle_t handle);
	virtual void OnClose();
	virtual void OnTimer(uint64_t curr_tick);

	virtual void OnWrite();

	virtual void HandlePdu(CImPdu* pPdu);

private:
	void _HandleClientFileLoginReq(CImPduClientFileLoginReq* pPdu);
	void _HandleClientFileData(CImPduClientFileData* pPdu);
	void _HandleClientFileGetOfflineReq(CImPduClientFileGetOfflineReq* pPdu);

	bool _IsAuth() { return m_bAuth; }
	void _TrySendFile();

private:
	bool		m_bAuth;
	uint32_t	m_user_id;
	string 		m_user_id_url;
	FileMap_t	m_save_file_map;
	list<file_stat_t*>	m_send_file_list;
};

void init_file_conn();


#endif /* FILECONN_H_ */
