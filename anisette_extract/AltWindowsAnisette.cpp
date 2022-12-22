#define _CRT_SECURE_NO_WARNINGS
#include <filesystem>
namespace fs = std::filesystem;

//#define _SILENCE_EXPERIMENTAL_FILESYSTEM_DEPRECATION_WARNING
//#include <experimental/filesystem>
//namespace fs = std::experimental::filesystem;


#include <vector>
#include <string>
#include <map>
#include <exception>
#include <memory>
#include <functional>
#include <iostream>

#pragma region Error

class Error : public std::exception
{
public:
	Error(int code) : _code(code), _userInfo(std::map<std::string, std::string>())
	{
	}

	Error(int code, std::map<std::string, std::string> userInfo) : _code(code), _userInfo(userInfo)
	{
	}

	virtual std::string localizedDescription() const
	{
		return "";
	}

	int code() const
	{
		return _code;
	}

	std::map<std::string, std::string> userInfo() const
	{
		return _userInfo;
	}

	virtual std::string domain() const
	{
		return "com.rileytestut.AltServer.Error";
	}

	friend std::ostream& operator<<(std::ostream& os, const Error& error)
	{
		os << "Error: (" << error.domain() << "): " << error.localizedDescription() << " (" << error.code() << ")";
		return os;
	}

private:
	int _code;
	std::map<std::string, std::string> _userInfo;
};

typedef std::map<std::string, std::string> AnisetteData;

enum class AnisetteErrorCode
{
	iTunesNotInstalled,
	iCloudNotInstalled,
	MissingApplicationSupportFolder,
	MissingAOSKit,
	MissingObjc,
	MissingFoundation,
	InvalidiTunesInstallation,
};

class AnisetteError : Error
{
public:
	AnisetteError(AnisetteErrorCode code) : Error((int)code)
	{
	}

	virtual std::string domain() const
	{
		return "com.rileytestut.AltServer.AnisetteError";
	}

	virtual std::string localizedDescription() const
	{
		if (this->_localizedDescription.size() > 0)
		{
			return this->_localizedDescription;
		}

		switch ((AnisetteErrorCode)this->code())
		{
		case AnisetteErrorCode::iTunesNotInstalled: return "iTunes Not Found";
		case AnisetteErrorCode::iCloudNotInstalled: return "iCloud Not Found";
		case AnisetteErrorCode::MissingApplicationSupportFolder: return "Missing 'Application Support' in 'Apple' Folder.";
		case AnisetteErrorCode::MissingAOSKit: return "Missing 'AOSKit.dll' in 'Internet Services' Folder.";
		case AnisetteErrorCode::MissingFoundation: return "Missing 'Foundation.dll' in 'Apple Application Support' Folder.";
		case AnisetteErrorCode::MissingObjc: return "Missing 'objc.dll' in 'Apple Application Support' Folder.";
		case AnisetteErrorCode::InvalidiTunesInstallation: return "Invalid iTunes installation.";
		}

		return "";
	}

	void setLocalizedDescription(std::string localizedDescription)
	{
		_localizedDescription = localizedDescription;
	}

private:
	std::string _localizedDescription;
};

#pragma endregion Error

class AnisetteDataManager
{
public:
	static AnisetteDataManager* instance();

	std::shared_ptr<AnisetteData> FetchAnisetteData();
	bool LoadDependencies();

	bool ResetProvisioning();

private:
	AnisetteDataManager();
	~AnisetteDataManager();

	static AnisetteDataManager* _instance;

	bool ReprovisionDevice(std::function<void(void)> provisionCallback);
	bool LoadiCloudDependencies();

	bool loadedDependencies;
};

#include <ShlObj.h>

#include "base64.h"

std::string getServerID() {
	return "5E03738B-03F6-46B5-9A6A-F022279B3040";
}

