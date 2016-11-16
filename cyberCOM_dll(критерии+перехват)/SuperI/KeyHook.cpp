#include "KeyHook.h"
/*хендл библа*/
extern HINSTANCE   modulehandle;
/*идентификатор потока*/
DWORD   dwThreadId=0;
/*дескриптор hook-точки*/
HHOOK   hndlKeyHook   = nullptr;
HHOOK   hndlKeyHookLL = nullptr;

extern KeyHook * pMe=nullptr;

LRESULT CALLBACK KeyHookProcLL(int nCode, WPARAM wParam, LPARAM lParam) {
	if(nCode < 0 || nCode != HC_ACTION)
		return CallNextHookEx(hndlKeyHook, nCode, wParam, lParam);	

	/*Определяем нужно-ли обрабатывать событие (клавиша нажата или отпущена)*/
	if(pMe->onKeyUPLL && (wParam == WM_SYSKEYUP || wParam == WM_KEYUP) || pMe->onKeyDOWNLL && (wParam == WM_SYSKEYDOWN || wParam == WM_KEYDOWN)) {
		KBDLLHOOKSTRUCT *st_hook = (KBDLLHOOKSTRUCT*)lParam;
		/*проверяем дополнительно нажатую клавишу*/
		std::wstring name;
		if(::GetKeyState(VK_LWIN) & 0x80)
			name += L"<LWin>";
		if(::GetKeyState(VK_RWIN) & 0x80) {
				name += (name.length() ? L"+" : L"");
				name += L"<RWin>";
		}
		if(::GetKeyState(VK_LSHIFT) & 0x80) {
				name += (name.length() ? L"+" : L"");
				name += L"<LShift>";
		}
		if(::GetKeyState(VK_RSHIFT) & 0x80) {
				name += (name.length() ? L"+" : L"");
				name += L"<RShift>";
		}
		if(::GetKeyState(VK_LCONTROL) & 0x80) {
				name += (name.length() ? L"+" : L"");
				name += L"<LCtrl>";
		}
		if(::GetKeyState(VK_RCONTROL) & 0x80) {
				name += (name.length() ? L"+" : L"");
				name += L"<RCtrl>";
		}
		if(::GetKeyState(VK_LMENU) & 0x80) {
				name += (name.length() ? L"+" : L"");
				name += L"<LAlt>";
		}
		if(::GetKeyState(VK_RMENU) & 0x80) {
				name += (name.length() ? L"+" : L"");
				name += L"<RAlt>";
		}
		if(::GetKeyState(VK_CAPITAL) & 0x80) {
				name += (name.length() ? L"+" : L"");
				name += L"<Caps Lock>";
		}
		/*Получаем имена клавишь*/
		wchar_t infString[100] = {0};
		BYTE    keystatebuff[256] = {0};
		if(::GetKeyboardState(keystatebuff)) {
			/*Имя клавиши*/
			name += (name.length() ? L"+" : L"");
			int num = name.length();
			std::wcscpy(infString, &name[0]);
			unsigned long msg = 1;
			msg += (st_hook->scanCode << 16);
			if(st_hook->scanCode != 0x3a) 
				msg += ((st_hook->flags & LLKHF_EXTENDED) << 24);						
			num += ::GetKeyNameText(msg, infString + num, 100 - num);
			infString[num++] = '\n';
			/*значение клавиши*/
			num += ::ToUnicode(st_hook->vkCode, st_hook->scanCode, keystatebuff, infString + num, 100 - num, st_hook->flags);
			infString[num++] = '\n';
			/*имя раскладки*/
			::GetKeyboardLayoutName(infString + num);
			if(num == 100)
				::wmemset(infString, 0, 100);
		}
	
		wchar_t resMsg[128] = {0};
		wsprintf(resMsg, L"%05d\n%s", st_hook->vkCode, infString);

		pMe->addMsg(L"Hook", resMsg);

		/*заблокировано*/
		/*if(pMe->KeyLock)
			return -1;*/
	}	
	
	return	CallNextHookEx(hndlKeyHook, nCode, wParam, lParam);
}

