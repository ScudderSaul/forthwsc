// forthwsc.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <io.h>
#include <fcntl.h>

#include <cmath>
#include <cfloat>
#include <ctime>
#include <cstdlib>
#include <cstdio>

#pragma warning(disable: 4786)

#include <vector>
#include <list>
#include <map>
#include <stack>
#include <algorithm>
#include <string>
#include <fstream>

#include "codeelement.h"

using namespace std;

int main( int argc, char *argv[ ], char *envp[ ] )
{
    Language lang;  // interpreter 
    string istd;
    char textp[200000l];  // big ugly file buffer
    string pstr;
    DWORD bre;
    long posa, posla, posb, ipos, poss, posbb;
    string ee;
    string astr;
    string bstr;
    string gstr;

    string fpth = " ";
    string hst;
    HANDLE hh;
    string ofilename;

    pstr.reserve(20000);
    gstr.reserve(10000);
    astr.reserve(5000);
    bstr.reserve(5000);

    hst = "Content-type: text/html\r\n\r\n";
    sprintf(textp, "%s", hst.c_str());

    cout << textp << endl;

    int eipi = 0;
//    pstr += "\n \" <font color = blue size=2><i> --- First test is code to define apache/IIS environment variables as Forth words. They are decompiled to show content.   --- \" printstring cr ";
    while(envp[eipi] != NULL && envp[eipi][0] != 0)
    {
      ee = envp[eipi];
      ipos = ee.find("=");
      if(ipos != ee.npos)
      {
            astr = "ENV_";
            astr.append(ee.substr(0, ipos));

            // hack till I can clean it
            if(strcmp("ENV_SERVER_SIGNATURE", astr.c_str()) == 0)
            {
                eipi++;
                continue;
            }

             
            if((ee.size() - ipos) > 0)
            {
                int msz = max(ee.size() - ipos, 253);
                bstr = ee.substr(ipos + 1, msz);    

 //               sprintf(textp, "\n \" %s \" string %s \n ' %s decompile ", bstr.c_str(), astr.c_str(), astr.c_str());
  //              pstr.append(textp);
//                cout << textp << endl;
            }
            else
            {
  //              sprintf(textp, "\n \" empty \"  string %s ", astr.c_str());
  //              pstr.append(textp);
//                cout << textp << endl;
            }

            if(strcmp("ENV_PATH_TRANSLATED", astr.c_str()) == 0)
            {
               fpth = bstr;
//               cout << "[" << fpth.c_str() << "]" << endl;

//               cout << "forth path = " << bstr.c_str() << endl;
            }
      }
     
      eipi++;
    }
 //   pstr += " \n  \" ----End of create/decompile of enviroment variables </i></font> \"  printstring cr  \n";

    if(fpth.size() < 3)  // if running from other than Apache I can use my own test file path without typing 
    {

        sprintf(textp, "C:\\\inetpub\\\wwwroot\\test4th.4hf");
        fpth = textp;
    }

    memset(textp, 0, 200000l);

//    cout << "after env" << endl;

    COORD ab;
    ab.X = 100;
    ab.Y = 5000;

  SetConsoleScreenBufferSize( GetStdHandle(STD_OUTPUT_HANDLE), ab);

    lang.SetStdOut();

//	ofilename = "console_put.txt";
//    lang.SetOut(ofilename.c_str());
//    hh = GetStdHandle(STD_INPUT_HANDLE);

hh = CreateFile(
  fpth.c_str(),                         // file name
  GENERIC_READ,                     // access mode
  FILE_SHARE_READ | FILE_SHARE_WRITE,                          // share mode
  NULL, // SD
  OPEN_EXISTING,                // how to create
  FILE_ATTRIBUTE_NORMAL,                 // file attributes
  NULL                        // handle to template file
);

  if(hh == INVALID_HANDLE_VALUE)
  {
     cout << "4th script not found" << endl;
     _exit(2);
  }

//  cout << "valid handle" << endl;

  ReadFile(
  hh,                // handle to file
  textp,             // data buffer
  200000,  // number of bytes to read
  &bre, // number of bytes read
  NULL    // overlapped buffer
);

    CloseHandle(hh);

    // done reading source file

     if(bre > 0)
     {
        textp[bre] = 0;
        istd.reserve(strlen(textp));
        istd = textp;
        posa = 0;
        posb = 0;
        poss = 0;
        posbb = 0;

       memset(textp, 0, 200000l);
       strcpy(textp, pstr.c_str()); 
       pstr.erase();

       // execute to show enviroment variables
       cout << " <pre> ";
//       cout << textp << endl;
       lang.Parse(textp, "STDIN");
       cout << " </pre> " << endl;


// find sections of forth code and execute
        while(true)
        {
            posa = istd.find("<?4TH", posb);
            posla = istd.find("<?4th", posb);

            if(posa == istd.npos && posla != istd.npos)
            {
               posa = posla;
            }

            if(posa != istd.npos && posla == istd.npos)
            {
               posla = posa;
            }            

            posa = min(posa, posla);

            if(posa != istd.npos)
            {
                posb = istd.find("?>", posa);
            }

            if(posa != istd.npos && posb != istd.npos)
            {
               if(poss != posa)
               {
                   gstr = istd.substr(poss, posa - poss);
                   gstr += " ";
                   cout << gstr.c_str() << endl;

                   poss = posb + 2;
               }

               posbb = posb;
               gstr = istd.substr(posa + 5, posb - posa - 5);
               gstr += " ";
               memset(textp, 0, 200000l);
               strcpy(textp, gstr.c_str()); 
               gstr.erase();
//               cout << textp << endl;

               lang.Parse(textp, "STDIN");
            }
            else
            {
              if(posa == istd.npos && posbb < poss)
              {
                    gstr = istd.substr(poss, istd.size() - poss);
                    gstr += " ";
                    cout << gstr.c_str();
              }

               pstr.erase();
               pstr.append(" bye ");
               memset(textp, 0, 200000l);
               strcpy(textp, pstr.c_str()); 
               pstr.erase();
//               cout << textp << endl;
               lang.Parse(textp, "STDIN");

               break;
            }
        }
     }
     else
     {
       _exit(3);
     }

  
	return 0;
}