//$Id: xAODTrigConfReadTest.cxx 612011 2014-08-15 13:43:57Z krasznaa $
// ROOT include(s):
#ifdef ROOTCORE
#   include <TChain.h>
#   include <TError.h>
#endif // ROOTCORE
// Infrastructure include(s):
#ifdef ROOTCORE
#   include "xAODRootAccess/Init.h"
#   include "xAODRootAccess/TEvent.h"
#   include "xAODRootAccess/tools/ReturnCheck.h"
#endif // ROOTCORE

// Local include(s):
#include "AsgTools/ToolHandle.h"
#include "AsgTools/SgTEvent.h"
#include "TrigConfHLTData/HLTTriggerElement.h"

#include "TrigConfxAOD/xAODConfigTool.h"
#include "xAODTrigger/TrigNavigation.h"

#include "TrigDecisionTool/TrigDecisionTool.h"
#include "TrigDecisionTool/FeatureContainer.h"
#include "TrigDecisionTool/Feature.h"

#include "TriggerMatchingTool/MatchingTool.h"

#include "TriggerMatchingTool/MatchConfig.h"

#include "xAODTrigMissingET/TrigMissingETContainer.h"
#include "xAODBTagging/BTaggingContainer.h"

#include "xAODEgamma/ElectronContainer.h"

#include "xAODMuon/MuonContainer.h"

void singleElectronTriggerAnalysis(asg::SgTEvent& event, Trig::TrigDecisionTool& tdt, Trig::IMatchingTool& matchtool){
  if(! tdt.isPassed("HLT_e14_tight")){
    std::cout << "single electron trigger did not pass.. " << std::endl;
    return;
  }



  std::cout << "single el passed" << std::endl;

  const xAOD::ElectronContainer* els = 0;
  event.retrieve(els,"Electrons");
  ::Info("singleElectronTrigger", "There are %i electrons in the event", els->size());
  
  int i = 0;
  for(auto x : *els){
    ::Info("singleElectronTrigger", "matching a reco electron...");


    std::vector<const xAOD::IParticle*> tomatch = {x};
    auto matchresult = matchtool.match_result(tomatch,"HLT_e14_tight");
    std::cout << "from result object: " << matchresult->isMatched() << std::endl;
    if(matchresult->isMatched()){
      auto matchobjs = matchresult->bestMatchedObjects();
      
      
      std::cout << "reco object is: " << x->eta() << " / " << x->phi() << std::endl; 
      std::cout << "trigger object is: " << matchobjs.at(0)->eta() << " / " << matchobjs.at(0)->phi() << std::endl; 
      
    }
    i++;
  }
  ::Info("singleElectronTrigger", "done");
}

void electronMuonTriggerAnalysis(asg::SgTEvent& event, Trig::TrigDecisionTool& tdt, Trig::IMatchingTool& matchtool){
  std::string chain_name("HLT_e17_lhloose_mu");
  if(! tdt.isPassed(chain_name)){
    std::cout << "mixed trigger did not pass.. " << std::endl;
    return;
  }  
  std::cout << "mixed passed" << std::endl;
  
  const xAOD::ElectronContainer* els = 0;
  event.retrieve(els,"Electrons");

  const xAOD::MuonContainer* mus = 0;
  event.retrieve(mus,"Muons");

  std::cout << "there are " << els->size() << " electrons and " << mus->size() << " muons" << std::endl;
  
}

void subsetMatching(asg::SgTEvent& event, Trig::TrigDecisionTool& tdt, Trig::IMatchingTool& matchtool){
  std::string chain_name("HLT_2e17_loose");
  if(! tdt.isPassed(chain_name)){
    std::cout << "subset.... di-electron trigger did not pass.. " << std::endl;
    return;
  }  

  const xAOD::ElectronContainer* els = 0;
  event.retrieve(els,"Electrons");
  
  for(int i = 0;i < els->size(); ++i){
    std::vector<const xAOD::IParticle*> tomatch = {els->at(i)};
    auto matchresult = matchtool.match_result(tomatch,chain_name);
    
    if(matchresult->isMatched()){
      auto matchobjs = matchresult->bestMatchedObjects();
      
      for(int k = 0; k < tomatch.size(); ++k ){
	std::cout << "subset reco object is: " << tomatch.at(k)->eta() << " / " << tomatch.at(k)->phi() << std::endl; 
	std::cout << "subset trigger object is: " << matchobjs.at(k)->eta() << " / " << matchobjs.at(k)->phi() << std::endl; 
	
      }//all reco objs
    }//if matched
  }//for electron
}