std::string appleFolderPath()
{
	wchar_t* programFilesCommonDirectory;
	SHGetKnownFolderPath(FOLDERID_ProgramFilesCommon, 0, NULL, &programFilesCommonDirectory);

	fs::path appleDirectoryPath(programFilesCommonDirectory);
	appleDirectoryPath.append("Apple");

	return appleDirectoryPath.string();
}

std::string internetServicesFolderPath()
{
	fs::path internetServicesDirectoryPath(appleFolderPath());
	internetServicesDirectoryPath.append("Internet Services");
	return internetServicesDirectoryPath.string();
}

std::string applicationSupportFolderPath()
{
	fs::path applicationSupportDirectoryPath(appleFolderPath());
	applicationSupportDirectoryPath.append("Apple Application Support");
	return applicationSupportDirectoryPath.string();
}

//#include <WinSock2.h>
#include <sstream>
//#include <Psapi.h>

#include <set>

//#define SPOOF_MAC 1

#define id void*
#define SEL void*

typedef id(__cdecl* GETCLASSFUNC)(const char* name);
typedef id(__cdecl* REGISTERSELFUNC)(const char* name);
typedef id(__cdecl* SENDMSGFUNC)(id self, void* _cmd);
typedef id(__cdecl* SENDMSGFUNC_OBJ)(id self, void* _cmd, id parameter1);
typedef id(__cdecl* SENDMSGFUNC_INT)(id self, void* _cmd, int parameter1);
typedef id* (__cdecl* COPYMETHODLISTFUNC)(id cls, unsigned int* outCount);
typedef id(__cdecl* GETMETHODNAMEFUNC)(id method);
typedef const char* (__cdecl* GETSELNAMEFUNC)(SEL sel);
typedef id(__cdecl* GETOBJCCLASSFUNC)(id obj);

typedef id(__cdecl* GETOBJECTFUNC)();

typedef id(__cdecl* CLIENTINFOFUNC)(id obj);
typedef id(__cdecl* COPYANISETTEDATAFUNC)(void*, int, void*);

#define odslog(msg) { std::stringstream ss; ss << msg << std::endl; OutputDebugStringA(ss.str().c_str()); }

extern std::string StringFromWideString(std::wstring wideString);

GETCLASSFUNC objc_getClass;
REGISTERSELFUNC sel_registerName;
SENDMSGFUNC objc_msgSend;
COPYMETHODLISTFUNC class_copyMethodList;
GETMETHODNAMEFUNC method_getName;
GETSELNAMEFUNC sel_getName;
GETOBJCCLASSFUNC object_getClass;

GETOBJECTFUNC GetDeviceID;
GETOBJECTFUNC GetLocalUserID;
CLIENTINFOFUNC GetClientInfo;
COPYANISETTEDATAFUNC CopyAnisetteData;

class ObjcObject
{
public:
	id isa;

	std::string description() const
	{
		id descriptionSEL = sel_registerName("description");
		id descriptionNSString = objc_msgSend((void*)this, descriptionSEL);

		id cDescriptionSEL = sel_registerName("UTF8String");
		const char* cDescription = ((const char* (*)(id, SEL))objc_msgSend)(descriptionNSString, cDescriptionSEL);

		std::string description(cDescription);
		return description;
	}
};


id __cdecl ALTClientInfoReplacementFunction(void*)
{
	ObjcObject* NSString = (ObjcObject*)objc_getClass("NSString");
	id stringInit = sel_registerName("stringWithUTF8String:");

	ObjcObject* clientInfo = (ObjcObject*)((id(*)(id, SEL, const char*))objc_msgSend)(NSString, stringInit, "<MacBookPro16,1> <Mac OS X;13.1;22C65> <com.apple.AuthKit/1 (com.apple.dt.Xcode/3594.4.19)>");

	odslog("Swizzled Client Info: " << clientInfo->description());

	return clientInfo;
}

