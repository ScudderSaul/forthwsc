#ifndef __CODEELEMENT_H__
#define __CODEELEMENT_H__
//+---------------------------------------------------------------------------------------+
//This file codeelement.h is copyright 1999-2008 by Saul Scudder
//It is distributed for non-profit use as freeware and no warranty is given with the use of
//this source for any purpose whatsoever.
//This copyright notice is to remain unmodified in the file
//_Zen_Soft c/o 
//Saul Scudder
//358 E Whitton Ave.
//Phoenix AZ 85012
//+---------------------------------------------------------------------------------------+

#include <io.h>
#include <cmath>
#include <cfloat>
#include <ctime>
#include <cstdlib>
#include <cstdio>
#include <cctype>
#include <cstddef> 

// #define USING_GCC 
#define USING_WINDOWS

#ifdef USING_WINDOWS
#include <windows.h>
#endif

#pragma warning(disable: 4786)

#include <vector>
#include <list>
#include <map>
#include <stack>
#include <deque>
#include <algorithm>
#include <string>
#include <fstream>
#include <iostream>
#include <strstream>
#include <sstream>


using namespace std;

class InterpreterData;      // interpreter instance data
class Word;                 // the defined words in the language
class Parser;               // a parser with a stack for includes
class Primitives;           // primitive fuction definitions
class Language;             // the top level class
class Element;              // parts of words

class ClassMethod;          // methods in classes
class ClassData;            // data in class objects
class ClassDescription;     // class description data for forth classes
class ClassDescriptions;    // all class descriptions 

#define ERRLISTMAX 4  // save last 5 errors

// errordata is the error text and an int ID

class ErrorData
{
  string text;
  int number;
public:

    ErrorData()
    {
      text.reserve(120);
      text = "None";
      number = 0;
    }

    ErrorData(const ErrorData & ed)
    {
      text.reserve(120);  
      text = ed.text;
      number = ed.number;
    }

   ErrorData(int num, string& ee)
   {
     text.reserve(120);     
     text = ee;
     number = num;
   }

   ErrorData & operator = (const ErrorData & ed)
    {
      text.reserve(120);  
      text = ed.text;
      number = ed.number;
      return(*this);
    }

   string& Text()
   {
     return(text);
   }

   int Number()
   {
     return(number);
   }

   bool NumberEqual(int nn)
   {
      return((nn == number));
   }
};

// Errors is an array of error data
// it can lookup errors and add them to the output stream and the tracing stream
// currently any language error stops interpretation 

class Errors // the list of usage errors
{
  vector< ErrorData > errors;
  ErrorData Euse;
  string ErrPrompt;
  string LastError[5];
  int ErrIndex;
  bool fatal;

public:
  Errors();

  Errors(const Errors& er)
  {
    for(int qq = 0;qq < 5;qq++)
    {
      LastError[qq].reserve(120);
    }

   ErrPrompt.reserve(120);
   errors = er.errors;
   Euse = er.Euse;
   fatal = er.fatal;
  }

  ~Errors();

 Errors& operator = (const Errors& er)
  {
   for(int qq = 0;qq < 5;qq++)
   {
      LastError[qq].reserve(120);
   }

   ErrPrompt.reserve(120);
   errors = er.errors;
   Euse = er.Euse;
   fatal = er.fatal;
   return(*this);
  }

  bool FindError(int index)
  {
    for(int ii = 0;ii < errors.size();ii++)
    {
       if(errors[ii].NumberEqual(index))
       {
          Euse = errors[ii];
          return(true);
       }
    }
    return(false);
  }

  string & Error(int index)
  {
    if(index < errors.size())
    {
      if(FindError(index))
      {
        return(Euse.Text());
      }
    }
    Euse = errors[errors.size() - 1];
    return(Euse.Text());
  }

  void ErrNumberError(long err, InterpreterData & intpr);
 

  bool Fatal()
  {
    return(fatal);
  }

  void FatalError(void)
  {
    fatal = true;
  }

  void ResetFatal(void)
  {
    fatal = false;
  }

  void AddError(int ii, string & ee)
  {
    ErrorData aerr(ii, ee);
    errors.push_back(aerr);
  }

  void AddErr(int ii, char * cc)
  {
    string cs;
    cs.reserve(120);
    cs = cc;
    AddError(ii, cs);
  }

  void ShowError(int ii, InterpreterData & intpr, string& extra);

  enum ErrorIDS
  {
    NO_TRAILING_QUOTE,
    SYNTAX_ERROR,
    NO_SUCH_WORD,
    IS_NUMBER,
    WORD_EXISTS,
    BUFFER_END,
    STACK_UNDERFLOW,
    AUX_STACK_UNDERFLOW,
    ONLY_COMPILED,
    NO_BEGIN,
    NO_DO,
    PRIMITIVE_EXISTS,
    RETURN_STACK_UNDERFLOW,
    ILLEGALDEFINING,
    DIVIDEBYZERO,
    NO_CLOSINGPAREN,
    NO_EOL,
    BAD_STACKPOSITION,
    BAD_POINTER,
    NO_CREATE,
    STL_ERROR,
    NO_CLASS_EXISTS,
    CLASS_EXISTS,
    NO_MEMBER_FOUND,
    METHOD_EXISTED,
    COMPILING_METHOD,
    NOT_COMPILING_METHOD,
    MEMBER_EXISTS,
    PRIVATE_METHOD,
    PRIVATE_MEMBER,
    AMBIGUIOUS_MEMBER,
    SYSTEM_EXECPTION,
    LOOP_INDEX_INVALID,
    UNKNOWNERROR                // always last
  };

};

class Vocabulary
{
   map<string, int, less<string> > vocabulary;
   string name;

public:

   Vocabulary(string & nam)
   {
      name.reserve(50);
      name = nam;
   }

   Vocabulary(const Vocabulary & voc) 
   {
      name.reserve(50);
      vocabulary = voc.vocabulary;
      name = voc.name;
   }

   
  Vocabulary & operator = (const Vocabulary & voc) 
   {
      name.reserve(50);
      vocabulary = voc.vocabulary;
      name = voc.name;
      return(*this);
   }

  bool operator < (const Vocabulary & voc) 
   {
      if(name < voc.name)
      {
          return(true);
      }
      return(false);
   }

  bool operator == (const Vocabulary & voc) 
   {
      if(name == voc.name)
      {
          return(true);
      }
      return(false);
   }

  void AddWord(string& ss, int ii)
  {
     vocabulary[ss] = ii;
  }

  int Find(string& ss)
  {
      map<string, int, less<string> > ::iterator jj;

      jj = vocabulary.find(ss);

      if(jj != vocabulary.end())
      {
        return((*jj).second);
      }
    return(-1);
  }

  int Find(char* ss)
  {
      string gg;
      gg.reserve(50);
      gg = ss;
      return(Find(gg));
  }

  string & Name()
  {
    return(name);
  }

  int Erase(string& ss)
  {
      map<string, int, less<string> > ::iterator jj;

      jj = vocabulary.find(ss);

      if(jj != vocabulary.end())
      {
        vocabulary.erase(jj);
        return(0);
      }
    return(-1);
  }


};

// The dictonary class is an array all defined word objects
class Dictonary
{
  vector<Word> words;        // stores actual use instances of word class
  vector<Vocabulary> vocs;   // stores vocabulary instances
  list<int> vocorder;        // search order
  int context;
  int current;
  int forthindex;
  int mark;


public:
  Dictonary()
  {
    words.reserve(1000);
    vocs.reserve(25);
    string vn;
    vn.reserve(120);
    vn = "forth";
    Vocabulary * vv = new Vocabulary(vn);
    vocs.push_back((*vv));
    vocorder.push_front(FindVocIndex(vn));
    delete vv;
    forthindex = FindVocIndex(vn);
    context = current = forthindex;   
  }

  Dictonary(const Dictonary& dict)
  {
     words.reserve(1000);
     vocs.reserve(25);

     words = dict.words;
     vocs = dict.vocs;
     vocorder = dict.vocorder;
     context = dict.context;
     current = dict.current;
  }

  ~Dictonary()
  {
    
  }

  Dictonary& operator = (const Dictonary& dict)
  {
     words.reserve(1000);
     vocs.reserve(25);

     words = dict.words;
     vocs = dict.vocs;
     vocorder = dict.vocorder;
     context = dict.context;
     current = dict.current;
     return(*this);
  }

 int FindVocIndex(string & ss)
 {
   int ii;
   for(ii = 0; ii < vocs.size();ii++)
   {
      if(ss == vocs[ii].Name())
      {
        return(ii);
      }
   }
   return(-1);
 }

 int FindVocIndex(char * ss)
 {
   string sv;
   sv.reserve(120);
   sv = ss;
   return(FindVocIndex(sv));
 }

 bool MakeContext(string & st)
 {
    int rr = FindVocIndex(st);
    if(rr >= 0)
    {
        list<int> ::iterator ii;
        ii = find(vocorder.begin(), vocorder.end(), rr);
        if(ii != vocorder.end())
        {           
           vocorder.splice(vocorder.begin(), vocorder, ii);
           context = rr;
           return(true);
           
        }
       else
       {
           vocorder.push_front(rr);
           context = rr;
           return(true);
       }
    }
   return(false);
 }

 bool MakeCurrent(string & st)
 {
    int ii;
    if( (ii = FindVocIndex(st)) != -1)
    {
      current = ii;
      return(true);
    }
    else
    {
      return(false);
    }
 }


// Function name	: ContextToCurrent
// Description	    : 
// Return type		: void 
// Argument         : void
 void ContextToCurrent(void)
 {
   current = context;
 }


// Function name	: RemoveVocSearch
// Description	    : 
// Return type		: bool 
// Argument         : string & st
 bool RemoveVocSearch(string & st)
 {
    int ii;

    if( (ii = FindVocIndex(st)) != -1)
    {    
      if(ii != forthindex)       // can't remove forth from search completly
      {
        vocorder.remove(ii);
        if(context == ii)
        {
          context = vocorder.front();
          return(true);
        }
      }
    }
    return(false);
 }

 bool AddVoc(string & st)
 {
    int ii;

    if( (ii = FindVocIndex(st)) == -1)
    {   
      Vocabulary * vv = new Vocabulary(st);
      vocs.push_back((*vv));
      delete vv;
      return(true);
    }
    return(false);
 }

 void SetMark()
 {
    mark = words.size();
 }

 int Forget(string & wname);
 int Exists(string& nam);
 int Find(string& nam);
 Word* FindInContext(string& nam);
 Word* FindAddress(string& nam);
 Word* Address(int nn);
 Word* Address(string& nam);
 Word* Address(char* namw);
 Word* MostRecent(void);
 bool VocNameForWord(Word * ww , string& result);
 void Add(Word & wrd);
 void Execute(int wordindex, InterpreterData & idat);
};

// class RSData is a object placed on our return stack
// it contains a pointer to the word and the element index that was executing
// when the word called some other word and was placed on the return stack

class RSData
{
  union
  {
    Word *wp;
    ClassMethod * mp;
  };
  bool wrd;
  long wind;

public:
  RSData(void)
  {
    wp = NULL;
    wind = 0;
  }

  RSData(const RSData& ret)
  {
    wp = ret.wp;
    wind = ret.wind;
  }

  RSData(Word * ww)
  {
    wp = ww;
    wind = 0;
  }

  RSData(ClassMethod * mm)
  {
    mp = mm;
    wind = 0;
  }

  RSData & operator = (const RSData& ret)
  {
    wp = ret.wp;
    wind = ret.wind;
    return(*this);
  }

  void IncWind()
  {
    wind++;
  }

  void SetIndex(int ind)
  {
    wind = ind;
  }

  int Index()
  {
    return(wind);
  }

  Word * Code()
  {
    return(wp);
  }

  void SetCode(ClassMethod * mm)
  {
   mp = mm;
  }

 void SetCode(Word * ww)
  {
   wp = ww;
  }
};

class STLVect
{
   vector<unsigned long> stlvect;

public:
   STLVect()
   {

   }

   STLVect (const STLVect & v)
   {
     stlvect = v.stlvect;
   }

   STLVect & operator = (const STLVect & v)
   {
     stlvect = v.stlvect;
     return(*this);
   }   

   long Size()
   {
     return(stlvect.size());
   }

   long Capacity()
   {
     return(stlvect.capacity());
   }

   void Clear()
   {
     stlvect.clear();
   }

   bool Empty()
   {
     return(stlvect.empty());
   }

 vector<unsigned long>::iterator Find(unsigned long qq)
   {
//     vector<unsigned long>::iterator aa;
     return((find(stlvect.begin(), stlvect.end(), qq)));
   }

 vector<unsigned long>::iterator Begin()
   {
     vector<unsigned long>::iterator aa;
     return((aa = stlvect.begin()));
   }

 vector<unsigned long>::iterator End()
   {
     vector<unsigned long>::iterator aa;
     return((aa = stlvect.end()));
   }

 vector<unsigned long>::reverse_iterator RBegin()
   {
     vector<unsigned long>::reverse_iterator aa;
     return((aa = stlvect.rbegin()));
   }

 vector<unsigned long>::reverse_iterator REnd()
   {
     vector<unsigned long>::reverse_iterator aa;
     return((aa = stlvect.rend()));
   }

 void Pop_Back()
 {
    stlvect.pop_back();
 }

 void Push_Back(unsigned long nn)
 {
    stlvect.push_back(nn);
 }

 unsigned long Back()
 {
    return(stlvect.back());
 }

 unsigned long Front()
 {
    return(stlvect.front());
 }
 
 vector<unsigned long>::iterator Insert(vector<unsigned long>::iterator ii, unsigned long d)
 {
    return(stlvect.insert(ii, d));
 }

};

class STLVectIterator
{
    vector<unsigned long>:: iterator ii;

public:
   STLVectIterator()
   {

   }

   STLVectIterator (const STLVectIterator & i)
   {
     ii = i.ii;
   }

   STLVectIterator & operator = (const STLVectIterator & i)
   {
     ii = i.ii;
     return(*this);
   } 

   STLVectIterator & operator = (vector<unsigned long>::iterator i)
   {
     ii = i;
     return(*this);
   }

   bool operator == (vector<unsigned long>::iterator i)
   {
       if(ii == i)
       {
         return(true);
       }
     return(false);
   }

   bool operator == (const STLVectIterator & i)
   {
       if(ii == i.ii)
       {
         return(true);
       }
     return(false);
   }

   vector<unsigned long>:: iterator& GetIt()
   {
     return(ii);
   }
};

class NamedSTLVect
{
   map< string, STLVect, less<string> > stlvects;

public:
   NamedSTLVect()
   {

   }

   NamedSTLVect (const NamedSTLVect & v)
   {
     stlvects = v.stlvects;
   }

   NamedSTLVect & operator = (const NamedSTLVect & v)
   {
     stlvects = v.stlvects;
     return(*this);
   }   

   long Size()
   {
     return(stlvects.size());
   }

   STLVect * Find(string s)
   {
     if(stlvects.find(s) != stlvects.end())
     {
       return(&stlvects[s]);
     }
     return(NULL);
   }

   bool Insert(string gg, STLVect * v)
   {
       pair<map<string, STLVect, less<string> >::iterator, bool> bb;
       bb = stlvects.insert(map<string, STLVect, less<string> >::value_type(gg, *v));
       return(bb.second);
   }
};

class STLMap
{
   map<unsigned long, unsigned long, less<unsigned long> > stlmap;

public:
   STLMap()
   {

   }

   STLMap (const STLMap & m)
   {
     stlmap = m.stlmap;
   }

   STLMap & operator = (const STLMap & m)
   {
     stlmap = m.stlmap;
     return(*this);
   }   

   long Size()
   {
     return(stlmap.size());
   }

   void Clear()
   {
     stlmap.clear();
   }

   bool Empty()
   {
     return(stlmap.empty());
   }

   map<unsigned long, unsigned long, less<unsigned long> >::iterator Find(unsigned long qq)
   {
     map<unsigned long, unsigned long, less<unsigned long> >::iterator aa;
     return((aa = stlmap.find(qq)));
   }

  map<unsigned long, unsigned long, less<unsigned long> >::iterator Begin()
   {
     map<unsigned long, unsigned long, less<unsigned long> >::iterator aa;
     return((aa = stlmap.begin()));
   }

 map<unsigned long, unsigned long, less<unsigned long> >::iterator End()
   {
     map<unsigned long, unsigned long, less<unsigned long> >::iterator aa;
     return((aa = stlmap.end()));
   }

 map<unsigned long, unsigned long, less<unsigned long> >::reverse_iterator RBegin()
   {
     map<unsigned long, unsigned long, less<unsigned long> >::reverse_iterator aa;
     return((aa = stlmap.rbegin()));
   }

 map<unsigned long, unsigned long, less<unsigned long> >::reverse_iterator REnd()
   {
     map<unsigned long, unsigned long, less<unsigned long> >::reverse_iterator aa;
     return((aa = stlmap.rend()));
   }

   bool Insert(unsigned long lk, unsigned long ld)
   {
       pair<map<unsigned long, unsigned long, less<unsigned long> >::iterator, bool> bb;
       bb = stlmap.insert(map<unsigned long, unsigned long, less<unsigned long> >::value_type(lk, ld));
       return(bb.second);
   }
};

class STLMapIterator
{
    map<unsigned long, unsigned long, less<unsigned long> >::iterator ii;

public:
   STLMapIterator()
   {

   }

   STLMapIterator (const STLMapIterator & i)
   {
     ii = i.ii;
   }

   STLMapIterator & operator = (const STLMapIterator & i)
   {
     ii = i.ii;
     return(*this);
   } 

   STLMapIterator & operator = (map<unsigned long, unsigned long, less<unsigned long> >::iterator& i)
   {
     ii = i;
     return(*this);
   }

   bool operator == (map<unsigned long, unsigned long, less<unsigned long> >::iterator& i)
   {
       if(ii == i)
       {
         return(true);
       }
     return(false);
   }

   map<unsigned long, unsigned long, less<unsigned long> >::iterator& GetIt()
   {
     return(ii);
   }
};



class NamedSTLMap
{
   map<string, STLMap, less<string> > stlmaps;

public:
   NamedSTLMap()
   {

   }

   NamedSTLMap (const NamedSTLMap & m)
   {
     stlmaps = m.stlmaps;
   }

   NamedSTLMap & operator = (const NamedSTLMap & m)
   {
     stlmaps = m.stlmaps;
     return(*this);
   }   

   long Size(void)
   {
     return(stlmaps.size());
   }

   STLMap * Find(string s)
   {
     if(stlmaps.find(s) != stlmaps.end())
     {
       return(&stlmaps[s]);
     }
     return(NULL);
   }

   bool Insert(string gg, STLMap * p)
   {
       pair<map<string, STLMap, less<string> >::iterator, bool> bb;
       bb = stlmaps.insert(map<string, STLMap, less<string> >::value_type(gg, *p));
       return(bb.second);
   }
};

// MemInstDat is an object used in the memory manager's array
// The memory manager for user allocated memory keeps track of memory addresss
// and types so they may be deleted with c++ syntax

class MemInstDat
{
  void * memadr;
  int memtype;
  long memsize;
  long destruct_hint;  //

public:

   MemInstDat()
   {
     memadr = NULL;
     memtype = 0;
     memsize = 0;
     destruct_hint = 0;
   }

