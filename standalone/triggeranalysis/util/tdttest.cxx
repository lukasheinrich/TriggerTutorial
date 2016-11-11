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

#include "xAODTrigMissingET/TrigMissingETContainer.h"
#include "xAODBTagging/BTaggingContainer.h"

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
  TrigConf::xAODConfigTool configTool( "xAODConfigTool" );

  ToolHandle<TrigConf::ITrigConfigTool> handle(&configTool);
  handle->initialize().ignore();
   
  Trig::TrigDecisionTool trigDecTool("TrigDecTool");
  trigDecTool.setProperty("ConfigTool",handle).ignore();
  trigDecTool.initialize().ignore();


  //configTool.setProperty("OutputLevel", MSG::DEBUG).ignore();
  //trigDecTool.setProperty("TrigDecisionKey","xTrigDecision").ignore();
  //trigDecTool.setProperty("OutputLevel", MSG::DEBUG).ignore();


  // Loop over a few events:
  const ::Long64_t entries = ( event.getEntries() > 10 ? 10 :
                               event.getEntries() );
  for( ::Long64_t entry = 0; entry < entries; ++entry ) {
    // Get the current entry:
    event.getEntry( entry );
   
    // Print some additional info for the first 10 events:
    ::Info( APP_NAME, "Processing entry %i", static_cast< int >( entry ) );
   
    bool passedL1 = trigDecTool.isPassed("L1_.*");
    bool passedL2 = trigDecTool.isPassed("L2_.*");
    bool passedEF = trigDecTool.isPassed("EF_.*");
    bool passedHLT = trigDecTool.isPassed("HLT_.*");
    ::Info( APP_NAME, "overall result L1: %d, L2: %d, EF: %d, HLT: %d",passedL1,passedL2,passedEF,passedHLT);
    ::Info( APP_NAME, "passed L2_xe25  %d ",trigDecTool.isPassed("L2_xe25"));
    ::Info( APP_NAME, "passed  EF_tau29T_medium1_xe55_tclcw   %d ",trigDecTool.isPassed("EF_tau29T_medium1_xe55_tclcw"));
    ::Info( APP_NAME, "passed  EF_xe80  %d ",trigDecTool.isPassed("EF_xe80"));
    ::Info( APP_NAME, "passed  HLT_xe50  %d ",trigDecTool.isPassed("HLT_xe50"));
    ::Info( APP_NAME, "passed  HLT_e28_tight1_iloose  %d ",trigDecTool.isPassed("HLT_e28_tight1_iloose"));
    ::Info( APP_NAME, "passed  HLT_tau25_medium1_mvonly_L1TAU8  %d ",trigDecTool.isPassed("HLT_tau25_medium1_mvonly_L1TAU8"));

    auto all_hlt = trigDecTool.getChainGroup("HLT_.*");

    int npassed = 0;
    int nall = 0;
    for( auto chain : all_hlt->getListOfTriggers()){
      if(trigDecTool.isPassed(chain)){
	std::cout << chain << " is passed" << std::endl;
	npassed++;
      }
      nall++;
    }
    std::cout << "overall stats: " << npassed  << "/" << nall << " chains passed" << std::endl;

  
    auto fc = all_hlt->features();
    std::cout << "all HLT feature container with " << fc.getCombinations().size() << " combinations" << std::endl;

    std::vector<uint32_t> test_clids = {
      1087532415,
      1092961325,
      1105575213,
      1134620868,
      1142962358,
      1158124735,
      117682415,
      1177172564,
      1178459224,
      1201561309,
      1219821989,
      1244316195,
      1264356158,
      1271751257,
      1287425431,
      1299522495,
      1314529658,
      1329567956,
      1334834594,
      1501,
      178962869,
      188050889,
      226775784,
      242645981,
      27498249,
      2802,
      6263,
      6455,
      76717345,
      7994217,
      9327453,
      98849495
    };
    
    for( auto clid : test_clids){
      auto feats = fc.typelessGet(clid);
      std::cout << "number of features for clid " << clid << " is: " << feats.size() << std::endl;
    }


    auto metFeatures = fc.elementFeature<xAOD::TrigMissingETContainer>();
    std::cout << "size of metFeatures: " << metFeatures.size() << std::endl;
    for(auto& metfeat : metFeatures){
 	std::cout << metfeat.cptr()->ex() << std::endl;
    }

    auto bTagFeatures = fc.containerFeature<xAOD::BTaggingContainer>();
    std::cout << "size of bTagFeatures: " << bTagFeatures.size() << std::endl;


    std::string nav;
    trigDecTool.ExperimentalAndExpertMethods()->enable();
    auto navi = trigDecTool.ExperimentalAndExpertMethods()->getNavigation();
    navi->printASCIIArt(nav);
    std::cout << nav << std::endl;

  }
 
  // Return gracefully:
  return 0;
}
