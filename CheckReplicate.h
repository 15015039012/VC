#pragma once
class CCheckReplicate
{
	private:
		HANDLE hMutex;
public:
	CCheckReplicate(LPCWSTR  oldmutex);
	~CCheckReplicate();
	bool CreateM(LPCWSTR  oldmutex);
	bool CheckValue;
};