   MemInstDat(const MemInstDat& md)
   {
     memadr = md.memadr;
     memtype = md.memtype;
     memsize = md.memsize;
     destruct_hint = md.destruct_hint;
   }

  MemInstDat(void *anadr, int type, long memsi, long dst = 0)
   {
     memadr = anadr;
     memtype = type;
     memsize = memsi;
     destruct_hint = dst;
   }

   enum MemType
   {
     m_Empty,
     m_CharArray,
     m_Short,
     m_Long,
     m_ShortArray,
     m_LongArray,
     m_string,
     m_Class,
     m_RSData,
     m_STLVectIterator,
     m_STLVect,
     m_STLMapIterator,
     m_STLMap,
     m_UserTypes = 100,
   };

   MemInstDat& operator = (const MemInstDat& md)
   {
     memadr = md.memadr;
     memtype = md.memtype;
     memsize = md.memsize;
     return(*this);
   }

   int Type(void)
   {
     return(memtype);
   }

   void * Addr(void)
   {
     return(memadr);
   }

   long Size(void)
   {
     return(memsize);
   }

   long Hint()
   {
     return(destruct_hint);
   }

   bool SameAdr(void * vv)
   {
     if(vv == memadr)
     {
       return(true);
     }
     return(false);
   }

};


// MemoryMung is the  memory manager for user allocated memory
// It keeps track of memory addresss and types so they may be deleted 
// with c++ syntax
class MemoryMung
{
  vector<MemInstDat> memdata;
  MemInstDat wrkmid;
  int restore_index;
 
public:
  MemoryMung()
  {
    memdata.reserve(400);
  }

 MemoryMung(MemoryMung & mm)
  {
    memdata = mm.memdata;
  }

 MemoryMung& operator = (const MemoryMung & mm)
  {
    memdata = mm.memdata;
    return(*this);
  }

  ~MemoryMung()
  {
    while(memdata.size() != 0)
    {
      MemInstDat aa = memdata[0];
      DeleteMem(aa.Addr(), aa.Type());
    }
  }

 int FindMem(void* vv)
 {
   for (int ii = 0;ii < memdata.size();ii++)
   {
     if(memdata[ii].SameAdr(vv))
     {
       return(ii);
     }
   }
   return(-1);
 }

 int InsertMem(void * vv, int ty, long sz, long hint = 0)
 {
    if(FindMem(vv) >= 0)
    {
//     error address already allocated and not de-alocated
        return(false);
    }
    else
    {
      MemInstDat* mid = new MemInstDat(vv, ty, sz, hint);
      if((memdata.capacity() - memdata.size()) < 20)
      {
        memdata.reserve((memdata.size() + 50));

      }
      memdata.push_back(*mid);
      delete mid;
      return(true);
    }
 }

 bool RemoveMem(void * vv, int ty)
 {
    int mm;
    if((mm = FindMem(vv)) != (-1))
    {
      wrkmid = memdata[mm];
      if(wrkmid.Type() == ty)
      {
         memdata.erase(memdata.begin() + mm);
      }
      return(true);
    }
    else
    {  
//      error memory not found 
        return(false);
    }
 }

 bool IsMem(void * vv, int ty)
 {
    int mm;
    if((mm = FindMem(vv)) != (-1))
    {
      wrkmid = memdata[mm];
      if(wrkmid.Type() == ty)
      {
         return(true);
      }
      return(false);
    }
    else
    {  
//     memory not found 
        return(false);
    }
 }

 long MemSize(void * vv)
 {
    int mm;
    if((mm = FindMem(vv)) != (-1))
    {
      wrkmid = memdata[mm];      
      return(wrkmid.Size());
    }
    else
    {  
//     memory not found 
        return(0l);
    }
 }

 int DeleteMem(void * vv, int ty)
 {
    bool rmv = RemoveMem(vv, ty);
    if(rmv == true)
    {
       char * ar;

       switch(wrkmid.Type())
       {
        case MemInstDat::m_Empty:
           break;
        case MemInstDat::m_CharArray:
           ar = ((char*) wrkmid.Addr()); 
           delete ar;
           break;
        case MemInstDat::m_Short:
           delete ((short *) wrkmid.Addr());
           break;
        case MemInstDat::m_Long:
           delete ((long *) wrkmid.Addr());
           break;
        case MemInstDat::m_ShortArray:
           delete [] ((short *) wrkmid.Addr());
           break;
        case MemInstDat::m_LongArray:
           delete [] ((long *) wrkmid.Addr());
           break;
        case MemInstDat::m_string:
           delete ((string *) wrkmid.Addr());
           break;
        case MemInstDat::m_Class:
           delete [] ((char *) wrkmid.Addr());
           break;
        case MemInstDat::m_RSData:
            delete ((RSData *) wrkmid.Addr()); 
            break;
        case MemInstDat::m_STLVect:
            delete ((STLVect *) wrkmid.Addr()); 
            break;
        case MemInstDat::m_STLVectIterator:
            delete ((STLVectIterator *) wrkmid.Addr()); 
            break;
        case MemInstDat::m_STLMap:
            delete ((STLMap *) wrkmid.Addr()); 
            break;
        case MemInstDat::m_STLMapIterator:
            delete ((STLMapIterator *) wrkmid.Addr()); 
            break;

        default:
            return(false);
           break;
       }
       return(true);
    }
    else
    {
      return(false);
    }
 }

 void * AllocateType(int ty, int sz, long hint = 0)
 {
    void * vv;
    string * st;

     switch(ty)
       {
         case MemInstDat::m_CharArray:
           vv = (void *) new char[sz];
           InsertMem(vv, ty, sz);
           break;
        case MemInstDat::m_Short:
           vv = (void *) new short;
           InsertMem(vv, ty, sizeof(short));
           break;
        case MemInstDat::m_Long:
           vv = (void *) new long;
           InsertMem(vv, ty, sizeof(long));
           break;
        case MemInstDat::m_ShortArray:
           vv = (void *) new short[sz];
           InsertMem(vv, ty, sz*sizeof(short));
           break;
        case MemInstDat::m_LongArray:
           vv = (void *) new long[sz];
           InsertMem(vv, ty, sz*sizeof(long));
           break;
       case MemInstDat::m_string:
           st = new string;
           st->reserve(sz);
           vv = (void *) st;
           InsertMem(vv, ty,sizeof(string));
           break;
         case MemInstDat::m_Class:
           vv = (void *) new char[sz];
           InsertMem(vv, ty, sz, hint);
           break;
        case MemInstDat::m_RSData:
           vv = (void *) new RSData;
           InsertMem(vv, ty, sizeof(RSData));
           break;
        case MemInstDat::m_STLVect:
           vv = (void *) new STLVect;
           InsertMem(vv, ty, sizeof(STLVect));
           break;
        case MemInstDat::m_STLVectIterator:
           vv = (void *) new STLVectIterator;
           InsertMem(vv, ty, sizeof(STLVectIterator));
           break;
        case MemInstDat::m_STLMap:
           vv = (void *) new STLMap;
           InsertMem(vv, ty, sizeof(STLMap));
           break;
        case MemInstDat::m_STLMapIterator:
           vv = (void *) new STLMapIterator;
           InsertMem(vv, ty, sizeof(STLMapIterator));
           break;
        default:
            vv = NULL;
           break;
       }
    return(vv);
 }

 int Typeof(void * p)
 {
    int mm;
    if((mm = FindMem(p)) != (-1))
    {
      wrkmid = memdata[mm];
      return((wrkmid.Type()));
    }
    else
    {  
//     memory not found 
        return(-1);
    }
 }

 unsigned long Hintof(void * p)
 {
    int mm;
    if((mm = FindMem(p)) != (-1))
    {
      wrkmid = memdata[mm];
      return((wrkmid.Hint()));
    }
    else
    {  
//     memory not found 
        return(-1);
    }
 }

};


// InterpreterData is the data needed by an interpreter thread
// it contains the dictonary, the memory manager, the data stack, the return stack
// the Errors array, the output stream, a text parser pointer, 
// and the word being compiled (if any) and the Primitives object 
// contaning all primitive calls

class InterpreterData
{
  vector < RSData  > rets;  // return stack
  vector < long  > dats;    // data stack
  vector < long  > aux;     // auxillary data stack
  vector < double > flt;    // floating point stack
  NamedSTLMap namedmaps;    // declared maps
  NamedSTLVect namedvects;  // declared vects
  unsigned long numericbase; // number base in use
  char nbuffer[120];
  char nobuffer[120];
  int nbufpos;
  bool nbufflg;
  unsigned long INloc;
  char wordbuf[1216];
  char * nwordbuf;
  
  RSData exvA;
  RSData exv0;             
  int retdepth;            // the depth of the return stack
  MemoryMung mmanager;     // the memory manager

  Dictonary * dictonary;   // the dictonary
  ClassDescriptions * cdescript; // class decriptions
  Parser * pparser;        // the parser
  Primitives *allprim;     // the primitives
  Errors Err;              // the errors class
  string Output;           // the output string
  string OutFile;          // the output file name
  ofstream outstream;      // the file output stream
//  ostrstream osstring;     // the string output stream
//  strstream osstring;
  stringstream osstring;
  string tempintrp;
  string DebugStr;
  string TraceFile;        // the tracing file name
  ofstream tracestream;    // the tracing stream
  stringstream tracestring; // when trace is to string
  bool tracing;            // true when traceing is on
  bool tracetostring;       // trace is to a buffer when true;
  bool usestdout;          // use the condole stdout for output
  bool usestringout;       // use the string output strbuffer for output
  bool usestringerror;     // use the string error strbuffer for error output
  Word * building;         // the word being compiled

  int compiling;           // true when compiling
  int immediate;
  int ufcnt; 

public:

  InterpreterData();
  InterpreterData(const InterpreterData& idat);
  ~InterpreterData();
  InterpreterData & operator = (const InterpreterData& idat);

  enum CompFlag
  {
    ExecuteCompile =   0x01,
    Hidden =           0x02,
    Private =          0x04,
    Protected =        0x08,
    Public =          0x010,
    ArrayType =       0x020,
    ClassType =       0x040
  };

// return stack methods

// Function name	: PushR
// Description	    : push the return stack
// RSData type		: void 
// Argument         : const RSData& aret
 void PushR(const RSData& aret)
 {
     try
     {
       rets.push_back(aret);
       retdepth = rets.size();
     }
     catch(...)
     {
        if(rets.capacity() - rets.size() < 5)
        {
           rets.reserve(rets.size() + 10);
           rets.push_back(aret);
           retdepth = rets.size();
        }
     }
 }


// Function name	: PopR
// Description	    : pop the return stack
// RSData type		: RSData 
// Argument         : void
 RSData PopR(void)
 {
   if(retdepth > 0)
   {
        exvA = rets.back();
        rets.pop_back();
        retdepth = rets.size();
   }
   else
   {
      ufcnt++;
      if( ufcnt <= 1)
      {
        string aa = "Fatal Error";
        Err.ShowError(Errors::RETURN_STACK_UNDERFLOW, *this, aa);
      }
      else
      {
        exit(0);
      }
   }
   
   return(exvA);
 }


// Function name	: SetRsv
// Description	    : re-set a return stack element
// RSData type		: void 
// Argument         : RSData& aret
// Argument         : int ind
 void SetRsv(RSData& aret, int ind)
 {
     int ep = (rets.size() - 1) - ind;
     if (ep >= 0 && ep < rets.size())
     {
       rets[ep] = aret;
     }
     else
     {
       string aa = "bad stack pos set";
       Err.ShowError(Errors::BAD_STACKPOSITION, *this, aa);
     }
 }


// Function name	: GetRsv
// Description	    : get a return stack element
// RSData type		: RSData 
// Argument         : int ind
 RSData GetRsv(int ind)
 {
    int ep = (rets.size() - 1) - ind;
    if (ep >= 0 && ep < rets.size())
     {
       return(rets[ep]);
     }
     else
     {
       string aa;
       aa.reserve(120);
       aa = "bad stack pos get";
       Err.ShowError(Errors::BAD_STACKPOSITION, *this, aa);
     }
     return(exvA);
 }


// Function name	: SetExv
// Description	    : 
// Return type		: void 
// Argument         : RSData& ev
void SetExv(RSData& ev)
{
  exv0 = ev;
}

// Function name	: GetExv
// Description	    : 
// Return type		: RSData& 
RSData& GetExv()
{
  return(exv0);
}

// Function name	: IncExv
// Description	    : increment the element index of the word executing 
// RSData type		: void 
 void IncExv()
 {
//   rets.back().IncWind();
//   exv0 = rets.back();
     exv0.IncWind();
 }


// Function name	: ExvWord
// Description	    : get a pointer to the word of the word executing
// RSData type		: Word* 
 Word* ExvWord()
 {
//   return((rets.back().Code()));
     return(exv0.Code());
 }

// Function name	: ExvIndex
// Description	    : get a element index of  the word of the word executing
// RSData type		: int 
 int ExvIndex()
 {
//   return((rets.back().Index()));
     return((exv0.Index()));
 }

// -----------------------------data stack words------------------------

// Function name	: PushD
// Description	    : push onto the data stack
// RSData type		: void 
// Argument         : long dd
 void PushD(long dd)
 {
   try
   {
      dats.push_back(dd);
   }
   catch(...)
   {
     if((dats.capacity() - dats.size()) < 100)
     {
        dats.reserve((dats.size() + 1000));
        dats.push_back(dd);
     }
   }
 }

// Function name	: PushD
// Description	    : push a pointer onto the data stack
// RSData type		: void 
// Argument         : void * dd
 void PushD(void * dd)
 {
   try
   {
      dats.push_back((unsigned long) dd);
   }
   catch(...)
   {
     if((dats.capacity() - dats.size()) < 100)
     {
        dats.reserve((dats.size() + 1000));
        dats.push_back((unsigned long) dd);
     }
   }
 }


// Function name	: PopD
// Description	    : pop an item from the data stack
// RSData type		: long 
// Argument         : void
 unsigned long PopD(void)
 {
   if(dats.empty())
   {
       Err.ShowError(Errors::STACK_UNDERFLOW, *this, NameRunning());
       return(0);
   }

   unsigned long  rv = dats.back();
   dats.pop_back();
   return(rv);
 }


// Function name	: GetDatv
// Description	    : 
// Return type		: long 
// Argument         : int ind
long GetDatv(int ind)
 {
    int ep = (dats.size() - 1) - ind;
    if (ep >= 0 && ep < dats.size())
     {
       return(dats[ep]);
     }
     else
     {
       string aa = "bad stack pos get";
       Err.ShowError(Errors::BAD_STACKPOSITION, *this, aa);
     }
    return(0);
 }


// Function name	: RollDatv
// Description	    : 
// Return type		: void 
// Argument         : int ind
void RollDatv(int ind)
{
    long ep = (dats.size() - 1) - ind;
    if (ep >= 0 && ep < dats.size())
     {
       long ra = dats[ep];
       vector<long>::iterator ferr = dats.begin() + ep;
       dats.erase(ferr);
//       dats.erase(&dats[ep]);
       dats.push_back(ra);
     }
     else
     {
       string aa = "bad stack pos get";
       Err.ShowError(Errors::BAD_STACKPOSITION, *this, aa);
     }
}




// Function name	: ShowDatv
// Description	    : 
// Return type		: void 
// Argument         : void
void ShowDatv(void)
{
    int ds = dats.size();
    int ep, smax, ii;
    
    if(ds > 0)
    {
        if(usestdout)
        {
            cout << "Stack: ";
        }
        if(usestringout)
        {
            osstring << "Stack: ";
        }
        else
        {
            outstream << "Stack: ";
        }
        smax = __min(dats.size(), 10);
        for(ii = 0;ii < smax;ii++)
        {
            ep = (dats.size() - 1) - ii;
            if(usestdout)
            {
                cout << dats[ep] << " ";
            }
            if(usestringout)
            {
                osstring  << dats[ep] << " ";
            }
            else
            {
                outstream  << dats[ep] << " ";
            }
        }
        
        LineOut();
    }
    else
    {
        if(usestdout)
        {
            cout << "empty stack" << endl;
        }
        if(usestringout)
        {
            osstring << "empty stack" << endl;
        }
        else
        {
            outstream << "empty stack" << endl;
        }
    }
}

void TraceDatv(void)
 {
    int ds = dats.size();
    int ep, smax, ii;

    if(ds > 0)
    {
       tracestream << " Stack: ";
       smax = __min(dats.size(), 10);
       for(ii = 0;ii < smax;ii++)
       {
         ep = (dats.size() - 1) - ii;
         tracestream << dats[ep] << " ";
       }
        tracestream << endl;
    }
    else
    {
       tracestream << "empty stack" << endl; 
    }
    if(tracetostring)
    {
       
        if(ds > 0)
        {
        tracestring << " Stack: ";
        smax = __min(dats.size(), 10);
        for(ii = 0;ii < smax;ii++)
        {
            ep = (dats.size() - 1) - ii;
            tracestring << dats[ep] << " ";
        }
            tracestring << endl;
        }
        else
        {
        tracestring << "empty stack" << endl; 
        }
    }
 }


void TraceAuxDatv(void)
 {
    int ds = aux.size();
    int ep, smax, ii;

    if(ds > 0)
    {
       tracestream << " aux: ";
       smax = __min(aux.size(), 10);
       for(ii = 0;ii < smax;ii++)
       {
         ep = (aux.size() - 1) - ii;
         tracestream << aux[ep] << " ";
       }
//       tracestream << endl;
    }
    else
    {
//      tracestream << " aux:empty " << endl; 
    }
    if(tracetostring)
    {
        if(ds > 0)
        {
        tracestring<< " aux: ";
        smax = __min(aux.size(), 10);
        for(ii = 0;ii < smax;ii++)
        {
            ep = (aux.size() - 1) - ii;
            tracestring << aux[ep] << " ";
        }
    //       tracestring << endl;
        }
        else
        {
    //      tracestring<< " aux:empty " << endl; 
        }
    }
 }


// --------------------end of data stack ---------------------------