LRESULT CALLBACK KeyHookProc(int nCode, WPARAM wParam, LPARAM lParam) {
	if(nCode < 0 || nCode != HC_ACTION)
	return CallNextHookEx(hndlKeyHook, nCode, wParam, lParam);	

	/*Определяем тип события (клавиша нажата или отпущена)*/
	bool isKeyUP = !((DWORD)lParam & 0x40000000);

	if(nCode < 0 || nCode != HC_ACTION && nCode != HC_NOREMOVE)  /* сообщение не обрабатывать*/
		return CallNextHookEx(hndlKeyHook, nCode, wParam, lParam);
	else if(pMe->onKeyUP && isKeyUP || pMe->onKeyDOWN && !isKeyUP) {
		unsigned long rc = 0;
		/*проверяем дополнительно нажатую клавишу*/
		std::wstring name;
		if(::GetKeyState(VK_LSHIFT) & 0x80) {
			rc = rc | 0x1;
			name += L"<LShift>";
		}
		rc <<= 1;
		if(::GetKeyState(VK_RSHIFT) & 0x80) {
			rc = rc | 0x1;
			name += (name.length() ? L"+" : L"");
			name += L"<RShift>";
		}
		rc <<= 1;
		if(::GetKeyState(VK_LCONTROL) & 0x80) {
			rc = rc | 0x1;
			name += (name.length() ? L"+" : L"");
			name += L"<LCtrl>";
		}
		rc <<= 1;
		if(::GetKeyState(VK_RCONTROL) & 0x80) {
			rc = rc | 0x1;
			name += (name.length() ? L"+" : L"");
			name += L"<RCtrl>";
		}
		rc <<= 1;
		if(::GetKeyState(VK_LMENU) & 0x80) {
			rc = rc | 0x1;
			name += (name.length() ? L"+" : L"");
			name += L"<LAlt>";
		}
		rc <<= 1;
		if(::GetKeyState(VK_RMENU) & 0x80) {
			rc = rc | 0x1;
			name += (name.length() ? L"+" : L"");
			name += L"<RAlt>";
		}
		rc <<= 1;
		if(::GetKeyState(VK_CAPITAL) & 0x01) {
			name += (name.length() ? L"+" : L"");
			name += L"<Caps Lock>";
		}

		//unsigned RepeatCount      = (DWORD)lParam & 0xFFFF;
		unsigned PreviousKeyState = (((DWORD)lParam >> 30) & 0x01);
		unsigned ExtendedKey = (((DWORD)lParam >> 24) & 0x01);
		unsigned int VirtualKey = wParam;

		rc = rc | ExtendedKey;
		rc <<= 8;
		rc = rc | (VirtualKey & 0x00FF);

		/*Получаем имена клавишь*/
		wchar_t infString[100] = {0};
		BYTE    keystatebuff[256] = {0};
		if(::GetKeyboardState(keystatebuff)) {
			unsigned ExtScanCode = ::MapVirtualKey(wParam, MAPVK_VK_TO_VSC);
			unsigned flags = (((DWORD)lParam >> 29) & 0x0001) ^ 0x0001;
			/*Имя клавиши*/
			name += (name.length() ? L"+" : L"");
			int num = name.length();
			std::wcscpy(infString, &name[0]);
			num += ::GetKeyNameText(lParam, infString + num, 100 - num);
			infString[num++] = '\n';
			/*значение клавиши*/
			num += ::ToUnicode(VirtualKey, ExtScanCode, keystatebuff, infString + num, 100 - num, flags);
			infString[num++] = '\n';
			/*имя раскладки*/
			::GetKeyboardLayoutName(infString + num);
			if(num == 100)
				::wmemset(infString, 0, 100);
		}

		wchar_t resMsg[128] = {0};
		wsprintf(resMsg, L"%05u\n%s", rc, infString);

		pMe->addMsg(L"Hook", resMsg);

		/*заблокировано*/
		if(pMe->KeyLock)
			return -1;
	}

	return	CallNextHookEx(hndlKeyHook, nCode, wParam, lParam);
}

