#include "ReGCUser.h"

ReGCUser::ReGCUser(QObject *parent)
	: QObject(parent)
{

}

ReGCUser::~ReGCUser()
{

}

//��Qtʵ�ַ�ʽ
/*
void ReGCUser::OnNetMsg(UINT msgId, DWORD wParam, TSKINF tskInf)
{
	if (msgId == NM_TASK && wParam == TSK_EXIT) 
		OnTaskExit(tskInf.tskGrp, tskInf.tskId);
	if (msgId == NM_TASK&&wParam == TSK_START) 
		OnTaskStart(tskInf.tskGrp, tskInf.tskId);
};
*/


void ReGCUser::OnNetMsg(UINT msgId, DWORD wParam, TSKINF tskInf)
{
	if (msgId == NM_TASK && wParam == TSK_EXIT)
		emit GCTExit(tskInf.tskId);//���ͽ���id
	else if (msgId == NM_TASK&&wParam == TSK_START)
		emit GCTStart(tskInf.tskId);//���Ϳ�ʼid
};

