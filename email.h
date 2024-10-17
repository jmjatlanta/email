#include <string>
#include <filesystem>

class EmailComposerImpl;

class EmailComposer
{
    public:
    EmailComposer();
    virtual ~EmailComposer();

    void setFrom(const std::string& addr);
    void addTo(const std::string& addr);
    void addCC(const std::string& addr);
    void addBCC(const std::string& addr);
    void setSubject(const std::string& sub);
    void setBody(const std::string& msg);
    void addAttachment(const std::filesystem::path& file);

    virtual void setSmtpUserPassword(const std::string& in);

    bool compose();

    private:
    EmailComposerImpl* impl;
};
