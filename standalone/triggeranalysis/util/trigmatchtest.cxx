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
  TrigConf::xAODConfigTool configTool( "TrigConf::xAODConfigTool" );
  configTool.initialize().ignore();
  Trig::TrigDecisionTool trigDecTool("TrigDecTool");
  trigDecTool.initialize().ignore();

  Trig::MatchingTool trigMatchTool("TriggerMatchingTool");
  trigMatchTool.initialize().ignore();

  
  // Loop over a few events:
  const ::Long64_t entries = ( event.getEntries() > 10 ? 10 :
                               event.getEntries() );
  for( ::Long64_t entry = 0; entry < entries; ++entry ) {
    // Get the current entry:

    event.getEntry( entry );

    // Print some additional info for the first 10 events:
    ::Info( APP_NAME, "Processing entry %i", static_cast< int >( entry ) );

    bool passedHLT = trigDecTool.isPassed("HLT_.*");

    
    
  }
  // Return gracefully:
  return 0;
}