bool KeyHook::SetOn(tVariant* pvarRetValue, tVariant* paParams, const long len) {
	if(!(len == 1 && paParams->vt == VTYPE_I4)) {		
		pAdapter->AddError(1, L"Error", L"Функция принимает:\n\t0 для WH_KEYBOARD_LL\n\t1 для WH_KEYBOARD", 0);
		return false;
	}

	if(paParams->lVal==0 && !isOnLL){		
		hndlKeyHookLL = ::SetWindowsHookEx(WH_KEYBOARD_LL, 	        /* тип hook-точки, которая устанавливается (WH_KEYBOARD позволит фильтровать поток сообщений WM_KEYDOWN и WM_KEYUP)*/
										 (HOOKPROC)KeyHookProcLL,   /* адрес подключаемой процедуры*/
										 modulehandle,	            /* дескриптор экземпляра прикладной программы (nullptr - значит по идентификтору потока)*/
										 0	                        /* идентификация потока, который устанавливает hook-точку*/);
		if(hndlKeyHookLL == nullptr)
			addError(L"Ошибка подключения процедуры обработки hook-точки");
		isOnLL = hndlKeyHookLL == nullptr ? false : true;

		/*включаем вид одного события, если оба выключены*/
		if(isOnLL && !(onKeyUPLL || onKeyDOWNLL))
			onKeyUPLL = true;

		if(pvarRetValue != nullptr) {
			pvarRetValue->vt = VTYPE_BOOL;
			pvarRetValue->bVal = isOnLL;
		}
	} else if(paParams->lVal == 1 && !isOn) {
		hndlKeyHook = ::SetWindowsHookEx(WH_KEYBOARD, 	            /* тип hook-точки, которая устанавливается (WH_KEYBOARD позволит фильтровать поток сообщений WM_KEYDOWN и WM_KEYUP)*/
										   (HOOKPROC)KeyHookProc,	/* адрес подключаемой процедуры*/
										   nullptr,	                /* дескриптор экземпляра прикладной программы (nullptr - значит по идентификтору потока)*/
										   dwThreadId	            /* идентификация потока, который устанавливает hook-точку*/);
		if(hndlKeyHook == nullptr)
			addError(L"Ошибка подключения процедуры обработки hook-точки");
		isOn = hndlKeyHook == nullptr ? false : true;
		
		/*включаем вид одного события, если оба выключены*/
		if(isOn && !(onKeyUP || onKeyDOWN))
			onKeyUP = true;

		if(pvarRetValue != nullptr) {
			pvarRetValue->vt = VTYPE_BOOL;
			pvarRetValue->bVal = isOn;
		}		
	}	

	return paParams->lVal == 0 ? isOnLL : isOn;
	
}

bool KeyHook::SetOff(tVariant* pvarRetValue, tVariant* paParams, const long len) {
	int type = 0;
	if(pvarRetValue == nullptr)
		type = 2;
	else if(!(len == 1 && paParams->vt == VTYPE_I4)) {
		pAdapter->AddError(1, L"Error", L"Функция принимает:\n\t0 для WH_KEYBOARD_LL\n\t1 для WH_KEYBOARD", 0);
		return false;
	} else
		type=paParams->lVal;

	if((type == 0 || type == 2) && !isOnLL)
		;//уже выключена
	else if((type == 0 || type == 2) && ::UnhookWindowsHookEx(hndlKeyHookLL)) {
		hndlKeyHookLL = 0;
		isOnLL = false;
	} 
	
	if((type == 1 || type == 2) && !isOn)
		;//уже выключена
	else if((type == 1 || type == 2) && ::UnhookWindowsHookEx(hndlKeyHook)) {
		hndlKeyHook = 0;
		isOn = false;
	}		
	
	if(pvarRetValue != nullptr) {
		pvarRetValue->vt = VTYPE_BOOL;
		pvarRetValue->bVal = !isOn;
	}

	return !isOn;
}

bool KeyHook::Send(tVariant* pvarRetValue, tVariant* paParams, const long len) {
	
	if(!(len == 1 && paParams->vt == VTYPE_I4)) {		
		pAdapter->AddError(1, L"Error", L"Функция принимает числовой виртуальный код клавиши", 0);
		return false;
	}
		
	INPUT ip;
	ip.type           = INPUT_KEYBOARD; // описываем событие клавиатуры	
	ip.ki.wScan       = 0;              // hardware scan code клавиши
	ip.ki.time        = 0;
	ip.ki.dwExtraInfo = 0;

	// код виртуальной клавиши
	ip.ki.wVk = paParams->intVal; 
	// делаем WM_KEYDOWN      
	ip.ki.dwFlags = 0;
	SendInput(1, &ip, sizeof(INPUT));
	// делаем WM_KEYUP
	ip.ki.dwFlags = KEYEVENTF_KEYUP; 
	SendInput(1, &ip, sizeof(INPUT));

	pvarRetValue->vt = VTYPE_BOOL;
	pvarRetValue->bVal = true;

	return true;
}