 string& NameRunning(void);  // the name of the running word (bug)

// floating point stack words


// Function name	: PushFD
// Description	    : 
// Return type		: void 
// Argument         : double dd
 void PushFD(double dd)
 {
   try
   {
     flt.push_back(dd);
   }
   catch(...)
   {
     if((flt.capacity() - flt.size()) < 100)
     {
        flt.reserve((flt.size() + 1000));
        flt.push_back(dd);
     }      
   }
 }


// Function name	: PopFD
// Description	    : pop an item from the double float stack
// return type		: double 
// Argument         : void
 double PopFD(void)
 {
   if(flt.empty())
   {
       Err.ShowError(Errors::STACK_UNDERFLOW, *this, NameRunning());
       return(0);
   }

   double  rv = flt.back();
   flt.pop_back();
   return(rv);
 }


// Function name	: GetFDatv
// Description	    : 
// Return type		: double 
// Argument         : int ind
double GetFDatv(int ind)
 {
    int ep = (flt.size() - 1) - ind;
    if (ep >= 0 && ep < flt.size())
     {
       return(flt[ep]);
     }
     else
     {
       string aa;
       aa.reserve(120);
       aa = "bad stack pos get";
       Err.ShowError(Errors::BAD_STACKPOSITION, *this, aa);
     }
    return(0.0);
 }

// Function name	: RollFDatv
// Description	    : 
// Return type		: void 
// Argument         : int ind
void RollFDatv(int ind)
{
    long ep = (flt.size() - 1) - ind;
    if (ep >= 0 && ep < flt.size())
     {
       double ra = flt[ep];
       vector<double>::iterator ferr = flt.begin() + ep;
//       flt.erase(&flt[ep]);
       flt.erase(ferr);
       flt.push_back(ra);
     }
     else
     {
       string aa;
       aa.reserve(120);
       aa = "bad stack pos get";
       Err.ShowError(Errors::BAD_STACKPOSITION, *this, aa);
     }
}

// auxillary stack words

// Function name	: PushAD
// Description	    : push onto the auxilary data stack
// RSData type		: void 
// Argument         : long dd
void PushAD(long dd)
{
    try
    {
        aux.push_back(dd);
    }
    catch(...)
    {
        if((aux.capacity() - aux.size()) < 100)
        {
            aux.reserve((aux.size() + 1000));
            aux.push_back(dd);
        }
    }
}

// Function name	: PushAD
// Description	    : push a pointer onto the auxilaty data stack
// RSData type		: void 
// Argument         : void * dd
 void PushAD(void * dd)
 {
   try
   {
      aux.push_back((unsigned long) dd);
   }
   catch(...)
   {
     if((aux.capacity() - aux.size()) < 100)
     {
        aux.reserve((aux.size() + 1000));
        aux.push_back((unsigned long) dd);
     }
   }
 }


// Function name	: GetADatv
// Description	    : 
// Return type		: long 
// Argument         : int ind
long GetADatv(int ind)
 {
    int ep = (aux.size() - 1) - ind;
    if (ep >= 0 && ep < aux.size())
     {
       return(aux[ep]);
     }
     else
     {
       string aa;
       aa.reserve(120);
       aa = "bad stack pos get";
       Err.ShowError(Errors::BAD_STACKPOSITION, *this, aa);
     }
    return(0);
 }


// Function name	: RollADatv
// Description	    : 
// Return type		: void 
// Argument         : int ind
void RollADatv(int ind)
{
    long ep = (aux.size() - 1) - ind;
    if (ep >= 0 && ep < aux.size())
     {
       long ra = aux[ep];
       vector<long>::iterator ferr = aux.begin() + ep;
       aux.erase(ferr);
//       aux.erase(&aux[ep]);
       aux.push_back(ra);
     }
     else
     {
       string aa;
       aa.reserve(120);
       aa = "bad stack pos get";
       Err.ShowError(Errors::BAD_STACKPOSITION, *this, aa);
     }
}

// labeled aux functions are for compilation control

 void PushLabeledAD(unsigned long label, long dd)
 {
   aux.push_back((unsigned long) dd);
   aux.push_back((unsigned long) label);
 }

void PushLabeledAD(char* nam, long dd)
 {  
   aux.push_back((unsigned long) dd);     // push data
   Word *ww = (dictonary->Address(nam)); 
   if(ww == NULL)
   {
      aux.push_back((unsigned long) ww);                           
      Err.ShowError(Errors::NO_SUCH_WORD, *this, NameRunning());
   }
   else
   {
     aux.push_back((unsigned long) ww); // push label
   }
 }

// Function name	: PopAD
// Description	    : pop an item from the data stack
// RSData type		: long 
// Argument         : void
 long PopAD(void)
 {
   if(aux.empty())
   {
       Err.ShowError(Errors::AUX_STACK_UNDERFLOW, *this, NameRunning());
       return(0);
   }

   long  rv = aux.back();
   aux.pop_back();
   return(rv);
 }

long PopLabeledAD(void)
 {
   PopAD();
   return(PopAD());
 }

long PeekLabeledAD(void)
 {
   long aa = PopAD(); // get label
   long bb = PopAD(); // get index
   PushAD(bb);
   PushAD(aa);
   return(bb);
 }

long FindFirstLabeledAD(unsigned long label)
{
  long ls = aux.size();
  int ii;
  unsigned long lt;

  for (ii = (ls -1);ii > 0;ii--)
  {
    lt = aux[ii];
    if(lt == label)
    {
      return(aux[ii - 1]);
    }
  }
  return(-1);
}

long FindNthLabeledAD(unsigned long label, int nn)
{
  long ls = aux.size();
  int ii;
  int cnt = 0;
  unsigned long lt;

  for (ii = (ls -1);ii > 0;ii--)
  {
    lt = aux[ii];
    if(lt == label)
    {
      cnt++;
      if(cnt == nn)
      {
         return(aux[(ii - 1)]);
      }
    }
  }
  return(-1);
}

long ExtractNthLabeledAD(unsigned long label, int nn)
{
  long ls = aux.size();
  int ii;
  int cnt = 0;
  unsigned long lt;

  for (ii = (ls -1);ii > 0;ii--)
  {
    lt = aux[ii];
    if(lt == label)
    {
      cnt++;
      if(cnt == nn)
      {
         long rv = aux[(ii - 1)];
         vector<long>::iterator serr = aux.begin() + ii - 1;
         vector<long>::iterator eerr = aux.begin() + ii;
         aux.erase(serr, eerr);
//         aux.erase(&aux[ii]);
//         aux.erase(&aux[ii - 1]);
         return(rv);
      }
    }
  }
  return(-1);
}


long FindNthLabeledAD(unsigned long labelA, unsigned long labelB, int nn)
{
  long ls = aux.size();
  int ii;
  int cnt = 0;
  unsigned long kk;

  for (ii = (ls -1);ii > 0;ii--)
  {
    kk = aux[ii];
    if((kk == labelA) || (kk == labelB))
    {
      cnt++;
      if(cnt == nn)
      {
         return(aux[(ii - 1)]);
      }
    }
  }
  return(-1);
}

long ExtractNthLabeledAD(unsigned long labelA, unsigned long labelB, int nn)
{
  long ls = aux.size();
  int ii;
  int cnt = 0;
  unsigned long kk;

  for (ii = (ls -1);ii > 0;ii--)
  {
    kk = aux[ii];
    if((kk == labelA) || (kk == labelB))
    {
      cnt++;
      if(cnt == nn)
      {
         long rv = aux[(ii - 1)];
         vector<long>::iterator serr = aux.begin() + ii - 1;
         vector<long>::iterator eerr = aux.begin() + ii;
         aux.erase(serr, eerr);
//         aux.erase(&aux[ii]);
//         aux.erase(&aux[ii - 1]);
         return(rv);
      }
    }
  }
  return(-1);
}

long FindFirstLabeledAD(char * nam)
{
  Word* ww = dictonary->Address(nam);
  if(ww == NULL)
  {
    Err.ShowError(Errors::NO_SUCH_WORD, *this, NameRunning());
    return(-1);
  }
  return(FindFirstLabeledAD((unsigned long) ww));
}

long FindNthLabeledAD(char * nam, int nn)
{
  Word* ww = dictonary->Address(nam);
  if(ww == NULL)
  {
    Err.ShowError(Errors::NO_SUCH_WORD, *this, NameRunning());
    return(-1);
  }
  return(FindNthLabeledAD((unsigned long) ww, nn));
}

long FindNthLabeledAD(char * namA, char * namB, int nn)
{
  Word* wwa = dictonary->Address(namA);
  Word* wwb = dictonary->Address(namB);
  if(wwa == NULL || wwb == NULL)
  {
    Err.ShowError(Errors::NO_SUCH_WORD, *this, NameRunning());
    return(-1);
  }
  return(FindNthLabeledAD((unsigned long) wwa, (unsigned long) wwb, nn));
}

long ExtractNthLabeledAD(char * nam, int nn)
{
  Word* ww = dictonary->Address(nam);
  if(ww == NULL)
  {
    Err.ShowError(Errors::NO_SUCH_WORD, *this, NameRunning());
    return(-1);
  }
  return(ExtractNthLabeledAD((unsigned long) ww, nn));
}

long ExtractNthLabeledAD(char * namA, char * namB, int nn)
{
  Word* wwa = dictonary->Address(namA);
  Word* wwb = dictonary->Address(namB);
  if(wwa == NULL || wwb == NULL)
  {
    Err.ShowError(Errors::NO_SUCH_WORD, *this, NameRunning());
    return(-1);
  }
  return(ExtractNthLabeledAD((unsigned long) wwa, (unsigned long) wwb, nn));
}

// Function name	: SizeD
// Description	    : the depth of the data stack
// RSData type		: long 
 long SizeD()
 {
  return(dats.size());
 }

 unsigned long *Base(void)
 {
   return(&numericbase);
 }

// functions for <# hold # #s #> 

  char *Nbuffer()
  {
    return(nbuffer);
  }

  char *NObuffer()
  {
    return(nobuffer);
  }

  int* NbufferPos()
  {
    return(&nbufpos);
  }

  bool NbufInUse()
  {
     return(nbufflg);
  }

  void NbufferOn()
  {
    nbufflg = true;
  }

  void NbufferOff()
  {
    nbufflg = false;
  }

  char * WordBuf(void)
  {
      nwordbuf += 304;
      if(nwordbuf < (wordbuf + 1215))
      {
        nwordbuf = wordbuf;
      }
     return(nwordbuf);
  }

  void StartRunImmediate()
  {
    immediate = true;
  }

  void StopRunImmediate()
  {
    immediate = false;
  }

  int Immediate()
  {
    return(immediate);
  }

  unsigned long * pIN()
  {
    return(&INloc);
  }

// set the compileing flag
 void Compile()
 {
   compiling = true;
 }

 int* State(void)
 {
   return(&compiling);
 }

 int Compiling()
 {
   return(compiling);
 }

 int CompilingNI()
 {
   if(immediate)
   {
     return(false);
   }
   return(compiling);
 }

 void SetCompiling(bool nn)
 {
   compiling = nn;
 }

 void EndCompile()
 {
   compiling = false;
 }

// return a pointer to the dictonary
 Dictonary * IDict()
 {
   return(dictonary);
 }

// return a pointer to the text parser
 Parser * IParser()
 {
   return(pparser);
 }

 ClassDescriptions * ClassDSCR()
 {
   return(cdescript);
 }

int Parse(char * txt, bool once = false, char *nm = NULL);
void ParseContainedClass(Parser * par, ClassDescription * nc);
bool ParseClass(string &awrd, Parser * par);
bool ParseDigits(char *wrd, Parser * par);

// return a reference to the memory manager
 MemoryMung & MManager()
 {
   return(mmanager);
 }

 NamedSTLMap & NamedMaps()
 {
   return(namedmaps);
 }

 NamedSTLVect & NamedVects()
 {
   return(namedvects);
 }

// set the primitives pointer
 void SetUpPrimitives(Primitives *ap)
 {
   allprim  = ap;
 }

 // return a primitives to the primitives object
 Primitives * ToPrimitives()
 {
   return(allprim);
 }
 
// return the output buffer
 string & Out(void)
 {
   return(Output);
 }

// open the output stream
void OpenOutput(void)
 {
   if(outstream.is_open())
   {
      outstream.close();
   }
   outstream.open(OutFile.c_str());
 }

 void UseStdOut(void);
 void NoStdOut(void);
 
 bool IsStdOut(void)
 {
    return(usestdout);
 }

 void UseStringOut(void);
 void NoStringOut(void);
 
 bool IsStringOut(void)
 {
    return(usestringout);
 }

// a new output stream file is opened
void NewOutputFile(const char * opath)
{
  OutFile = opath;
  OpenOutput();
}

void NewOutputString(void)
{
    osstring.str("\n");  // , ios_base::in | ios_base::out
//    osstring.rdbuf( )->seekpos(0);
}

stringstream & GetOutputString()
{
   return(osstring);
}

// write the last output string to the stream
void WriteOut(void)
{
    if(usestdout)
    {
        cout << ((char *) Output.c_str());
    }
    if(usestringout)
    {
        osstring << Output.c_str();
    }
    else
    {
        outstream << Output.c_str();
    }
    Output.erase(Output.begin(), Output.end());
    Output.reserve(1000);
}

#ifdef USING_WINDOWS
void WindowsError()
{
    WriteOut();
    LineOut();
    LPVOID lpMsgBuf;
    FormatMessage( 
        FORMAT_MESSAGE_ALLOCATE_BUFFER | 
        FORMAT_MESSAGE_FROM_SYSTEM | 
        FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL,
        GetLastError(),
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
        (LPTSTR) &lpMsgBuf,
        0,
        NULL 
        );
    
    if(Output.capacity() < (strlen((char *)lpMsgBuf) + 100))
    {
       Output.reserve((Output.size() + strlen((char *)lpMsgBuf) + 100)); 
    }

    Output = (char *)lpMsgBuf;

    // Free the buffer.
    LocalFree( lpMsgBuf );
    WriteOut();
    LineOut();
}
#endif

// add cr
void LineOut()
{
    if(usestdout)
    {
        cout << endl;
    }
    if(usestringout)
    {
        osstring << "\n\r"; // endl;
    }
    else
    {
        outstream << endl;
    }
}


// open a trace stream
void OpenTrace(void)
{
    if(tracestream.is_open())
    {
        tracestream.close();
    }
    tracestream.open(TraceFile.c_str());
}

// close the tracing stream
void CloseTrace(void)
{
    tracestream.close();
}

// a new traceing file
void NewTraceFile(char * ts)
{
    TraceFile = ts;
    OpenTrace();
    if(tracetostring)
    {
      tracestring.str() = "\n";
    }
}

string & GetTraceFileName()
{
  return(TraceFile);
}

stringstream & GettraceStream()
{
  return(tracestring);
}

// output a trace string
void TraceOut(string& Traceput)
{
    tracestream << Traceput.c_str();
    if(retdepth > 0)
    {
        tracestream << " rdepth = " << retdepth << ", sdepth = " << SizeD();
    }
    if(dats.size() > 0)
    {
        if(aux.size() > 0)
        {
          TraceAuxDatv();
        }
        TraceDatv();
       //      tracestream << " TOS =" << dats.back() << endl;
    }
    else
    {
        tracestream << endl;
    }
    if(tracetostring)
    {
        tracestring << Traceput.c_str();
        if(retdepth > 0)
        {
            tracestring << " rdepth = " << retdepth << ", sdepth = " << SizeD();
        }
        if(dats.size() > 0)
        {
            if(aux.size() > 0)
            {
              TraceAuxDatv();
            }
            TraceDatv();
        //      tracestream << " TOS =" << dats.back() << endl;
        }
        else
        {
           tracestring << endl;
        }
    }
}

// output trace char text
void TraceOut(char * ts)
{
    tracestream << ts;
    if(retdepth > 0)
    {
        tracestream << " rdepth = " << retdepth << ", sdepth = " << SizeD();
    }
    if(dats.size() > 0)
    {
        TraceDatv();
        //      tracestream << " TOS =" << dats.back() << endl;
    }
    else
    {
        tracestream << endl;
    }   
   if(tracetostring)
    {
        tracestring << ts;
        if(retdepth > 0)
        {
            tracestring << " rdepth = " << retdepth << ", sdepth = " << SizeD();
        }
        if(dats.size() > 0)
        {
            TraceDatv();
            //      tracestream << " TOS =" << dats.back() << endl;
        }
        else
        {
            tracestring << endl;
        }   
    }
}

void DebugTrace(char * cc)
{
  if(Tracing())
  {

    tracestream << " << debug: ";
    if(tracetostring)
    {
       tracestring  << " << debug: ";
    }
    TraceOut(cc);

  }
}

void DebugTrace(string & cc)
{
  if(Tracing())
  {

    tracestream << " << debug: ";
    if(tracetostring)
    {
       tracestring  << " << debug: ";
    }
    TraceOut(cc);

  }
}

void TraceOn()
{
  tracing = true;
}

void TraceOff()
{
  tracing = false;
}

bool Tracing()
{
 return(tracing);
}

void TraceToStringOn()
{
   tracetostring = true;
}

void TraceToStringOff()
{
   tracetostring = false;
}

void DecompileCurrent(void);
void DecompileOne(void);
void DecompileN(int n);

// show error ii with extra debug info
 void ShowError(int ii, string& extra)
 {
   Err.ShowError(ii, *this, extra); 
 }

// show error ii with extra debug info
void ShowError(int ii)
 {
//   string tmp = NameRunning();
   string tmp = " <<< ";
   Err.ShowError(ii, *this, tmp); 
 }

 void ShowError(int ii, string& extra, unsigned long nn)
 {
   char errtxt[300];
   sprintf(errtxt," %s = %li" , extra.c_str(), nn);
   extra = errtxt;
   Err.ShowError(ii, *this, extra); 
 }

 void ShowError(int ii, string& extra, void * vv)
 {
   char errtxt[300];
   sprintf(errtxt," %s = %lx" , extra.c_str(), vv);
   extra = errtxt;
   Err.ShowError(ii, *this, extra); 
 }

 void ErrNumberError(long fer)
 {
   Err.ErrNumberError(fer, *this);
 }

// Function name	: TempString
// Description	    : Non-compiled strings need to go somewhere
// RSData type		: string& 
 string& TempString()
 {    
    return(tempintrp);
    
 }

// Function name	: TempString
// Description	    : empty the temporary string
// RSData type		: string& 
 void EmptyTempString()
 {    
   tempintrp.erase(tempintrp.begin(),tempintrp.end());
   tempintrp.reserve(300);
 }

 void DebugString(const char * ds)
 {
   DebugStr.erase(DebugStr.begin(),DebugStr.end());
   DebugStr.reserve(300);  
   DebugStr = ds;
 }

// a fatal error
  bool Fatal()
  {
    return(Err.Fatal());
  }

  void FatalError(void)
  {
    Err.FatalError();
  }

  void ResetFatal(void)
  {
    Err.ResetFatal();
  }

// return the address of the word being compiled
 Word * Building()
 {
   return(building);
 }

// set the word being compiled
 void SetBuilding(Word * bb)
 {
   building = bb;
 }

// compile helpers 
 void BuildWithName(string & name);
 void BuildAddElement(Element& el);
 void BuildAddElement(Word* pww);
 void BuildAddElement(string* pss);
 void BuildAddElement(long* pqq);
 void BuildAddElement(long qq);
 void BuildAddElement(ClassMethod* pm);
 void BuildAddElement(ClassData* pd);

 void BuildFinishWord(void);
 
 int BuildLastUsedElement(void);
 int BuildLastElementCalling(Word *pwrd);
 
};

class Primitives
{
  friend Language;
  friend Element;
  friend Word;
  InterpreterData * indat;
  
public:

 Primitives(InterpreterData * idat)
 {
   indat = idat;
 }

 Primitives()
 {
   
 } 

 __int64 Pop64(InterpreterData & idat);
 void Push64(unsigned __int64 vv, InterpreterData & idat);

 void L_plus(InterpreterData & idat);
 void L_minus(InterpreterData & idat);
 void L_colon(InterpreterData & idat);
 void L_semicolon(InterpreterData & idat);
 void L_quote(InterpreterData & idat);
 void L_dotquote(InterpreterData & idat);
 void L_Comment(InterpreterData & idat);
 void L_CppComment(InterpreterData & idat);
 void L_ForthComment(InterpreterData & idat);
 void L_dot(InterpreterData & idat);
 void L_udot(InterpreterData & idat);
 void L_ddot(InterpreterData & idat);
 void L_dudot(InterpreterData & idat);
 void L_emit(InterpreterData & idat);
 void L_CR(InterpreterData & idat);
 void L_printstring(InterpreterData & idat);

