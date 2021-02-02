#include <cstring>
#include <string>

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include "MessageBoxHook.h"
#include "Util.h"

namespace
{
	void LogText(const char *text)
	{
		const size_t textLength = std::strlen(text);

		std::string result = "MessageBox: ";
		result.reserve(result.length() + textLength + 2);  // CRLF

		for (size_t i = 0; i < textLength; i++)
		{
			switch (text[i])
			{
				case '\b': result += "\\b"; break;
				case '\f': result += "\\f"; break;
				case '\n': result += "\\n"; break;
				case '\r': result += "\\r"; break;
				case '\t': result += "\\t"; break;
				case '\v': result += "\\v"; break;
				default:   result += text[i];
			}
		}

		result += "\r\n";

		DWORD bytesWritten;
		WriteFile(GetStdHandle(STD_ERROR_HANDLE), result.c_str(), (DWORD) result.length(), &bytesWritten, NULL);
	}

	std::string ConvertUTF16To8(const wchar_t *value)
	{
		std::string result;

		int charCount = WideCharToMultiByte(CP_UTF8, 0, value, -1, NULL, 0, NULL, NULL);
		if (charCount > 0)
		{
			result.resize(charCount - 1);  // the count includes terminating null character in this case

			WideCharToMultiByte(CP_UTF8, 0, value, -1, const_cast<char*>(result.data()), charCount, NULL, NULL);
		}

		return result;
	}

	int __stdcall MessageBoxIndirectA_Hook(const MSGBOXPARAMSA *params)
	{
		LogText(params->lpszText);

		return IDOK;
	}

	int __stdcall MessageBoxIndirectW_Hook(const MSGBOXPARAMSW *params)
	{
		LogText(ConvertUTF16To8(params->lpszText).c_str());

		return IDOK;
	}

	void Hook(void *pFunc, void *pNewFunc)
	{
		if (!pFunc)
		{
			return;
		}

	#ifdef BUILD_64BIT
		unsigned char code[] = {
			0x48, 0xB8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  // mov rax, 0x0
			0xFF, 0xE0                                                   // jmp rax
		};

		std::memcpy(&code[2], &pNewFunc, 8);
	#else
		unsigned char code[] = {
			0xB8, 0x00, 0x00, 0x00, 0x00,  // mov eax, 0x0
			0xFF, 0xE0                     // jmp eax
		};

		std::memcpy(&code[1], &pNewFunc, 4);
	#endif

		Util::FillMem(pFunc, code, sizeof code);
	}
}

void MessageBoxHook::Init()
{
	HMODULE user32 = LoadLibraryA("user32.dll");

	if (user32)
	{
		Hook(GetProcAddress(user32, "MessageBoxIndirectA"), MessageBoxIndirectA_Hook);
		Hook(GetProcAddress(user32, "MessageBoxIndirectW"), MessageBoxIndirectW_Hook);
	}
}
