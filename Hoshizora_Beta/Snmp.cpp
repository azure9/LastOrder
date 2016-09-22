#include "stdafx.h"
#include "Snmp.h"

extern Snmp* snmp;

bool operator == (CSnmp *pThis,const string &x)
{
	return pThis->GetDeviceIPAddress().get_printable()==x;
}

CSnmp::CSnmp(LPCSTR szAddress,LPCSTR ro,LPCSTR rw)
	//-1 represent for connection failed
	//-2 represent for communiting [thread safe]
	//- 临界区考虑在CSnmp而不是CSnmpBase建立、这样可以同时操作多台交换机
{
	OctetStr roCommunity(ro);
	OctetStr rwCommunity(rw);
	ipClient=szAddress;
	ipClient.set_port(161);
	snmp_version ver=version2c;
	conClient.set_version(ver);
	conClient.set_retry(3);
	conClient.set_default_timeout(100);
	conClient.set_address(ipClient);
	conClient.set_readcommunity(roCommunity);
	conClient.set_writecommunity(rwCommunity);
}


CSnmp::~CSnmp(void)
{
}

Vb CSnmp::GetNext(Oid &oid)
{
	while (!cs.try_lock())
	{
		Sleep(10);
	}
	Vb vb;                                 // construct a Vb object
	SnmpTarget *target = &conClient;
	Pdu pdu;                               // construct a Pdu object
	vb.set_oid(oid);                      // set the Oid portion of the Vb
	pdu += vb;                             // add the vb to the Pdu
	if (snmp->get_next( pdu, *target)==SNMP_CLASS_SUCCESS)
	{
		pdu.get_vb(vb,0);
	}
	cs.unlock();
	return vb;
}

Vb CSnmp::GetNext(char *oid)
{
	return GetNext((Oid)oid);
}

vector<Vb> CSnmp::GetList(vector<Oid> & oidList)
{
	while (!cs.try_lock())
	{
		Sleep(10);
	}
	vector<Vb> resList;
	SnmpTarget *target = &conClient;
	Pdu pdu;                               // construct a Pdu object
	Vb vb;                                 // construct a Vb object

	for (size_t index = 0;index<oidList.size();++index)
	{
		vb.set_oid(oidList[index]);
		pdu += vb;
	}

	if (snmp->get( pdu, *target)==SNMP_CLASS_SUCCESS)
	{
		resList.resize(oidList.size());
		for (size_t i=0;i<oidList.size();++i)
		{
			pdu.get_vb(resList[i],i);
		}
	}
	cs.unlock();

	return move(resList);
}

Vb CSnmp::Get(Oid &oid)
{
	while (!cs.try_lock())
	{
		Sleep(10);
	}
	Vb vb;                                 // construct a Vb object
	SnmpTarget *target = &conClient;
	Pdu pdu;                               // construct a Pdu object
	vb.set_oid(oid);                      // set the Oid portion of the Vb
	pdu += vb;                             // add the vb to the Pdu
	if (snmp->get( pdu, *target)==SNMP_CLASS_SUCCESS)
	{
		pdu.get_vb(vb,0);
	}
	cs.unlock();

	return vb;
}

Vb CSnmp::Get(char * oid)
{
	return Get((Oid)oid);
}

map<Oid,Vb> CSnmp::WalkEntry(const char * oidEntry,int count,int roffset)
{
	return move(WalkEntry((Oid)oidEntry,count,roffset));
}

map<Oid,Vb> CSnmp::WalkEntry(Oid &oidEntry,int count,int roffset)
{
	while (!cs.try_lock())
	{
		Sleep(10);
	}
	map<Oid,Vb> resList;
	SnmpTarget *target;  
	target = &conClient;  
	Pdu pdu;                               // construct a Pdu object  
	Vb vb;                                 // construct a Vb object  
	vb.set_oid(oidEntry);                      // set the Oid portion of the Vb  
	pdu += vb;                             // add the vb to the Pdu
	while (snmp->get_bulk( pdu,*target,0,count+1)== SNMP_CLASS_SUCCESS) 
	{
		for ( int z=0;z<pdu.get_vb_count(); z++)
		{
			pdu.get_vb( vb,z);  
			Oid tmp;  
			vb.get_oid(tmp);  
			if (oidEntry.nCompare(oidEntry.len(), tmp) != 0)  
			{  
				goto cleanup;
			}  
			// look for var bind exception, applies to v2 only
			if ( vb.get_syntax() != sNMP_SYNTAX_ENDOFMIBVIEW)
			{
				tmp.rtrim(roffset?roffset:tmp.len()-oidEntry.len());
				resList[tmp]=vb;
			}  
			else
			{  
				//End of MIB Reached  
				goto cleanup;
			}  
		}  
		// last vb becomes seed of next request  
		pdu.set_vblist(&vb, 1);
	}
cleanup:
	cs.unlock();

	return move(resList);
}

vector<Vb> CSnmp::WalkTable(char *oidEntry, int count)//in Order
{
	return move(WalkTable((Oid)oidEntry,count));
}

vector<Vb> CSnmp::WalkTable(Oid &oidEntry, int count)//in Order
{
	while (!cs.try_lock())
	{
		Sleep(10);
	}
	vector<Vb> resList;
	SnmpTarget *target;  
	target = &conClient;  
	Pdu pdu;                               // construct a Pdu object  
	Vb vb;                                 // construct a Vb object  
	vb.set_oid(oidEntry);                      // set the Oid portion of the Vb  
	pdu += vb;                             // add the vb to the Pdu
	while (snmp->get_bulk( pdu,*target,0,count+1)== SNMP_CLASS_SUCCESS) 
	{
		for ( int z=0;z<pdu.get_vb_count(); z++)
		{
			pdu.get_vb( vb,z);  
			Oid tmp;  
			vb.get_oid(tmp);  
			if (oidEntry.nCompare(oidEntry.len(), tmp) != 0)  
			{  
				goto cleanup;
			}  
			// look for var bind exception, applies to v2 only
			if ( vb.get_syntax() != sNMP_SYNTAX_ENDOFMIBVIEW)
			{
				resList.push_back(vb);
			}  
			else
			{  
				//End of MIB Reached  
				goto cleanup;
			}  
		}  
		// last vb becomes seed of next request  
		pdu.set_vblist(&vb, 1);
	}
cleanup:
	cs.unlock();
	
	return move(resList);
}

MacAddress CSnmp::GetDeviceMAC(void)
{
	MacAddress mac;
	Get("1.3.6.1.2.1.17.1.1.0").get_value(mac);
	return move(mac);
}

IpAddress CSnmp::GetDeviceIPAddress(void)
{
	return ipClient;
}

bool CSnmp::Set(Vb setValue)
{
	while (!cs.try_lock())
	{
		Sleep(10);
	}
	bool status = false;
	SnmpTarget *target = &conClient;
	Pdu pdu;                               // construct a Pdu object
	pdu += setValue;                             // add the vb to the Pdu
	if (snmp->set( pdu, *target)==SNMP_CLASS_SUCCESS)
	{
		status = true;
	}
	cs.unlock();
	return status;
}

bool CSnmp::SetList(Pdu & pdu)
{
	while (!cs.try_lock())
	{
		Sleep(10);
	}
	bool status = false;
	SnmpTarget *target = &conClient;
	// construct a Pdu object
	if (snmp->set( pdu, *target)==SNMP_CLASS_SUCCESS)
	{
		status = true;
	}
	cs.unlock();
	return status;
}