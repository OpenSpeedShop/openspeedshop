// ApplicationGroupObject
class ApplicationGroupObject
{
 private:
  bool remote;
  CommandWindowID *RemoteFW;
  EXPID RemoteExp;
  std::string App_Name;
  Experiment   *App;

 public:
  ApplicationGroupObject ()
    { remote = false;
      RemoteFW = NULL;
      RemoteExp = 0;
      App = NULL;
    }
  ApplicationGroupObject (std::string name, Experiment  *Local_App)
    { *this = ApplicationGroupObject ();
      App_Name = name;
      App = Local_App;
    }
  ApplicationGroupObject (CommandWindowID *Rfw, EXPID Rexp)
    { *this = ApplicationGroupObject ();
      remote = true;
      RemoteFW = Rfw;
      RemoteExp = Rexp;
    }
  bool Application_Is_Remote() {return remote;}
  Experiment   *Application_App() {return App;}
  void Print(FILE *TFile) {
      fprintf(TFile," %s",App_Name.c_str());
    }
};

// ExperimentObject
// Note: ALL the instrumentation in the experiment's list is applied
//       to ALL of the executables in the experiment's list.
class ExperimentObject;
extern EXPID Experiment_Sequence_Number;
extern std::list<ExperimentObject *> ExperimentObject_list;

class ExperimentObject
{
 private:
  EXPID Exp_ID;
  std::list<ApplicationGroupObject *> ApplicationObjectList;
  std::list<Collector *> CollectorList;

 public:
  ExperimentObject ()
    { Exp_ID = ++Experiment_Sequence_Number;
      ExperimentObject_list.push_front(this);
    }
  ~ExperimentObject () {}
  void ExperimentObject_Merge_Application(std::list<ApplicationGroupObject *> App_List)
    {
      ApplicationObjectList.merge(App_List);
    }
  void ExperimentObject_Merge_Instrumentation(std::list<Collector *> Collector_List)
    {
      CollectorList.merge(Collector_List);
    }
  void ExperimentObject_Add_Application(ApplicationGroupObject *App)
    {
      ApplicationObjectList.push_front(App);
    }
  void ExperimentObject_Add_Instrumention(Collector *Inst)
    {
      CollectorList.push_front(Inst);
    }
  EXPID ExperimentObject_ID() {return Exp_ID;}
  void Print(FILE *TFile)
    { fprintf(TFile,"Experiment %lld:\n",ExperimentObject_ID());
      std::list<ApplicationGroupObject *>::iterator AppObji = ApplicationObjectList.begin();
      if (AppObji != ApplicationObjectList.end()) {
        fprintf(TFile,"  ");
        for (AppObji = ApplicationObjectList.begin(); AppObji != ApplicationObjectList.end(); AppObji++) {
          (*AppObji)->Print(TFile);
        }
        fprintf(TFile,"\n");
      }
      std::list<Collector *>::iterator  Ci = CollectorList.begin();
      if (Ci != CollectorList.end()) {
        fprintf(TFile," ");
        for (Ci = CollectorList.begin(); Ci != CollectorList.end(); Ci++) {
          fprintf(TFile," %s",(*Ci)->getMetadata().getShortName().c_str());
        }
        fprintf(TFile,"\n");
      }
    }
  void Dump(FILE *TFile)
    { std::list<ExperimentObject *>::reverse_iterator expi;
      for (expi = ExperimentObject_list.rbegin(); expi != ExperimentObject_list.rend(); expi++)
      {
        (*expi)->Print(TFile);
      }
    }
};

ExperimentObject *Find_Experiment_Object (EXPID ExperimentID);
bool SS_expAttach   	(CommandObject *cmd);
bool SS_expCreate   	(CommandObject *cmd);
bool SS_expFocus    	(CommandObject *cmd);
bool SS_Record	    	(CommandObject *cmd);
bool SS_no_value    	(CommandObject *cmd);
bool SS_string_value    (CommandObject *cmd);
bool SS_int_value   	(CommandObject *cmd);