void compareObjLists(std::vector<const xAOD::IParticle*>& recos, std::vector<const xAOD::IParticle*>& trigs){
  for (int k = 0; k < recos.size(); ++k){
    std::cout << "reco object is: " << recos.at(k)->eta() << " / " << recos.at(k)->phi() << std::endl; 
    std::cout << "trigger object is: " << trigs.at(k)->eta() << " / " << trigs.at(k)->phi() << std::endl; 
  }
}

void diLeptonTriggerAnalysis(asg::SgTEvent& event, Trig::TrigDecisionTool& tdt, Trig::IMatchingTool& matchtool){
  std::string chain_name("HLT_2e17_loose");
  if(! tdt.isPassed(chain_name)){
    std::cout << "di-electron trigger did not pass.. " << std::endl;
    return;
  }  
  std::cout << "di el passed" << std::endl;

  const xAOD::ElectronContainer* els = 0;
  event.retrieve(els,"Electrons");
  ::Info("diLeptonTriggerAnalysis", "There are %i electrons in the event", els->size());

  for(int i = 0;i < els->size(); ++i){
    for(int j = i+1; j < els->size(); ++j){
      std::cout << "contructing pair from indices: i: " << i << " j: " << j << std::endl;


      {
	std::vector<const xAOD::IParticle*> tomatch = {els->at(i), els->at(j)};

	auto matchbool = matchtool.match(tomatch,chain_name,{{xAOD::Type::Electron, 0.9}});
	  
	auto matchresult = matchtool.match_result(tomatch,chain_name);
	std::cout << "from result object: " << matchresult->isMatched() << std::endl;
	if(matchresult->isMatched()){
	  auto matchobjs = matchresult->bestMatchedObjects();
	  compareObjLists(tomatch,matchobjs);
	}
      }

      //try other order:

      {
	std::cout << "other order" << std::endl;
	std::vector<const xAOD::IParticle*> tomatch = {els->at(j), els->at(i)};
	auto matchresult = matchtool.match_result(tomatch,chain_name);
	std::cout << "from result object: " << matchresult->isMatched() << std::endl;
	if(matchresult->isMatched()){
	  auto matchobjs = matchresult->bestMatchedObjects();
	  compareObjLists(tomatch,matchobjs);
	}
	std::cout << "other order done" << std::endl;
      }
      
    }
  }
}

int main( int argc, char* argv[] ) {

   // Get the application's name:
  const char* APP_NAME = "Standalone_TDT";//argv[ 0 ]; //just give it a name....
  // Check that we received at least one file name:
  if( argc < 2 ) {
    ::Error( APP_NAME, "Usage: %s <xAOD file1> [xAOD file2]...", APP_NAME );
    return 1;
  }
 
  // Initialise the application:
  RETURN_CHECK( APP_NAME, xAOD::Init( APP_NAME ) );
 
  // Set up a TChain that will be used to read in the files:
  ::TChain chain( "CollectionTree" );
  for( int i = 1; i < argc; ++i ) {
    chain.Add( argv[ i ] );
  }

  // Create a TEvent object:
  xAOD::TEvent event( xAOD::TEvent::kClassAccess );
  RETURN_CHECK( APP_NAME, event.readFrom( &chain ) );

  asg::SgTEvent sgtevent(&event);

  // Create the trigger configuration tool:
  TrigConf::xAODConfigTool configTool( "TrigConf::xAODConfigTool" );
  configTool.initialize().ignore();
  Trig::TrigDecisionTool trigDecTool("TrigDecisionTool");
  trigDecTool.initialize().ignore();
  
  Trig::MatchingTool trigMatchTool("TriggerMatchingTool");
  trigMatchTool.initialize().ignore();
  trigMatchTool.setProperty("OutputLevel",MSG::DEBUG);

  
  // Loop over a few events:
  const ::Long64_t entries = ( event.getEntries() > 1000 ? 1000 :
                               event.getEntries() );
  for( ::Long64_t entry = 0; entry < entries; ++entry ) {
    // Get the current entry:
    event.getEntry( entry );
    ::Info( APP_NAME, "Processing entry %i", static_cast< int >( entry ) );

    //singleElectronTriggerAnalysis(sgtevent,trigDecTool,trigMatchTool);
    diLeptonTriggerAnalysis(sgtevent,trigDecTool,trigMatchTool);
    electronMuonTriggerAnalysis(sgtevent,trigDecTool,trigMatchTool);
    //subsetMatching(sgtevent,trigDecTool,trigMatchTool);
  }
  // Return gracefully:
  return 0;
}
