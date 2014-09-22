/*
 * FileConn.cpp
 *
 *  Created on: 2013-12-9
 *      Author: ziteng@mogujie.com
 */

#include "FileConn.h"
#include "TokenValidator.h"
#include <sys/stat.h>

static ConnMap_t g_file_conn_map;
static UserMap_t g_file_user_map;

static char g_current_save_path[BUFSIZ];
static uchar_t g_tmp_file_buf[PARTIAL_FILE_SIZE];

static char* get_last_path_component(char* path)
{
	char* pos = strrchr(path, '/');	// UNIX/LINUX path separator
	if (!pos) {
		pos = strrchr(path, '\\');	// Windows path separator
	}

	if (pos) {
		return pos + 1;
	} else {
		return (char*)path;
	}
}

static uint32_t get_file_size(FILE* fp)
{
	fseek(fp, 0, SEEK_END);
	uint32_t size = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	return size;
}

void file_conn_timer_callback(void* callback_data, uint8_t msg, uint32_t handle, void* pParam)
{
	uint64_t cur_time = get_tick_count();
	for (ConnMap_t::iterator it = g_file_conn_map.begin(); it != g_file_conn_map.end(); ) {
		ConnMap_t::iterator it_old = it;
		it++;

		CFileConn* pConn = (CFileConn*)it_old->second;
		pConn->OnTimer(cur_time);
	}
}

void init_file_conn()
{
	char work_path[BUFSIZ];
	if(!getcwd(work_path, BUFSIZ)) {
		log("getcwd failed\n");
	} else {
		snprintf(g_current_save_path, BUFSIZ, "%s/offline_file", work_path);
	}

	log("save offline files to %s\n", g_current_save_path);

	int ret = mkdir(g_current_save_path, 0755);
	if ( (ret != 0) && (errno != EEXIST) ) {
		log("!!!mkdir failed to save offline files\n");
	}

	netlib_register_timer(file_conn_timer_callback, NULL, 1000);
}

CFileConn::CFileConn()
{
	//log("CFileConn\n");
	m_bAuth = false;
	m_user_id = 0;
}

CFileConn::~CFileConn()
{
	log("~CFileConn, user_id=%u\n", m_user_id);

	for (FileMap_t::iterator it = m_save_file_map.begin(); it != m_save_file_map.end(); it++) {
		file_stat_t* file = it->second;
		fclose(file->fp);
		delete file;
	}
	m_save_file_map.clear();


}

void CFileConn::Close()
{
	if (m_handle != NETLIB_INVALID_HANDLE) {
		netlib_close(m_handle);
		g_file_conn_map.erase(m_handle);
	}

	if (m_user_id > 0) {
		g_file_user_map.erase(m_user_id);
	}

	ReleaseRef();
}

void CFileConn::OnConnect(net_handle_t handle)
{
	m_handle = handle;

	g_file_conn_map.insert(make_pair(handle, this));

	netlib_option(handle, NETLIB_OPT_SET_CALLBACK, (void*)imconn_callback);
	netlib_option(handle, NETLIB_OPT_SET_CALLBACK_DATA, (void*)&g_file_conn_map);

	uint32_t socket_buf_size = NETLIB_MAX_SOCKET_BUF_SIZE;
	netlib_option(handle, NETLIB_OPT_SET_SEND_BUF_SIZE, &socket_buf_size);
	netlib_option(handle, NETLIB_OPT_SET_RECV_BUF_SIZE, &socket_buf_size);
}

void CFileConn::OnClose()
{
	log("client onclose: handle=%d\n", m_handle);
	Close();
}

void CFileConn::OnTimer(uint64_t curr_tick)
{
	if (curr_tick > m_last_send_tick + CLIENT_HEARTBEAT_INTERVAL) {
		CImPduHeartbeat pdu;
		SendPdu(&pdu);
	}

	if (curr_tick > m_last_recv_tick + CLIENT_TIMEOUT) {
		log("client timeout, user_id=%u\n", m_user_id);
		Close();
	}
}

void CFileConn::OnWrite()
{
	CImConn::OnWrite();

	_TrySendFile();
}

void CFileConn::HandlePdu(CImPdu* pPdu)
{
	switch (pPdu->GetPduType()) {
	case IM_PDU_TYPE_HEARTBEAT:
		break;
	case IM_PDU_TYPE_CLIENT_FILE_LOGIN_REQ:
		_HandleClientFileLoginReq((CImPduClientFileLoginReq*) pPdu);
		break;
	case IM_PDU_TYPE_CLIENT_FILE_DATA:
		_HandleClientFileData((CImPduClientFileData*) pPdu);
		break;
	case IM_PDU_TYPE_CLIENT_FILE_GET_OFFLINE_REQ:
		_HandleClientFileGetOfflineReq( (CImPduClientFileGetOfflineReq*)pPdu );
		break;

	default:
		log("no such pdu_type: %u\n", pPdu->GetPduType());
		break;
	}
}

