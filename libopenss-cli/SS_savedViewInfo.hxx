/******************************************************************************
** Copyright (c) 2005 Silicon Graphics, Inc. All Rights Reserved.
** Copyright (c) 2007-2010 Krell Institute  All Rights Reserved.
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
 * Definition of the savedViewInfo class.
 *
 */


struct save_file_header
{
  int64_t type;  // Format type of header.
// Format type 1:
  int64_t data_offset; // Offset from start of file to data needed for view.
  int64_t command_offset; // Offset from start of file to command used to generate view data.
  int64_t eoc_offset; // Offset from start of file to End-Of-Column marker.
  int64_t eol_offset; // Offset from start of file to End-Of-Line marker.
  Time last_sample; // Time stamp on last record in database used to generate saved information.
// Add for Format type 2:
  uint64_t generation_time; // Time elapsed for generation of saved information.

 public:
  save_file_header ()
  {
    type = 0;
    data_offset = 0;
    command_offset = 0;
    eoc_offset = 0;
    eol_offset = 0;
    last_sample = 0;
    generation_time = 0;
  }
};


// savedViewInfo
// Track remembered Views that are associated with each experiment.
class savedViewInfo {
  private:
    std::string file_name; // Full character string name.
    int64_t file_id; // Unique ID appended to database name to create 'file_name'.
    bool new_file;  // Was this file generated during the current session?
    bool doNotSave; // An error occured or reuse is restricted to the current session.
    Time start;     // When generation of the view was initiated.
    Time end;       // When generation of the view was completed.
    save_file_header svh; // Header for saved information.
    std::string eoc_marker;  // End-Of-Column string.
    std::string eol_marker;  // End-Of-Line string.
    std::string original_cmd;  // Command saved in header.

   // Copy constructor is not available.
    savedViewInfo () {}

  public:
   // Constructor must have file name and status.
    savedViewInfo (std::string FileName, bool NewFile, int64_t FileId) {
      file_name = FileName;
      file_id = FileId;
      new_file = NewFile;
      doNotSave = false;
      start = 0;
      end = 0;
    }
    ~savedViewInfo () {
      if (new_file) {
       // Decide whether to save or remove files created during this session.
        if ( (start != 0) &&
             (end != 0) &&
             (!doNotSave) &&
             (OPENSS_SAVE_VIEWS_TIME >= 0) &&
             ( (OPENSS_SAVE_VIEWS_TIME == 0) ||
               (((end - start)/1000000000) >= OPENSS_SAVE_VIEWS_TIME) ) ) {
         // Leave file around.
        } else {
         // Delete file.
          (void) remove (file_name.c_str());
        }
      }
    }

    std::string FileName () { return file_name; }
    int64_t FileID () { return file_id; }
    bool NewFile () { return new_file; }
    Time StartTime () { return start; }
    Time EndTime () { return end; }
    uint64_t GenTime () { return svh.generation_time; }
    int64_t file_offset_to_data() { return svh.data_offset;  }
    std::string GenCmd () { return original_cmd; }
 
    void setDoNotSave() { doNotSave = true; }
    void setStartTime () {
     // Start measurement clock.
      if (new_file == true) start = Time::Now();
    }
    void setEndTime () {
     // Stop measurement clock and compute elapsed time.
      if (new_file == true) {
        end = Time::Now();
        if (start != 0) svh.generation_time = (end - start);
      }
    }
    void setHeader (save_file_header &svi, std::string eoc_str,
                    std::string eol_str, std::string cmd_str) {
      svh = svi;
      if (svh.type < 2) svh.generation_time = 0;
      eoc_marker = eoc_str;
      eol_marker = eol_str;
      original_cmd = cmd_str;
    }
    std::ostream *writeHeader () {
     // Check need to stop measurement clock.
      if ( (start != 0) &&
           (end == 0) ) {
        setEndTime();
      }
     // Open an output stream to save the redirected report.
      std::string base = FileName();
      std::ostream *tofile = new std::ofstream (base.c_str(), std::ios::out);
     // Save the header descriptor in binary form.
      char *cp = (char *)(&svh);
      for (int i = 0; i < sizeof(svh); i ++) { *tofile << cp[i]; }
     // Save associated command and column and end of line markers in character form.
        *tofile << ":" << original_cmd
                << ":" << eoc_marker
                << ":" << eol_marker
                << ":" << std::flush;
     // Leave file open to receive output of `expView` command.
      return tofile;
    }
    bool Header_Matches (std::string eoc_str,
                         std::string eol_str,
                         std::string cmd_str) {
      return ( (eoc_marker == eoc_str) &&
               (eol_marker == eol_str) &&
               (original_cmd == cmd_str) );
    }
//    std::string File_Name() { return file_name; }
};
