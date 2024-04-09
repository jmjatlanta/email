#include <vector>
#include <sstream>
#include "email.h"
#ifdef _WINDOWS
#include "Windows.h"
#include "MAPI.h"
#include "libloaderapi.h"
#include <locale>
#include <codecvt>
#endif

class EmailComposerImpl
{
    public:
    virtual ~EmailComposerImpl() {}
    void addTo(const std::string& in) { toCollection.push_back(in); }
    void addCC(const std::string& in) { ccCollection.push_back(in); }
    void addBCC(const std::string& in) { bccCollection.push_back(in); }
    void addAttachment(const std::filesystem::path& file) { attachments.push_back(file); }
    void setSubject(const std::string& in) { subject = in; }
    void setBody(const std::string& in) { body = in; }

    virtual bool compose() = 0;

    protected:
    std::string from;
    std::vector<std::string> toCollection;
    std::vector<std::string> ccCollection;
    std::vector<std::string> bccCollection;
    std::string subject;
    std::string body;
    std::vector<std::filesystem::path> attachments;
};

#ifdef _WINDOWS
class MapiEmailComposer : public EmailComposerImpl
{
	public:
	MapiEmailComposer();
	~MapiEmailComposer();

	bool compose() override;

	protected:
	HMODULE lib;
	LPMAPILOGON mapiLogon = nullptr;
	LPMAPILOGOFF mapiLogoff = nullptr;
	LPMAPISENDMAILW mapiSendMailW = nullptr;
    LHANDLE session;
};
#endif

class FirefoxEmailComposer : public EmailComposerImpl
{
    public:
    ~FirefoxEmailComposer() {}

    bool compose() override;
};

EmailComposer::EmailComposer()
{
#ifdef __linux__
    impl = new FirefoxEmailComposer();
#elif _WINDOWS
	impl = new MapiEmailComposer();
#endif
}

EmailComposer::~EmailComposer()
{
    delete impl;
};

void EmailComposer::addTo(const std::string& in) { impl->addTo(in); }
void EmailComposer::addCC(const std::string& in) { impl->addCC(in); }
void EmailComposer::addBCC(const std::string& in) { impl->addBCC(in); }
void EmailComposer::setSubject(const std::string& in) { impl->setSubject(in); }
void EmailComposer::setBody(const std::string& in) { impl->setBody(in); }
void EmailComposer::addAttachment(const std::filesystem::path& in) { impl->addAttachment(in); }
bool EmailComposer::compose() { return impl->compose(); }

#ifdef _WINDOWS
/****
 * @brief return a value from HKLM as a string
 * @throws std::runtime_error 
 * @returns the value from the registry
*/
std::wstring GetStringValueFromHKLM(const std::wstring& regSubKey, const std::wstring& regValue)
{
    size_t bufferSize = 0xFFF; // If too small, will be resized down below.
    std::wstring valueBuf; // Contiguous buffer since C++11.
    valueBuf.resize(bufferSize);
    auto cbData = static_cast<DWORD>(bufferSize * sizeof(wchar_t));
    auto rc = RegGetValueW(
        HKEY_LOCAL_MACHINE,
        regSubKey.c_str(),
        regValue.c_str(),
        RRF_RT_REG_SZ,
        nullptr,
        static_cast<void*>(valueBuf.data()),
        &cbData
    );
    while (rc == ERROR_MORE_DATA)
    {
        // Get a buffer that is big enough.
        cbData /= sizeof(wchar_t);
        if (cbData > static_cast<DWORD>(bufferSize))
        {
            bufferSize = static_cast<size_t>(cbData);
        }
        else
        {
            bufferSize *= 2;
            cbData = static_cast<DWORD>(bufferSize * sizeof(wchar_t));
        }
        valueBuf.resize(bufferSize);
        rc = RegGetValueW(
            HKEY_LOCAL_MACHINE,
            regSubKey.c_str(),
            regValue.c_str(),
            RRF_RT_REG_SZ,
            nullptr,
            static_cast<void*>(valueBuf.data()),
            &cbData
        );
    }
    if (rc == ERROR_SUCCESS)
    {
        cbData /= sizeof(wchar_t);
        valueBuf.resize(static_cast<size_t>(cbData - 1)); // remove end null character
        return valueBuf;
    }
    else
    {
        throw std::runtime_error("Windows system error code: " + std::to_string(rc));
    }
}

