#pragma once
#pragma warning(disable:4005)
#pragma warning(disable:4091)
#pragma warning(disable:4244)
#pragma warning(disable:4800)
#define _CRT_SECURE_NO_WARNINGS
#ifndef _SNMP_PUBLIC_INCLUDE
#define _SNMP_PUBLIC_INCLUDE
#include <winsock2.h>
#include "snmp_pp/snmp_pp.h"

#pragma comment(lib,"Ws2_32.lib")
#pragma comment(lib,"snmp++.lib")

using namespace Snmp_pp;
#endif