 void L_equal(InterpreterData & idat);
 void L_greaterthan(InterpreterData & idat);
 void L_dup(InterpreterData & idat);
 void L_questiondup(InterpreterData & idat);
 void L_swap(InterpreterData & idat);

 // control words
 void L_do(InterpreterData & idat);
 void L_rundo(InterpreterData & idat);
 void L_runloop(InterpreterData & idat);
 void L_loop(InterpreterData & idat);
 void L_runiinloop(InterpreterData & idat);
 void L_iinloop(InterpreterData & idat);
 void L_runjinloop(InterpreterData & idat);
 void L_jinloop(InterpreterData & idat);
 void L_runplusloop(InterpreterData & idat);
 void L_plusloop(InterpreterData & idat);
 void L_runquestiondo(InterpreterData & idat);
 void L_questiondo(InterpreterData & idat);
 void L_runbegin(InterpreterData & idat);
 void L_begin(InterpreterData & idat);
 void L_rununtil(InterpreterData & idat);
 void L_until(InterpreterData & idat);
 void L_runleave(InterpreterData & idat);
 void L_leave(InterpreterData & idat);
 void L_rununloop(InterpreterData & idat);
 void L_unloop(InterpreterData & idat);
 void L_runif(InterpreterData & idat);
 void L_if(InterpreterData & idat);
 void L_runelse(InterpreterData & idat);
 void L_else(InterpreterData & idat);
 void L_runthen(InterpreterData & idat);
 void L_then(InterpreterData & idat);
 void L_runwhile(InterpreterData & idat);
 void L_while(InterpreterData & idat);
 void L_runrepeat(InterpreterData & idat);
 void L_repeat(InterpreterData & idat);
 void L_runagain(InterpreterData & idat);
 void L_again(InterpreterData & idat);
 void L_runcase(InterpreterData & idat);
 void L_case(InterpreterData & idat);
 void L_runof(InterpreterData & idat);
 void L_of(InterpreterData & idat);
 void L_runendof(InterpreterData & idat);
 void L_endof(InterpreterData & idat);
 void L_runendcase(InterpreterData & idat);
 void L_endcase(InterpreterData & idat);

// end control words
 void L_depth(InterpreterData & idat);
 void L_drop(InterpreterData & idat);
 void L_over(InterpreterData & idat);
 void L_slashmod(InterpreterData & idat);
 void L_mod(InterpreterData & idat);

// void L_roll(InterpreterData & idat);
 void L_rot(InterpreterData & idat);
 void L_multiply(InterpreterData & idat);
 void L_starslash(InterpreterData & idat);
 void L_starslashmod(InterpreterData & idat);
 void L_divide(InterpreterData & idat);
 void L_constant(InterpreterData & idat);
 void L_variable(InterpreterData & idat);
 void L_store(InterpreterData & idat);
 void L_fetch(InterpreterData & idat);
 void L_oneplus(InterpreterData & idat);
 void L_oneminus(InterpreterData & idat);
 void L_twotimes(InterpreterData & idat);
 void L_twoplus(InterpreterData & idat);
 void L_twominus(InterpreterData & idat);
 void L_twoslash(InterpreterData & idat);
 void L_abs(InterpreterData & idat);
 void L_plusstore(InterpreterData & idat);
 void L_and(InterpreterData & idat);
 void L_not(InterpreterData & idat);
 void L_or(InterpreterData & idat);
 void L_xor(InterpreterData & idat);
 void L_rshift(InterpreterData & idat);
 void L_lshift(InterpreterData & idat);
 void L_max(InterpreterData & idat);
 void L_min(InterpreterData & idat);
 void L_zeroless(InterpreterData & idat);
 void L_zeronot(InterpreterData & idat);
 void L_zeroequal(InterpreterData & idat);
 void L_zerogreater(InterpreterData & idat);
 void L_lessthan(InterpreterData & idat);
 void L_ulessthan(InterpreterData & idat);
 void L_notequal(InterpreterData & idat);
 void L_greaterthanequal(InterpreterData & idat);
 void L_lessthanequal(InterpreterData & idat);
 void L_negate(InterpreterData & idat);
 void L_stringcopy(InterpreterData & idat);
 void L_stringplus(InterpreterData & idat);
 void L_stringequal(InterpreterData & idat);
 void L_substring(InterpreterData & idat);
 void L_stringvariable(InterpreterData & idat);
 void L_stringtocstr(InterpreterData & idat);
 void L_cstrtostring(InterpreterData & idat);
 void L_chararray(InterpreterData & idat);
 void L_chararraylen(InterpreterData & idat);
 void L_deletechararray(InterpreterData & idat);
 void L_cstore(InterpreterData & idat);
 void L_cfetch(InterpreterData & idat);
 void L_wstore(InterpreterData & idat);
 void L_wfetch(InterpreterData & idat);
 void L_count(InterpreterData & idat);
 void L_type(InterpreterData & idat);
 void L_traceon(InterpreterData & idat);
 void L_traceoff(InterpreterData & idat);
 void L_addtrace(InterpreterData & idat);
 void L_stackdepth(InterpreterData & idat);
 void L_strcpy(InterpreterData & idat);
 void L_strncpy(InterpreterData & idat);
 void L_memcpy(InterpreterData & idat);
 void L_memset(InterpreterData & idat);
 void L_hex(InterpreterData & idat);
 void L_octal(InterpreterData & idat);
 void L_decimal(InterpreterData & idat);
 void L_base(InterpreterData & idat);
 void L_sTod(InterpreterData & idat);
 void L_mtimes(InterpreterData & idat);
 void L_smslashrem(InterpreterData & idat);
 void L_fmslashmod(InterpreterData & idat);
 void L_umtimes(InterpreterData & idat);
 void L_umslashmod(InterpreterData & idat);
 void L_dplus(InterpreterData & idat);
 void L_greaternumber(InterpreterData & idat);

 void L_dnegate(InterpreterData & idat);
 void L_dzeroless(InterpreterData & idat);
 void L_dminus(InterpreterData & idat);
 void L_dzeroequal(InterpreterData & idat);
 void L_dtwostar(InterpreterData & idat);
 void L_dtwoslash(InterpreterData & idat);
 void L_dmax(InterpreterData & idat);
 void L_dmin(InterpreterData & idat);

 void L_structure(InterpreterData & idat);
 void L_createinstance(InterpreterData & idat);
 void L_Nchar(InterpreterData & idat);
 void L_Nshort(InterpreterData & idat);
 void L_Nlong(InterpreterData & idat);
 void L_Narraylong(InterpreterData & idat);
 void L_Narrayshort(InterpreterData & idat);
 void L_Narrayobjects(InterpreterData & idat);
 void L_endstructure(InterpreterData & idat);

 void L_ReadEntireFile(InterpreterData & idat);
 void L_include(InterpreterData & idat);
 void L_included(InterpreterData & idat);
 void L_CreateFile(InterpreterData & idat);
 void L_OpenFile(InterpreterData & idat);
 void L_FileSize(InterpreterData & idat);
 void L_FilePosition(InterpreterData & idat);
 void L_ReadFile(InterpreterData & idat);
 void L_CloseFile(InterpreterData & idat);
 void L_ReadOnly(InterpreterData & idat);
 void L_ReadWrite(InterpreterData & idat);
 void L_RepositionFile(InterpreterData & idat);
 void L_WriteOnly(InterpreterData & idat);
 void L_WriteFile(InterpreterData & idat);
 void L_ShowFileError(InterpreterData & idat);

 void L_greaterin(InterpreterData & idat);
 void L_decompile(InterpreterData & idat);
 void L_execute(InterpreterData & idat);
 void L_tic(InterpreterData & idat);
 void L_forget(InterpreterData & idat);

 void L_dot_s(InterpreterData & idat);

 void L_immediate(InterpreterData & idat);
// L_tocontext hidden
 void L_tocontext(InterpreterData & idat);
 void L_forth(InterpreterData & idat);
 void L_vocabulary(InterpreterData & idat);
 void L_definitions(InterpreterData & idat);
 void L_literal(InterpreterData & idat);
// may remove this one
 void L_vocquery(InterpreterData & idat);

 void L_bl(InterpreterData & idat);
 void L_word(InterpreterData & idat);
 void L_create(InterpreterData & idat);
 void L_endcreate(InterpreterData & idat);

 void L_allot(InterpreterData & idat);
 void L_rundoes(InterpreterData & idat);
 void L_does(InterpreterData & idat);
 void L_execute_at(InterpreterData & idat);
 void L_runexit(InterpreterData & idat);
 void L_exit(InterpreterData & idat);
 void L_recurse(InterpreterData & idat);
 void L_leftbracket(InterpreterData & idat);
 void L_rightbracket(InterpreterData & idat);
 void L_quit(InterpreterData & idat);
 void L_release(InterpreterData & idat);
 void L_rpush(InterpreterData & idat);
 void L_rpop(InterpreterData & idat);
 void L_rfetch(InterpreterData & idat);
 void L_nip(InterpreterData & idat);
 void L_pick(InterpreterData & idat);
 void L_reverserot(InterpreterData & idat);
 void L_twoswap(InterpreterData & idat);
 void L_twoover(InterpreterData & idat);
 void L_twodup(InterpreterData & idat);
 void L_twodrop(InterpreterData & idat);
 void L_twostore(InterpreterData & idat);
 void L_twofetch(InterpreterData & idat);
 void L_cell(InterpreterData & idat);
 void L_cellplus(InterpreterData & idat);
 void L_charplus(InterpreterData & idat);
 void L_cells(InterpreterData & idat);
 void L_chars(InterpreterData & idat);
 void L_true(InterpreterData & idat);
 void L_false(InterpreterData & idat);
 void L_memsize(InterpreterData & idat);
 void L_auxpush(InterpreterData & idat);
 void L_auxpop(InterpreterData & idat);
 void L_auxswap(InterpreterData & idat);
 void L_auxdrop(InterpreterData & idat);
 void L_auxover(InterpreterData & idat);
 void L_auxrot(InterpreterData & idat);
 void L_auxreverserot(InterpreterData & idat);
 void L_auxtuck(InterpreterData & idat);
 void L_auxnip(InterpreterData & idat);
 void L_auxroll(InterpreterData & idat);
 void L_auxpick(InterpreterData & idat);
 void L_auxtwodup(InterpreterData & idat);
 void L_auxtwodrop(InterpreterData & idat);
 void L_auxfetch(InterpreterData & idat);


 void L_timeanddate(InterpreterData & idat);
 void L_tuck(InterpreterData & idat);
 void L_roll(InterpreterData & idat);
 void L_within(InterpreterData & idat);
 

 void L_ftimes(InterpreterData & idat);
 void L_fplus(InterpreterData & idat);
 void L_fminus(InterpreterData & idat);
 void L_fdivide(InterpreterData & idat);
 void L_dtof(InterpreterData & idat);
 void L_ftod(InterpreterData & idat);
 void L_fzeroless(InterpreterData & idat);
 void L_fzeroequal(InterpreterData & idat);
 void L_fless(InterpreterData & idat);
 void L_flessequal(InterpreterData & idat);
 void L_fgreater(InterpreterData & idat);
 void L_fgreaterequal(InterpreterData & idat);
 void L_fdrop(InterpreterData & idat);
 void L_fdup(InterpreterData & idat);
 void L_fnip(InterpreterData & idat);
 void L_froll(InterpreterData & idat);
 void L_fpick(InterpreterData & idat);
 void L_fover(InterpreterData & idat);
 void L_frot(InterpreterData & idat);
 void L_freverserot(InterpreterData & idat);
 void L_ftuck(InterpreterData & idat);
 void L_fpower(InterpreterData & idat);
 void L_fabs(InterpreterData & idat);
 void L_facos(InterpreterData & idat);
 void L_fcos(InterpreterData & idat);
 void L_fcosh(InterpreterData & idat);
 void L_fasin(InterpreterData & idat);
 void L_fsin(InterpreterData & idat);
 void L_fsincos(InterpreterData & idat);
 void L_fsinh(InterpreterData & idat);
 void L_fatan(InterpreterData & idat);
 void L_ftan(InterpreterData & idat);
 void L_ftanh(InterpreterData & idat);
 void L_fatan2(InterpreterData & idat);
 void L_fexp(InterpreterData & idat);
 void L_fln(InterpreterData & idat);
 void L_flog(InterpreterData & idat);
 void L_sqrt(InterpreterData & idat);
 void L_fdot(InterpreterData & idat);
 void L_fmin(InterpreterData & idat);
 void L_fmax(InterpreterData & idat);
 void L_fnegate(InterpreterData & idat);
// end floating point

 void L_evaluate(InterpreterData & idat);

// STL methods
 void L_STLVect(InterpreterData & idat);
 void L_STLVectIterator(InterpreterData & idat);
 void L_STLMap(InterpreterData & idat);
 void L_STLMapIterator(InterpreterData & idat);
 void L_STLInsert(InterpreterData & idat);
 void L_STLFind(InterpreterData & idat);
 void L_STLBegin(InterpreterData & idat);
 void L_STLEnd(InterpreterData & idat);
 void L_STLItFrom(InterpreterData & idat);
 void L_STLItInc(InterpreterData & idat);
 void L_STLItEquals(InterpreterData & idat);
// STL done

 // class defining words
 void L_MethodSemicolon(InterpreterData & idat);
 void L_MethodColon(InterpreterData & idat);
 void L_This(InterpreterData & idat);
 void L_DashRightGreater(InterpreterData & idat);
 void L_DashDot(InterpreterData & idat);
 void L_MakeClass(InterpreterData & idat);
 void L_class(InterpreterData & idat);
 void L_EnclosedByte(InterpreterData & idat);
 void L_EnclosedBytes(InterpreterData & idat);
 void L_EnclosedWord(InterpreterData & idat);
 void L_EnclosedWords(InterpreterData & idat);
 void L_EnclosedDWord(InterpreterData & idat);
 void L_EnclosedDWords(InterpreterData & idat);
 void L_EnclosedPointer(InterpreterData & idat);
 void L_EnclosedPointers(InterpreterData & idat);
 void L_EnclosedArrays(InterpreterData & idat);
 void L_EnclosedClass(InterpreterData & idat);
 void L_EnclosedClasses(InterpreterData & idat);
 void L_endclass(InterpreterData & idat);
 void L_privatecolon(InterpreterData & idat);
 void L_publiccolon(InterpreterData & idat);
 void L_protectedcolon(InterpreterData & idat);
 void L_DecompileClass(InterpreterData & idat);
 void L_MakeClassCompileTest(InterpreterData & idat);
 void L_ColonEqual(InterpreterData & idat);
 void L_ExecuteConstructor(InterpreterData & idat);
 void L_NewClass(InterpreterData & idat);
 void L_DeleteClass(InterpreterData & idat);
 void L_squote(InterpreterData & idat);
 void L_cquote(InterpreterData & idat);
 void L_invert(InterpreterData & idat);   // put near xor
 void L_source(InterpreterData & idat);
 void L_postpone(InterpreterData & idat);
 void L_bracketcharbracket(InterpreterData & idat);
 void L_bracketticbracket(InterpreterData & idat);
 void L_char(InterpreterData & idat);
 void L_find(InterpreterData & idat);
 void L_state(InterpreterData & idat);
 void L_poundgreater(InterpreterData & idat);
 void L_lesspound(InterpreterData & idat);
 void L_pounds(InterpreterData & idat);
 void L_pound(InterpreterData & idat);
 void L_hold(InterpreterData & idat);
 void L_sign(InterpreterData & idat);
 void L_space(InterpreterData & idat);
 void L_spaces(InterpreterData & idat);
 void L_fill(InterpreterData & idat);
 void L_move(InterpreterData & idat);

 // will work with other 'types'
 void L_delete(InterpreterData & idat);

// class defining done

 void L_bye(InterpreterData & idat);

 void L_nop(InterpreterData & idat);
};

// pointer to Primitives::function passed InterpterData and returning void 

typedef void (Primitives::*PVFN)( InterpreterData &);

// typedef void (*PVFN)( InterpreterData & intrp );

// Primitive parts of words in the language are elements

class Element
{
  int type;
  union
  {
    Word * call;
    PVFN fun;
    unsigned long literal;
    string *litstring;
    ClassMethod * method;
    ClassData * data;
  };

public:

enum elementtypes
{
  Primitive,
  Definition,
  Literal,
  LitString,
  AddressV,
  MethodDefinition,
  DataDefinition
};

  Element(const Element & el)
  {
    type = el.type;
    string *ast = NULL;

    switch(type)
    {
      case Primitive:
           fun = el.fun;
          break;
      case Literal:
           literal = el.literal;
          break;
      case Definition:
           call = el.call;
          break;
      case LitString:
           ast = new string();   // the string not just the pointer is copied
           ast->reserve(el.litstring->size());
           *ast = *el.litstring;
           litstring = ast; 
          break;
      case AddressV:
           literal = el.literal;
           break;
      case MethodDefinition:
           method = el.method;
           break;
      case DataDefinition:
           data = el.data;
           break;
      default:
          break;
    }
  }

  Element(string * val)
  {
    type = LitString;
    litstring = val;
  }

  Element(unsigned long val)
  {
    type = Literal; 
    literal = val;
  }

 Element(long * pval)
  {
    type = AddressV; 
    literal = *pval;
  }

  Element(Word * def)
  {
    type = Definition;
    call = def;
  }

  Element(ClassMethod * mp)
  {
    type = MethodDefinition;
    method = mp;
  }

  Element(ClassData * dp)
  {
    type = DataDefinition;
    data = dp;
  }

  Element(PVFN ff)
  {
    type = Primitive;
    fun = ff;
  }

  ~Element()
  {
    if(type == LitString)
    {
       if(litstring != NULL)
       {
         delete litstring;
//         litstring = NULL;
       }
    }
  }

  
  void FetchLiteralAdr(unsigned long *nv)
  {
    if(type == Literal || type == AddressV)
    {
      nv = &literal;
    }
    else
    {
      nv = NULL;
    }
  }

  void FetchLiteralVal(unsigned long *nv)
  {
    if(type == Literal || type == AddressV)
    {
      *nv = literal;
    }
  }

 void SetLiteralVal(unsigned long *nv)
  {
    if(type == Literal || type == AddressV)
    {
      literal = *nv;
    }
  }

  void FetchLitStringAdr(string* cs)
  {
    if(type == LitString)
    {
      cs = litstring;
    }
    else
    {
       cs = NULL;
    }
  }

  void FetchLitString(string& cs)
  {
    if(type == LitString)
    {
      cs = *litstring;
    }
  }

 void FetchDefinitionAdr(Word *ws)
  {
    if(type == Definition)
    {
      ws = call;
    }
    else
    {
      ws = NULL;
    }
  }

  void FetchMethodDefinitionAdr(ClassMethod *mp)
  {
    if(type == MethodDefinition)
    {
      mp = method;
    }
    else
    {
      mp = NULL;
    }
  }

  void FetchDataDefinitionAdr(ClassData *dp)
  {
    if(type == DataDefinition)
    {
      dp = data;
    }
    else
    {
      dp = NULL;
    }
  }