id __cdecl ALTDeviceIDReplacementFunction()
{
	ObjcObject* NSString = (ObjcObject*)objc_getClass("NSString");
	id stringInit = sel_registerName("stringWithUTF8String:");

	auto deviceIDString = getServerID();

	ObjcObject* deviceID = (ObjcObject*)((id(*)(id, SEL, const char*))objc_msgSend)(NSString, stringInit, deviceIDString.c_str());

	odslog("Swizzled Device ID: " << deviceID->description());

	return deviceID;
}

void convert_filetime(struct timeval* out_tv, const FILETIME* filetime)
{
	// Microseconds between 1601-01-01 00:00:00 UTC and 1970-01-01 00:00:00 UTC
	static const uint64_t EPOCH_DIFFERENCE_MICROS = 11644473600000000ull;

	// First convert 100-ns intervals to microseconds, then adjust for the
	// epoch difference
	uint64_t total_us = (((uint64_t)filetime->dwHighDateTime << 32) | (uint64_t)filetime->dwLowDateTime) / 10;
	total_us -= EPOCH_DIFFERENCE_MICROS;

	// Convert to (seconds, microseconds)
	out_tv->tv_sec = (time_t)(total_us / 1000000);
	out_tv->tv_usec = (long)(total_us % 1000000);
}

AnisetteDataManager* AnisetteDataManager::_instance = nullptr;

AnisetteDataManager* AnisetteDataManager::instance()
{
	if (_instance == 0)
	{
		_instance = new AnisetteDataManager();
	}

	return _instance;
}

AnisetteDataManager::AnisetteDataManager() : loadedDependencies(false)
{
}

AnisetteDataManager::~AnisetteDataManager()
{
}

#define JUMP_INSTRUCTION_SIZE 5 // 0x86 jump instruction is 5 bytes total (opcode + 4 byte address).

