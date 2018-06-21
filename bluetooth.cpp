#include <iostream>
#include <WinSock2.h>
#include <bluetoothapis.h>
#include <conio.h>
#include <ws2bth.h>

#pragma comment(lib, "Bthprops.lib")
#define MAX_BT_ADAPTERS 10
#define MAX_BT_DEVICES 10

using namespace std;
//uchwyty przechowujące znalezione adaptery i urządzenia
HBLUETOOTH_RADIO_FIND bt_radio_find = NULL;
HBLUETOOTH_DEVICE_FIND bt_dev_find = NULL;
HANDLE adapters[MAX_BT_ADAPTERS];

//odpowiedz
DWORD dw;

int main()
{
	// Szukanie adapterow BT podlaczonych do komputera
	BLUETOOTH_FIND_RADIO_PARAMS bt_find_radio_params = { sizeof(BLUETOOTH_FIND_RADIO_PARAMS) };
	std::cout << "Szukanie adapterow" << std::endl;
	bt_radio_find = BluetoothFindFirstRadio(&bt_find_radio_params, &adapters[0]);
	if (bt_radio_find == NULL)
	{
		cout << "Brak podlaczonego adaptera. Blad 1\n";
		_getch();
		return -1;
	}
	int adapters_found = 1;
	while (BluetoothFindNextRadio(&bt_find_radio_params, &adapters[adapters_found]) != NULL)
	{
		adapters_found++;
		if (adapters_found == MAX_BT_ADAPTERS - 1) {
			cout << "Podlaczonych jest wiecej niz 10 adapterow BT do komputera.";
			break;
		}
	}
	
	//Zebranie informacji o adapterze
	BLUETOOTH_RADIO_INFO m_bt_info = { sizeof(BLUETOOTH_RADIO_INFO), 0, };
	for (int i = 0; i < adapters_found; i++)
	{
		BluetoothGetRadioInfo(adapters[i], &m_bt_info);
		wprintf(L"Adapter	: %s\n", m_bt_info.szName);
	}
	
	//Przygotowanie adaptera do wyszukiwania urzadzen
	BLUETOOTH_DEVICE_SEARCH_PARAMS bt_dev_search_params = { sizeof(BLUETOOTH_DEVICE_SEARCH_PARAMS), 1, 0, 1, 1, 1, 4, NULL };
	bt_dev_search_params.hRadio = adapters[0];

	// Wyszukiwanie urzadzen BT w poblizu komputera
	cout << "\nWyszukiwanie urzadzen Bluettoth w poblizu \n";
	BLUETOOTH_DEVICE_INFO devices[MAX_BT_DEVICES];
	BLUETOOTH_DEVICE_INFO bt_dev_info;
	devices[0].dwSize = sizeof(BLUETOOTH_DEVICE_INFO);
	bt_dev_find = BluetoothFindFirstDevice(&bt_dev_search_params, devices);

	int devices_found;
	if (bt_dev_find == NULL) {
		cout << "Nie znaleziono zadnych urzadzen BT." << endl;
		return 0;
	}
	else {
		devices_found = 1;
		devices[devices_found].dwSize = sizeof(BLUETOOTH_DEVICE_INFO);
		while (BluetoothFindNextDevice(bt_dev_find, &devices[devices_found]) != NULL) {
			devices_found++;
			devices[devices_found].dwSize = sizeof(BLUETOOTH_DEVICE_INFO);
		}
		BluetoothFindDeviceClose(bt_dev_find);

		for (int i = 0; i < devices_found; i++)
		{
			wprintf(L"Urzadzenie %d %s\n", i, devices[i].szName);
		}
	}

	//Wybranie urzadzenia
	cout << "Koniec wyszukiwania - ilosc urzadzen : " << devices_found << "\n"
		<< "\nWybierz urzadzenie: ";
	int selected_device;
	cin >> selected_device;

	// Autoryzowanie urzadzenia
	cout << "\nAutoryzacja\n";
	dw = BluetoothAuthenticateDeviceEx(NULL, &adapters[0], &devices[selected_device], NULL, MITMProtectionRequired);

	cout << "Koniec\n";
	_getch();
	return 0;
}






























//http://www.winsocketdotnetworkprogramming.com/winsock2programming/winsock2advancedotherprotocol4k.html
