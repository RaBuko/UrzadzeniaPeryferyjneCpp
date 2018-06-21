#include <winscard.h>
#include <stdio.h>
#include <conio.h>

#pragma comment(lib, "winscard.lib")

#define MAX_READER_NAME_SIZE 40
#define MAX_ATR_SIZE 33
#define MAX_RESP_SIZE 178


DWORD dwPref, dwState, dwProtocol, dwRespLen;
BYTE dwAtr[MAX_ATR_SIZE];
DWORD dwReaders = SCARD_AUTOALLOCATE;
LPTSTR mszReaders;
SCARDCONTEXT hContext;
SCARDHANDLE hCard;
long answer;
LPCWSTR mszGroups;
LPWSTR pcReaders;
BYTE pbResp1[MAX_RESP_SIZE];

void transmit(BYTE *text, unsigned lenghtOfText, unsigned respLenght)
{
	printf("\nSCardTransmit: ");
	dwRespLen = respLenght;
	answer = SCardTransmit(hCard, SCARD_PCI_T0, text,
		lenghtOfText, NULL, pbResp1, &dwRespLen);

	if (answer != SCARD_S_SUCCESS)
	{
		SCardDisconnect(hCard, SCARD_RESET_CARD);
		SCardReleaseContext(hContext);
		printf("failed\n");
		free(mszReaders);
		return;
	}
	else printf("success\n");
	printf("Response APDU : ");
	// wydruk odpowiedzi karty
	for (unsigned i = 0; i < dwRespLen; i++)
	{
		printf("%02X", pbResp1[i]);
	}
	printf("\n\n");
}

int main(int argc, char ** argv)
{
	BYTE SELECT_TELECOM[] = { 0xA0, 0xA4, 0x00, 0x00, 0x02, 0x7F, 0x10 };		//A0A40000027F10
	BYTE GET_RESPONSE[] = { 0xA0, 0xC0, 0x00, 0x00, 0x1A };						//A0C000001A
	BYTE SELECT_SMS[] = { 0xA0, 0xA4, 0x00, 0x00, 0x02, 0x6F, 0x3C };
	//EFSMS (FID = ’6F3C’, ang. short message service) – w pliku tym przechowywane s ²a
	//krótkie wiadomosci tekstowe wys³ane i otrzymane z sieci; plik ma struktur²e rekordow ²a ´
	//	(kaÿzdy z rekordów o wielkosci 176 bajtów)//A0A400026F3C
	BYTE GET_RESPONSE1[] = { 0xA0, 0xC0, 0x00, 0x00, 0x0F };
	BYTE READ[] = { 0xA0, 0xB2, 0x03, 0x04, 0xB0 };
	BYTE GET_RESPONSE2[] = { 0xA0, 0x67, 0x00, 0x00, 0x0F };

	DWORD maxAtrSize = MAX_ATR_SIZE;
	answer = SCardEstablishContext(SCARD_SCOPE_SYSTEM, NULL, NULL, &hContext);
	if (answer != SCARD_S_SUCCESS)
	{
		printf("blad: 1\n");
		_getch();
		return -1;
	}

	mszGroups = 0;
	answer = SCardListReaders(hContext, NULL, (LPTSTR)&mszReaders, &dwReaders);

	if (answer != SCARD_S_SUCCESS)
	{
		SCardReleaseContext(hContext);
		printf("blad: 2\n");
		if (answer == SCARD_E_INSUFFICIENT_BUFFER)
			printf("uchwyt");
		_getch();
		return -1;
	}

	unsigned i = 0;
	while (mszReaders[i] != NULL && mszReaders[i + 1])
	{
		if (mszReaders[i] == NULL)
			i++;
		printf("%c", mszReaders[i]);
		i++;
	}
	printf("\n");

	answer = SCardConnect(hContext, mszReaders,
		SCARD_SHARE_EXCLUSIVE, SCARD_PROTOCOL_T0 | SCARD_PROTOCOL_T1,
		&hCard, &dwPref);
	if (answer != SCARD_S_SUCCESS)
	{
		printf("blad: 3\n");
		_getch();
		return -1;
	}

	DWORD dwReaderLen = MAX_READER_NAME_SIZE;
	pcReaders = (LPWSTR)malloc(sizeof(char) * MAX_READER_NAME_SIZE);
	answer = SCardStatus(hCard, pcReaders, &dwReaderLen,
		&dwState, &dwProtocol, dwAtr, &maxAtrSize);

	printf("Status czytnika : %lx\n", dwState);
	printf("Protokoly : %lx\n", dwProtocol - 1);
	printf("Wielkosc ATR : %d\n", maxAtrSize);
	printf("Wartosc ATR : ");
	for (i = 0; i < maxAtrSize; i++)
	{
		printf("%02X", dwAtr[i]);
	}

	printf("\n\nSELECT TELECOM");
	transmit(SELECT_TELECOM, 7, 30);
	printf("GET RESPONSE");
	transmit(GET_RESPONSE, 5, 30);
	printf("SELECT SMS");
	transmit(SELECT_SMS, 7, 30);
	printf("GET_RESPONSE");
	transmit(GET_RESPONSE1, 5, 30);
	printf("READ");
	transmit(READ, 5, 178);
	printf("\n");
	SCardDisconnect(hCard, SCARD_RESET_CARD);

	_getch();

}