/***
 * @return the path to the default mapi dll
*/
std::wstring getMapiDllFromRegistry()
{
    std::wstring regSubKey;
    regSubKey = L"SOFTWARE\\Clients\\Mail";
    std::wstring expectedValueFromRegistry;
    expectedValueFromRegistry = L"";
    try
    {
        std::wstring valueFromRegistry = GetStringValueFromHKLM(regSubKey, expectedValueFromRegistry);
        std::wstring fullPath = regSubKey + L"\\" + valueFromRegistry;
        std::wstring dllPath = L"DLLPath";
        return GetStringValueFromHKLM(fullPath, dllPath);
    }
    catch(const std::exception&)
    {
    }
    return std::wstring{};
}

MapiEmailComposer::MapiEmailComposer()
{
    std::wstring dll = getMapiDllFromRegistry();
    if (dll.empty())
        dll = L"MAPI32.dll";
	lib = LoadLibraryW(dll.c_str());
	if (lib == 0)
		throw std::invalid_argument("Could not load MAPI32.DLL");

	mapiLogon = (LPMAPILOGON)GetProcAddress(lib, "MAPILogon");
	mapiLogoff = (LPMAPILOGOFF)GetProcAddress(lib, "MAPILogoff");
	mapiSendMailW = (LPMAPISENDMAILW)GetProcAddress(lib, "MAPISendMailW");
	if (mapiLogon == nullptr 
			|| mapiLogoff == nullptr 
			|| mapiSendMailW == nullptr)
		throw std::invalid_argument("Unable to get MAPI initializers");
    auto retval = mapiLogon(0, nullptr, nullptr, MAPI_LOGON_UI, 0L, &session);
    if (retval != 0 || session <= 0)
        throw std::invalid_argument("Unable to log on to MAPI session");
}
MapiEmailComposer::~MapiEmailComposer()
{
    mapiLogoff(session, 0, 0, 0);
	if (lib != nullptr)
		FreeLibrary(lib);
}
bool MapiEmailComposer::compose()
{
    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
	// recipient
	MapiRecipDescW recipient;
	recipient.ulRecipClass = 1;
	recipient.ulReserved = 0;
	recipient.ulEIDSize = 0;
	recipient.lpEntryID = NULL;
    std::wstring to;
	if (toCollection.size() > 0)
    {
        to = converter.from_bytes(toCollection[0]);
		recipient.lpszName = &to[0];
        recipient.lpszAddress = &to[0];
    }

    // start building message
	MapiMessageW message;
	ZeroMemory(&message, sizeof(message));

    // attachments
    MapiFileDescW* attach = nullptr;
    size_t attach_size = attachments.size();
    std::wstring* str = new std::wstring[attach_size];
    if (attach_size == 0)
    {
	    message.nFileCount = 0;
	    message.lpFiles = nullptr;
    }
    else
    {
        attach = new MapiFileDescW[attach_size];

        message.nFileCount = attach_size;
        message.lpFiles = attach;
        for(size_t i = 0; i < attachments.size(); ++i)
        {
            str[i] = converter.from_bytes(attachments[i].string());
            attach[i].ulReserved = 0;
            attach[i].flFlags = 0;
            attach[i].nPosition = -1;
            attach[i].lpFileType = nullptr;
            attach[i].lpszFileName = &str[i][0];
            attach[i].lpszPathName = &str[i][0];
        }
    }

	// Rest of message
    std::wstring sub = converter.from_bytes(subject);
	message.lpszSubject = &sub[0];
    std::wstring bod = converter.from_bytes(body);
	message.lpszNoteText = &bod[0];
	message.lpRecips = &recipient;
	message.nRecipCount = 1;

    ULONG_PTR ulUIParam = 0;
	auto ret = mapiSendMailW(session, ulUIParam, &message, MAPI_LOGON_UI | MAPI_DIALOG, 0);
    delete [] attach;
    delete [] str;
	return ret == 0;
}
#endif

bool FirefoxEmailComposer::compose()
{
	std::stringstream command;
	command << "thunderbird -compose ";
	bool isFirst = true;
	if (!subject.empty())
	{
	    if (!isFirst)
		command << ",";
	    isFirst = false;
	    command << "\"subject='" << subject << "'";
	}
	for(auto addr : toCollection)
	{
	    if (!isFirst)
		command << ",";
	    isFirst = false;
	    command << "to='" << addr << "'";
	}
	if (!body.empty())
	{
	    if (!isFirst)
		command << ",";
	    isFirst = false;
	    command << "body='" << body << "'" << "\"";
	}
	for(auto att : attachments)
	{
	    if (!isFirst)
		command << ",";
	    isFirst = false;
	    command << "attachment='" << att.string() << "'";
	}
	return system(command.str().c_str()) == 0;
}