bool AnisetteDataManager::LoadiCloudDependencies()
{
	wchar_t* programFilesCommonDirectory;
	SHGetKnownFolderPath(FOLDERID_ProgramFilesCommon, 0, NULL, &programFilesCommonDirectory);

	fs::path appleDirectoryPath(programFilesCommonDirectory);
	appleDirectoryPath.append("Apple");

	fs::path internetServicesDirectoryPath(appleDirectoryPath);
	internetServicesDirectoryPath.append("Internet Services");

	fs::path iCloudMainPath(internetServicesDirectoryPath);
	iCloudMainPath.append("iCloud_main.dll");

	HINSTANCE iCloudMain = LoadLibrary(iCloudMainPath.c_str());
	if (iCloudMain == NULL)
	{
		return false;
	}

	// Retrieve known exported function address to provide reference point for accessing private functions.
	uintptr_t exportedFunctionAddress = (uintptr_t)GetProcAddress(iCloudMain, "PL_FreeArenaPool");
	size_t exportedFunctionDisassembledOffset = 0x1aa2a0;


	/* Reprovision Anisette Function */

	size_t anisetteFunctionDisassembledOffset = 0x241ee0;
	size_t difference = anisetteFunctionDisassembledOffset - 0x1aa2a0;

	CopyAnisetteData = (COPYANISETTEDATAFUNC)(exportedFunctionAddress + difference);
	if (CopyAnisetteData == NULL)
	{
		return false;
	}


	/* Anisette Data Functions */

	size_t clientInfoFunctionDisassembledOffset = 0x23e730;
	size_t clientInfoFunctionRelativeOffset = clientInfoFunctionDisassembledOffset - exportedFunctionDisassembledOffset;
	GetClientInfo = (CLIENTINFOFUNC)(exportedFunctionAddress + clientInfoFunctionRelativeOffset);

	size_t deviceIDFunctionDisassembledOffset = 0x23d8b0;
	size_t deviceIDFunctionRelativeOffset = deviceIDFunctionDisassembledOffset - exportedFunctionDisassembledOffset;
	GetDeviceID = (GETOBJECTFUNC)(exportedFunctionAddress + deviceIDFunctionRelativeOffset);

	size_t localUserIDFunctionDisassembledOffset = 0x23db30;
	size_t localUserIDFunctionRelativeOffset = localUserIDFunctionDisassembledOffset - exportedFunctionDisassembledOffset;
	GetLocalUserID = (GETOBJECTFUNC)(exportedFunctionAddress + localUserIDFunctionRelativeOffset);

	if (GetClientInfo == NULL || GetDeviceID == NULL || GetLocalUserID == NULL)
	{
		return false;
	}

	{
		/** Client Info Swizzling */

		int64_t* targetFunction = (int64_t*)GetClientInfo;
		int64_t* replacementFunction = (int64_t*)&ALTClientInfoReplacementFunction;

		SYSTEM_INFO system;
		GetSystemInfo(&system);
		int pageSize = system.dwAllocationGranularity;

		uintptr_t startAddress = (uintptr_t)targetFunction;
		uintptr_t endAddress = startAddress + 1;
		uintptr_t pageStart = startAddress & -pageSize;

		// Mark page containing the target function implementation as writable so we can inject our own instruction.
		DWORD permissions = 0;
		BOOL value = VirtualProtect((LPVOID)pageStart, endAddress - pageStart, PAGE_EXECUTE_READWRITE, &permissions);

		if (!value)
		{
			return false;
		}

		int32_t jumpOffset = (int64_t)replacementFunction - ((int64_t)targetFunction + JUMP_INSTRUCTION_SIZE); // Add jumpInstructionSize because offset is relative to _next_ instruction.

		// Construct jump instruction.
		// Jump doesn't return execution to target function afterwards, allowing us to completely replace the implementation.
		char instruction[5];
		instruction[0] = '\xE9'; // E9 = "Jump near (relative)" opcode
		((int32_t*)(instruction + 1))[0] = jumpOffset; // Next 4 bytes = jump offset

		// Replace first instruction in target target function with our unconditional jump to replacement function.
		char* functionImplementation = (char*)targetFunction;
		for (int i = 0; i < JUMP_INSTRUCTION_SIZE; i++)
		{
			functionImplementation[i] = instruction[i];
		}
	}

	{
		/** Device ID Swizzling */

		int64_t* targetFunction = (int64_t*)GetDeviceID;
		int64_t* replacementFunction = (int64_t*)&ALTDeviceIDReplacementFunction;

		SYSTEM_INFO system;
		GetSystemInfo(&system);
		int pageSize = system.dwAllocationGranularity;

		uintptr_t startAddress = (uintptr_t)targetFunction;
		uintptr_t endAddress = startAddress + 1;
		uintptr_t pageStart = startAddress & -pageSize;

		// Mark page containing the target function implementation as writable so we can inject our own instruction.
		DWORD permissions = 0;
		BOOL value = VirtualProtect((LPVOID)pageStart, endAddress - pageStart, PAGE_EXECUTE_READWRITE, &permissions);

		if (!value)
		{
			return false;
		}

		int32_t jumpOffset = (int64_t)replacementFunction - ((int64_t)targetFunction + JUMP_INSTRUCTION_SIZE); // Add jumpInstructionSize because offset is relative to _next_ instruction.

		// Construct jump instruction.
		// Jump doesn't return execution to target function afterwards, allowing us to completely replace the implementation.
		char instruction[5];
		instruction[0] = '\xE9'; // E9 = "Jump near (relative)" opcode
		((int32_t*)(instruction + 1))[0] = jumpOffset; // Next 4 bytes = jump offset

		// Replace first instruction in target target function with our unconditional jump to replacement function.
		char* functionImplementation = (char*)targetFunction;
		for (int i = 0; i < JUMP_INSTRUCTION_SIZE; i++)
		{
			functionImplementation[i] = instruction[i];
		}
	}
}