 bool FetchDefinitionName(string & s);

// bool FetchMethodDefinitionName(string & s);
// bool FetchMethodDefinitionClass();
// bool FetchDataDefinitionName(string & s);
// bool FetchDataDefinitionClass()

 int Type()
 {
   return(type);
 }

  const Element & operator = (const Element & el)
  {
    type = el.type;
    string * ast;

    switch(type)
    {
      case Primitive:
           fun = el.fun;
          break;
      case Literal:
           literal = el.literal;
          break;
      case Definition:
           call = el.call;
          break;
      case LitString:
           ast = new string();
           ast->reserve(el.litstring->size());
           *ast = *el.litstring;
           litstring = ast; 
          break;
      case AddressV:
           literal = el.literal;
           break;
      case MethodDefinition:
           method = el.method;
           break;
      case DataDefinition:
           data = el.data;
           break;
      default:
          break;
    }
    return(*this);
  }

// Function name	: IsWord
// Description	    : does this element call the given word?
// RSData type		: bool 
// Argument         : Word * ww
 bool IsWord(Word * ww)
 {
   if(type == Definition)
   {
      if(call == ww)
      {
        return(true);
      }
   }
   return(false);
 }

 bool IsMethod(ClassMethod * pm)
 {
  if(type == MethodDefinition)
   {
      if(method == pm)
      {
        return(true);
      }
   }
   return(false);    
 }

 bool IsData(ClassData * pd)
 {
  if(type == DataDefinition)
   {
      if(data == pd)
      {
        return(true);
      }
   }
   return(false);    
 }

 bool SameWord(char *nam, InterpreterData & idat);
 void Execute(InterpreterData & intrp, int imed = 0);
 friend Word;
 friend ClassMethod;
 friend ClassData;
};


// the class of words in the language

class Word
{
  string name;
  unsigned long flags;
  vector<Element > word;

 public: 

  enum exerrors
  {
    Ok,
    None,
    ErrorExec,
  };

 Word()
 {
   flags = 0L;
   word.reserve(60);
 }

 ~Word()
 {

 }

 Word(const Word & Wrd)
 {
   name = Wrd.name;
   word = Wrd.word;
   flags = Wrd.flags;
 }

 Word & operator = (const Word & Wrd)
 {
   name = Wrd.name;
   word = Wrd.word;
   flags = Wrd.flags;
   return(*this);
 }

 void NewName(string & cs)
 {
   name = cs;
 }

 void AddElement(Element & el)
 {
   if((word.capacity() - word.size()) < 10)
   {
     word.reserve(word.capacity() + 20);
   }
   word.push_back(el);
 }

 void SetFlags(unsigned long ff)
 {
   flags |= ff;
 }

 unsigned long FlagsMatch(unsigned long ff)
 {
   return((flags & ff));
 }

 string& Name()
 {
   return(name);
 }

 string *NamePtr()
 {
   return(&name);
 }

 int NextUse(Word * Wrd, int index)
 {
    for(int ii = (index + 1);ii < word.size();ii++)
    {
       if(word[ii].IsWord(Wrd))
       {
         return(ii);
       }
    }
    return(-1);
 }

 int LastUse(Word * Wrd, int index)
 {
    for(int ii = index;ii >= 0;ii--)
    {
       if(word[ii].IsWord(Wrd))
       {
         return(ii);
       }
    }
    return(-1);
 }

 int NextUse(ClassMethod * cm, int index)
 {
    for(int ii = (index + 1);ii < word.size();ii++)
    {
       if(word[ii].IsMethod(cm))
       {
         return(ii);
       }
    }
    return(-1);
 }

 int LastUse(ClassMethod * cm, int index)
 {
    for(int ii = index;ii >= 0;ii--)
    {
       if(word[ii].IsMethod(cm))
       {
         return(ii);
       }
    }
    return(-1);
 }

int NextUse(ClassData * dm, int index)
 {
    for(int ii = (index + 1);ii < word.size();ii++)
    {
       if(word[ii].IsData(dm))
       {
         return(ii);
       }
    }
    return(-1);
 }

 int LastUse(ClassData * dm, int index)
 {
    for(int ii = index;ii >= 0;ii--)
    {
       if(word[ii].IsData(dm))
       {
         return(ii);
       }
    }
    return(-1);
 }


 int NextIndex()
 {
   return(word.size());
 }

 void LiteralAddress(int index, unsigned long* ll)
 {
   if (index >= 0 && index < word.size())
   {
     word[index].FetchLiteralAdr(ll);
   }
   else
   {
     ll = NULL;
   }
 }

 void LiteralValue(int index, unsigned long* ll)
 {
   if (index >= 0 && index < word.size())
   {
     word[index].FetchLiteralVal(ll);
   }
 }

 void SetLiteralValue(int index, unsigned long* ll)
 {
   if (index >= 0 && index < word.size())
   {
     word[index].SetLiteralVal(ll);
   }
 }

 void StringAddress(int index, string * cs)
 {
   if (index >= 0 && index < word.size())
   {
     word[index].FetchLitStringAdr(cs);
   }
   else
   {
     cs = NULL; 
   }
 }

 void StringAsCopy(int index, string& cs)
 {
   if (index >= 0 && index < word.size())
   {
     word[index].FetchLitString(cs);
   }
 }

  void DefinitionAddress(int index, Word * ww)
  {
    if (index >= 0 && index < word.size())
    {
       word[index].FetchDefinitionAdr(ww);
    }
    else
    {
       ww = NULL; 
    }
  }

  void DefinitionAddress(int index, ClassMethod * cm)
  {
    if (index >= 0 && index < word.size())
    {
       word[index].FetchMethodDefinitionAdr(cm);
    }
    else
    {
       cm = NULL; 
    }
  }

 void DefinitionAddress(int index, ClassData * dm)
  {
    if (index >= 0 && index < word.size())
    {
       word[index].FetchDataDefinitionAdr(dm);
    }
    else
    {
       dm = NULL; 
    }
  }


 int Execute(int index, InterpreterData & intrp, int imed = 0);

 bool SameWord(int index, char *nam, InterpreterData & idat)
 {
    return(word[index].SameWord(nam, idat));
 }

void Decompile(string& dtext, InterpreterData & intrp);

friend ClassMethod; 

};

// data element description for forth classes;
class ClassData 
{
   unsigned long size;
   unsigned long flags;
   unsigned long element_size;
   unsigned long number_of;
   string name;
   unsigned long offset;
   ClassDescription * pclassdecription;
   ClassDescription * dclass_desc;

public:

    enum data_types
    {
       ExecuteCompile =  0x01,
       Hidden =          0x02,
       Private =         0x04,
       Protected =       0x08,
       Public =         0x010,
       ArrayType =      0x020,
       ClassType =      0x040
    };

   ClassData()
   {
      name.reserve(120);
      offset = 0l;
      size = 0l;
      flags = 0l;
      number_of = 1;
      element_size = 0;
      pclassdecription = NULL;
      dclass_desc = NULL;
   }

   ClassData(string & nam, unsigned long siz, ClassDescription *pc,  unsigned long num = 1, ClassDescription *dc = NULL);
 
   ClassData(const ClassData& cd)
   {
      name.reserve(120);
      name = cd.name;
      offset = cd.offset;
      size = cd.size;
      flags = cd.flags;
      number_of = cd.number_of;
      element_size = cd.element_size;
      pclassdecription = cd.pclassdecription;
      dclass_desc = cd.dclass_desc; 
   }

   ClassData& operator = (const ClassData& cd)
   {
      name.reserve(120);
      name = cd.name;
      offset = cd.offset;
      size = cd.size;
      flags = cd.flags;
      number_of = cd.number_of;
      element_size = cd.element_size;
      pclassdecription = cd.pclassdecription;
      dclass_desc = cd.dclass_desc;
      return(*this);
   }

   void SetFlags(unsigned long ff)
   {
       flags |= ff;
   }
   
   unsigned long FlagsMatch(unsigned long ff)
   {
       return((flags & ff));
   }

   unsigned long EraseFlags(unsigned long gg)
   {
       unsigned long rr = ~(flags & gg);
       flags &= rr;
       return(flags);
   }
   
   void ResetPermision(unsigned long nn)
   {
       EraseFlags(Private);
       EraseFlags(Protected);
       EraseFlags(Public);
       flags |= nn;
   }

   unsigned long Size()
   {
     return(size);
   }

   unsigned long Offset()
   {
      return(offset);
   }

   unsigned long Offset(unsigned long o)
   {
     if(FlagsMatch(ArrayType))
     {
       long nb = (o * element_size) + offset;
       return(nb);       
     }
     return(offset);
   }

   unsigned int Array()
   {
     return(FlagsMatch(ArrayType));
   }

   unsigned long ElementSize()
   {
      return(element_size);
   }

   unsigned long ElementNumber()
   {
     return(number_of);
   }

   string & Name()
   {
     return(name);
   }

   unsigned int IsPrivate()
   {
     return(FlagsMatch(Private));
   }

   unsigned int IsClass()
   {
     return(FlagsMatch(ClassType));
   }

  void SetClassDecription(ClassDescription * pcd)
  {
     pclassdecription = pcd;
  }

  ClassDescription * GetClassDescription(void)
  {
     return(pclassdecription);
  }

  int Resolve(InterpreterData & idat);

   void SetName(string & sn)
   {
     name = sn;
   }

   void SetPrivate()
   {
     SetFlags(Private);
   }

   void SetSize(unsigned long num, unsigned long siz)
   {
     if(num > 1)
      {
        size = num *siz;
        number_of = num;
        element_size = siz;
        SetFlags(ArrayType);
      }
      else
      {
        size = siz;
        number_of = 1;
        element_size = size; 
      }
   }

   void SetOffset(unsigned long gs)
   {
       offset = gs;
   }

   void SetDClassDesc(ClassDescription * dc)
   {
     dclass_desc = dc;
     SetFlags(ClassType);
   }

   ClassDescription * DClassDesc(void)
   {
      return(dclass_desc);
   }

   void Decompile(string& dtext, InterpreterData & idat); 
};

// method object for forth classes
class ClassMethod
{
  string name;
  unsigned long flags;
  vector<Element > method;
  unsigned long mindex;  // method index
  ClassDescription * pclassdecription;

 public:

  enum CompFlag
  {
    ExecuteCompile =   0x01,
    Hidden =           0x02,
    Private =          0x04,
    Protected =        0x08,
    Public =          0x010,
    ArrayType =       0x020,
    ClassType =       0x040
  };

  enum exerrors
  {
    Ok,
    None,
    ErrorExec,
  };

  // duplicate of 

 ClassMethod()
 {
   flags = 0L;
   name.reserve(120);
   method.reserve(20);
   pclassdecription = NULL;
 }

 ClassMethod(string& nm, ClassDescription *cd)
 {
   flags = 0L;
   name.reserve(120);
   method.reserve(20);
   name = nm;
   pclassdecription = cd;
 }

 ~ClassMethod()
 {
   ;
 }

 ClassMethod(const ClassMethod & mm)
 {
   name = mm.name;
   method.reserve(mm.method.size() + 1);
   method = mm.method;
   flags = mm.flags;
   pclassdecription = mm.pclassdecription;
 }

 explicit ClassMethod(const Word & ww, ClassDescription * cd)
 {
   name = ww.name;
   method.reserve(ww.word.size() + 1);
   method = ww.word;
   flags = ww.flags;
   pclassdecription = cd;
 }

 ClassMethod & operator = (const ClassMethod & mm)
 {
   name = mm.name;
   method = mm.method;
   flags = mm.flags;
   pclassdecription = mm.pclassdecription;
   return(*this);
 }

 void NewName(string & cs)
 {
   name = cs;
 }

 void AddClassDecription(ClassDescription * pcd)
 {
   pclassdecription = pcd;
 }

 ClassDescription * GetClassDescription(void)
 {
   return(pclassdecription);
 }

 void AddElement(Element & el)
 {
   if((method.capacity() - method.size()) < 10)
   {
     method.reserve(method.capacity() + 20);
   }
   method.push_back(el);
 }

 unsigned long MethodIndex(void)
 {
   return(mindex);
 }

 void SetMethodIndex(unsigned long ii)
 {
   mindex = ii;
 }

 void SetFlags(unsigned long ff)
 {
   flags |= ff;
 }

 unsigned long FlagsMatch(unsigned long ff)
 {
   return((flags & ff));
 }

 unsigned long EraseFlags(unsigned long gg)
 {
   unsigned long rr = ~(flags & gg);
   flags &= rr;
   return(flags);
 }

 void ResetPermision(unsigned long nn)
 {
    EraseFlags(Private);
    EraseFlags(Protected);
    EraseFlags(Public);
    flags |= nn;
 }

 string& Name()
 {
   return(name);
 }

 string *NamePtr()
 {
   return(&name);
 }

 void SetName(string & ss)
 {
   name = ss;
 }

 int NextUse(ClassMethod * mm, int index)
 {
    for(int ii = (index + 1);ii < method.size();ii++)
    {
       if(method[ii].IsMethod(mm))
       {
         return(ii);
       }
    }
    return(-1);
 }

 int NextUse(ClassData * dm, int index)
 {
    for(int ii = (index + 1);ii < method.size();ii++)
    {
       if(method[ii].IsData(dm))
       {
         return(ii);
       }
    }
    return(-1);
 }

 int NextUse(Word * ww, int index)
 {
    for(int ii = (index + 1);ii < method.size();ii++)
    {
       if(method[ii].IsWord(ww))
       {
         return(ii);
       }
    }
    return(-1);
 }

 int LastUse(ClassMethod * mm, int index)
 {
    for(int ii = index;ii >= 0;ii--)
    {
       if(method[ii].IsMethod(mm))
       {
         return(ii);
       }
    }
    return(-1);
 }

 int LastUse(ClassData * mm, int index)
 {
    for(int ii = index;ii >= 0;ii--)
    {
       if(method[ii].IsData(mm))
       {
         return(ii);
       }
    }
    return(-1);
 }

 int LastUse(Word * ww, int index)
 {
    for(int ii = index;ii >= 0;ii--)
    {
       if(method[ii].IsWord(ww))
       {
         return(ii);
       }
    }
    return(-1);
 }

 int NextIndex()
 {
   return(method.size());
 }

 void LiteralAddress(int index, unsigned long* ll)
 {
   if (index >= 0 && index < method.size())
   {
     method[index].FetchLiteralAdr(ll);
   }
   else
   {
     ll = NULL;
   }
 }

 void LiteralValue(int index, unsigned long* ll)
 {
   if (index >= 0 && index < method.size())
   {
     method[index].FetchLiteralVal(ll);
   }
 }

 void SetLiteralValue(int index, unsigned long* ll)
 {
   if (index >= 0 && index < method.size())
   {
     method[index].SetLiteralVal(ll);
   }
 }

 void StringAddress(int index, string * cs)
 {
   if (index >= 0 && index < method.size())
   {
     method[index].FetchLitStringAdr(cs);
   }
   else
   {
     cs = NULL; 
   }
 }

 void StringAsCopy(int index, string& cs)
 {
   if (index >= 0 && index < method.size())
   {
     method[index].FetchLitString(cs);
   }
 }

  void DefinitionAddress(int index, Word * ww)
  {
    if (index >= 0 && index < method.size())
    {
       method[index].FetchDefinitionAdr(ww);
    }
    else
    {
       ww = NULL; 
    }
  }

  void DefinitionAddress(int index, ClassMethod * cm)
  {
    if (index >= 0 && index < method.size())
    {
       method[index].FetchMethodDefinitionAdr(cm);
    }
    else
    {
       cm = NULL; 
    }
  }

 void DefinitionAddress(int index, ClassData * dm)
  {
    if (index >= 0 && index < method.size())
    {
       method[index].FetchDataDefinitionAdr(dm);
    }
    else
    {
       dm = NULL; 
    }
  }

 int Execute(int index, InterpreterData & intrp, int imed = 0);

 bool SameWord(int index, char *nam, InterpreterData & idat)
 {
    return(method[index].SameWord(nam, idat));
 }

 void Decompile(string& dtext, InterpreterData & idat); 

};

class ClassDescription
{
   long objsize;
//   bool compiling;
//   bool compiling_method;
   map<string, ClassData, less<string> >objectdata;
   map<string, ClassMethod, less<string> >methods;
   vector<string> obj_order;
   vector<string> inherited_order;
   string name;
   deque<char *> thisstack;
//   bool compile_is_privite;
   unsigned long current_offset;
   InterpreterData * pidat;
   unsigned long flags;
   
 public:

   enum DescriptionFlags
   {
     VirtualClass =      0x01,
     CanInherit =        0x02,
     CompilePrivate =    0x04,
     CompileProtected =  0x08,
     CompilePublic =    0x010,
     CompilingNow =     0x020,
     CompilingMethod =  0x040
   };

  ClassDescription()
  {
    objsize = 0;
    name.reserve(120);
    flags = 0l;
    SetFlags((CompilingNow | CompilePrivate));
 //   compiling = true;
 //   compiling_method = false;
 //   compile_is_privite = true;
    pidat = NULL;
    obj_order.reserve(10);
    inherited_order.reserve(10);
  }

  ClassDescription(string& nn, InterpreterData * pi)
  {
    name.reserve(120);
    name = nn;
    flags = 0l;
    SetFlags((CompilingNow | CompilePrivate));
    objsize = 0;
//    compiling = true;
//    compiling_method = false;
//    compile_is_privite = true;
    pidat = pi;
    obj_order.reserve(10);
    inherited_order.reserve(10);
  }

  ClassDescription(const ClassDescription & fc)
  {
    name = fc.name;
    objsize = fc.objsize;
//    compiling = fc.compiling;
    objectdata = fc.objectdata;
    methods = fc.methods;
//    compiling_method = fc.compiling_method;
//    compile_is_privite = fc.compile_is_privite;
    pidat = fc.pidat;
    obj_order.reserve(fc.obj_order.capacity());
    obj_order = fc.obj_order;
    inherited_order = fc.inherited_order;
    flags = fc.flags;
  }

  ClassDescription& operator = (const ClassDescription & fc)
  {
    name = fc.name;
    objsize = fc.objsize;
//    compiling = fc.compiling;
    objectdata = fc.objectdata;
    methods = fc.methods;
//    compiling_method = fc.compiling_method;
//    compile_is_privite = fc.compile_is_privite;
    pidat = fc.pidat;
    obj_order.reserve(fc.obj_order.capacity());
    obj_order = fc.obj_order;
    inherited_order.reserve(fc.inherited_order.capacity());
    inherited_order = fc.inherited_order;
    flags = fc.flags;
    return(*this);
  }

   bool operator < (const ClassDescription& cd) 
   {
      if(name < cd.name)
      {
          return(true);
      }
      return(false);
   }

  bool operator == (const ClassDescription& cd) 
   {
      if(name == cd.name)
      {
          return(true);
      }
      return(false);
   }

 void SetFlags(unsigned long ff)
 {
   flags |= ff;
 }

 unsigned long FlagsMatch(unsigned long ff)
 {
   return((flags & ff));
 }

 unsigned long EraseFlags(unsigned long gg)
 {
   unsigned long rr = ~(flags & gg);
   flags &= rr;
   return(flags);
 }

