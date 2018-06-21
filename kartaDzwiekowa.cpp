#include <iostream>
#include <conio.h>
#include <dsound.h>
#include <mmsystem.h>

#include <Windows.h>

#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif // _MSC_VER

#include <stdio.h>

#pragma comment( lib, "Winmm.lib" )
#pragma comment( lib, "dsound.lib" )

using namespace std;
//#pragma comment(lib, "winmm.lib")

LPDIRECTSOUND lpDSO;
WAVEFORMATEX wav;
HWAVEOUT hwaveout;
WAVEHDR wavhdr;
WAVEFORMATEX    WaveFormat;
HWAVEIN         WaveHandle;
HWAVEOUT        WaveOUTHandle;
WAVEHDR         WaveHeader;
WAVEHDR         WaveOutHeader;
char*           Buffer = NULL;
unsigned int    BufferSize = 0;

class Menu
{
public:
	Menu();
	~Menu() {}
	bool _end;
	bool _endSecondMenu;
	void MainMenu();
	void PlaySoundMenu();
	void WaveOutMenu();
	void DirectSoundMenu();
};
Menu::Menu()
{
	_end = false;
	MainMenu();
}
void Menu::MainMenu()
{
	while (!_end)
	{
		system("cls");
		cout << "1. PlaySound\n"
			<< "2. WaveOut\n"
			<< "3. DirectSound\n"
			<< "0. Wyjscie z programu\n";

		switch (_getch())
		{
		case '1':
			PlaySoundMenu();
			break;
		case '2':
			WaveOutMenu();
			break;
		case '3':
			DirectSoundMenu();
			break;
		case '0':
			exit(0);
		default:
			break;
		}
	}

}
void Menu::PlaySoundMenu()
{
	system("cls");
	_endSecondMenu = false;
	while (!_endSecondMenu)
	{
		system("cls");
		cout << "PLAYSOUND\n";
		cout << "1. Odtworz dzwiek\n";
		cout << "2. Zatrzymaj dzwiek\n";
		switch (_getch())
		{
		case '1':
			cout << "Podaj nazwe pliku: ";
			char nameOfFile[50];
			cin >> nameOfFile;
			PlaySound(nameOfFile, NULL, SND_FILENAME | SND_ASYNC | SND_LOOP);
			break;
		case '2':
			PlaySound(NULL, NULL, SND_ASYNC);
			break;
		default:
			_endSecondMenu = true;
		}

	}
}
void WaveOutLoadAndPlay(char * nameOfFile)
{
		FILE *plik;
		auto err = fopen_s(&plik, nameOfFile, "rb");
		WAVEFORMATEX wav;
		if (plik) 
{
			BYTE id[5]; //four bytes to hold 'RIFF'
			id[4] = 0;
			DWORD size; //32 bit value to hold file size

			fread(id, sizeof(BYTE), 4, plik); //read in first four bytes
			if (!strcmp((char *)id, "RIFF")) 
			{
				cout << "ChunkID: " << (char*)id << endl;

				fread(&size, sizeof(DWORD), 1, plik);
				cout << "Chunk Size: " << size << endl;

				// przesuwamy sie na 20 bajt pliku
				fseek(plik, 20, SEEK_SET);

				// czytamy format pliku
				fread(&(wav.wFormatTag), 2, 1, plik);
				cout << "Format pliku: " << wav.wFormatTag << endl;

				// czytamy liczbe kanalow
				fread(&(wav.nChannels), 2, 1, plik);
				cout << "L. kanalow: " << wav.nChannels << endl;

				// czytamy liczbe probek na sekunde
				fread(&(wav.nSamplesPerSec), 4, 1, plik);
				cout << "L. probek na sekunde: " << wav.nSamplesPerSec << endl;

				// liczba bajtow na sekunde (srednio)
				fread(&(wav.nAvgBytesPerSec), 4, 1, plik);

				// liczba bajtow na probke dla obydwu kanalow
				fread(&(wav.nBlockAlign), 2, 1, plik);

				// liczba bitow na probke
				fread(&(wav.wBitsPerSample), 2, 1, plik);
				cout << "L. bitow na probke: " << wav.wBitsPerSample << endl;

				// rozmiar informacji dodatkowych o pliku
				wav.cbSize = 0;
				err = waveOutOpen(&hwaveout, WAVE_MAPPER, &wav, NULL, NULL, CALLBACK_NULL);
				if (err != MMSYSERR_NOERROR) {
					fclose(plik);
					cout << "Blad otwierania waveOutOpen(), Kod bledu: " << err << endl;
					return;
				}

				// 40 bajt pliku
				fseek(plik, 40, SEEK_SET);

				// dlugosc bloku danych
				fread(&(wavhdr.dwBufferLength), 4, 1, plik);

				// alokacja pamieci na bufor z danymi
				wavhdr.lpData = (LPSTR)malloc(wavhdr.dwBufferLength);

				if (wavhdr.lpData == NULL) {
					fclose(plik);
					cout << "Blad alokacji pamieci." << endl;
					return;
				}
				// kopiowanie danych do bufora
				if (fread(wavhdr.lpData, wavhdr.dwBufferLength, 1, plik) != 1) {
					fclose(plik);
					cout << "Blad kopiowania danych do bufora." << endl;
					return;
				}

				wavhdr.dwFlags = 0;
				wavhdr.dwLoops = 0;

				// przygtowania
				if (waveOutPrepareHeader(hwaveout, &wavhdr, sizeof(WAVEHDR)) != MMSYSERR_NOERROR) {
					cout << "Blad przygotowywania do odtwarzania.";
					return;
				}

				// zapisywanie do karty dzwiekowej
				if (waveOutWrite(hwaveout, &wavhdr, sizeof(WAVEHDR)) != MMSYSERR_NOERROR) {
					cout << "Blad zapisywania danch." << endl;
					return;
				}
			}
			
		}
		else
			cout << "Blad wczytywania pliku " << nameOfFile << endl;
		
		fclose(plik);

}
void Menu::WaveOutMenu()
{
	system("cls");
	_endSecondMenu = false;
	while (!_endSecondMenu)
	{
		system("cls");
		cout << "WAVEOUT\n";
		cout << "1. Wczytaj naglowek pliku wav i odtworz go\n";
		cout << "2. Zatrzymaj dzwiek\n";
		switch (_getch())
		{
		case '1':
			cout << "Podaj nazwe pliku: ";
			char nameOfFile[50];
			cin >> nameOfFile;
			WaveOutLoadAndPlay(nameOfFile);
			_getch();
			break;
		case '2':
			waveOutReset(hwaveout);
			waveOutClose(hwaveout);
			break;
		default:
			_endSecondMenu = true;
		}

	}
}

