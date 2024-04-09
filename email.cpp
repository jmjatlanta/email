#include <vector>
#include <sstream>
#include "email.h"

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

class FirefoxEmailComposer : public EmailComposerImpl
{
    public:
    ~FirefoxEmailComposer() {}

    bool compose() override
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
};

EmailComposer::EmailComposer()
{
    impl = new FirefoxEmailComposer();
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

