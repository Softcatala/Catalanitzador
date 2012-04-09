/* 
 * Copyright (C) 2012 Jordi Mas i Hernàndez <jmas@softcatala.org>
 * 
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  
 * 02111-1307, USA.
 */

#include "stdafx.h"
#include "Sha1Sum.h"
#include "StringConversion.h"

Sha1Sum::Sha1Sum(wstring file)
{
	m_file = file;
}

wstring Sha1Sum::ComputeforFile()
{
	_compute();
	g_log.Log(L"Sha1Sum::ComputeforFile for %s returns %s", (wchar_t *) m_file.c_str(), (wchar_t *) m_sum.c_str());
	return m_sum;
}

wstring Sha1Sum::ReadFromFile()
{
	HANDLE hFile;
	char sha1[SHA1LEN + 1];
	DWORD cbRead = 0;

	m_sum.erase();

	hFile = CreateFile(m_file.c_str(), GENERIC_READ,
        FILE_SHARE_READ,  NULL, OPEN_EXISTING, FILE_FLAG_SEQUENTIAL_SCAN, NULL);

	if (hFile != NULL)
	{
		if (ReadFile(hFile, sha1, SHA1LEN, &cbRead, NULL) == TRUE)
		{
			int i = 0;
			for (i = 0; i < SHA1LEN && isspace(sha1[i]) == false; i++);

			sha1[i] = NULL;

			StringConversion::ToWideChar(sha1, m_sum);			
		}
		CloseHandle(hFile);
	}

	g_log.Log(L"Sha1Sum::ReadFromFile for %s returns %s", (wchar_t *) m_file.c_str(), (wchar_t *) m_sum.c_str());
	return m_sum;
}

void Sha1Sum::_compute()
{
	DWORD dwStatus = 0;
    BOOL bResult = FALSE;
    HCRYPTPROV hProv = 0;
    HCRYPTHASH hHash = 0;
    HANDLE hFile;
    BYTE buffer[BUFSIZE];
    DWORD cbRead = 0;
    BYTE rgbHash[SHA1LEN];    
    char digits[] = "0123456789abcdef";
    
	m_sum.erase();
	hFile = CreateFile(m_file.c_str(), GENERIC_READ,
        FILE_SHARE_READ,  NULL, OPEN_EXISTING, FILE_FLAG_SEQUENTIAL_SCAN, NULL);

    if (INVALID_HANDLE_VALUE == hFile)
        return;
    
    if (!CryptAcquireContext(&hProv, NULL,  NULL, PROV_RSA_FULL, CRYPT_VERIFYCONTEXT))
    {
        CloseHandle(hFile);
        return;
    }

    if (!CryptCreateHash(hProv, CALG_SHA1, 0, 0, &hHash))
    {
        CloseHandle(hFile);
        CryptReleaseContext(hProv, 0);
        return;
    }

    while (bResult = ReadFile(hFile, buffer, BUFSIZE, &cbRead, NULL))
    {
        if (0 == cbRead)        
            break;        

        if (!CryptHashData(hHash, buffer, cbRead, 0))
        {
            CryptReleaseContext(hProv, 0);
            CryptDestroyHash(hHash);
            CloseHandle(hFile);
            return;
        }
    }

    if (!bResult)
    {
        CryptReleaseContext(hProv, 0);
        CryptDestroyHash(hHash);
        CloseHandle(hFile);     
    }

    cbRead = SHA1LEN;
    if (CryptGetHashParam(hHash, HP_HASHVAL, rgbHash, &cbRead, 0))
    {
        for (DWORD i = 0; i < cbRead; i++)
        {
			wchar_t c[16];
			wsprintf(c, L"%c%c", digits[rgbHash[i] >> 4], digits[rgbHash[i] & 0xf]);
			m_sum += c;
        }
    }
    
    CryptDestroyHash(hHash);
    CryptReleaseContext(hProv, 0);
    CloseHandle(hFile);
}
 
bool Sha1Sum::operator==(Sha1Sum& other)
{
	wstring s1, s2;
	s1 = this->GetSum();
	s2 = other.GetSum();

	return s1 == s2;
}

