#include <stdint.h>
#include <cstdio>
#include <iostream>
#include <sstream>
#include <typeinfo>
#include <sys/types.h>
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/cxx11/all_of.hpp>
#include <boost/algorithm/cxx11/any_of.hpp>
#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/find.hpp>
#include <boost/algorithm/string/join.hpp>
#include <boost/algorithm/string/predicate.hpp>

#include "Processor.hxx"
#include "Session.hxx"
#include "SpiDriver.hxx"
#include "BadUsageException.hxx"

using namespace std;

class SendProcessor : public Processor
{
public:
    SendProcessor()
        : Processor(getCommand())
    {
    }

    string getCommand() {
        return string("send");
    }

    string getShortDescription() {
        return string("Send SPI packets");
    }

    void myUsage() {
        printf("Usage: %s [options] <data>\n", getCommand().c_str());
        printf("  Options:\n");
        printf("    -c: send as ASCII characters\n");
    }

    void process(const vector<string>& argv, Session* session) {

        size_t ia = 1;
        // read options
        bool asChar = false;
        while (ia < argv.size()) {
            const string& value = argv[ia];
            if (value == "-c") {
                asChar = true;
            }
            else {
                break;
            }
            ++ia;
        }

        if (ia >= argv.size()) {
            throw BadUsageException();
        }

        // make the byte array to send
        vector<uint8_t> data;
        std::string error;
        for (; ia < argv.size(); ++ia) {
            try {
                string token = argv[ia];
                unsigned int temp;

                bool isAscii = asChar;
                bool isHex = false;
                bool isDec = false;

                // syntax check
                if (!isAscii) {
                    if (boost::starts_with(token, "0x") || boost::starts_with(token, "0X")) {
                        string payload = token.substr(2);
                        isHex = boost::algorithm::all_of(payload.begin(), payload.end(),
                                                         boost::is_any_of("0123456789aAbBcCdDeEfF"));
                        isAscii = !isHex;
                    }
                    else {
                        isDec = boost::algorithm::all_of(token.begin(), token.end(), boost::is_digit());
                        isAscii = !isDec;
                    }
                }

                if (isAscii) {
                    const string& dataString = argv[ia];
                    for (size_t i = 0; i < dataString.size(); ++i) {
                        data.push_back(dataString.at(i));
                    }
                }
                else {
                    if (isHex) {
                        stringstream ss;
                        ss << hex << token;
                        ss >> temp;
                    }
                    if (isDec) {
                        temp = boost::lexical_cast<int>(token);
                    }
                    if (temp > 0xff) {
                        error = "data out of range (must be [0..255])";
                        throw boost::bad_lexical_cast();
                    }
                    data.push_back(temp);
                }
            }
            catch (const boost::bad_lexical_cast& ex) {
                string spaces(session->getPrompt().size(), ' ');
                for (size_t i = 0; i < ia; ++i) {
                    spaces.append(argv[i].size() + 1, ' ');
                }
                printf("%s^ %s\n", spaces.c_str(), error.empty() ? ex.what() : error.c_str());
                printf("Send failed.\n");
                return;
            }
        }

        for (size_t i = 0; i < data.size(); ++i) {
            printf("data[%d] = 0x%x (%c)\n", i, data[i], data[i]);
        }

        session->getSpi()->send(data);
    }
}
sendProcessor;
