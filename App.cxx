#include <stdio.h>
#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <iostream>
#include <string>
#include <boost/tokenizer.hpp>

#include "App.hxx"
#include "Session.hxx"

App::App()
{
    session = new Session();
}

App*
App::initialize(int argc, char* argv[])
{
    App* app = new App();
    return app;
}

void
App::usage(const char* command)
{
    fprintf(stderr, "Usage: %s\n", command);
}

void
App::terminate()
{
}


void
App::run()
{
    bool gotQuit = false;
    do {
        char* line = readline("spish> ");
        if (line == NULL) {
            break;
        }

        if (*line != '\0') {
            char* expansion;
            int result = history_expand(line, &expansion);

            if (result < 0 || result == 2) {
                printf("%s\n", expansion);
            }
            else {
                add_history(expansion);
                gotQuit = processInput(expansion);
            }
            free(expansion);
        }
        free(line);
        
    } while (!gotQuit);

}

bool
App::processInput(const std::string& line)
{
    std::vector<std::string> argv;

    typedef boost::tokenizer<boost::escaped_list_separator<char> > tokenizer;
    boost::escaped_list_separator<char> sep('\\', ' ', '"');
    tokenizer tokens(line, sep);
    for (tokenizer::iterator tok_iter = tokens.begin();
         tok_iter != tokens.end(); ++tok_iter)
    {
        argv.push_back(*tok_iter);
    }

    return session->processCommand(argv);
}