void DirectSoundLoadAndPlay(LPCSTR nazwa)
{

}
void Menu::DirectSoundMenu()
{
	system("cls");
	_endSecondMenu = false;
	while (!_endSecondMenu)
	{
		system("cls");
		cout << "DIRECT SOUND\n";
		cout << "1. Odtworz dzwiek\n";
		cout << "2. Zatrzymaj dzwiek\n";
		switch (_getch())
		{
		case '1':
			cout << "Podaj nazwe pliku: ";
			char nameOfFile[50];
			cin >> nameOfFile;
			DirectSoundLoadAndPlay(nameOfFile);
			break;
		case '2':
			break;
		default:
			_endSecondMenu = true;
		}

	}
}
int main()
{
	Menu* menu = new Menu();
	return 0;


}





















bool odtwarzajDirectSound(LPCSTR nazwa) {
	HRESULT hr;
	CoInitialize(NULL);

	hr = DirectSoundCreate(NULL, &lpDSO, NULL);

	DSBUFFERDESC bd;
	LPDIRECTSOUNDBUFFER ppdsb;

	memset(&bd, 0, sizeof(DSBUFFERDESC));
	bd.dwSize = sizeof(DSBUFFERDESC);
	bd.dwFlags = DSBCAPS_PRIMARYBUFFER;
	bd.dwBufferBytes = 0;
	bd.lpwfxFormat = NULL;

	HANDLE bufor = 0;

	lpDSO->SetCooperativeLevel(GetDesktopWindow(), DSSCL_PRIORITY);


	if (DS_OK != lpDSO->CreateSoundBuffer(&bd, &ppdsb, NULL))
		cout << "Blad CreateSoundBuffer" << endl;

	LPVOID lpvWrite;
	DWORD  dwLength;
	LPVOID lpvWrite2;
	DWORD  dwLength2;

	int tmp = 5;

	tmp = ppdsb->Lock(
		0,          // Offset at which to start lock.
		0,          // Size of lock; ignored because of flag.
		&lpvWrite,  // Gets address of first part of lock.
		&dwLength,  // Gets size of first part of lock.
		NULL,       // Address of wraparound not needed. 
		NULL,       // Size of wraparound not needed.
		DSBLOCK_ENTIREBUFFER);  // Flag.

	if (tmp == DS_OK) {

		//wav = waveOut("a.wav", lpvWrite);
		dwLength = wavhdr.dwBufferLength;

		if (bufor == 0)
			cout << "Blad alokacji pamieci.22" << endl;


	}
	else if (tmp == DSERR_PRIOLEVELNEEDED)
		cout << tmp << endl;


	ppdsb->Unlock(lpvWrite, dwLength, NULL, 0);


	if (DS_OK == lpDSO->CreateSoundBuffer(&bd, &ppdsb, NULL)) {
		//ppdsb->SetFormat (wav);
	}


	//ppdsb->SetFormat(wav);
	ppdsb->SetCurrentPosition(0);

	ppdsb->SetVolume(DSBVOLUME_MAX);

	if (ppdsb->Play(0, 0, DSBPLAY_LOOPING) != DS_OK) {
		cout << "BLAD: ppdsb->Play()" << endl;
	}

	cout << "konic";

	return true;
}

