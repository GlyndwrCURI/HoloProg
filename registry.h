#pragma once
#include <string>
#include <Shlwapi.h>
#include <tchar.h>

typedef std::basic_string<TCHAR> stdstring;

class CRegStdBase {
public:	//methods
	/**
	 * Removes the whole registry key including all values. So if you set the registry
	 * entry to be HKCU\Software\Company\Product\key\value there will only be
	 * HKCU\Software\Company\Product key in the registry.
	 * \return ERROR_SUCCESS or an nonzero errorcode. Use FormatMessage() to get an error description.
	 */
	DWORD removeKey() { RegOpenKeyEx(m_base, m_path.c_str(), 0, KEY_WRITE, &m_hKey); return SHDeleteKey(m_base, m_path.c_str()); }
	/**
	 * Removes the value of the registry object. If you set the registry entry to
	 * be HKCU\Software\Company\Product\key\value there will only be
	 * HKCU\Software\Company\Product\key\ in the registry.
	 * \return ERROR_SUCCESS or an nonzero errorcode. Use FormatMessage() to get an error description.
	 */
	LONG removeValue() { RegOpenKeyEx(m_base, m_path.c_str(), 0, KEY_WRITE, &m_hKey); return RegDeleteValue(m_hKey, m_key.c_str()); }

public:	//members
	HKEY m_base;		///< handle to the registry base
	HKEY m_hKey;		///< handle to the open registry key
	stdstring m_key;		///< the name of the value
	stdstring m_path;		///< the path to the key
};

/**
 * \ingroup CommonClasses
 * std::string value in registry. with this class you can use std::string values in registry
 * almost like normal std::string variables in your program.
 * Usage:
 * in your header file, declare your registry std::string variable:
 * \code
 * CRegStdString regvalue;
 * \endcode
 * next initialize the variable e.g. in the constructor of your class:
 * \code
 * regvalue = CRegStdString("Software\\Company\\SubKey\\MyValue", "default");
 * \endcode
 * this will set the registry value "MyValue" under HKEY_CURRENT_USER with path 
 * "Software\Company\SubKey" to the variable. If the key does not yet exist or
 * an error occured during read from the registry, a default
 * value of "default" is used when accessing the variable.
 * to avoid too much access to the registry the value is cached inside the object.
 * once the value is read, no more read accesses to the registry will be made.
 * this means the variable will contain a wrong value if the corresponding registry
 * entry is changed by anything else than this variable! If you think that could happen
 * then use 
 * \code
 * regvalue.read();
 * \endcode
 * to force a refresh of the variable with the registry.
 * a write to the registry is only made if the new value assigned with the variable
 * is different than the last assigned value.
 * to force a write use the method write();
 * another option to force reads and writes to the registry is to specify TRUE as the
 * third parameter in the constructor.
 *
 * \par requirements 
 * win98 or later, win2k or later, win95 with IE4 or later, winNT4 with IE4 or later
 * import library Shlwapi.lib
 *
 * \author Stefan Kueng (stefan_kueng@catv.rol.ch)
 *
 * \par license 
 * This code is absolutely free to use and modify. The code is provided "as is" with
 * no expressed or implied warranty. The author accepts no liability if it causes
 * any damage to your computer, causes your pet to fall ill, increases baldness 
 * or makes your car start emitting strange noises when you start it up.
 * This code has no bugs, just undocumented features!
 * 
 * \version 1.0
 * first version which uses std::string instead of CString.
 * \date 10-2002
 */
class CRegStdString : public CRegStdBase {
public:
	CRegStdString();
	/**
	 * Constructor.
	 * \param key the path to the key, including the key. example: "Software\\Company\\SubKey\\MyValue"
	 * \param def the default value used when the key does not exist or a read error occured
	 * \param force set to TRUE if no cache should be used, i.e. always read and write directly from/to registry
	 * \param base a predefined base key like HKEY_LOCAL_MACHINE. see the SDK documentation for more information.
	 */
	CRegStdString(stdstring key, stdstring def = _T(""), BOOL force = FALSE, HKEY base = HKEY_CURRENT_USER);
	~CRegStdString(void);
	
	stdstring read();						///< reads the value from the registry
	void	write();					///< writes the value to the registry
		
	operator stdstring();
	CRegStdString& operator=(stdstring s);
	CRegStdString& operator+=(stdstring s) { return *this = (stdstring)*this + s; }
	operator LPCTSTR();
	
	
protected:

