// Atomic.h

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef Atomich
#define Atomich

#include <afxsock.h> // MFC socket extensions
#include <Direct.h>
#include <locale.h>

/* If your PC is connected to the internet you can use Atomic Clocks to synchronise your PCs clock over the internet for free!
Just use:
  CAtomic().Synch();
*/

#ifndef Languageh
  static const CString sNoSockets    ("Windows sockets initialization failed.");
  static const CString sNoTimeServers("Couldn't connect to any Time Servers.");
  static const CString sCorrected    ("Corrected");
  static const CString sOut          ("Out by ");
  static const CString sDays         ("Days");
  static const CString sOn           ("on");
#endif

class CAtomic {
 CTimeSpan LocalTimeZone;
       int LocalDST;
      char LocalZoneSign;
public:
  CAtomic() {
    setlocale(LC_ALL, ""); /* Make sure times get printed in the local format!
Adelaide is +930 and timezone is -570
Sun, 11 Jun 2000 19:51:25 +0930
time_t time; // time_t==long
short timezone;
WORD millitm;
short dstflag;
The following lines only work if the environment variables have been set first:
#include <sys\timeb.h>
    _tzset();
    _timeb time;
    _ftime(&time);
*/
    TIME_ZONE_INFORMATION tzi;
    if(LocalDST=(GetTimeZoneInformation(&tzi)==TIME_ZONE_ID_DAYLIGHT)) tzi.Bias+=tzi.DaylightBias;
    if(tzi.Bias<0) {
      tzi.Bias=-tzi.Bias;
      LocalZoneSign='-';
    }else LocalZoneSign='+';
    int LocalZoneHour  =tzi.Bias/60;
    int LocalZoneMinute=tzi.Bias%60;
    LocalTimeZone=CTimeSpan(0,LocalZoneHour,LocalZoneMinute,0);
  }
  virtual ~CAtomic() {}
  CTimeSpan GetLocalTimeZone()   const {return LocalTimeZone;}
  int       GetLocalDST()        const {return LocalDST;}
  char      GetLocalZoneSign()   const {return LocalZoneSign;}
  char      GetInverseZoneSign() const {return LocalZoneSign=='+' ? '-' : '+';}

  CString Synch(bool DoSet=true) { // If it asserts in AfxSocketInit you're using statically linked multi-threaded MFC Version 6... You need VS Servicepack 3 or the fix from http://support.microsoft.com/support/kb/articles/Q193/1/01.asp
    if(!AfxSocketInit()) return sNoSockets; // Put here to allow multi-threaded use (You won't need it in your Application class)
    union{
      DWORD dw;
      BYTE  b[4];
    }Time;
    for(int Site=0;; ++Site) { // Sometimes a site will connect but not give the time!
      CSocket SNTPSocket;
      if(!SNTPSocket.Create()) return sNoSockets;
      switch(Site) { // Simplest "State Machine" for each SNTP site: (Search the Internet for "SNTP Site List" and use those closest to you)
        case 0: if(SNTPSocket.Connect("ntp2a.mcc.ac.uk"            , 37)) break; else continue; // Manchester, England: University of Manchester
        case 1: if(SNTPSocket.Connect("ntp2b.mcc.ac.uk"            , 37)) break; else continue; // Manchester, England: University of Manchester
        case 2: if(SNTPSocket.Connect("ntp2c.mcc.ac.uk"            , 37)) break; else continue; // Manchester, England: University of Manchester
        case 3: if(SNTPSocket.Connect("time.nist.gov"              , 37)) break; else continue; // 192.43.244.18   NCAR, Boulder, Colorado
        case 4: if(SNTPSocket.Connect("time-a.nist.gov"            , 37)) break; else continue; // 129.6.15.28     NIST, Gaithersburg, Maryland
        case 5: if(SNTPSocket.Connect("time-b.nist.gov"            , 37)) break; else continue; // 129.6.15.29     NIST, Gaithersburg, Maryland
        case 6: if(SNTPSocket.Connect("time-C.timefreq.bldrdoc.gov", 37)) break; else continue; // 132.163.4.103   NIST, Boulder, Colorado
        case 7: if(SNTPSocket.Connect("nist1.datum.com"            , 37)) break; else continue; // 63.149.208.50   Datum, San Jose, California
        case 8: if(SNTPSocket.Connect("india.Colorado.EDU"         , 37)) break; else continue; // 128.138.140.44  University of Colorado, Boulder (utcnist.colorado.edu)) Site=2;
        default: return sNoTimeServers;
      }
// TRACE("%u\r\n",Site); continue; // uncomment this line to get a debug list of working Sites. If it stalls, the site isn't replying.
      CSocketFile SNTPSocketFile(&SNTPSocket);
      CArchive SMTPArIn(&SNTPSocketFile,CArchive::load);
      try {SMTPArIn >> Time.b[3] >> Time.b[2] >> Time.b[1] >> Time.b[0];}
      catch(...) {continue;} // If the site didn't give the time, try the next site:
      break;
    }

    CTime LastTime(CTime::GetCurrentTime());

    Time.dw-=2208988800; // Adjust from 1900 based to CTime's 1970 based time (2,208,988,800 corresponds to 00:00  1 Jan 1970 GMT)

    TIME_ZONE_INFORMATION TZI;
    switch(GetTimeZoneInformation(&TZI)) { // UTC to Local Time
      case TIME_ZONE_ID_UNKNOWN : Time.dw+=60*(TZI.Bias); break;
      case TIME_ZONE_ID_STANDARD: Time.dw+=60*(TZI.Bias+TZI.StandardBias); break;
      case TIME_ZONE_ID_DAYLIGHT: Time.dw+=60*(TZI.Bias+TZI.DaylightBias); break;
      case TIME_ZONE_ID_INVALID : break;
    }
    SYSTEMTIME SystemTime;
    ((CTime)Time.dw).GetAsSystemTime(SystemTime);
    BOOL Done=FALSE;
    if(DoSet) Done=SetSystemTime(&SystemTime); // Done will be FALSE if user doesn't have clock-setting priviledges

    switch(GetTimeZoneInformation(&TZI)) { // UTC to Local Time
      case TIME_ZONE_ID_UNKNOWN : Time.dw-=60*(TZI.Bias); break;
      case TIME_ZONE_ID_STANDARD: Time.dw-=60*(TZI.Bias+TZI.StandardBias); break;
      case TIME_ZONE_ID_DAYLIGHT: Time.dw-=60*(TZI.Bias+TZI.DaylightBias); break;
      case TIME_ZONE_ID_INVALID : break;
    }
    return ((CTime)Time.dw-LastTime).Format((Done ? sCorrected : sOut)+": %D "+sDays+" %H:%M:%S")+((CTime)Time.dw).Format(" "+sOn+" %c");
  }
};

#endif // ndef Atomich