/*
OMNIKEY Smart Card Reader USB
Status czytnika : 6
Protokoly : 0
Wielkosc ATR : 15
Wartosc ATR : 3BB994004014474733533035415330

SELECT TELECOM
SCardTransmit: success
Response APDU : 9F 19

GET RESPONSE
SCardTransmit: success
Response APDU : 67 19

SELECT SMS
SCardTransmit: success
Response APDU : 9F 0F

GET_RESPONSE
SCardTransmit: success
Response APDU : 00 00 06 E0 6F 3C 04 03 11 F0 55 01 02 01 B0 90 00

READ
SCardTransmit: success
Response APDU : 07 07 91 84 06 92 15 11 F1 11 00 03 81 11 F1 00 00 00 06 31 D9 2
C 26 9B 01 FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF
FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF
FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF F
F FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF
FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF
FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF 90 00

Text message
To:
111
Message:
123123

Additional information
PDU type:
SMS-SUBMIT
Reference:
0
SMSC:
+48602951111
Validity:
5 minutes
Val. format:
Relative
Data coding:
SMS Default Alphabet


OMNIKEY Smart Card Reader USB
Status czytnika : 6
Protokoly : 0
Wielkosc ATR : 15
Wartosc ATR : 3BB994004014474733533035415330

SELECT TELECOM
SCardTransmit: success
Response APDU : 9F19

GET RESPONSE
SCardTransmit: success
Response APDU : 6719

SELECT SMS
SCardTransmit: success
Response APDU : 9F0F

GET_RESPONSE
SCardTransmit: success
Response APDU : 000006E06F3C040311F055010201B09000

READ
SCardTransmit: success
Response APDU : 0707918406921511F11100038133F300000006F7FBFD7EBF03FFFFFFFFFFFFFF
FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF
FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF
FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF
FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF9000

Text message
To:
333
Message:
wwwwww
/////////

OMNIKEY Smart Card Reader USB
Status czytnika : 6
Protokoly : 0
Wielkosc ATR : 15
Wartosc ATR : 3BB994004014474733533035415330

SELECT TELECOM
SCardTransmit: success
Response APDU : 9F19

GET RESPONSE
SCardTransmit: success
Response APDU : 6719

SELECT SMS
SCardTransmit: success
Response APDU : 9F0F

GET_RESPONSE
SCardTransmit: success
Response APDU : 000006E06F3C040311F055010201B09000

READ
SCardTransmit: success
Response APDU : 0707918406921511F111000881433224430000000964F3996C3E93CD67FFFFFF
FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF
FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF
FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF
FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF9000

Text message
To:
34234234
Message:
dfgdfgdfg

*/