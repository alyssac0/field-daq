/********************************************************************\

Name:         gm2GalilFe.cxx
Created by:   Matteo Bartolini
Modified by:  Joe Grange, Ran Hong

Contents:     readout code to talk to Galil motion control

$Id$

\********************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include "midas.h"
#include "mcstd.h"
#include "gclib.h"
#include "gclibo.h"
#include <iostream>
#include <string>
#include <iomanip>
#include <vector>
#include <unistd.h>
#include <sys/timeb.h>
#include <fstream>
#include <sstream>
#include <thread>
#include <mutex>

#include "TrolleyInterface.h"

using namespace std;
using namespace TrolleyInterface;

/* make frontend functions callable from the C framework */
#ifdef __cplusplus
extern "C" {
#endif

  // i am defining some Galil libraries variables

  //----------------------------------------------------------
  /*-- Globals -------------------------------------------------------*/

  /* The frontend name (client name) as seen by other MIDAS clients   */
  char *frontend_name = "gm2TrolleyFe";
  /* The frontend file name, don't change it */
  char *frontend_file_name = __FILE__;

  /* frontend_loop is called periodically if this variable is TRUE    */
  BOOL frontend_call_loop = FALSE;

  /* a frontend status page is displayed with this frequency in ms */
  INT display_period = 3000;

  /* maximum event size produced by this frontend */
  INT max_event_size = 100000;

  /* maximum event size for fragmented events (EQ_FRAGMENTED) */
  INT max_event_size_frag = 5 * 1024 * 1024;

  /* buffer size to hold events */
  INT event_buffer_size = 100 * 10000;


  /*-- Function declarations -----------------------------------------*/
  INT frontend_init();
  INT frontend_exit();
  INT begin_of_run(INT run_number, char *error);
  INT end_of_run(INT run_number, char *error);
  INT pause_run(INT run_number, char *error);
  INT resume_run(INT run_number, char *error);
  INT frontend_loop();

  INT read_galil_event(char *pevent, INT off);
  INT read_trigger_event(char *pevent, INT off);

  INT poll_event(INT source, INT count, BOOL test);
  INT interrupt_configure(INT cmd, INT source, POINTER_T adr);


  /*-- Equipment list ------------------------------------------------*/


  EQUIPMENT equipment[] = {


    {"TrolleyInterface",                /* equipment name */
      {1, 0,                   /* event ID, trigger mask */
	"SYSTEM",               /* event buffer */
	EQ_POLLED,            /* equipment type */
	0,                      /* event source */
	"MIDAS",                /* format */
	TRUE,                   /* enabled */
	RO_RUNNING|   /* read when running and on odb */
	RO_ODB,
	100,                  /* poll every 0.1 sec */
	0,                      /* stop run after this event limit */
	0,                      /* number of sub events */
	60,                      /* log history, logged once per minute */
	"", "", "",},
      read_galil_event,       /* readout routine */
    },

    {""}
  };


#ifdef __cplusplus
}
#endif

HNDLE hDB, hkeyclient;

typedef struct TrlyNMRDataStruct{
  INT TimeStamp;
  INT TensionArray[2];
  INT VelocityArray[3];
  INT PositionArray[3];
  INT OutputVArray[3];
}TrlyNMRDataStruct;

typedef struct BarcodeDataStruct{
  double TimeStamp;
  double BarcodeArray[6];
}BarcodeDataStruct;

typedef struct TrlyNMRDataStructD{
  double TensionArray[2];
  double VelocityArray[3];
  double PositionArray[3];
  double OutputVArray[3];
}TrlyNMRDataStructD;

typedef struct BarcodeDataStructD{
  double TimeStamp;
  double BarcodeArray[6];
}BarcodeDataStructD;

vector<TrlyNMRDataStruct> TrlyNMRDataBuffer;
vector<BarcodeDataStruct> BarcodeDataBuffer;

thread read_thread;
mutex mlock;

void ReadFromDevice();
bool RunActive;
int ReadGroupSize = 17;

ofstream TrolleyOutFile;
ofstream TrolleyOutFileDirect;

/********************************************************************\
  Callback routines for system transitions

  These routines are called whenever a system transition like start/
  stop of a run occurs. The routines are called on the following
occations:

frontend_init:  When the frontend program is started. This routine
should initialize the hardware.

frontend_exit:  When the frontend program is shut down. Can be used
to releas any locked resources like memory, commu-
nications ports etc.

begin_of_run:   When a new run is started. Clear scalers, open
rungates, etc.

end_of_run:     Called on a request to stop a run. Can send
end-of-run event and close run gates.

pause_run:      When a run is paused. Should disable trigger events.

resume_run:     When a run is resumed. Should enable trigger events.
\********************************************************************/