bool AnisetteDataManager::LoadDependencies()
{
	fs::path appleFolderPath(appleFolderPath());
	if (!fs::exists(appleFolderPath))
	{
		throw AnisetteError(AnisetteErrorCode::iTunesNotInstalled);
	}

	fs::path internetServicesDirectoryPath(internetServicesFolderPath());
	if (!fs::exists(internetServicesDirectoryPath))
	{
		throw AnisetteError(AnisetteErrorCode::iCloudNotInstalled);
	}

	fs::path aosKitPath(internetServicesDirectoryPath);
	aosKitPath.append("AOSKit.dll");

	if (!fs::exists(aosKitPath))
	{
		throw AnisetteError(AnisetteErrorCode::MissingAOSKit);
	}

	fs::path applicationSupportDirectoryPath(applicationSupportFolderPath());
	if (!fs::exists(applicationSupportDirectoryPath))
	{
		throw AnisetteError(AnisetteErrorCode::MissingApplicationSupportFolder);
	}

	fs::path objcPath(applicationSupportDirectoryPath);
	objcPath.append("objc.dll");

	if (!fs::exists(objcPath))
	{
		throw AnisetteError(AnisetteErrorCode::MissingObjc);
	}

	fs::path foundationPath(applicationSupportDirectoryPath);
	foundationPath.append("Foundation.dll");

	if (!fs::exists(foundationPath))
	{
		throw AnisetteError(AnisetteErrorCode::MissingFoundation);
	}

	BOOL result = SetCurrentDirectory(applicationSupportDirectoryPath.c_str());
	DWORD dwError = GetLastError();

	HINSTANCE objcLibrary = LoadLibrary(objcPath.c_str());
	HINSTANCE foundationLibrary = LoadLibrary(foundationPath.c_str());
	HINSTANCE AOSKit = LoadLibrary(aosKitPath.c_str());

	dwError = GetLastError();

	if (objcLibrary == NULL || AOSKit == NULL || foundationLibrary == NULL)
	{
		char buffer[256];
		FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL, GetLastError(),
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), buffer, 256, NULL);

		auto error = AnisetteError(AnisetteErrorCode::InvalidiTunesInstallation);
		error.setLocalizedDescription(buffer);
		throw error;
	}

	/* Objective-C runtime functions */

	objc_getClass = (GETCLASSFUNC)GetProcAddress(objcLibrary, "objc_getClass");
	sel_registerName = (REGISTERSELFUNC)GetProcAddress(objcLibrary, "sel_registerName");
	objc_msgSend = (SENDMSGFUNC)GetProcAddress(objcLibrary, "objc_msgSend");

	class_copyMethodList = (COPYMETHODLISTFUNC)GetProcAddress(objcLibrary, "class_copyMethodList");
	method_getName = (GETMETHODNAMEFUNC)GetProcAddress(objcLibrary, "method_getName");
	sel_getName = (GETSELNAMEFUNC)GetProcAddress(objcLibrary, "sel_getName");
	object_getClass = (GETOBJCCLASSFUNC)GetProcAddress(objcLibrary, "object_getClass");

	if (objc_getClass == NULL)
	{
		throw AnisetteError(AnisetteErrorCode::InvalidiTunesInstallation);
	}

#if SPOOF_MAC
	if (!this->LoadiCloudDependencies())
	{
		return false;
	}
#endif

	this->loadedDependencies = true;

	return true;
}

