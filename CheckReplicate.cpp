#include "stdafx.h"
#include "CheckReplicate.h"


CCheckReplicate::CCheckReplicate(LPCWSTR  oldmutex)
{
	CheckValue = false;
	hMutex = OpenMutex(MUTEX_ALL_ACCESS, FALSE, oldmutex);
	if (hMutex != 0)  CheckValue = true;
	else CheckValue = false;

}

bool CCheckReplicate::CreateM(LPCWSTR  oldmutex)
{
	 hMutex = CreateMutex(NULL, false, oldmutex);
	if (hMutex != 0)  return true;
	else
		return false;

}

CCheckReplicate::~CCheckReplicate()
{
	ReleaseMutex(hMutex);
}
