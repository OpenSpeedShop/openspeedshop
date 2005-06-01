/*******************************************************************************
** Copyright (c) 2005 Silicon Graphics, Inc. All Rights Reserved.
**
** This library is free software; you can redistribute it and/or modify it under
** the terms of the GNU Lesser General Public License as published by the Free
** Software Foundation; either version 2.1 of the License, or (at your option)
** any later version.
**
** This library is distributed in the hope that it will be useful, but WITHOUT
** ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
** FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more
** details.
**
** You should have received a copy of the GNU Lesser General Public License
** along with this library; if not, write to the Free Software Foundation, Inc.,
** 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*******************************************************************************/

#ifndef CMD_OUTPUT_H
#define CMD_OUTPUT_H

class ss_ostream_buf : public std::basic_streambuf<char>
{
 public:
  ss_ostream_buf () : std::basic_streambuf<char>() {}
};

class ss_ostream : public std::basic_streambuf<char>
{
 private:
  bool issuePrompt;
  std::ostream *my_stream;
  std::streambuf *my_old_buf;
  std::string my_string;

  pthread_mutex_t stream_in_use;

  virtual void output_string (std::string s) {
    fprintf(stderr,"%s",s.c_str());
  }
  virtual void flush_stream () {
    fflush(stderr);
  }

 public:
  ss_ostream () {
    // ss_ostream_buf *new_buf = new ss_ostream_buf();;
    my_stream = new std::ostream(new ss_ostream_buf());

    my_old_buf = my_stream->rdbuf();
    my_stream->rdbuf(this);

    issuePrompt = false;
    my_string.clear();
    releaseLock();
  }
  ss_ostream (std::ostream *stream) : my_stream(stream) {
    my_old_buf = stream->rdbuf();
    stream->rdbuf(this);
  }
  ~ss_ostream () {
   // output anything that is left
    if (!my_string.empty()) {
      (void) overflow (*"\n");
    }
    if (my_old_buf != NULL) {
      my_stream->rdbuf(my_old_buf);
    }
    delete my_stream;
  }

  std::ostream &mystream () { return *my_stream; }
  void Set_Issue_Prompt (bool prompt) { issuePrompt = prompt; }
  bool Issue_Prompt () { return issuePrompt; }

  void sputn (const char *p, int64_t n) {
    (void) xsputn( p, std::streamsize(n));
  }
  void sput (const char *p) {
    (void) xsputn( p, strlen(p));
  }
  void flush () {
    if (!my_string.empty()) {
      output_string (my_string);
      my_string.clear();
    }
    flush_stream();
  }

  void acquireLock() { Assert(pthread_mutex_lock(&stream_in_use) == 0); }
  void releaseLock() { flush (); Assert(pthread_mutex_unlock(&stream_in_use) == 0); }

 protected:
  virtual int_type overflow(int_type v) {
    if (v == '\n') {
      output_string (my_string);
      output_string ("\n");
      my_string.clear();
    } else {
      my_string.push_back(v);
    }
    return v;
  };
  virtual std::streamsize xsputn(const char *p, std::streamsize n) {
    my_string.append(p, p + n);

    int pos = 0;
    while (pos != std::string::npos) {
      pos = my_string.find('\n');
      if (pos != std::string::npos) {
        std::string tmp(my_string.begin(), my_string.begin() + pos + 1);
        output_string (tmp);
        my_string.erase(my_string.begin(), my_string.begin() + pos + 1);
      }
    }

    return n;
  };
};

#endif // CMD_OUTPUT_H
