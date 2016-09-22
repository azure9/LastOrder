#include "stdafx.h"
#include "BaseInfo.h"


CBaseInfo::CBaseInfo(LPCSTR dbPath)
	:db(dbPath),scheme(-1)
{
	statement st(db);
	//////////////////////////////////////////////////////////////////////////
	//	Load communitys
	{
		string ro,rw;

		st << "select ro,rw from community",into(ro),into(rw);

		st.reset(true);
		while (st.exec())
		{
			communitys.push_back(pair<string,string>(move(ro),move(rw)));
		}
	}

	//////////////////////////////////////////////////////////////////////////
	//	Load schemes
	{
		string name,prefix,custom_cmd;

		st << "select name,prefix,custom_cmd from scheme",into(name),into(prefix),into(custom_cmd);

		st.reset(true);
		while (st.exec())
		{
			vSC.push_back(SchemeContent(name,prefix,custom_cmd));
		}
	}
}


CBaseInfo::~CBaseInfo(void)
{
}

vector<SchemeContent> CBaseInfo::LoadScheme(void)
{
	return vSC;
}

SchemeContent CBaseInfo::LoadScheme(int idxScheme)
{
	return vSC[idxScheme];
}

int CBaseInfo::GetScheme()
{
	return scheme;
}

int CBaseInfo::SetScheme(int scheme)
{
	this->scheme = scheme;
	return 0;
}

int CBaseInfo::UpdateAllSchemeDB(vector<vector<pair<string,string>>> & v)
{
	db << "BEGIN;";
	int oldscheme = scheme;
	const size_t cntV = v.size();
	for (size_t idx = 0;idx<cntV;++idx)
	{
		scheme = idx;
		if (UpdateSelectDB(v[idx]))//if there's no new ip here we don't need to exec custom cmd
		{
			auto && cmd = vSC[idx].custom_cmd;
			if (!cmd.empty())
			{
				string single;
				stringstream ss(cmd);
				for (;getline(ss,single,';');)
				{
					db << single.c_str();
				}
			}
		}
	}
	scheme = oldscheme;
	db << "COMMIT;";
	return 0;
}

size_t CBaseInfo::GetAllPortDescr(map<pair<int,int>,string> &v)
{
	statement st(db);
	string room;
	int device_idx,port;

	if (scheme == -1)
	{
		st << "select room,device_idx,port from switcher_port",
			into(room),into(device_idx),into(port);
	}
	else
	{
		st << "select room,device_idx,port from switcher_port where scheme=:scheme",
			into(room),into(device_idx),into(port),use(scheme);
	}

	st.reset(true);

	while (st.exec())
	{
		if (room[0]!='*')
		{
			v[pair<int,int>(device_idx,port)] = room;
		}
	}

	return v.size();
}

