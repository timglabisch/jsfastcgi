#include <v8.h>

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

using namespace v8;

int main(int argc, char* argv[]) {
  // Get the default Isolate created at startup.
  Isolate* isolate = Isolate::GetCurrent();

  // Create a stack-allocated handle scope.
  HandleScope handle_scope(isolate);

  // Create a new context.
  Persistent<Context> context = Context::New();
  
  // Enter the created context for compiling and
  // running the hello world script. 
  Context::Scope context_scope(context);

  std::ifstream t("code.js");
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
  printf("%s\n", *ascii);
  return 0;
}