/*-- Frontend Init -------------------------------------------------*/

INT frontend_init()
{ 
  //Connect to trolley interface
  int err = DeviceConnect("192.168.1.123");  

  if (err==0){
    //cout << "connection successful\n";
    cm_msg(MINFO,"init","Trolley Interface connection successful");
  }
  else {
    //   cout << "connection failed \n";
    cm_msg(MERROR,"init","Trolley Interface connection failed. Error code: %d",err);
  }

  return SUCCESS;
}

/*-- Frontend Exit -------------------------------------------------*/

INT frontend_exit()
{
  return SUCCESS;
}

/*-- Begin of Run --------------------------------------------------*/

INT begin_of_run(INT run_number, char *error)
{
  //Get run number
  INT RunNumber;
  INT RunNumber_size = sizeof(RunNumber);
  cm_get_experiment_database(&hDB, NULL);
  db_get_value(hDB,0,"/Runinfo/Run number",&RunNumber,&RunNumber_size,TID_INT, 0);
  char filename[1000];
  sprintf(filename,"/home/rhong/gm2/g2-field-team/field-daq/resources/TrolleyTextOut/TrolleyOutput%04d.txt",RunNumber);
  TrolleyOutFile.open(filename,ios::out);
  sprintf(filename,"/home/rhong/gm2/g2-field-team/field-daq/resources/TrolleyTextOut/DirectTrolleyOutput%04d.txt",RunNumber);
  TrolleyOutFileDirect.open(filename,ios::out);
  
  //Load script
/*  GReturn b = G_NO_ERROR;
  char ScriptName[500];
  INT ScriptName_size = sizeof(ScriptName);
  db_get_value(hDB,0,"/Equipment/Galil/Settings/CmdScript",ScriptName,&ScriptName_size,TID_STRING,0);
  string FullScriptName = string("/home/rhong/gm2/g2-field-team/field-daq/resources/GalilMotionScripts/")+string(ScriptName)+string(".dmc");
//  cout <<"Galil Script to load: " << FullScriptName<<endl;
  cm_msg(MINFO,"begin_of_run","Galil Script to load: %s",FullScriptName.c_str());
  */
//Get ReadGroupSize from odb
  INT ReadGroupSize_size = sizeof(ReadGroupSize);
  db_get_value(hDB,0,"/Equipment/Galil/Settings/ReadGroupSize",&ReadGroupSize,&ReadGroupSize_size,TID_INT, 0);
//  cout <<"ReadGroup size: " << ReadGroupSize<<endl;
  cm_msg(MINFO,"begin_of_run","ReadGroup size: %d",ReadGroupSize);

//  GProgramDownload(g,"",0); //to erase prevoius programs
  //dump the buffer
/*  int rc = GALIL_EXAMPLE_OK; //return code
  char buffer[5000000];
  rc = GMessage(g, buffer, sizeof(buffer));
  b=GProgramDownloadFile(g,FullScriptName.c_str(),0);
  GCmd(g, "XQ #TH1,0");*/
  RunActive=true;
  //Start thread
  read_thread = thread(ReadFromDevice);
  return SUCCESS;
}

/*-- End of Run ----------------------------------------------------*/

INT end_of_run(INT run_number, char *error)

{
  mlock.lock();
  RunActive=false;
  mlock.unlock();
//  cm_msg(MINFO,"end_of_run","Trying to join threads.");
  read_thread.join();
  cm_msg(MINFO,"end_of_run","All threads joined.");
  TrlyNMRDataBuffer.clear();
  TrolleyOutFile.close();
  TrolleyOutFileDirect.close();
  cm_msg(MINFO,"end_of_run","Data buffer is emptied.");
  return SUCCESS;
}

/*-- Pause Run -----------------------------------------------------*/

INT pause_run(INT run_number, char *error)
{
  return SUCCESS;
}

/*-- Resuem Run ----------------------------------------------------*/

INT resume_run(INT run_number, char *error)
{ 
  return SUCCESS;
}

/*-- Frontend Loop -------------------------------------------------*/

INT frontend_loop()
{
  /* if frontend_call_loop is true, this routine gets called when
     the frontend is idle or once between every event */
  return SUCCESS;
}

/*------------------------------------------------------------------*/

/********************************************************************\

  Readout routines for different events

  \********************************************************************/

/*-- Trigger event routines ----------------------------------------*/