int CBaseInfo::UpdateSelectDB(vector<pair<string,string>> &v)//ip-port & location return new ip inserted
{
	if (scheme==-1)
	{
		// must set scheme!
		return -1;
	}

	map<pair<int,int>,string> vOldRoomDescr;

	size_t cntOldPort = GetAllPortDescr(vOldRoomDescr);
	//////////////////////////////////////////////////////////////////////////
	map<string,int> SList;
	const size_t cntIP = GetAllIP(SList);

	int device_idx = 0,old_port = 0;
	statement st(db);

	vector<string> vNewIP;
	map<pair<int,int>,string> vNewRoomDescr;

	const size_t cntv = v.size();
	for (size_t idx = 0;idx<cntv;++idx)
	{
		auto && ip_port = v[idx].second;
		size_t pos = ip_port.find_first_of('-');
		size_t last_pos = ip_port.find_last_of('-');
		if (pos!=string::npos)
		{
			string ip(&ip_port[0],&ip_port[pos]);
			string port(&ip_port[last_pos+1],&ip_port[ip_port.size()]);
			int port_idx = atol(port.c_str());

			auto iter = SList.find(ip);
			if (iter == SList.end())
			{
				auto it = SList.insert(pair<string,int>(ip,SList.size()));
				iter = it.first;
				vNewIP.push_back(ip);
			}

			vNewRoomDescr[pair<int,int>(iter->second,port_idx)] = v[idx].first;
		}
		else
			;//printf("error! %s\n",ip_port.c_str());
	}

	map<pair<int,int>,string> rsl;

	set_difference(vNewRoomDescr.begin(), vNewRoomDescr.end(), vOldRoomDescr.begin(), vOldRoomDescr.end(),
		inserter(rsl, rsl.end()));

	// update switcher port // attention to set scheme!

	for (auto it = rsl.begin();it!=rsl.end();++it)
	{
		st << "replace into switcher_port(device_idx,room,port,scheme) values(:idx,:room,:port,:scheme);",
			use(it->first.first),
			use(it->second),
			use(it->first.second),
			use(scheme);

		st.reset(true);
		st.exec();
	}

	//refresh device list
	
	const size_t cntNewDevice = vNewIP.size();
	for (size_t idx = 0;idx<cntNewDevice;++idx)//new device found!
	{
		st << "insert into device_list values(null,:ip,0,:scheme,0,null,null);",
			use(vNewIP[idx]),use(scheme);

		st.reset(true);
		st.exec();
	}

	return cntNewDevice;
}

int CBaseInfo::GetDeviceScheme(string & ip)
{
	statement st(db);
	int scheme;
	st << "select scheme from device_list where ip=:ip",
		into(scheme),use(ip);

	st.reset(true);

	if (st.exec())
	{
		return scheme;
	}
	return -1;
}

pair<string,string> CBaseInfo::GetCommunity(string ip)
{
	statement st(db);
	int idx;
	st << "select community from device_list where ip=:ip",into(idx),use(ip);
	
	st.reset(true);

	try
	{
		if (st.exec())
		{
			return communitys[idx];
		}
	}
	catch (...)
	{
		
	}

	return pair<string,string>("public","private");//return default
}

string CBaseInfo::GetLocationInfoS(string ip)
{
	statement st(db);
	string loc,cab;
	st << "select location,Cabinet from device_list where ip=:ip and type=0",//switcher
		into(loc),into(cab),use(ip);
	st.reset(true);
	st.exec();
	if (loc.empty())
	{
		return string("");
	}
	return string(loc+" ¹ñ»ú"+cab);
}


vector<string> CBaseInfo::RoomImagination(string room)
{
	statement st(db);
	room = '%'+room+'%';
	vector<string> v;
	string rooms;
	if (scheme == -1)
	{
		st << "select room from switcher_port where room like :room limit 0,20",use(room),into(rooms);
	}
	else
	{
		st << "select room from switcher_port where room like :room and scheme=:scheme limit 0,20",
			use(room),use(scheme),into(rooms);
	}
	
	st.reset(true);
	while (st.exec())
	{
		v.push_back(rooms);
	}
	return move(v);
}


vector<string> CBaseInfo::IPImagination(string & ip,int device_type)
{
	statement st(db);
	ip = '%'+ip+'%';
	vector<string> v;
	string ips;
	if (scheme == -1)
	{
		st << "select ip from device_list where ip like :ip and type=:type limit 0,20",
			use(ip),use(device_type),into(ips);
	}
	else
	{
		st << "select ip from device_list where ip like :ip and type=:type and scheme=:scheme limit 0,20",
			use(ip),use(device_type),use(scheme),into(ips);
	}

	st.reset(true);
	while (st.exec())
	{
		v.push_back(ips);
	}
	return move(v);
}

vector<string> CBaseInfo::IPImaginationS(string & ip)
{
	return IPImagination(ip,0);//device_type:0 switcher
}

vector<string> CBaseInfo::IPImaginationR(string & ip)
{
	return IPImagination(ip,1);//device_type:1 router
}