std::shared_ptr<AnisetteData> AnisetteDataManager::FetchAnisetteData()
{
	if (!this->loadedDependencies)
	{
		this->LoadDependencies();
	}

#if SPOOF_MAC
	if (GetClientInfo == NULL || GetDeviceID == NULL || GetLocalUserID == NULL)
	{
		return NULL;
	}
#endif

	std::shared_ptr<AnisetteData> anisetteData = NULL;

	this->ReprovisionDevice([&anisetteData]() {
		// Device is temporarily provisioned as a Mac, so access anisette data now.

		ObjcObject* NSString = (ObjcObject*)objc_getClass("NSString");
		id stringInit = sel_registerName("stringWithUTF8String:");

		/* One-Time Pasword */
		ObjcObject* dsidString = (ObjcObject*)((id(*)(id, SEL, const char*))objc_msgSend)(NSString, stringInit, "-2");
		ObjcObject* machineIDKey = (ObjcObject*)((id(*)(id, SEL, const char*))objc_msgSend)(NSString, stringInit, "X-Apple-MD-M");
		ObjcObject* otpKey = (ObjcObject*)((id(*)(id, SEL, const char*))objc_msgSend)(NSString, stringInit, "X-Apple-MD");

		ObjcObject* AOSUtilities = (ObjcObject*)objc_getClass("AOSUtilities");
		ObjcObject* headers = (ObjcObject*)((id(*)(id, SEL, id))objc_msgSend)(AOSUtilities, sel_registerName("retrieveOTPHeadersForDSID:"), dsidString);

		ObjcObject* machineID = (ObjcObject*)((id(*)(id, SEL, id))objc_msgSend)(headers, sel_registerName("objectForKey:"), machineIDKey);
		ObjcObject* otp = (ObjcObject*)((id(*)(id, SEL, id))objc_msgSend)(headers, sel_registerName("objectForKey:"), otpKey);

		if (otp == NULL || machineID == NULL)
		{
			std::cerr << "otp or machine is Null!" << std::endl;
			return;
		}

		odslog("OTP: " << otp->description() << " MachineID: " << machineID->description());

		/* Device Hardware */

		ObjcObject* deviceDescription = (ObjcObject*)ALTClientInfoReplacementFunction(NULL);
		ObjcObject* deviceID = (ObjcObject*)ALTDeviceIDReplacementFunction();

		if (deviceDescription == NULL || deviceID == NULL)
		{
			std::cerr << "device Info is Null!" << std::endl;
			return;
		}

#if SPOOF_MAC
		ObjcObject* localUserID = (ObjcObject*)GetLocalUserID();
#else
		std::string description = deviceID->description();

		std::vector<unsigned char> deviceIDData(description.begin(), description.end());
		//auto encodedDeviceID = StringFromWideString(utility::conversions::to_base64(deviceIDData));
		auto encodedDeviceID = macaron::Base64::Encode(deviceIDData);

		ObjcObject* localUserID = (ObjcObject*)((id(*)(id, SEL, const char*))objc_msgSend)(NSString, stringInit, encodedDeviceID.c_str());
#endif

		std::string deviceSerialNumber = "C02LKHBBFD57";

		if (localUserID == NULL)
		{
			std::cerr << "localUserID is Null!" << std::endl;
			return;
		}

		anisetteData = std::make_shared<AnisetteData>();
		anisetteData->insert(std::make_pair("X-Apple-I-MD-M", machineID->description()));
		anisetteData->insert(std::make_pair("X-Apple-I-MD", otp->description()));
		anisetteData->insert(std::make_pair("X-Apple-I-MD-LU", localUserID->description()));
		anisetteData->insert(std::make_pair("X-Apple-I-MD-RINFO", "17106176"));
		anisetteData->insert(std::make_pair("X-Mme-Device-Id", deviceID->description()));
		anisetteData->insert(std::make_pair("X-Apple-I-SRL-NO", deviceSerialNumber));
		anisetteData->insert(std::make_pair("X-MMe-Client-Info", deviceDescription->description()));

		time_t ts = time(NULL);
		char dateString[100] = { 0 };
		strftime(dateString, sizeof dateString, "%Y-%m-%dT%H:%M:%SZ", gmtime(&ts));
		anisetteData->insert(std::make_pair("X-Apple-I-Client-Time", dateString));
		anisetteData->insert(std::make_pair("X-Apple-Locale", "en_US"));
		anisetteData->insert(std::make_pair("X-Apple-I-TimeZone", "PST"));
		
		});

	return anisetteData;
}