  void PushInheritedThis(char * nths);
  void PopInheritedThis(void);

  void PushThis(char * nth)
  {
    thisstack.push_front(nth);
    PushInheritedThis(nth);
  }

  char *ThisFront(void)
  {
    return(thisstack.front());
  }

  void PopThis(void)
  {
    thisstack.pop_front();
    PopInheritedThis();
  }

  bool IsMethodCompiling()
  {
    if(FlagsMatch(CompilingMethod))
    {
      return(true);
    }
    return(false);
  }

  void SetMethodCompile(bool bv)
  {
    if(bv == true)
    {
       SetFlags(CompilingMethod);
    }
    else
    {
       EraseFlags(CompilingMethod);
    }
  }

      bool Compiling(void)
      {
           if(FlagsMatch(CompilingNow))
            {
              return(true);
            }
            return(false);
      }
      
      bool IsPrivateMode()
      {
           if(FlagsMatch(CompilePrivate))
            {
              return(true);
            }
            return(false);
      }
      
      bool IsProtectedMode()
      {
          if(FlagsMatch(CompileProtected))
          {
              return(true);
          }
          return(false);
      }
      
      void SetPrivateMode(bool pm)
      {
          if(pm == true)
          {
              pidat->DebugTrace("set class compile to private:"); 
              EraseFlags(CompilePublic); 
              EraseFlags(CompileProtected); 
              SetFlags(CompilePrivate);
          }
          else
          {
              pidat->DebugTrace("set class compile to public:"); 
              EraseFlags(CompilePrivate); 
              EraseFlags(CompileProtected); 
              SetFlags(CompilePublic);
          }
      }
      
      void SetProtectedMode()
      {
          pidat->DebugTrace("set class compile to protected:"); 
          EraseFlags(CompilePublic); 
          EraseFlags(CompilePrivate); 
          SetFlags(CompileProtected);
          
      }
      
      void SetPublicdMode()
      {
          pidat->DebugTrace("set class compile to protected:"); 
          EraseFlags(CompileProtected); 
          EraseFlags(CompilePrivate); 
          SetFlags(CompilePublic);
      }

  unsigned long CurrentOffset()
  {
    return(current_offset);
  }

  void Inherit(string &nam, unsigned long mode);

  void AddInheretedName(string & sc)
  {
       if((inherited_order.capacity() - inherited_order.size()) < 3)
        {
           inherited_order.reserve(inherited_order.size() + 10);
        }
       inherited_order.push_back(sc);
  }

 ClassDescription * InheritedClassDescription(string & cn);

 ClassData * InheritedClassData(string & nam);
 ClassMethod * InheritedClassMethod(string & nam);
 void ResetNamesForInherit(string &prepend); 
 void CallDestructor(void);
 void CallConstructor(char * tt);

  bool MethodExists(string & nm)
  {
      map<string, ClassMethod, less<string> >::iterator ii;
      ii = methods.find(nm);
       
     if(ii != methods.end())
      {
        return(true);
      }
     else
     {
       if(InheritedClassMethod(nm) != NULL)
       {
         return(true);
       }
     }
    return(false);
  }

  bool DataExists(string & nm)
  {
      map<string, ClassData, less<string> >::iterator ii;
      ii = objectdata.find(nm);
       
     if(ii != objectdata.end())
      {
        return(true);
      }
     else
     {
       if(InheritedClassData(nm) != NULL)
       {
         return(true);
       }
     }

    return(false);
  }

  bool AddMethod(ClassMethod & cm, bool override)
  {
     if(IsPrivateMode())
     {
         cm.ResetPermision(ClassMethod::Private);
     }
     else
     {
         if(IsProtectedMode())
         {
             cm.ResetPermision(ClassMethod::Protected);
         }
         else
         {
             cm.ResetPermision(ClassMethod::Public);
         }
     }

     map<string, ClassMethod, less<string> >::iterator ii;
     ii = methods.find(cm.Name());
       
     if(ii == methods.end())
      {
         methods[cm.Name()] = cm;
         char test1[200];
         sprintf(test1, "Added method %s in ClassDescription::AddMethod()", cm.Name().c_str());
         pidat->DebugTrace(test1); 
         return(true);
     }
     else
     {
       if(override)
       {
//           map<string, ClassMethod, less<string> >::iterator ii;
//           ii = methods.find(cm.Name());
//           methods.erase(ii);
//           methods[cm.Name()] =  cm;
//           return(true);
       }
     }
     return(false);
  }

  bool AddData(ClassData & dm)
  {
     if(IsPrivateMode())
     {
         dm.ResetPermision(ClassData::Private);
     }
     else
     {
       if(IsProtectedMode())
       {
           dm.ResetPermision(ClassData::Protected);
       }
       else
       {
           dm.ResetPermision(ClassData::Public);
       }
     }

     if(dm.IsClass())
     {
       pidat->DebugTrace("adding class in AddData");
     }

     if(DataExists(dm.Name()) == false)
     {
        objectdata[dm.Name()] =  dm;
        if((obj_order.capacity() - obj_order.size()) < 3)
        {
           obj_order.reserve(obj_order.size() + 10);
        }
        obj_order.push_back(dm.Name());
        current_offset += dm.Size();
        return(true);
     }
     return(false);
  }
  
  ClassMethod * MethodDesc(string & nm)
  {
      map<string, ClassMethod, less<string> >::iterator ii;
      ClassMethod * pcm = NULL;
      char test1[100];

      sprintf(test1, "%s method search for %s", name.c_str(), nm.c_str());
      pidat->DebugTrace(test1);

      ii = methods.find(nm);
      
      if(ii != methods.end())
      {
          pcm = &(*ii).second;
         
          sprintf(test1, "%s found method %s in ClassDescription::MethodDesc() as %lx", name.c_str(), nm.c_str(), (void *)pcm);
          pidat->DebugTrace(test1); 
          return(pcm);
      }
      pcm = InheritedClassMethod(nm);
      return(pcm);
  }
  
  ClassData * DataDesc(string & nm)
  {
      map<string, ClassData, less<string> >::iterator ii;
      ClassData * pcd = NULL;
      char test1[100];

      sprintf(test1, "%s data search for %s", name.c_str(), nm.c_str());
      pidat->DebugTrace(test1);
      ii = objectdata.find(nm);
      
      if(ii != objectdata.end())
      {
          pcd = &(*ii).second;
          
          sprintf(test1, "%s found object %s in ClassDescription::DataDesc() as %lx", name.c_str(), nm.c_str(), (void *)pcd);
          pidat->DebugTrace(test1); 
          return(pcd);
      }
      pcd = InheritedClassData(nm); 
      return(pcd);
  }

   bool MethodDescObject(string & nm, ClassMethod & mm)
  {
      map<string, ClassMethod, less<string> >::iterator ii;
      ii = methods.find(nm);
       
     if(ii != methods.end())
      {
         mm = (*ii).second;
         return(true);
       }
    return(false);
  }

   bool DataDescObject(string & nm, ClassData & dm)
  {
      map<string, ClassData, less<string> >::iterator ii;
      ii = objectdata.find(nm);
       
     if(ii != objectdata.end())
      {
         dm = (*ii).second;
         return(true);
      }
    return(false);
  }


  string & Name(void)
  {
    return(name);
  }

  void SetName(string & ss)
  {
    name = ss;
  }

  bool RenameMethod(string &onam, string &nnam)
  {
      map<string, ClassMethod, less<string> >::iterator ii;
      ii = methods.find(onam);
      ClassMethod mm;
       
     if(ii != methods.end())
      {
         mm = (*ii).second;
         mm.SetName(nnam);
         methods.erase(ii);
         methods[nnam] =  mm;
         return(true);
       }
    return(false);    
  }

  unsigned long Size(void)
  {
    return(objsize);
  }

  unsigned long CalkObjSize(void)
  {
    map<string, ClassData, less<string> >::iterator ii;
    vector<string>:: iterator jj;
    unsigned long rets = 0l;

    for(jj = obj_order.begin(); jj != obj_order.end(); jj++)
    {
      ii = objectdata.find((*jj));
      if(ii != objectdata.end())
      {
        (*ii).second.SetOffset(rets);
        rets += (*ii).second.Size();        
      }
    }
    return(rets);
  }

// for permision inheritance

  void ResetAllPrivate(void)
  {
      map<string, ClassData, less<string> >::iterator ii;
      map<string, ClassMethod, less<string> >::iterator kk;
      vector<string>:: iterator jj;
      
      for(jj = obj_order.begin(); jj != obj_order.end(); jj++)
      {
          ii = objectdata.find((*jj));
          if(ii != objectdata.end())
          {
              (*ii).second.ResetPermision(ClassData::Private);
          }
      }
  }
  
  void ResetAllProtected(void)
  {
      map<string, ClassData, less<string> >::iterator ii;
      map<string, ClassMethod, less<string> >::iterator kk;
      vector<string>:: iterator jj;
      
      for(jj = obj_order.begin(); jj != obj_order.end(); jj++)
      {
          ii = objectdata.find((*jj));
          if(ii != objectdata.end())
          {
              if(ii->second.FlagsMatch(ClassData::Public))
              {
                  (*ii).second.ResetPermision(ClassData::Protected);
              }
          }
      }
  }
  
  void Done(void)
  {
      EraseFlags(CompilingNow);
      objsize = CalkObjSize();
  }

  void Decompile(string & tt, InterpreterData & idat)
  {
      map<string, ClassData, less<string> >::iterator ii;
      map<string, ClassMethod, less<string> >::iterator kk;
      vector<string>:: iterator jj;
      unsigned long rets = 0l;
      
      tt += "\nClass Name: \"";
      tt += name;
      tt += "\"";
      
      for(jj = obj_order.begin(); jj != obj_order.end(); jj++)
      {
          ii = objectdata.find((*jj));
          if(ii != objectdata.end())
          {
              idat.DebugTrace("decompiling object");
              (*ii).second.Decompile(tt, idat);
          }
      }
      for(kk = methods.begin(); kk != methods.end(); kk++)
      {
          if(kk != methods.end())
          {
              idat.DebugTrace("decompiling method");
              (*kk).second.Decompile(tt, idat);
          }
      }
      tt += "\n --End-of-class-decompile---";
  }

};

// idat.ClassDSCR()

class ClassDescriptions
{
   map<string, ClassDescription, less<string> > cld;
   deque<string> compiling;
   InterpreterData * pidat;

public:

   ClassDescriptions()
   {
     InterpreterData * pidat = NULL;
   }

   ClassDescriptions(const ClassDescriptions & cds)
   {
     cld = cds.cld;
     compiling = cds.compiling;
   }

   void SetInterpreterData(InterpreterData * pi)
   {
     pidat = pi;
   }
   
   bool Find(string & sn, ClassDescription * pcd)
   {
       map<string, ClassDescription, less<string> >::iterator ii;
       char test1[200];

       ii = cld.find(sn);
       if(ii != cld.end())
       {
         pcd = &((*ii).second);

         sprintf(test1, "Address in ClassDescription * cd = %lx in ClassDescriptions::Find(%s) (found)", (void *)pcd, sn.c_str());
         pidat->DebugTrace(test1); 
         return(true);
       }
       pcd = NULL;
       sprintf(test1, "NULL Address in ClassDescription * cd = %lx in ClassDescriptions::Find(%s) (not found)", (void *)pcd, sn.c_str());
       pidat->DebugTrace(test1); 
       return(false);
   }

   ClassDescription * Find(string & sn)
   {
       map<string, ClassDescription, less<string> >::iterator ii;
       char test1[200];
       ClassDescription *pcd;

       ii = cld.find(sn);
       if(ii != cld.end())
       {
         pcd = &((*ii).second);

         sprintf(test1, "Address in ClassDescription * cd = %lx in ClassDescriptions::Find(%s) (found)", (void *)pcd, sn.c_str());
         pidat->DebugTrace(test1); 
         return(pcd);
       }
       pcd = NULL;
       sprintf(test1, "NULL Address in ClassDescription * cd = %lx in ClassDescriptions::Find(%s) (not found)", (void *)pcd, sn.c_str());
       pidat->DebugTrace(test1); 
       return(pcd);
   }

  bool Find(char * wrd, ClassDescription * acd)
   {
       if(wrd == NULL)
       {
         return(false);
       }
       string snn;
       snn.reserve(120);
       snn = wrd;
       bool rv = Find(snn, acd);

       return(rv);
   }

  bool AddDescription(ClassDescription & dd)
  {
      ClassDescription * nd = NULL;
 //     char test1[200];
      
      pidat->DebugTrace("In AddDescription first find should fail"); 
      map<string, ClassDescription, less<string> >::iterator ii;
      
      ii = cld.find(dd.Name());
      if(ii != cld.end())
      {
          return(false);
      }
           
      cld[dd.Name()] = dd;
//      nd = NULL;
//      pidat->DebugTrace("In AddDescription second find should succeed"); 
//     Sleep(10);
//     ii = cld.find(dd.Name());
//      if(ii != cld.end())
//      {
//          nd = &((*ii).second);
//          sprintf(test1, "Address in ClassDescription * nd = %lx in ClassDescriptions::AddDescription after 2nd find", (void *) nd);
//          pidat->DebugTrace(test1); 
          compiling.push_front(dd.Name());
          return(true);
//      }
//      return(false);
  }

  bool AddInheritedDescription(ClassDescription & dd)
  {
      ClassDescription * nd = NULL;
 //     char test1[200];
      
      
      map<string, ClassDescription, less<string> >::iterator ii;
      
      ii = cld.find(dd.Name());
      if(ii != cld.end())
      {
          pidat->DebugTrace("AddInheritedDescription FAILED"); 
          return(false);
      }
           
      cld[dd.Name()] = dd;
      pidat->DebugTrace("AddInheritedDescription OK"); 
      return(true);
  }

   ClassDescription *CompilingWhat(void)
   {
     string name_is;
     name_is.reserve(120);
     ClassDescription * pcd = NULL; 
     if(compiling.empty() == false)
     {
        name_is = compiling.front();
        map<string, ClassDescription, less<string> >::iterator ii;
        ii = cld.find(name_is);
        if(ii != cld.end())
        {
           pcd = &((*ii).second);
           char test1[200];
           sprintf(test1, "Address in pcd = %lx in ClassDescriptions::CompilingWhat(void)", (void *)pcd);
           pidat->DebugTrace(test1); 
           return(pcd);
        }
        else
        {
          return(NULL);
        }
     }
     return(NULL);
   }

   bool Compiled()
   {
      ClassDescription *cp = CompilingWhat();
      if(cp != NULL)
      {
        cp->Done();
        compiling.pop_front(); 
        pidat->DebugTrace("in ClassDescriptions::Compiled() pop done"); 
        return(true);
      }
      else
      {
        return(false);
      }
   }
  
};

class ParserData
{
  char * buffer;
  char * linestart;
  char * lineend;
  char * ppos;
  char * lppos;
  char line[1024];
  char cline[1024];
  InterpreterData * pidat;
  char bufname[80];

public:

    ParserData(char *buf = NULL, InterpreterData * pi = NULL, char *nm = NULL)
    {
      buffer = buf;
      linestart = lineend = buffer;
      ppos = lppos = line;
      pidat = pi;
      if(nm != NULL)
      {
        strncpy(bufname, nm, 79);
      }
      else
      {
        strcpy(bufname, "Unknown");
      }
      NextLine();
    }

   ParserData(ParserData & pp)
    {
      buffer = pp.buffer;
      linestart = pp.linestart;
      lineend = pp.lineend;
      ppos = pp.ppos;
      lppos = pp.lppos;
      pidat = pp.pidat;
      strncpy(bufname, pp.bufname, 79);
    }

  ParserData & operator == (ParserData & pp)
    {
      buffer = pp.buffer;
      linestart = pp.linestart;
      lineend = pp.lineend;
      ppos = pp.ppos;
      lppos = pp.lppos;
      pidat = pp.pidat;
      strncpy(bufname, pp.bufname, 79);
      return(*this);
    }

  void ParserTrace(const char * cc, const char * ee = NULL, int np = 0)
  {
    char dbl[220];
    if(ee)
    {
       if(np == 0)
       {
         sprintf(dbl, "ParserData:%s %s >>%s<< [%i]", bufname, ee, cc, strlen(cc));
       }
       else
       {
         sprintf(dbl, "ParserData:%s %s >>%s<< [%i] (%li)", bufname, ee, cc, strlen(cc), np);
       }
    }
    else
    {
       if(np == 0)
       {
         sprintf(dbl, "ParserData:%s >>%s<<", bufname, cc);     
       }
       else
       {
          sprintf(dbl, "ParserData:%s >>%s<< (%li)", bufname, cc, np);         
       }
    }

    pidat->DebugTrace(dbl); 
  }

  void ResetLinePos(unsigned long nn)
  {
    if(nn >= 0 && nn < strlen(line))
    {
       ppos = nn + line;
       lppos = ppos;
    }
  }
  
  unsigned long LinePos(void)
  {
//      ParserTrace("LinePos", line, (ppos - line));
      return(ppos - line);  
  }

    bool NextLine(void)
    {
 //       ParserTrace(" Entered Nextline");
        char * oldend = lineend;
        if(*lineend != 0)
        {
          lineend++;
        }
        else
        {
           line[0] = 0;
           linestart = lineend;
           ppos = line;
           return(false);
        }
        while(((*lineend) != '\n') && (*lineend) != 0)
        {
            lineend++;
        }
        
        linestart = oldend;
        if(linestart != buffer)
        {
           linestart++;
        }
        memcpy(line, linestart, (lineend - linestart));
        line[(lineend - linestart)] = 0;
        ppos = line;
        lppos = line;
//        ParserTrace("BufferLine:", line);
//        ParserTrace("NextLine:", line);
        if(*lineend)
        {
//          ParserTrace(" Exiting Nextline true");
          return(true);
        }
 //       ParserTrace(" Exiting Nextline false");
        return(false);
    }

    bool AdvancepWhite(void)
    {
//       ParserTrace("entered AdvancepWhite:");
       while((*ppos) != 0 && (isspace(*ppos)))
       {
         ppos++;
       }
       if((*ppos) == 0)
       {
         return(false);
       }
//       ParserTrace(ppos, "AdvancepWhite:");
       return(true);
    }

    bool AdvancelpWhite(void)
    {
       while(isspace(*lppos))
       {
         lppos++;
       }
//       ParserTrace(lppos, "AdvancelpWhite:");
       return(true);
    }

    bool AdvancepBlack()
    {
      while((*ppos) != 0 && !(isspace(*ppos)))
       {
         ppos++;
       }
       
       if((*ppos) == 0)
       {
//         ParserTrace(ppos, "AdvancepBlack: ret == false");
         return(false);
       }
//       ParserTrace(ppos, "AdvancepBlack: ret == true");
       return(true);
    }

    bool GetWord(char * ww)
    {
       int dd = ppos - lppos;
       if(dd)
       {
          memcpy(ww, lppos, dd);
          ww[(ppos - lppos)] = 0;
//          ParserTrace(ww, "word:");
          lppos = ppos;
       }
       else
       {
//          ParserTrace("Word length 0", "word:");
//          ParserTrace(ppos, "word:ppos");
//          ParserTrace(lppos, "word:lppos");
       }
       return(true);
    }
    