string CBaseInfo::GetIPFromDescrS(string descr,int &port)
{
	statement st(db);
	string ip;int idx;
	port = -1;
	if (scheme == -1)
	{
		st << "select device_idx,port from switcher_port where room=:descr",
			into(idx),into(port),use(descr);
	}
	else
	{
		st << "select device_idx,port from switcher_port where room=:descr and scheme=:scheme",
			into(idx),into(port),use(descr),use(scheme);
	}

	st.reset(true);
	if (st.exec())
	{
		st << "select ip from device_list where idx=:id and type=0",
			into(ip),use(idx+1);

		st.reset(true);
		st.exec();
	}
	return move(ip);
}

map<int,string> CBaseInfo::GetPortListDescrS(string ip)
{
	statement st(db);
	map<int,string> m;
	string rooms;
	int idx;

	if (scheme == -1)
	{
		st << "select idx from device_list where ip=:ip and type=0",
			into(idx),use(ip);
	}
	else
	{
		st << "select idx from device_list where ip=:ip and type=0 and scheme=:scheme",
			into(idx),use(ip),use(scheme);
	}


	st.reset(true);
	if (st.exec())
	{
		int port = 0;
		st << "select port,room from switcher_port where device_idx=:idx",
			use(idx-1),into(port),into(rooms);

		st.reset(true);
		while (st.exec())
		{
			if (rooms[0]!='*')
			{
				m[port] = rooms;
			}
		}
	}
	return move(m);
}

string CBaseInfo::GetPortDescrS(string ip,int port)
{
	statement st(db);
	string rooms;
	int idx;

	if (scheme == -1)
	{
		st << "select idx from device_list where ip=:ip and type=0",
			into(idx),use(ip);
	}
	else
	{
		st << "select idx from device_list where ip=:ip and type=0 and scheme=:scheme",
			into(idx),use(ip),use(scheme);
	}


	st.reset(true);
	if (st.exec())
	{
		st << "select room from switcher_port where device_idx=:idx and port=:port",
			use(idx),use(port),into(rooms);

		st.reset(true);
		if (st.exec())
		{
			if (rooms[0]!='*')
			{
				return rooms;
			}
		}
	}
	return "";
}

vector<string> CBaseInfo::GetAllIPS(void)
{
	statement st(db);
	vector<string> v;
	string ips;
	if (scheme == -1)//select all
	{
		st << "select ip from device_list where type=0",into(ips);
	}
	else
		st << "select ip from device_list where type=0 and scheme=:scheme",
		into(ips),use(scheme);

	st.reset(true);
	while (st.exec())
	{
		v.push_back(ips);
	}
	return move(v);
}

vector<string> CBaseInfo::GetAllGateway(void)
{
	statement st(db);
	vector<string> v;
	string ips;
	if (scheme == -1)//select all
	{
		st << "select ip from device_list where type=2",into(ips);
	}
	else
		st << "select ip from device_list where type=2 and scheme=:scheme",
		into(ips),use(scheme);

	st.reset(true);
	while (st.exec())
	{
		v.push_back(ips);
	}
	return move(v);
}

size_t CBaseInfo::GetAllIP(map<string,int> & v)
{
	statement st(db);
	string ips;int idx;
	st << "select idx,ip from device_list",into(idx),into(ips);
	st.reset(true);

	while (st.exec())
	{
		v[ips] = idx-1;//real idx is start from 0
	}

	return v.size();
}

vector<string> CBaseInfo::GetAllIPR(void)
{
	statement st(db);
	vector<string> v;
	string ips;
	if (scheme == -1)//select all
	{
		st << "select ip from device_list where type=1",into(ips);
	}
	else
		st << "select ip from device_list where type=1 and scheme=:scheme",
		into(ips),use(scheme);

	st.reset(true);
	while (st.exec())
	{
		v.push_back(ips);
	}
	return move(v);
}