INT poll_event(INT source, INT count, BOOL test)
  /* Polling routine for events. Returns TRUE if event
     is available. If test equals TRUE, don't return. The test
     flag is used to time the polling */
{
  static unsigned int i;
  if (test) {
    for (i = 0; i < count; i++) {
      usleep(10);
    }
    return 0;
  }

  mlock.lock();
  bool check = (TrlyNMRDataBuffer.size()>ReadGroupSize) && (BarcodeDataBuffer.size()>ReadGroupSize);
//  cout <<"poll "<<GalilDataBuffer.size()<<" "<<int(check)<<endl;
  mlock.unlock();
  if (check)return 1;
  else return 0;
}

/*-- Interrupt configuration ---------------------------------------*/

INT interrupt_configure(INT cmd, INT source, POINTER_T adr)
{
  switch (cmd) {
    case CMD_INTERRUPT_ENABLE:
      break;
    case CMD_INTERRUPT_DISABLE:
      break;
    case CMD_INTERRUPT_ATTACH:
      break;
    case CMD_INTERRUPT_DETACH:
      break;
  }
  return SUCCESS;
}

/*-- Event readout -------------------------------------------------*/


INT read_galil_event(char *pevent, INT off){
  INT *pdata;
  double *pdatab;

  //Init bank
  bk_init32(pevent);

  //Write data to banks
  bk_create(pevent, "GALI", TID_DWORD, (void **)&pdata);
  
  mlock.lock();
  for (int i=0;i<ReadGroupSize;i++){
    *pdata++ = TrlyNMRDataBuffer[i].TimeStamp;
    for (int j=0;j<2;j++){
      *pdata++ = TrlyNMRDataBuffer[i].TensionArray[j];
    }
    for (int j=0;j<2;j++){
      *pdata++ = TrlyNMRDataBuffer[i].PositionArray[j];
    }
    for (int j=0;j<2;j++){
      *pdata++ = TrlyNMRDataBuffer[i].VelocityArray[j];
    }
    for (int j=0;j<2;j++){
      *pdata++ = TrlyNMRDataBuffer[i].OutputVArray[j];
    }
  }
  TrlyNMRDataBuffer.erase(TrlyNMRDataBuffer.begin(),TrlyNMRDataBuffer.begin()+ReadGroupSize);
  mlock.unlock();
  bk_close(pevent,pdata);

  //Write barcode data to banks
//  bk_create(pevent, "BARC", TID_DOUBLE, (void **)&pdatab);
  mlock.lock();
/*  for (int i=0;i<ReadGroupSize;i++){
    *pdatab++ = BarcodeDataBuffer[i].TimeStamp;
    for (int j=0;j<6;j++){
      *pdatab++ = BarcodeDataBuffer[i].BarcodeArray[j];
    }
  }*/
  BarcodeDataBuffer.erase(BarcodeDataBuffer.begin(),BarcodeDataBuffer.begin()+ReadGroupSize);
  mlock.unlock();
//  bk_close(pevent,pdatab);

  return bk_size(pevent);
}