    bool ToAny(char nn)
    {
        while((*ppos) != 0 && (*ppos) != nn)
        {
            ppos++;
        }
        if((*ppos) == 0)
        {
            return(false);
        }
//        ParserTrace(ppos, "toAny:");
//        ParserTrace(lppos, "toAny:");
        return(true);   
    }
    
    bool GetAny(string &cc)
    {
        if(ppos == lppos)
        {
            return(false);
        }
//        ParserTrace(ppos, "GetAny:");
        int dd = ppos - lppos;
        memcpy(cline, lppos, dd);
        cline[dd] = 0;
//        ParserTrace(cline, "GetAny:cline");
        cc = &cline[1];

//        ppos++;
//        ParserTrace(ppos, "GetAny p:"); 
//        ParserTrace(ppos, "GetAny lp:"); 
        lppos = ppos;
        return(true);
    }

 char GetChar(void)
 {
   if((*ppos) == 0)
   {
     return(0);
   }
   return(*(ppos + 1));
 }

 void Advance(void)
 {
   if((*ppos) != 0)
   {
      ppos++;
      lppos = ppos;
      if((*ppos) == 0)
      {
        NextLine();
      }
   }
 }

 char * Takeline(void)
 {   
   return(line);
 }

 char *BufName()
 {
    return(bufname);
 }

};


// The parser class with a stack of current parsings

class Parser
{
  stack<ParserData *> buffstack;
  ParserData* bufnow;
  char theword[1000];
  bool usestdout;
  string aline;
  int outx;
#ifdef USING_WINDOWS
  HANDLE cinput;
  CONSOLE_SCREEN_BUFFER_INFO sbi;
#endif
  InterpreterData * pidat;

public:

//  Parser(char * buf)
//  {
//     bufnow = new ParserData(buf);
//     usestdout = false;
//     aline.reserve(1000);
//  }

  Parser(InterpreterData * idp = NULL)
  {
     bufnow = NULL;
     usestdout = false;
     aline.reserve(1000);
     pidat = idp;
  }

  Parser(const Parser & pp)
  {
    buffstack = pp.buffstack;
    bufnow = pp.bufnow;
    usestdout = pp.usestdout;
    aline = pp.aline;
    pidat = pp.pidat;
  }

  ~Parser()
  {
    while(bufnow != NULL)
    {
      delete bufnow;
      bufnow = NULL;
      if( buffstack.empty() != true)
      {
        PopBuf();
      }
    }
  }

 Parser& operator = (const Parser & pp)
  {
    buffstack = pp.buffstack;
    bufnow = pp.bufnow;
    usestdout = pp.usestdout;
    aline = pp.aline;
    pidat = pp.pidat;
    return(*this);
  }

 void UseStdOut(void)
 {
     usestdout = true;
 }

 void NoStdOut(void)
 {
     usestdout = false;
 }

 bool IsStdOut(void)
 {
    return(usestdout);
 }

 void Prompt()
 {
    if(usestdout)
    {
      cout << endl;
      cout << "Forth: ";
#ifdef USING_WINDOWS
      GetConsoleScreenBufferInfo(cinput, &sbi);
      outx = sbi.dwCursorPosition.X;
#endif
    }
 }

 void NoPrompt()
 {
    if(usestdout)
    {
      cout << endl;
#ifdef USING_WINDOWS
      GetConsoleScreenBufferInfo(cinput, &sbi);
      outx = sbi.dwCursorPosition.X;
#endif
    }
 }

 char * NextWord(bool once = false)
 {
   if(bufnow == NULL)
   {
     *pidat->pIN() = 0;
     return(NULL);
   }
//   DebugComment("Entered nextword", bufnow->BufName());

   bufnow->ResetLinePos((*pidat->pIN()));
 
   while(bufnow->AdvancepWhite() == false)
   {
//      DebugComment("false from advance p white", bufnow->BufName());

      if(bufnow->NextLine() == false)
      {
//        DebugComment("false from Nextline", bufnow->BufName());

        if(once == false)
        {
           PopBuf();
           if(bufnow == NULL)
           {
               if(usestdout)
               {
            #ifdef USING_WINDOWS
                     cinput = GetStdHandle(STD_INPUT_HANDLE);
                     GetConsoleScreenBufferInfo(cinput, &sbi);
                     if(sbi.dwCursorPosition.X != outx)
                     {
                       Prompt();
                     }
            #endif
                }
             return(NULL);
           }
        }
        else
        { 
           PopBuf();
           if(bufnow == NULL)
           {
             return(NULL);
           }
           *pidat->pIN() = bufnow->LinePos();
           return(NULL);
        }
      }
     *pidat->pIN() = bufnow->LinePos();
   }

   bufnow ->AdvancelpWhite();

   if(bufnow->AdvancepBlack() == false)
   {
      bufnow->GetWord(theword);
      *pidat->pIN() = bufnow->LinePos();
      return(theword); 
   }
   bufnow->GetWord(theword);
   *pidat->pIN() = bufnow->LinePos();
    return(theword);
 }

 bool AllDigits(char *txt)
 {

    if(isdigit(txt[0]) == false && txt[0] != '-')
     {
       return(false);
     }
    else
    {
       if(txt[0] == '-' && strlen(txt) == 1)
       {
          return(false);
       }
    }

   for(int ii = 1;ii < strlen(txt);ii++)
   {
     if(isdigit(txt[ii]) == false)
     {
       return(false);
     }
   }
   return(true);
 }
 
 bool GetAny(string& cs, int once = false, char nn = '"')
 {
     if(bufnow == NULL)
     {
       return(false);
     }

     bufnow->ResetLinePos((*pidat->pIN()));
//     pidat->DebugTrace("Entered Parser::Getany"); 

     cs = " ..";

     if(bufnow->ToAny(nn) == false)
     {
//         pidat->DebugTrace(" Parser::Getany toAny ret == false"); 
         if(bufnow->GetAny(cs) == false)
         {
             *pidat->pIN() = bufnow->LinePos(); 
             return(false);
         }
         else
         {
             *pidat->pIN() = bufnow->LinePos(); 
             return(true);
         }
     }
     else
     {
//        pidat->DebugTrace(" Parser::Getany toAny ret == true"); 
        bufnow->GetAny(cs);
        *pidat->pIN() = bufnow->LinePos();
     }
   return(true);
 }

 char GetChar(void)
 {
   return(bufnow->GetChar());
 }

 void Advance(void)
 {
    bufnow->Advance();
    *pidat->pIN() = bufnow->LinePos();
 }
 
 bool NewLine(bool once)
 {
     if(bufnow == NULL)
     {
         return(false);
     }
     
     if(bufnow->NextLine() == false)
     {
//         pidat->DebugTrace("false from Nextline");
         if(once == false)
         {
             PopBuf();
             if(bufnow == NULL)
             {
                 if(usestdout)
                 {
#ifdef USING_WINDOWS
                     cinput = GetStdHandle(STD_INPUT_HANDLE);
                     GetConsoleScreenBufferInfo(cinput, &sbi);
                     if(sbi.dwCursorPosition.X != outx)
                     {
                         Prompt();
                     }
#endif
                 }
                 return(false);
             }
         }
         else
         { 
             PopBuf();
             *pidat->pIN() = bufnow->LinePos();
             return(true);
         }      
     }
     *pidat->pIN() = bufnow->LinePos();
     return(true);
 }

 void DebugComment(char *a, char * b)
 {
    char nv[80];
    
    sprintf(nv, "%s %s", a, b);
    pidat->DebugTrace(nv);
 }

 void PushBuf(char * buf, char * nm = NULL)
 {
   if(bufnow != NULL)
   {
      DebugComment("pushing ", bufnow->BufName());
      buffstack.push(bufnow);
   }
       bufnow = new ParserData(buf, pidat, nm);
       DebugComment("New buffer is", bufnow->BufName());
      *pidat->pIN() = bufnow->LinePos();
 }

 void PopBuf(void)
 {
   if(bufnow != NULL)
   {    
      delete bufnow;
      bufnow = NULL;
   }   
   if(buffstack.empty() == false)
   {
      bufnow = buffstack.top();
      DebugComment("pop buf now, new buffer is ", bufnow->BufName());
      buffstack.pop();
      *pidat->pIN() = bufnow->LinePos();
   }
 }

 char * Lastline()
 {
   if(bufnow)
   {
     return(bufnow->Takeline());
   }
   return(NULL);
 }
};



// top level class

