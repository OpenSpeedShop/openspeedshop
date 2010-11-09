/*******************************************************************************
** Copyright (c) 2005 Silicon Graphics, Inc. All Rights Reserved.
** Copyright (c) 2006-2010 Krell Institute  All Rights Reserved.
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

/** @file
 *
 * Definition of the ss_ostream_buf class.
 * Definition of the ss_ostream class.
 *
 */

//#define DEBUG_SYNC 1

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
  bool PromptNeeded;
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
    my_stream = new std::ostream(new ss_ostream_buf());

    my_old_buf = my_stream->rdbuf();
    my_stream->rdbuf(this);

    issuePrompt = false;
    PromptNeeded = false;
    my_string.clear();
    Assert(pthread_mutex_init(&stream_in_use, NULL) == 0); // dynamic initialization
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
    pthread_mutex_destroy(&stream_in_use);
  }

  std::ostream &mystream () {
    return *my_stream;
  }
  void Set_Issue_Prompt (bool prompt) {
    issuePrompt = prompt;
    PromptNeeded = prompt;
  }
  void Issue_Prompt () {
    if (PromptNeeded) {
      *my_stream << Current_OpenSpeedShop_Prompt;
      PromptNeeded = false;
    }
  }

  void sputn (const char *p, int64_t n) {
    (void) xsputn( p, std::streamsize(n));
    if (issuePrompt && (n > 0)) PromptNeeded = true;
  }
  void sput (const char *p) {
    (void) xsputn( p, strlen(p));
    if (issuePrompt && (strlen(p) > 0)) PromptNeeded = true;
  }
  void flush () {
    if (!my_string.empty()) {
      output_string (my_string);
      my_string.clear();
    }
    flush_stream();
  }

  void acquireLock() { 
#ifdef DEBUG_SYNC
     printf("[TID=%ld], LOCK: acquireLock, before calling pthread_mutex_lock(&stream_in_use=%ld)\n", pthread_self(), stream_in_use);
#endif
     Assert(pthread_mutex_lock(&stream_in_use) == 0); 
#ifdef DEBUG_SYNC
     printf("[TID=%ld], LOCK: acquireLock, after calling pthread_mutex_lock(&stream_in_use=%ld)\n", pthread_self(), stream_in_use);
#endif
  }
  void releaseLock() { 
     flush (); 
#ifdef DEBUG_SYNC
     printf("[TID=%ld], UNLOCK: releaseLock, before calling pthread_mutex_unlock(&stream_in_use=%ld)\n", pthread_self(), stream_in_use);
#endif
     Assert(pthread_mutex_unlock(&stream_in_use) == 0); 
#ifdef DEBUG_SYNC
     printf("[TID=%ld], UNLOCK: releaseLock, after calling pthread_mutex_unlock(&stream_in_use=%ld)\n", pthread_self(), stream_in_use);
#endif
  }

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
    if (issuePrompt && (n > 0)) PromptNeeded = true;

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