	stdstring	m_value;				///< the cached value of the registry
	stdstring	m_defaultvalue;			///< the default value to use
	BOOL	m_read;						///< indicates if the value has already been read from the registry
	BOOL	m_force;					///< indicates if no cache should be used, i.e. always read and write directly from registry
};

/**
 * \ingroup CommonClasses
 * DWORD value in registry. with this class you can use DWORD values in registry
 * like normal DWORD variables in your program.
 * Usage:
 * in your header file, declare your registry DWORD variable:
 * \code
 * CRegStdWORD regvalue;
 * \endcode
 * next initialize the variable e.g. in the constructor of your class:
 * \code
 * regvalue = CRegStdWORD("Software\\Company\\SubKey\\MyValue", 100);
 * \endcode
 * this will set the registry value "MyValue" under HKEY_CURRENT_USER with path 
 * "Software\Company\SubKey" to the variable. If the key does not yet exist or
 * an error occured during read from the registry, a default
 * value of 100 is used when accessing the variable.
 * now the variable can be used like any other DWORD variable:
 * \code
 * regvalue = 200;						//stores the value 200 in the registry
 * int temp = regvalue + 300;			//temp has value 500 now
 * regvalue += 300;						//now the registry has the value 500 too
 * \endcode
 * to avoid too much access to the registry the value is cached inside the object.
 * once the value is read, no more read accesses to the registry will be made.
 * this means the variable will contain a wrong value if the corresponding registry
 * entry is changed by anything else than this variable! If you think that could happen
 * then use 
 * \code
 * regvalue.read();
 * \endcode
 * to force a refresh of the variable with the registry.
 * a write to the registry is only made if the new value assigned with the variable
 * is different than the last assigned value.
 * to force a write use the method write();
 * another option to force reads and writes to the registry is to specify TRUE as the
 * third parameter in the constructor.
 *
 * \par requirements 
 * win98 or later, win2k or later, win95 with IE4 or later, winNT4 with IE4 or later
 * import library Shlwapi.lib
 *
 * \author Stefan Kueng
 *
 * \par license 
 * This code is absolutely free to use and modify. The code is provided "as is" with
 * no expressed or implied warranty. The author accepts no liability if it causes
 * any damage to your computer, causes your pet to fall ill, increases baldness 
 * or makes your car start emitting strange noises when you start it up.
 * This code has no bugs, just undocumented features!
 * 
 * \version 1.0 
 * \date 11-2002
 * \todo
 * \bug
 * \warning
 *
 */
class CRegStdWORD : public CRegStdBase {
public:
	CRegStdWORD();
	/**
	 * Constructor.
	 * \param key the path to the key, including the key. example: "Software\\Company\\SubKey\\MyValue"
	 * \param def the default value used when the key does not exist or a read error occured
	 * \param force set to TRUE if no cache should be used, i.e. always read and write directly from/to registry
	 * \param base a predefined base key like HKEY_LOCAL_MACHINE. see the SDK documentation for more information.
	 */
	CRegStdWORD(stdstring key, DWORD def = 0, BOOL force = FALSE, HKEY base = HKEY_CURRENT_USER);
	~CRegStdWORD(void);
	
	DWORD read();						///< reads the value from the registry
	void	write();					///< writes the value to the registry
		
	operator DWORD();
	CRegStdWORD& operator=(DWORD d);
	CRegStdWORD& operator+=(DWORD d) { return *this = *this + d;}
	CRegStdWORD& operator-=(DWORD d) { return *this = *this - d;}
	CRegStdWORD& operator*=(DWORD d) { return *this = *this * d;}
	CRegStdWORD& operator/=(DWORD d) { return *this = *this / d;}
	CRegStdWORD& operator%=(DWORD d) { return *this = *this % d;}
	CRegStdWORD& operator<<=(DWORD d) { return *this = *this << d;}
	CRegStdWORD& operator>>=(DWORD d) { return *this = *this >> d;}
	CRegStdWORD& operator&=(DWORD d) { return *this = *this & d;}
	CRegStdWORD& operator|=(DWORD d) { return *this = *this | d;}
	CRegStdWORD& operator^=(DWORD d) { return *this = *this ^ d;}
	
	
protected:

	DWORD	m_value;				///< the cached value of the registry
	DWORD	m_defaultvalue;			///< the default value to use
	BOOL	m_read;					///< indicates if the value has already been read from the registry
	BOOL	m_force;				///< indicates if no cache should be used, i.e. always read and write directly from registry
};