class Language
{
  InterpreterData idat;
  Primitives aprim;
  bool busy;

public:
  Language()
  {
      idat.SetUpPrimitives(&aprim);
      idat.NewOutputFile(".\\InterpOut.txt");
// Start forth words -----------------------------

// defining words
      SimpleWord(&Primitives::L_colon, ":", idat, idat.ExecuteCompile);
      SimpleWord(&Primitives::L_semicolon, ";", idat, idat.ExecuteCompile);
      SimpleWord(&Primitives::L_constant, "constant", idat, 0l);
      SimpleWord(&Primitives::L_variable, "variable", idat, 0l);
      SimpleWord(&Primitives::L_create, "create", idat, 0l);
      SimpleWord(&Primitives::L_endcreate, ";create", idat, 0l);
      SimpleWord(&Primitives::L_allot, "allot", idat, 0l);
      SimpleWord(&Primitives::L_rundoes, "rundoes>", idat, idat.ExecuteCompile);
      SimpleWord(&Primitives::L_does, "does>", idat, idat.ExecuteCompile);
      SimpleWord(&Primitives::L_postpone, "postpone", idat, idat.ExecuteCompile);

// comment words
      SimpleWord(&Primitives::L_Comment, "(", idat, idat.ExecuteCompile);
      SimpleWord(&Primitives::L_CppComment, "//", idat, idat.ExecuteCompile);
      SimpleWord(&Primitives::L_ForthComment, "\\", idat, idat.ExecuteCompile);
// output words
      SimpleWord(&Primitives::L_quote, "\"", idat, idat.ExecuteCompile);
      SimpleWord(&Primitives::L_dotquote, ".\"", idat, idat.ExecuteCompile);
      SimpleWord(&Primitives::L_dot, ".", idat, 0l);
      SimpleWord(&Primitives::L_udot, "u.", idat, 0l);
      SimpleWord(&Primitives::L_ddot, "d.", idat, 0l);
      SimpleWord(&Primitives::L_dudot, "du.", idat, 0l);
      SimpleWord(&Primitives::L_emit, "emit", idat, 0l);
      SimpleWord(&Primitives::L_nop, "nop", idat, 0l);
      SimpleWord(&Primitives::L_CR, "cr", idat, 0l);
      SimpleWord(&Primitives::L_printstring, "printstring", idat, 0l);
      SimpleWord(&Primitives::L_type, "type", idat, 0l);
      SimpleWord(&Primitives::L_count, "count", idat, 0l);
      SimpleWord(&Primitives::L_word, "word", idat, 0l);

// control words
      SimpleWord(&Primitives::L_runif, "runif", idat, 0l);
      SimpleWord(&Primitives::L_if, "if", idat, idat.ExecuteCompile);
      SimpleWord(&Primitives::L_runelse, "runelse", idat, 0l);
      SimpleWord(&Primitives::L_else, "else", idat, idat.ExecuteCompile);
      SimpleWord(&Primitives::L_runthen, "runthen", idat, 0l);
      SimpleWord(&Primitives::L_then, "then", idat, idat.ExecuteCompile);

      SimpleWord(&Primitives::L_runbegin, "runbegin", idat, 0l);
      SimpleWord(&Primitives::L_begin, "begin", idat, idat.ExecuteCompile);
      SimpleWord(&Primitives::L_rununtil, "rununtil", idat, 0l);
      SimpleWord(&Primitives::L_until, "until", idat, idat.ExecuteCompile);

      SimpleWord(&Primitives::L_rundo, "rundo", idat, 0l);
      SimpleWord(&Primitives::L_do, "do", idat, idat.ExecuteCompile);
      SimpleWord(&Primitives::L_runloop, "runloop", idat, 0l);
      SimpleWord(&Primitives::L_loop, "loop", idat, idat.ExecuteCompile);
      SimpleWord(&Primitives::L_runiinloop, "runi", idat, 0l);
      SimpleWord(&Primitives::L_iinloop, "i", idat, idat.ExecuteCompile);
      SimpleWord(&Primitives::L_runjinloop, "runj", idat, 0l);
      SimpleWord(&Primitives::L_jinloop, "j", idat, idat.ExecuteCompile);
      SimpleWord(&Primitives::L_runleave, "runleave", idat, 0l);
      SimpleWord(&Primitives::L_leave, "leave", idat, idat.ExecuteCompile);
      SimpleWord(&Primitives::L_runplusloop, "+runloop", idat, 0l);
      SimpleWord(&Primitives::L_plusloop, "+loop", idat, idat.ExecuteCompile);
      SimpleWord(&Primitives::L_runquestiondo, "?rundo", idat, 0l);
      SimpleWord(&Primitives::L_questiondo, "?do", idat, idat.ExecuteCompile);
      SimpleWord(&Primitives::L_rununloop, "rununloop", idat, 0l);
      SimpleWord(&Primitives::L_unloop, "unloop", idat, idat.ExecuteCompile);

      SimpleWord(&Primitives::L_runwhile, "runwhile", idat,  0l);
      SimpleWord(&Primitives::L_while, "while", idat, idat.ExecuteCompile);
      SimpleWord(&Primitives::L_runrepeat, "runrepeat", idat, 0l);
      SimpleWord(&Primitives::L_repeat, "repeat", idat, idat.ExecuteCompile);
      SimpleWord(&Primitives::L_runagain, "runagain", idat, 0l);
      SimpleWord(&Primitives::L_again, "again", idat, idat.ExecuteCompile);

      SimpleWord(&Primitives::L_runcase, "runcase", idat, 0l);
      SimpleWord(&Primitives::L_case, "case", idat, idat.ExecuteCompile);
      SimpleWord(&Primitives::L_runof, "runof", idat, 0l);
      SimpleWord(&Primitives::L_of, "of", idat, idat.ExecuteCompile);
      SimpleWord(&Primitives::L_runendof, "runendof", idat,  0l);
      SimpleWord(&Primitives::L_endof, "endof", idat, idat.ExecuteCompile);
      SimpleWord(&Primitives::L_runendcase, "runendcase", idat, 0l);
      SimpleWord(&Primitives::L_endcase, "endcase", idat, idat.ExecuteCompile);
      
      SimpleWord(&Primitives::L_runexit, "runexit", idat, idat.ExecuteCompile);
      SimpleWord(&Primitives::L_exit, "exit", idat, idat.ExecuteCompile);
      SimpleWord(&Primitives::L_recurse, "recurse", idat, 0l);

      SimpleWord(&Primitives::L_quit, "quit", idat, 0l);

// stack words
      SimpleWord(&Primitives::L_stackdepth, "depth", idat, 0l);
      SimpleWord(&Primitives::L_drop, "drop", idat, 0l);
      SimpleWord(&Primitives::L_over, "over", idat, 0l);
      SimpleWord(&Primitives::L_swap, "swap", idat, 0l);
      SimpleWord(&Primitives::L_dup, "dup", idat, 0l);
      SimpleWord(&Primitives::L_questiondup, "?dup", idat, 0l);
      SimpleWord(&Primitives::L_rot, "rot", idat, 0l);
      SimpleWord(&Primitives::L_rpush, ">r", idat, 0l);
      SimpleWord(&Primitives::L_rpop, "r>", idat, 0l);
      SimpleWord(&Primitives::L_rfetch, "r@", idat, 0l);
      SimpleWord(&Primitives::L_nip, "nip", idat, 0l);
      SimpleWord(&Primitives::L_pick, "pick", idat, 0l);
      SimpleWord(&Primitives::L_tuck, "tuck", idat, 0l);
      SimpleWord(&Primitives::L_roll, "roll", idat, 0l);
      SimpleWord(&Primitives::L_reverserot, "-rot", idat, 0l);


      SimpleWord(&Primitives::L_twodup, "2dup", idat, 0l);
      SimpleWord(&Primitives::L_twodrop, "2drop", idat, 0l);
      SimpleWord(&Primitives::L_twoswap, "2swap", idat, 0l);
      SimpleWord(&Primitives::L_twoover, "2over", idat, 0l);

      SimpleWord(&Primitives::L_auxpop, "aux>", idat, 0l);
      SimpleWord(&Primitives::L_auxpush, ">aux", idat, 0l);
      SimpleWord(&Primitives::L_auxswap, "auxswap", idat, 0l);
      SimpleWord(&Primitives::L_auxover, "auxover", idat, 0l);
      SimpleWord(&Primitives::L_auxdrop, "auxdrop", idat, 0l);
      SimpleWord(&Primitives::L_auxrot, "auxrot", idat, 0l);
      SimpleWord(&Primitives::L_auxreverserot, "-auxrot", idat, 0l);
      SimpleWord(&Primitives::L_auxfetch, "aux@", idat, 0l);
      SimpleWord(&Primitives::L_auxnip, "auxnip", idat, 0l);
      SimpleWord(&Primitives::L_auxpick, "auxpick", idat, 0l);
      SimpleWord(&Primitives::L_auxtuck, "auxtuck", idat, 0l);
      SimpleWord(&Primitives::L_auxroll, "auxroll", idat, 0l);
      SimpleWord(&Primitives::L_auxtwodup, "aux2dup", idat, 0l);
      SimpleWord(&Primitives::L_auxtwodrop, "aux2drop", idat, 0l);

// address words
      SimpleWord(&Primitives::L_fetch, "@", idat, 0l);
      SimpleWord(&Primitives::L_store, "!", idat, 0l);
      SimpleWord(&Primitives::L_cstore, "c!", idat, 0l);
      SimpleWord(&Primitives::L_cfetch, "c@", idat, 0l);
      SimpleWord(&Primitives::L_wstore, "w!", idat, 0l);
      SimpleWord(&Primitives::L_wfetch, "w@", idat, 0l);
      SimpleWord(&Primitives::L_plusstore, "+!", idat, 0l);
      SimpleWord(&Primitives::L_twostore, "2!", idat, 0l);
      SimpleWord(&Primitives::L_twofetch, "2@", idat, 0l);

// arithmatic words
      SimpleWord(&Primitives::L_plus, "+", idat, 0l);
      SimpleWord(&Primitives::L_minus, "-", idat, 0l);
      SimpleWord(&Primitives::L_multiply, "*", idat, 0l);
      SimpleWord(&Primitives::L_starslash, "*/", idat, 0l);
      SimpleWord(&Primitives::L_starslashmod, "*/mod", idat, 0l);
      SimpleWord(&Primitives::L_divide, "/", idat, 0l);
      SimpleWord(&Primitives::L_oneplus, "1+", idat, 0l);
      SimpleWord(&Primitives::L_oneminus, "1-", idat, 0l);
      SimpleWord(&Primitives::L_twotimes, "2*", idat, 0l);
      SimpleWord(&Primitives::L_twoplus, "2+", idat, 0l);
      SimpleWord(&Primitives::L_twominus, "2-", idat, 0l);
      SimpleWord(&Primitives::L_twoslash, "2/", idat, 0l);
      SimpleWord(&Primitives::L_abs, "abs", idat, 0l);
      SimpleWord(&Primitives::L_and, "and", idat, 0l);
      SimpleWord(&Primitives::L_not, "not", idat, 0l);
      SimpleWord(&Primitives::L_or, "or", idat, 0l);
      SimpleWord(&Primitives::L_xor, "xor", idat, 0l);
      SimpleWord(&Primitives::L_rshift, "rshift", idat, 0l);
      SimpleWord(&Primitives::L_lshift, "lshift", idat, 0l);
      SimpleWord(&Primitives::L_invert, "invert", idat, 0l);
      SimpleWord(&Primitives::L_base, "base", idat, 0l);
      SimpleWord(&Primitives::L_hex, "hex", idat, 0l);
      SimpleWord(&Primitives::L_decimal, "decimal", idat, 0l);
      SimpleWord(&Primitives::L_octal, "octal", idat, 0l);

      SimpleWord(&Primitives::L_fmslashmod, "fm/mod", idat, 0l);
      SimpleWord(&Primitives::L_smslashrem, "sm/rem", idat, 0l);
      SimpleWord(&Primitives::L_sTod, "s>d", idat, 0l);
      SimpleWord(&Primitives::L_mtimes, "m*", idat, 0l);
      SimpleWord(&Primitives::L_umtimes, "um*", idat, 0l);
      SimpleWord(&Primitives::L_umslashmod, "um/mod", idat, 0l);
      SimpleWord(&Primitives::L_slashmod, "/mod", idat, 0l);
      SimpleWord(&Primitives::L_mod, "mod", idat, 0l);
      SimpleWord(&Primitives::L_greaternumber, ">number", idat, 0l);

      SimpleWord(&Primitives::L_dplus, "d+", idat, 0l);
      SimpleWord(&Primitives::L_dnegate, "dnegate", idat, 0l);
      SimpleWord(&Primitives::L_dzeroless, "d0<", idat, 0l);
      SimpleWord(&Primitives::L_dminus, "d-", idat, 0l);
      SimpleWord(&Primitives::L_dzeroequal, "d0=", idat, 0l);
      SimpleWord(&Primitives::L_dtwostar, "d2*", idat, 0l);
      SimpleWord(&Primitives::L_dtwoslash, "d2/", idat, 0l);
      SimpleWord(&Primitives::L_dmax, "dmax", idat, 0l);
      SimpleWord(&Primitives::L_dmin, "dmin", idat, 0l);

      SimpleWord(&Primitives::L_max, "max", idat, 0l);
      SimpleWord(&Primitives::L_min, "min", idat, 0l);
      SimpleWord(&Primitives::L_negate, "negate", idat, 0l);

      SimpleWord(&Primitives::L_hold, "hold", idat, 0l);
      SimpleWord(&Primitives::L_sign, "sign", idat, 0l);
      SimpleWord(&Primitives::L_pounds, "#s", idat, 0l);
      SimpleWord(&Primitives::L_pound, "#", idat, 0l);
      SimpleWord(&Primitives::L_lesspound, "<#", idat, 0l);
      SimpleWord(&Primitives::L_poundgreater, "#>", idat, 0l);

// test words
      SimpleWord(&Primitives::L_lessthan, "<", idat, 0l);
      SimpleWord(&Primitives::L_ulessthan, "u<", idat, 0l);
      SimpleWord(&Primitives::L_notequal, "<>", idat, 0l);
      SimpleWord(&Primitives::L_equal, "==", idat, 0l);
      SimpleWord(&Primitives::L_equal, "=", idat, 0l);           // note definition of ==
      SimpleWord(&Primitives::L_greaterthan, ">", idat, 0l);
      SimpleWord(&Primitives::L_zeroless, "0<", idat, 0l);
      SimpleWord(&Primitives::L_zeroequal, "0=", idat, 0l);
      SimpleWord(&Primitives::L_zerogreater, "0>", idat, 0l);
      SimpleWord(&Primitives::L_zeronot, "0<>", idat, 0l);
      SimpleWord(&Primitives::L_greaterthanequal, ">=", idat, 0l);
      SimpleWord(&Primitives::L_lessthanequal, "<=", idat, 0l);
      SimpleWord(&Primitives::L_within, "within", idat, 0l);

// varius string memory and array words
      SimpleWord(&Primitives::L_stringcopy, "str=", idat, 0l);
      SimpleWord(&Primitives::L_stringplus, "str+", idat, 0l);
      SimpleWord(&Primitives::L_stringequal, "str==", idat, 0l);
      SimpleWord(&Primitives::L_substring, "substr", idat, 0l);
      SimpleWord(&Primitives::L_stringvariable, "string", idat, 0l);
      SimpleWord(&Primitives::L_stringtocstr, "str>cstr", idat, 0l);
      SimpleWord(&Primitives::L_cstrtostring, "cstr>str", idat, 0l);
      SimpleWord(&Primitives::L_chararray, "char[]:", idat, 0l);
      SimpleWord(&Primitives::L_chararraylen, "char[]_len", idat, 0l);
      SimpleWord(&Primitives::L_deletechararray, "delchar[]", idat, 0l);
      SimpleWord(&Primitives::L_strcpy, "strcpy", idat, 0l);
      SimpleWord(&Primitives::L_strncpy, "strncpy", idat, 0l);
      SimpleWord(&Primitives::L_memcpy, "memcpy", idat, 0l);
      SimpleWord(&Primitives::L_memset, "memset", idat, 0l);
      SimpleWord(&Primitives::L_memsize, "msize", idat, 0l);
      SimpleWord(&Primitives::L_bl, "bl", idat, 0l);
      SimpleWord(&Primitives::L_squote, "s\"", idat, idat.ExecuteCompile);
      SimpleWord(&Primitives::L_cquote, "c\"", idat, idat.ExecuteCompile);
      SimpleWord(&Primitives::L_source, "source", idat, 0l);
      SimpleWord(&Primitives::L_space, "space", idat, 0l);
      SimpleWord(&Primitives::L_spaces, "spaces", idat, 0l);
      SimpleWord(&Primitives::L_greaterin, ">in", idat, 0l);
      SimpleWord(&Primitives::L_fill, "fill", idat, 0l);
      SimpleWord(&Primitives::L_move, "move", idat, 0l);

// debug words
      SimpleWord(&Primitives::L_traceon, "traceon", idat, 0l);
      SimpleWord(&Primitives::L_traceoff, "traceoff", idat, 0l);
      SimpleWord(&Primitives::L_addtrace, "addtrace", idat, 0l);
      SimpleWord(&Primitives::L_dot_s, ".s", idat, 0l);

// structure words
      SimpleWord(&Primitives::L_structure, "structure:", idat, 0l);
      SimpleWord(&Primitives::L_createinstance, "new-structure", idat, 0l);
      SimpleWord(&Primitives::L_Nchar, "+char", idat, 0l);
      SimpleWord(&Primitives::L_Nshort, "+short", idat, 0l);
      SimpleWord(&Primitives::L_Nlong, "+long", idat, 0l);
      SimpleWord(&Primitives::L_Narraylong, "+long[]", idat, 0l);
      SimpleWord(&Primitives::L_Narrayshort, "+short[]", idat, 0l);
      SimpleWord(&Primitives::L_Narrayobjects, "+object[]", idat, 0l);
      SimpleWord(&Primitives::L_endstructure, "structure;", idat, 0l);
// end structure words

// system words
      SimpleWord(&Primitives::L_ReadEntireFile, "ReadEntireFile", idat, 0l);
      SimpleWord(&Primitives::L_include, "include", idat, 0l);
      SimpleWord(&Primitives::L_included, "included", idat, 0l);
      SimpleWord(&Primitives::L_timeanddate, "time&date", idat, 0l);
      SimpleWord(&Primitives::L_find, "find", idat, 0l);

      SimpleWord(&Primitives::L_CreateFile, "create-file", idat, 0l);
      SimpleWord(&Primitives::L_OpenFile, "open-file", idat, 0l);
      SimpleWord(&Primitives::L_FileSize, "file-size", idat, 0l);
      SimpleWord(&Primitives::L_FilePosition, "file-position", idat, 0l);
      SimpleWord(&Primitives::L_ReadFile, "read-file", idat, 0l);
      SimpleWord(&Primitives::L_CloseFile, "close-file", idat, 0l);
      SimpleWord(&Primitives::L_ReadOnly, "r/o", idat, 0l);
      SimpleWord(&Primitives::L_ReadWrite, "r/w", idat, 0l);
      SimpleWord(&Primitives::L_WriteOnly, "w/o", idat, 0l);
      SimpleWord(&Primitives::L_WriteFile, "write-file", idat, 0l);
      SimpleWord(&Primitives::L_RepositionFile, "reposition-file", idat, 0l);
      SimpleWord(&Primitives::L_ShowFileError, "file-error", idat, 0l);

// vocabulary and compile control words
      SimpleWord(&Primitives::L_tic, "'", idat, 0l);
      SimpleWord(&Primitives::L_forget, "forget", idat, 0l);
      SimpleWord(&Primitives::L_execute, "execute", idat, 0l);
      SimpleWord(&Primitives::L_decompile, "decompile", idat, 0l);

      SimpleWord(&Primitives::L_immediate, "immediate", idat, idat.ExecuteCompile);
      SimpleWord(&Primitives::L_forth, "forth", idat, idat.ExecuteCompile);
      SimpleWord(&Primitives::L_vocabulary, "vocabulary", idat, idat.ExecuteCompile);
      SimpleWord(&Primitives::L_definitions, "definitions", idat, idat.ExecuteCompile);
      SimpleWord(&Primitives::L_literal, "literal", idat, idat.ExecuteCompile);
      SimpleWord(&Primitives::L_vocquery, "is-in-voc", idat, 0l);

      SimpleWord(&Primitives::L_leftbracket, "[", idat, idat.ExecuteCompile);
      SimpleWord(&Primitives::L_rightbracket, "]", idat, idat.ExecuteCompile);
      SimpleWord(&Primitives::L_bracketticbracket, "[']", idat, idat.ExecuteCompile);
      SimpleWord(&Primitives::L_release, "release", idat, 0l);

      SimpleWord(&Primitives::L_cell, "cell", idat, 0l);
      SimpleWord(&Primitives::L_cellplus, "cell+", idat, 0l);
      SimpleWord(&Primitives::L_cells, "cells", idat, 0l);
      SimpleWord(&Primitives::L_chars, "chars", idat, 0l);
      SimpleWord(&Primitives::L_charplus, "char+", idat, 0l);

      SimpleWord(&Primitives::L_true, "true", idat, 0l);
      SimpleWord(&Primitives::L_false, "false", idat, 0l);

// floating point support

      SimpleWord(&Primitives::L_ftimes, "f*", idat, 0l);
      SimpleWord(&Primitives::L_fplus, "f+", idat, 0l);
      SimpleWord(&Primitives::L_fminus, "f-", idat, 0l);
      SimpleWord(&Primitives::L_fdivide, "f/", idat, 0l);
      SimpleWord(&Primitives::L_dtof, "d>f", idat, 0l);
      SimpleWord(&Primitives::L_ftod, "f>d", idat, 0l);
      SimpleWord(&Primitives::L_fzeroless, "f0<", idat, 0l);
      SimpleWord(&Primitives::L_fzeroequal, "f0=", idat, 0l);
      SimpleWord(&Primitives::L_fless, "f<", idat, 0l);
      SimpleWord(&Primitives::L_flessequal, "f<=", idat, 0l);
      SimpleWord(&Primitives::L_fgreater, "f>", idat, 0l);
      SimpleWord(&Primitives::L_fgreaterequal, "f>=", idat, 0l);
      SimpleWord(&Primitives::L_fdrop, "fdrop", idat, 0l);
      SimpleWord(&Primitives::L_fdup, "fdup", idat, 0l);
      SimpleWord(&Primitives::L_fnip, "fnip", idat, 0l);
      SimpleWord(&Primitives::L_froll, "froll", idat, 0l);
      SimpleWord(&Primitives::L_fpick, "fpick", idat, 0l);
      SimpleWord(&Primitives::L_fover, "fover", idat, 0l);
      SimpleWord(&Primitives::L_frot, "frot", idat, 0l);
      SimpleWord(&Primitives::L_freverserot, "-frot", idat, 0l);
      SimpleWord(&Primitives::L_ftuck, "ftuck", idat, 0l);
      SimpleWord(&Primitives::L_fpower, "f**", idat, 0l);
      SimpleWord(&Primitives::L_fabs, "fabs", idat, 0l);
      SimpleWord(&Primitives::L_facos, "facos", idat, 0l);
      SimpleWord(&Primitives::L_fcosh, "fcosh", idat, 0l);
      SimpleWord(&Primitives::L_fcos, "fcos", idat, 0l);
      SimpleWord(&Primitives::L_fasin, "fasin", idat, 0l);
      SimpleWord(&Primitives::L_fsin, "fsin", idat, 0l);
      SimpleWord(&Primitives::L_fsincos, "fsincos", idat, 0l);
      SimpleWord(&Primitives::L_fsinh, "fsinh", idat, 0l);
      SimpleWord(&Primitives::L_fatan, "fatan", idat, 0l);
      SimpleWord(&Primitives::L_ftan, "ftan", idat, 0l);
      SimpleWord(&Primitives::L_ftanh, "ftanh", idat, 0l);
      SimpleWord(&Primitives::L_fatan2, "fatan2", idat, 0l);
      SimpleWord(&Primitives::L_fexp, "fexp", idat, 0l);
      SimpleWord(&Primitives::L_fln, "fln", idat, 0l);
      SimpleWord(&Primitives::L_flog, "flog", idat, 0l);
      SimpleWord(&Primitives::L_sqrt, "fsqrt", idat, 0l);
      SimpleWord(&Primitives::L_fdot, "f.", idat, 0l);
      SimpleWord(&Primitives::L_fmin, "fmin", idat, 0l);
      SimpleWord(&Primitives::L_fmax, "fmax", idat, 0l);
      SimpleWord(&Primitives::L_fnegate, "fnegate", idat, 0l);
// end floating point words

// class defining words

      SimpleWord(&Primitives::L_DashRightGreater, "->", idat, idat.ExecuteCompile);
      SimpleWord(&Primitives::L_DashDot, "-:", idat, idat.ExecuteCompile);
      SimpleWord(&Primitives::L_class, "class:", idat, idat.ExecuteCompile);
      SimpleWord(&Primitives::L_This, "this", idat, 0l);
      SimpleWord(&Primitives::L_MethodSemicolon, "m;", idat, idat.ExecuteCompile);
      SimpleWord(&Primitives::L_MethodColon, "m:", idat, idat.ExecuteCompile);
      SimpleWord(&Primitives::L_EnclosedByte, "[byte]", idat, idat.ExecuteCompile);
      SimpleWord(&Primitives::L_EnclosedBytes, "[bytes]", idat, idat.ExecuteCompile);
      SimpleWord(&Primitives::L_EnclosedWord, "[word]", idat, idat.ExecuteCompile);
      SimpleWord(&Primitives::L_EnclosedWords, "[words]", idat, idat.ExecuteCompile);
      SimpleWord(&Primitives::L_EnclosedDWord, "[dword]", idat, idat.ExecuteCompile);
      SimpleWord(&Primitives::L_EnclosedDWords, "[dwords]", idat, idat.ExecuteCompile);
      SimpleWord(&Primitives::L_EnclosedPointer, "[pointer]", idat, idat.ExecuteCompile);
      SimpleWord(&Primitives::L_EnclosedPointers, "[pointers]", idat, idat.ExecuteCompile);
      SimpleWord(&Primitives::L_EnclosedArrays, "[arrays]", idat, idat.ExecuteCompile);
      SimpleWord(&Primitives::L_EnclosedClass, "[class]", idat, idat.ExecuteCompile);
      SimpleWord(&Primitives::L_EnclosedClasses, "[classes]", idat, idat.ExecuteCompile);
      SimpleWord(&Primitives::L_endclass, "class;", idat, idat.ExecuteCompile);
      SimpleWord(&Primitives::L_publiccolon, "public:", idat, idat.ExecuteCompile);
      SimpleWord(&Primitives::L_privatecolon, "private:", idat, idat.ExecuteCompile);
      SimpleWord(&Primitives::L_protectedcolon, "protected:", idat, idat.ExecuteCompile);
      SimpleWord(&Primitives::L_DecompileClass, "(decompile)-class", idat, idat.ExecuteCompile);
      SimpleWord(&Primitives::L_ColonEqual, ":=", idat, idat.ExecuteCompile);
      SimpleWord(&Primitives::L_NewClass, "new", idat, 0l);
      SimpleWord(&Primitives::L_DeleteClass, "delete", idat, 0l);
      SimpleWord(&Primitives::L_state, "state", idat, 0l);
       
// end class defining words

      SimpleWord(&Primitives::L_bye, "bye", idat, 0l);

      SimpleWord(&Primitives::L_evaluate, "evaluate", idat, 0l);
      SimpleWord(&Primitives::L_bracketcharbracket, "[char]", idat, idat.ExecuteCompile);
      SimpleWord(&Primitives::L_char, "char", idat, 0l);

      idat.IDict()->SetMark();
      // set up initial return stack to nop
      string nnn = "nop";
      Word *ww = idat.IDict()->Address(nnn);
      RSData ar(ww);
      
      idat.PushR(ar);
      busy = false;
  }

 Language(const Language & lang)
 {
   idat = lang.idat;
   aprim = lang.aprim;
 }

 int Parse(char * txt, char *nm = NULL);
 int Parse(string & intxt, string & intxtname);

// Function name	: SimpleWord
// Description	    : Adds a primitive to the language
// RSData type		: void 
// Argument         : PVFN p 
// Argument         : char * name
// Argument         : InterpreterData & idat
// Argument         : unsigned long flg
 void SimpleWord(PVFN p, char * name, InterpreterData & idat, unsigned long flg)
 {
   string cs;
   cs = name;
   if(idat.IDict()->Exists(cs) != -1)
   {
     idat.ShowError(Errors::PRIMITIVE_EXISTS, cs);
     return;
   }
   Word * awrd = new Word();
   Element *el = new Element(p);
   awrd->NewName(cs);
   awrd->AddElement(*el);
   delete el;
   awrd->SetFlags(flg);
   idat.IDict()->Add(*awrd);
   delete awrd;
 }

 string & Out()
 {
   return(idat.Out());
 }

 void SetOut(const char *path)
 {
   idat.NewOutputFile(path);
 }

 void SetStdOut()
 {
   idat.UseStdOut();
 }

 void SetStringOut()
 {
   idat.NewOutputString();
   idat.UseStringOut();
 }

 stringstream &  GetStringOut()
 {
   return(idat.GetOutputString());
 }

 string & GetTraceFileName()
 {
   return(idat.GetTraceFileName());
 }

stringstream & GettraceStream()
{
  return(idat.GettraceStream());
}

void TraceToStringOn()
{
   idat.TraceToStringOn();
}

void TraceToStringOff()
{
   idat.TraceToStringOff();
}

 void Prompt(void)
 {
   idat.IParser()->Prompt();
 }

 bool Busy(void)
 {
    return(busy);
 }

// Function name	: = 
// Description	    : 
// Return type		: Language& operator 
// Argument         : const Language & lang
Language& operator = (const Language & lang)
 {
   idat = lang.idat;
   aprim = lang.aprim;
   return(*this);
 }

};



#endif //__CODEELEMENT_H__