bool AnisetteDataManager::ReprovisionDevice(std::function<void(void)> provisionCallback)
{
#if !SPOOF_MAC
	provisionCallback();
	return true;
#else
	std::string adiDirectoryPath = "C:\\ProgramData\\Apple Computer\\iTunes\\adi";

	/* Start Provisioning */

	// Move iCloud's ADI files (so we don't mess with them).
	for (const auto& entry : fs::directory_iterator(adiDirectoryPath))
	{
		if (entry.path().extension() == ".pb")
		{
			fs::path backupPath = entry.path();
			backupPath += ".icloud";

			fs::rename(entry.path(), backupPath);
		}
	}

	// Copy existing AltServer .pb files into original location to reuse the MID.
	for (const auto& entry : fs::directory_iterator(adiDirectoryPath))
	{
		if (entry.path().extension() == ".altserver")
		{
			fs::path path = entry.path();
			path.replace_extension();

			fs::rename(entry.path(), path);
		}
	}

	auto cleanUp = [adiDirectoryPath]() {
		/* Finish Provisioning */

		// Backup AltServer ADI files.
		for (const auto& entry : fs::directory_iterator(adiDirectoryPath))
		{
			// Backup AltStore file
			if (entry.path().extension() == ".pb")
			{
				fs::path backupPath = entry.path();
				backupPath += ".altserver";

				fs::rename(entry.path(), backupPath);
			}
		}

		// Copy iCloud ADI files back to original location.
		for (const auto& entry : fs::directory_iterator(adiDirectoryPath))
		{
			if (entry.path().extension() == ".icloud")
			{
				// Move backup file to original location
				fs::path path = entry.path();
				path.replace_extension();

				fs::rename(entry.path(), path);

				odslog("Copying iCloud file from: " << entry.path().string() << " to: " << path.string());
			}
		}
	};

	// Calling CopyAnisetteData implicitly generates new anisette data,
	// using the new client info string we injected.
	ObjcObject* error = NULL;
	ObjcObject* anisetteDictionary = (ObjcObject*)CopyAnisetteData(NULL, 0x1, &error);

	try
	{
		if (anisetteDictionary == NULL)
		{
			odslog("Reprovision Error:" << ((ObjcObject*)error)->description());

			ObjcObject* localizedDescription = (ObjcObject*)((id(*)(id, SEL))objc_msgSend)(error, sel_registerName("localizedDescription"));
			if (localizedDescription)
			{
				int errorCode = ((int(*)(id, SEL))objc_msgSend)(error, sel_registerName("code"));
				throw LocalizedError(errorCode, localizedDescription->description());
			}
			else
			{
				throw ServerError(ServerErrorCode::InvalidAnisetteData);
			}
		}

		odslog("Reprovisioned Anisette:" << anisetteDictionary->description());

		setReprovisionedDevice(true);

		// Call callback while machine is provisioned for AltServer.
		provisionCallback();
	}
	catch (std::exception& exception)
	{
		cleanUp();

		throw;
	}

	cleanUp();

	return true;
#endif
}

bool AnisetteDataManager::ResetProvisioning()
{
	std::string adiDirectoryPath = "C:\\ProgramData\\Apple Computer\\iTunes\\adi";

	// Remove existing AltServer .pb files so we can create new ones next time we provision this device.
	for (const auto& entry : fs::directory_iterator(adiDirectoryPath))
	{
		if (entry.path().extension() == ".altserver")
		{
			fs::remove(entry.path());
		}
	}

	return true;
}

#include <iostream>

int main()
{
	auto anisetteData = AnisetteDataManager::instance()->FetchAnisetteData();
	if (!anisetteData) {
		std::cerr << "Anisette Data failed!" << std::endl;
		return 1;
	}
	for (const auto& elem : *anisetteData)
	{
		std::cout << elem.first << ": " << elem.second << "\n";
	}
}
