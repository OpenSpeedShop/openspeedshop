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
 * Definition of the ArgStruct class.
 *
 */


#ifndef ARGCLASS_H
#define ARGCLASS_H
class ArgStruct
{
  public:
    ArgStruct(int ac, const char **av)
    {
      argc = ac;
      if (argc==0)
	argv=NULL;
      else
	{
	  // Martin: fixed use of calloc
	  //argv = (char **)calloc( argc, sizeof(char *) * argc );
	  argv = (char **)calloc( argc, sizeof(char *) );
	  for(int i=0;i<argc;i++)
	    {
	      argv[i] = strdup(av[i]);
	    }
	}
    }

    void addArg(const char *a)
    {
      char **old_argv = argv;

      // Martin: fixed use of calloc
      //argv = (char **)calloc( argc, sizeof(char *) * (argc+1) );
      argv = (char **)calloc( argc+1, sizeof(char *) );
      int i = 0;
      for(i=0;i<argc;i++)
      {
        argv[i] = strdup(old_argv[i]);
      }
      argv[i] = strdup(a);
      argc++;
      // Martin: fix: freeing old memory
      if (old_argv!=NULL) free(old_argv);
    }

    int argc;
    char **argv;
};
#endif // ARGCLASS_H