//ReadFromDevice
void ReadFromDevice(){
  int ReadThreadActive = 1;
  db_set_value(hDB,0,"/Equipment/TrolleyInterface/Monitor/ReadThreadActive",&ReadThreadActive,sizeof(ReadThreadActive), 1 ,TID_BOOL); 
  char buffer[5000000];
  hkeyclient=0;
  string Device;
  int  rc = GALIL_EXAMPLE_OK; //return code
  /* residule string */
  string ResidualString = string("");

  int position =0;
//  timeb starttime,currenttime;
//  ftime(&starttime);
 
  //Readout loop
  int i=0;
  int jj=0;
  double Time,Time0;
  while (1){
    db_set_value(hDB,0,"/Equipment/TrolleyInterface/Monitor/ReadLoopIndex",&i,sizeof(i), 1 ,TID_INT); 
    bool localRunActive;
    mlock.lock();
    localRunActive = RunActive;
    mlock.unlock();
    if (!localRunActive)break;
    //Read Message to buffer
    mlock.lock();
    rc = GMessage(g, buffer, sizeof(buffer));
    mlock.unlock();
//    ftime(&currenttime);
//    double time = (currenttime.time-starttime.time)*1000 + (currenttime.millitm - starttime.millitm);
//    cout<<buffer<<endl;

//    GalilOutFileDirect<<buffer;
    string BufString = string(buffer);
    //Add the residual from last read
    if (ResidualString.size()!=0)BufString = ResidualString+BufString;
    ResidualString.clear();

    size_t foundnewline = BufString.find("\n");
    //  static  bool flag = false;

    int iTrly = 0;
    int iBarcode = 0;
    TrlyNMRDataStruct TrlyNMRDataUnit;
    TrlyNMRDataStructD TrlyNMRDataUnitD;
    BarcodeDataStruct BarcodeDataUnit;

    jj=0;
    while (foundnewline!=string::npos){
      stringstream iss (BufString.substr(0,foundnewline-1));
      // output returned by Galil is stored in the following variables

      iss >> Device;
      if(Device.compare("Trolley")==0){
	//iss >> GalilDataUnit.TimeStamp;
	iss >> Time;
	if (i==0 && jj==0)Time0=Time;
	Time-=Time0;
	for (int j=0;j<2;j++){
	  iss >> TrlyNMRDataUnitD.TensionArray[j];
	}
	for (int j=0;j<2;j++){
	  iss >> TrlyNMRDataUnitD.PositionArray[j];
	}
	for (int j=0;j<2;j++){
	  iss >> TrlyNMRDataUnitD.VelocityArray[j];
	}
	for (int j=0;j<2;j++){
	  iss >> TrlyNMRDataUnitD.OutputVArray[j];
	}
	//Convert to INT
	TrlyNMRDataUnit.TimeStamp = INT(Time);
	for (int j=0;j<2;j++){
	  TrlyNMRDataUnit.TensionArray[j] = INT(1000* TrlyNMRDataUnitD.TensionArray[j]);
	}
	for (int j=0;j<2;j++){
	  TrlyNMRDataUnit.PositionArray[j] = INT(TrlyNMRDataUnitD.PositionArray[j]);
	}
	for (int j=0;j<2;j++){
	  TrlyNMRDataUnit.VelocityArray[j] = INT(TrlyNMRDataUnitD.VelocityArray[j]);
	}
	for (int j=0;j<2;j++){
	  TrlyNMRDataUnit.OutputVArray[j] = INT(1000*TrlyNMRDataUnitD.OutputVArray[j]);
	}

	mlock.lock();
	TrlyNMRDataBuffer.push_back(TrlyNMRDataUnit);
	mlock.unlock();

	iTrly++;
	//Write to txt output
	TrolleyOutFile<<"Trolley "<<int(Time)<<" "<<TrlyNMRDataUnit.TensionArray[0]<<" "<<TrlyNMRDataUnit.TensionArray[1]<<" "<<TrlyNMRDataUnit.PositionArray[0]<<" "<<TrlyNMRDataUnit.PositionArray[1]<<" "<<TrlyNMRDataUnit.VelocityArray[0]<<" "<<TrlyNMRDataUnit.VelocityArray[1]<<" "<<TrlyNMRDataUnit.OutputVArray[0]<<" "<<TrlyNMRDataUnit.OutputVArray[1]<<endl;
      }else if(Device.compare("Barcode")==0){
	iss >> BarcodeDataUnit.TimeStamp;
	for (int j=0;j<6;j++){
	  iss >> BarcodeDataUnit.BarcodeArray[j];
	}
	mlock.lock();
	BarcodeDataBuffer.push_back(BarcodeDataUnit);
	mlock.unlock();
	iBarcode++;
	//Write to txt output
	TrolleyOutFile<<"Barcode "<<int(Time)<<" "<<BarcodeDataUnit.BarcodeArray[0]<<" "<<BarcodeDataUnit.BarcodeArray[1]<<" "<<BarcodeDataUnit.BarcodeArray[2]<<" "<<BarcodeDataUnit.BarcodeArray[3]<<" "<<BarcodeDataUnit.BarcodeArray[4]<<" "<<BarcodeDataUnit.BarcodeArray[5]<<endl;
      }

      BufString = BufString.substr(foundnewline+1,string::npos);
      foundnewline = BufString.find("\n");
      jj++;
    }
    /*db_set_value(hDB,0,"/Equipment/Galil/Monitor/MotorPositions",&GalilDataUnitD.PositionArray,sizeof(GalilDataUnitD.PositionArray), 3 ,TID_DOUBLE); 
    db_set_value(hDB,0,"/Equipment/Galil/Monitor/MotorVelocities",&GalilDataUnitD.VelocityArray,sizeof(GalilDataUnitD.VelocityArray), 3 ,TID_DOUBLE); 
    db_set_value(hDB,0,"/Equipment/Galil/Monitor/Tensions",&GalilDataUnitD.TensionArray,sizeof(GalilDataUnitD.TensionArray), 2 ,TID_DOUBLE); 
    */
    if (BufString.size()!=0){
      //  GalilOutFile << "Remaining string: ";
      ResidualString = BufString;
      //    cout <<ResidualString<<endl;
    }
    i++;
  }
  ReadThreadActive = 0;
  db_set_value(hDB,0,"/Equipment/TrolleyInterface/Monitor/ReadThreadActive",&ReadThreadActive,sizeof(ReadThreadActive), 1 ,TID_BOOL); 
}
