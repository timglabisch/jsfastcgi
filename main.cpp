#include <v8.h>

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>


#include <stdlib.h>
#ifdef _WIN32
#include <process.h>
#else+
#include <unistd.h>
extern char ** environ;
#endif
#include "fcgio.h"
#include "fcgi_config.h"  // HAVE_IOSTREAM_WITHASSIGN_STREAMBUF

#include "modul_point.h"


using namespace v8;
using namespace std;


// Maximum number of bytes allowed to be read from stdin
static const unsigned long STDIN_MAX = 1000000;

Isolate* isolate;

static void penv(const char * const * envp)
{
    cout << "<PRE>\n";
    for ( ; *envp; ++envp)
    {
        cout << *envp << "\n";
    }
    cout << "</PRE>\n";
}

static long gstdin(FCGX_Request * request, char ** content)
{
    char * clenstr = FCGX_GetParam("CONTENT_LENGTH", request->envp);
    unsigned long clen = STDIN_MAX;

    if (clenstr)
    {
        clen = strtol(clenstr, &clenstr, 10);
        if (*clenstr)
        {
            cerr << "can't parse \"CONTENT_LENGTH="
                 << FCGX_GetParam("CONTENT_LENGTH", request->envp)
                 << "\"\n";
            clen = STDIN_MAX;
        }

        // *always* put a cap on the amount of data that will be read
        if (clen > STDIN_MAX) clen = STDIN_MAX;

        *content = new char[clen];

        cin.read(*content, clen);
        clen = cin.gcount();
    }
    else
    {
        // *never* read stdin when CONTENT_LENGTH is missing or unparsable
        *content = 0;
        clen = 0;
    }

    // Chew up any remaining stdin - this shouldn't be necessary
    // but is because mod_fastcgi doesn't handle it correctly.

    // ignore() doesn't set the eof bit in some versions of glibc++
    // so use gcount() instead of eof()...
    do cin.ignore(1024); while (cin.gcount() == 1024);

    return clen;
}

v8::Handle<v8::Value> LogCallback(const v8::Arguments &args) {
    v8::String::AsciiValue ascii(args[0]);

    std::string s (*ascii);
    cout << s.c_str();
}

void runjs(char* script_filename) {
  // Get the default Isolate created at startup.
  isolate = Isolate::GetCurrent();

  // Create a stack-allocated handle scope.
  HandleScope handle_scope(isolate);

  Handle<ObjectTemplate> global = ObjectTemplate::New();
  global->Set(String::New("out"), FunctionTemplate::New(LogCallback));

  Handle<ObjectTemplate> somecls = ObjectTemplate::New();
  somecls->Set(String::New("somefunc"), FunctionTemplate::New(LogCallback));
  global->Set(String::New("somecls"), somecls);

  modul_point::addToV8Scope(&global, isolate);

  // Create a new context.
  Persistent<Context> context = Context::New(NULL, global);
  // Enter the created context for compiling and
  // running the hello world script. 
  Context::Scope context_scope(context);

  std::ifstream t(script_filename);
  std::stringstream codeBuffer;
  codeBuffer << t.rdbuf();

  // Create a string containing the JavaScript source code.
  Handle<String> source = String::New(codeBuffer.str().c_str());

  // Compile the source code.
  Handle<Script> script = Script::Compile(source);
  
  // Run the script to get the result.
  Handle<Value> result = script->Run();
  
  // Dispose the persistent context.
  context.Dispose(isolate);

  // Convert the result to an ASCII string and print it.
  String::AsciiValue ascii(result);

  //std::string s (*ascii);

  //return s.c_str();
}

int main (void)
{
    int count = 0;
    long pid = getpid();

    streambuf * cin_streambuf  = cin.rdbuf();
    streambuf * cout_streambuf = cout.rdbuf();
    streambuf * cerr_streambuf = cerr.rdbuf();

    FCGX_Request request;

    FCGX_Init();
    FCGX_InitRequest(&request, 0, 0);

    while (FCGX_Accept_r(&request) == 0)
    {
        fcgi_streambuf cin_fcgi_streambuf(request.in);
        fcgi_streambuf cout_fcgi_streambuf(request.out);
        fcgi_streambuf cerr_fcgi_streambuf(request.err);

#if HAVE_IOSTREAM_WITHASSIGN_STREAMBUF
        cin  = &cin_fcgi_streambuf;
        cout = &cout_fcgi_streambuf;
        cerr = &cerr_fcgi_streambuf;
#else
        cin.rdbuf(&cin_fcgi_streambuf);
        cout.rdbuf(&cout_fcgi_streambuf);
        cerr.rdbuf(&cerr_fcgi_streambuf);
#endif


        char * content;
        unsigned long clen = gstdin(&request, &content);

        cout << "Content-type: text/html\r\n"
                "\r\n";
                


        char* script_filename = FCGX_GetParam("SCRIPT_FILENAME", request.envp);
        
        runjs(script_filename);

        
        if (clen) cout.write(content, clen);

        if (content) delete []content;
    }

#if HAVE_IOSTREAM_WITHASSIGN_STREAMBUF
    cin  = cin_streambuf;
    cout = cout_streambuf;
    cerr = cerr_streambuf;
#else
    cin.rdbuf(cin_streambuf);
    cout.rdbuf(cout_streambuf);
    cerr.rdbuf(cerr_streambuf);
#endif

    return 0;
}