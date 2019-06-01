#include <istream>
#include <ext/stdio_filebuf.h>

#include <unistd.h>
#include <paths.h>

namespace ipstream_details
{
  template<typename CharT, typename Traits>
  __gnu_cxx::stdio_filebuf<CharT, Traits> open_process(const char *program)
  {
    int pdes[2];
    pid_t pid;
    char *args[] = { (char*)"sh", (char*)"-c", nullptr, nullptr };
    __gnu_cxx::stdio_filebuf<char> filebuf;

    if (pipe(pdes) < 0) {
      return {};
    }

    if ((pid = fork()) < 0) {
      close(pdes[0]);
      close(pdes[1]);
      return {};
    }
    else if (pid == 0) {
      close(pdes[0]);
      if (pdes[1] != STDOUT_FILENO) {
        dup2(pdes[1], STDOUT_FILENO);
        close(pdes[1]);
      }
      args[2] = (char *)program;
      execv(_PATH_BSHELL, args);
      _exit(127); // unistd version of exit
    }
    
    filebuf = __gnu_cxx::stdio_filebuf<char>(pdes[0], std::ios_base::in);
    close(pdes[1]);
    return filebuf;
  }
}

template<typename CharT, typename Traits = std::char_traits<CharT>>
struct basic_ipstream : std::basic_istream<CharT, Traits>
{

  basic_ipstream() : Base(&filebuf), filebuf() {}
  
  basic_ipstream(const char *program) 
    : Base(&filebuf), filebuf(ipstream_details::open_process<CharT, Traits>(program))
  {
  }

  basic_ipstream(const std::string &program) 
    : Base(&filebuf), filebuf(ipstream_details::open_process<CharT, Traits>(program.c_str()))
  {
  }

  bool is_open() const
  {
    return filebuf.is_open();
  }

  void open(const char *program)
  {
    filebuf = ipstream_details::open_process<CharT, Traits>(program);
  }

  void open(const std::string &program)
  {
    filebuf = ipstream_details::open_process<CharT, Traits>(program.c_str());
  }

  __gnu_cxx::stdio_filebuf<CharT, Traits>* rdbuf()
  {
    return &filebuf;
  }

  __gnu_cxx::stdio_filebuf<CharT, Traits> const* rdbuf() const
  {
    return &filebuf;
  }

  void close()
  {
    filebuf.close();
  }

private:
  using Base = std::basic_istream<CharT, Traits>;
  __gnu_cxx::stdio_filebuf<CharT, Traits> filebuf;
};

using ipstream = basic_ipstream<char>;
using wipstream = basic_ipstream<wchar_t>;