void CFileConn::_HandleClientFileLoginReq(CImPduClientFileLoginReq* pPdu)
{
	string user_id_url(pPdu->GetUserId(), pPdu->GetUserIdLen());
	string token(pPdu->GetToken(), pPdu->GetTokenLen());

	m_user_id = urltoid(user_id_url.c_str());
	m_user_id_url = user_id_url;
	log("client login, user_id=%u, token=%s\n", m_user_id, token.c_str());

	if (!IsTokenValid(m_user_id, token.c_str())) {
		log("token not valid, close the connection\n");
		CImPduClientFileLoginRes pdu(1);
		SendPdu(&pdu);
		Close();
		return;
	}

	m_bAuth = true;

	UserMap_t::iterator it = g_file_user_map.find(m_user_id);
	if (it != g_file_user_map.end()) {
		CFileConn* pConn = (CFileConn*)it->second;
		if (pConn == this) {
			return;
		}

		pConn->Close();
	}

	g_file_user_map.insert(make_pair(m_user_id, this));

	CImPduClientFileLoginRes pdu(0);
	SendPdu(&pdu);
}

void CFileConn::_HandleClientFileData(CImPduClientFileData* pPdu)
{
	if (!_IsAuth()) {
		return;
	}

	string to_id_url(pPdu->GetToId(), pPdu->GetToIdLen());
	uint32_t to_id = urltoid(to_id_url.c_str());
	string file_name(pPdu->GetFileName(), pPdu->GetFileNameLen());
	uint32_t file_size = pPdu->GetFileSize();
	uint32_t file_offset = pPdu->GetFileOffset();
	uint32_t file_type = pPdu->GetFileType();
	uint32_t file_data_size = pPdu->GetFileDataLen();
	uchar_t* file_data = pPdu->GetFileData();

	if ( (file_offset == 0) || (file_offset + file_data_size == file_size) ) {
		log("ClientFileData, %u->%u, fileName: %s, (%u, %u), fileType=%u\n",
			m_user_id, to_id, file_name.c_str(), file_size, file_offset, file_type);
	}

	if (file_type == FILE_TYPE_ONLINE) {
		UserMap_t::iterator it = g_file_user_map.find(to_id);
		if (it == g_file_user_map.end()) {
			log("no such user, user_id=%u\n", to_id);
			return;
		}

		CFileConn* pConn = (CFileConn*)it->second;
		pConn->SendPdu(pPdu);
	} else if (file_type == FILE_TYPE_OFFLINE) {
		file_stat_t* file = NULL;
		if (file_offset == 0) {
			char save_path[BUFSIZ];
			snprintf(save_path, BUFSIZ, "%s/%s", g_current_save_path, to_id_url.c_str());
			int ret = mkdir(save_path, 0755);
			if ( (ret != 0) && (errno != EEXIST) ) {
				log("mkdir failed for path: %s\n", save_path);
				return;
			}

			strncat(save_path, "/", BUFSIZ);
			strncat(save_path, get_last_path_component((char*)file_name.c_str()), BUFSIZ);

			file = new file_stat_t;
			file->peer_user_id = to_id_url;
			file->file_path = file_name;
			file->file_size = file_size;
			file->transfer_size = 0;
			file->save_path = save_path;
			file->fp = fopen(save_path, "wb");

			if (file->fp) {
				m_save_file_map.insert(make_pair(file_name, file));
			} else {
				log("open file for write failed\n");
			}

		} else {
			FileMap_t::iterator it = m_save_file_map.find(file_name);
			if (it != m_save_file_map.end()) {
				file = it->second;
			}
		}

		if (file) {
			fwrite(file_data, 1, file_data_size, file->fp);
		}

		// all file data have received
		if (file_offset + file_data_size == file_size) {
			string from_id_url(pPdu->GetFromId(), pPdu->GetFromIdLen());
			CImPduClientFileUploadFinish pdu(from_id_url.c_str(), to_id_url.c_str(),
					file_name.c_str(), file->save_path.c_str(), file_size);
			SendPdu(&pdu);

			fclose(file->fp);
			delete file;
			m_save_file_map.erase(file_name);
		}
	} else {
		log("no such file type\n");
	}
}

void CFileConn::_HandleClientFileGetOfflineReq(CImPduClientFileGetOfflineReq* pPdu)
{
	string from_id_url(pPdu->GetFromId(), pPdu->GetFromIdLen());
	string file_path(pPdu->GetFilePath(), pPdu->GetFilePathLen());
	log("HandleClientFileGetOfflineReq, from_id=%s, file_path=%s\n", from_id_url.c_str(), file_path.c_str());

	FILE* fp = fopen(file_path.c_str(), "rb");
	if (!fp) {
		// TODO return failure packet
		log("can not open file\n");
		return;
	}

	file_stat_t* file = new file_stat_t;
	file->fp = fp;
	file->file_path = file_path;
	file->file_size = get_file_size(fp);
	file->transfer_size = 0;
	file->peer_user_id = from_id_url;
	m_send_file_list.push_back(file);

	_TrySendFile();
}

void CFileConn::_TrySendFile()
{
	if (IsBusy()) {
		return;
	}

	while (!m_send_file_list.empty()) {
		file_stat_t* file = m_send_file_list.front();
		uint32_t size = fread(g_tmp_file_buf, 1, PARTIAL_FILE_SIZE, file->fp);
		if (size == 0) {
			fclose(file->fp);
			delete file;
			m_send_file_list.pop_front();
			continue;
		}

		CImPduClientFileData pdu(file->peer_user_id.c_str(), m_user_id_url.c_str(), file->file_path.c_str(),
				file->file_size, file->transfer_size, FILE_TYPE_OFFLINE, size, g_tmp_file_buf);

		SendPdu(&pdu);

		file->transfer_size += size;

		if (IsBusy()) {
			break;
		}
	}
}


