#include "stdafx.h"

using namespace std;
#if defined(_MSC_VER)
// Sleep time for Windows is 1 ms while it's 1 ns for POSIX
// Beware using this for your app. This is just to give a
// basic idea on usage
#include <windows.h>
#else
#include <unistd.h>
#define Sleep(x) usleep(x)
#endif

#include "jx.h"

#define flush_console(...)        \
  do {                            \
    fprintf(stdout, __VA_ARGS__); \
    fflush(stdout);               \
        } while (0)

void ConvertResult(JXValue *result, std::string &to_result) {
  switch (result->type_) {
  case RT_Null:
    to_result = "null";
    break;
  case RT_Undefined:
    to_result = "undefined";
    break;
  case RT_Boolean:
    to_result = JX_GetBoolean(result) ? "true" : "false";
    break;
  case RT_Int32: {
    std::stringstream ss;
    ss << JX_GetInt32(result);
    to_result = ss.str();
  } break;
  case RT_Double: {
    std::stringstream ss;
    ss << JX_GetDouble(result);
    to_result = ss.str();
  } break;
  case RT_Buffer: {
    to_result = JX_GetString(result);
  } break;
  case RT_JSON:
  case RT_String: {
    to_result = JX_GetString(result);
  } break;
  case RT_Error: {
    to_result = JX_GetString(result);
  } break;
  default:
    to_result = "null";
    return;
  }
}

void callback(JXResult *results, int argc) {
  // do nothing
}

void sampleMethod(JXResult *results, int argc) {
  flush_console("sampleMethod Called;\n");

  std::stringstream ss_result;
  for (int i = 0; i < argc; i++) {
    std::string str_result;
    ConvertResult(&results[i], str_result);
    ss_result << i << " : ";
    ss_result << str_result << "\n";
  }

  flush_console("%s", ss_result.str().c_str());

  // return an Array back to JS Land
  const char *str = "[1, 2, 3]";

  // results[argc] corresponds to return value
  JX_SetJSON(&results[argc], str, strlen(str));
}

int main(int argc, char **args) {
  char *path = args[0];
  // Call JX_Initialize only once per app

  JX_Initialize(args[0], callback);
  // Creates a new engine for the current thread
  // It's our first engine instance hence it will be the
  // parent engine for all the other engine instances.
  // If you need to destroy this engine instance, you should
  // destroy everything else first. For the sake of this sample
  // we have our first instance sitting on the main thread
  // and it will be destroyed when the app exists.
  JX_InitializeNewEngine();

  char *contents = "console.log('hello world');";

  // define the entry file contents
  JX_DefineMainFile(contents);

  // define native -named- method
  // we will be reaching to this method from the javascript side like this;
  // process.natives.sampleMethod( ... )
  JX_DefineExtension("sampleMethod", sampleMethod);

  JX_StartEngine();

  // loop for possible IO
  // or JX_Loop() without usleep / while
  while (JX_LoopOnce() != 0) Sleep(1);

  JXValue result;
  JX_Evaluate(
    "var arr = process.natives.sampleMethod('String Parameter', {foo:1}); \n"
    "console.log('result: ', arr, 'length:', arr.length ); \n"
    "setTimeout(function() { \n"
    "  console.log('end!'); \n"
    "}, 100);",
    "myscript", &result);

  JX_Free(&result);
  // loop for possible IO
  // or JX_Loop() without usleep / while
  while (JX_LoopOnce() != 0) Sleep(1);

  JX_StopEngine();
}