//
//bool wczytajWav(LPCSTR nazwa) {
//	if (gra == true) {
//		cout << "BLAD: Dzwiek jest juz odtwarzany." << endl;
//		return false;
//	}
//	pauza = false;
//
//	FILE *plik;
//	fopen_s(&plik, nazwa, "rb");
//	WAVEFORMATEX wav;
//	if (plik) {
//		BYTE id[5]; //four bytes to hold 'RIFF'
//		id[4] = 0;
//		DWORD size; //32 bit value to hold file size
//
//		fread(id, sizeof(BYTE), 4, plik); //read in first four bytes
//		if (!strcmp((char *)id, "RIFF")) {
//			cout << "ChunkID: " << (char*)id << endl;
//
//			fread(&size, sizeof(DWORD), 1, plik);
//			cout << "Chunk Size: " << size << endl;
//
//			// przesuwamy sie na 20 bajt pliku
//			fseek(plik, 20, SEEK_SET);
//			// czytamy format pliku
//			fread(&(wav.wFormatTag), 2, 1, plik);
//			cout << "Format pliku (kod): " << wav.wFormatTag << endl;
//			// czytamy liczbe kanalow
//			fread(&(wav.nChannels), 2, 1, plik);
//			cout << "Liczba kanalow: " << wav.nChannels << endl;
//			// czytamy liczbe probek na sekunde
//			fread(&(wav.nSamplesPerSec), 4, 1, plik);
//			cout << "Liczba prbek//s: " << wav.nSamplesPerSec << endl;
//			// liczba bajtow na sekunde (srednio)
//			fread(&(wav.nAvgBytesPerSec), 4, 1, plik);
//			// liczba bajtow na probke dla obydwu kanalow
//			fread(&(wav.nBlockAlign), 2, 1, plik);
//			// liczba bitow na probke
//			fread(&(wav.wBitsPerSample), 2, 1, plik);
//			cout << "Liczba bitow//probke: " << wav.wBitsPerSample << endl;
//			// rozmiar informacji dodatkowych o pliku
//			wav.cbSize = 0;
//
//			// otwieramy urzadzenie waveOut
//			if (waveOutOpen(&hwaveout, WAVE_MAPPER, &wav, NULL, NULL, CALLBACK_NULL) != MMSYSERR_NOERROR) {
//				fclose(plik);
//				cout << "Blad otwierania waveOutOpen()" << endl;
//				return false;
//			}
//
//			// 40 bajt pliku
//			fseek(plik, 40, SEEK_SET);
//			// dlugosc bloku danych
//			fread(&(wavhdr.dwBufferLength), 4, 1, plik);
//
//			// alokacja pamieci na bufor z danymi
//			wavhdr.lpData = (LPSTR)malloc(wavhdr.dwBufferLength);
//			if (wavhdr.lpData == NULL) {
//				fclose(plik);
//				cout << "Blad alokacji pamieci." << endl;
//				return false;
//			}
//
//			// kopiowanie danych do bufora
//			if (fread(wavhdr.lpData, wavhdr.dwBufferLength, 1, plik) != 1) {
//				fclose(plik);
//				cout << "Blad kopiowania danych do bufora." << endl;
//				return false;
//			}
//
//			wavhdr.dwFlags = 0;
//			wavhdr.dwLoops = 0;
//
//			// przygtowania
//			if (waveOutPrepareHeader(hwaveout, &wavhdr, sizeof(WAVEHDR)) != MMSYSERR_NOERROR) {
//				cout << "Blad przygotowywania do odtwarzania.";
//				return false;
//			}
//
//			// zapisywanie do karty dzwiekowej
//			if (waveOutWrite(hwaveout, &wavhdr, sizeof(WAVEHDR)) != MMSYSERR_NOERROR) {
//				cout << "Blad zapisywania danch." << endl;
//				return false;
//			}
//
//			gra = true;
//		}
//		else
//			cout << "Format pliku inny niz RIFF." << endl;
//		fclose(plik);
//	}
//	else
//		cout << "Blad wczytywania pliku " << nazwa << endl;
//
//	fclose(plik);
//	return 0;
//}
//
//}
//
//
//void odtworzWav() {
//	int Res = waveOutOpen(&WaveOUTHandle, WAVE_MAPPER, &WaveFormat, 0, 0, WAVE_FORMAT_QUERY);
//	if (Res == WAVERR_BADFORMAT)
//		return;
//
//	//otwieramy urzadzenie
//	Res = waveOutOpen(&WaveOUTHandle, WAVE_MAPPER, &WaveFormat, 0, 0, CALLBACK_WINDOW);
//	waveOutPrepareHeader(WaveOUTHandle, &WaveHeader, sizeof(WAVEHDR));
//	waveOutWrite(WaveOUTHandle, &WaveHeader, sizeof(WAVEHDR));
//}
//
//void pauzaWav() {
//	if (pauza == true) {
//		pauza = false;
//		waveOutRestart(hwaveout);
//	}
//	else {
//		if (waveOutPause(hwaveout) != MMSYSERR_NOERROR)
//			cout << "Blad pauzowania." << endl;
//		else
//			pauza = true;
//	}
//}
//
//void stopWav() {
//	if (gra == true) {
//		waveOutReset(hwaveout);
//		waveOutClose(hwaveout);
//		gra = false;
//	}
//}
