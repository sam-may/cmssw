#include "DQM/L1TMonitor/interface/L1TObjectsTiming.h"


L1TObjectsTiming::L1TObjectsTiming(const edm::ParameterSet& ps)
    : ugmtMuonToken_(consumes<l1t::MuonBxCollection>(ps.getParameter<edm::InputTag>("muonProducer"))),
      stage2CaloLayer2JetToken_(consumes<l1t::JetBxCollection>(ps.getParameter<edm::InputTag>("stage2CaloLayer2JetProducer"))),
      stage2CaloLayer2EGammaToken_(consumes<l1t::EGammaBxCollection>(ps.getParameter<edm::InputTag>("stage2CaloLayer2EGammaProducer"))),
      stage2CaloLayer2TauToken_(consumes<l1t::TauBxCollection>(ps.getParameter<edm::InputTag>("stage2CaloLayer2TauProducer"))),
      stage2CaloLayer2EtSumToken_(consumes<l1t::EtSumBxCollection>(ps.getParameter<edm::InputTag>("stage2CaloLayer2EtSumProducer"))),
      l1tStage2uGtProducer_(consumes<GlobalAlgBlkBxCollection>(ps.getParameter<edm::InputTag>("ugtProducer"))),
      monitorDir_(ps.getUntrackedParameter<std::string>("monitorDir")),
      verbose_(ps.getUntrackedParameter<bool>("verbose")),
      gtUtil_(new l1t::L1TGlobalUtil(ps, consumesCollector(), *this, ps.getParameter<edm::InputTag>("ugtProducer"), ps.getParameter<edm::InputTag>("ugtProducer"))),
      algoBitFirstBxInTrain_(-1),
      algoBitLastBxInTrain_(-1),
      algoNameFirstBxInTrain_(ps.getUntrackedParameter<std::string>("firstBXInTrainAlgo","")),
      algoNameLastBxInTrain_(ps.getUntrackedParameter<std::string>("lastBXInTrainAlgo","")),
      bxrange_(5) 
    
{

    muons_eta_phi.reserve(bxrange_);
    jet_eta_phi.reserve(bxrange_);
    egamma_eta_phi.reserve(bxrange_);
    tau_eta_phi.reserve(bxrange_);
    etsum_eta_phi_MET.reserve(bxrange_);
    etsum_eta_phi_METHF.reserve(bxrange_);
    etsum_eta_phi_MHT.reserve(bxrange_);
    etsum_eta_phi_MHTHF.reserve(bxrange_);
    muons_eta_phi_isolated.reserve(bxrange_);
    jet_eta_phi_isolated.reserve(bxrange_);
    egamma_eta_phi_isolated.reserve(bxrange_);
    tau_eta_phi_isolated.reserve(bxrange_);
    etsum_eta_phi_MET_isolated.reserve(bxrange_);
    etsum_eta_phi_METHF_isolated.reserve(bxrange_);
    etsum_eta_phi_MHT_isolated.reserve(bxrange_);
    etsum_eta_phi_MHTHF_isolated.reserve(bxrange_);
    muons_eta_phi_firstbunch.reserve(bxrange_);
    jet_eta_phi_firstbunch.reserve(bxrange_);
    egamma_eta_phi_firstbunch.reserve(bxrange_);
    tau_eta_phi_firstbunch.reserve(bxrange_);
    etsum_eta_phi_MET_firstbunch.reserve(bxrange_);
    etsum_eta_phi_METHF_firstbunch.reserve(bxrange_);
    etsum_eta_phi_MHT_firstbunch.reserve(bxrange_);
    etsum_eta_phi_MHTHF_firstbunch.reserve(bxrange_);
    muons_eta_phi_lastbunch.reserve(bxrange_);
    jet_eta_phi_lastbunch.reserve(bxrange_);
    egamma_eta_phi_lastbunch.reserve(bxrange_);
    tau_eta_phi_lastbunch.reserve(bxrange_);
    etsum_eta_phi_MET_lastbunch.reserve(bxrange_);
    etsum_eta_phi_METHF_lastbunch.reserve(bxrange_);
    etsum_eta_phi_MHT_lastbunch.reserve(bxrange_);
    etsum_eta_phi_MHTHF_lastbunch.reserve(bxrange_);
    
}

L1TObjectsTiming::~L1TObjectsTiming() {}

void L1TObjectsTiming::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
    edm::ParameterSetDescription desc;
    desc.add<edm::InputTag>("muonProducer")->setComment("L1T muons");;
    desc.add<edm::InputTag>("stage2CaloLayer2JetProducer")->setComment("L1T jets");
    desc.add<edm::InputTag>("stage2CaloLayer2EGammaProducer")->setComment("L1T egamma");
    desc.add<edm::InputTag>("stage2CaloLayer2TauProducer")->setComment("L1T taus");
    desc.add<edm::InputTag>("stage2CaloLayer2EtSumProducer")->setComment("L1T etsums");
    desc.add<edm::InputTag>("ugtProducer")->setComment("uGT output");
    desc.addUntracked<std::string>("monitorDir", "")->setComment("Target directory in the DQM file. Will be created if not existing.");
    desc.addUntracked<bool>("verbose", false);
    desc.addUntracked<std::string>("firstBXInTrainAlgo","")->setComment("Pick the right algo name for  L1 First Collision In Train");
    desc.addUntracked<std::string>("lastBXInTrainAlgo","")->setComment("Pick the right algo name for  L1 Last Collision In Train");
    descriptions.add("l1tObjectsTiming", desc);
}

void L1TObjectsTiming::dqmBeginRun(const edm::Run& r, const edm::EventSetup& c) {
   // Get the trigger menu information
    gtUtil_->retrieveL1Setup(c);
    // Get the algo bits needed for the timing histograms
    if (!gtUtil_->getAlgBitFromName(algoNameFirstBxInTrain_, algoBitFirstBxInTrain_)) {
      edm::LogWarning("L1TObjectsTiming") << "Algo \"" << algoNameFirstBxInTrain_ << "\" not found in the trigger menu " << gtUtil_->gtTriggerMenuName() << ". Could not retrieve algo bit number.";
    }
    if (!gtUtil_->getAlgBitFromName(algoNameLastBxInTrain_, algoBitLastBxInTrain_)) {
      edm::LogWarning("L1TObjectsTiming") << "Algo \"" << algoNameLastBxInTrain_ << "\" not found in the trigger menu " << gtUtil_->gtTriggerMenuName() << ". Could not retrieve algo bit number.";
    }


}

void L1TObjectsTiming::beginLuminosityBlock(const edm::LuminosityBlock&, const edm::EventSetup&) {}

void L1TObjectsTiming::bookHistograms(DQMStore::IBooker& ibooker, const edm::Run&, const edm::EventSetup&) {


  // Subsystem Monitoring and Muon Output
  std::string bx_obj[5]={"minus2","minus1","0","plus1","plus2"};

  ibooker.setCurrentFolder(monitorDir_+"/L1TMuon"+"/timing"); 
  for(unsigned int i=0; i<bxrange_; ++i) {
    muons_eta_phi.push_back(ibooker.book2D("muons_eta_phi_bx_"+bx_obj[i],"L1T Muon #eta vs #phi BX="+bx_obj[i],25, -2.5, 2.5, 25, -3.2, 3.2));
    muons_eta_phi.at(i)->setAxisTitle("#eta", 1);
    muons_eta_phi.at(i)->setAxisTitle("#phi", 2);
  }
  
  ibooker.setCurrentFolder(monitorDir_+"/L1TJet"+"/timing"); 
  for(unsigned int i=0; i<bxrange_; ++i) { 
    jet_eta_phi.push_back(ibooker.book2D("jet_eta_phi_bx_"+bx_obj[i],"L1T Jet #eta vs #phi BX="+bx_obj[i],25, -2.5, 2.5, 25, -3.2, 3.2));
    jet_eta_phi.at(i)->setAxisTitle("#eta", 1);
    jet_eta_phi.at(i)->setAxisTitle("#phi", 2);
  }
   
  ibooker.setCurrentFolder(monitorDir_+"/L1TEGamma"+"/timing"); 
  for(unsigned int i=0; i<bxrange_; ++i) {
    egamma_eta_phi.push_back(ibooker.book2D("egamma_eta_phi_bx_"+bx_obj[i],"L1T EGamma #eta vs #phi BX="+bx_obj[i],25, -2.5, 2.5, 25, -3.2, 3.2));
    egamma_eta_phi.at(i)->setAxisTitle("#eta", 1);
    egamma_eta_phi.at(i)->setAxisTitle("#phi", 2);
  }

  ibooker.setCurrentFolder(monitorDir_+"/L1TTau"+"/timing");
  for(unsigned int i=0; i<bxrange_; ++i) {
    tau_eta_phi.push_back(ibooker.book2D("tau_eta_phi_bx_"+bx_obj[i],"L1T Tau #eta vs #phi BX="+bx_obj[i],25, -2.5, 2.5, 25, -3.2, 3.2));
    tau_eta_phi.at(i)->setAxisTitle("#eta", 1);
    tau_eta_phi.at(i)->setAxisTitle("#phi", 2);
  }

  ibooker.setCurrentFolder(monitorDir_+"/L1TEtSum"+"/timing");
  for(unsigned int i=0; i<bxrange_; ++i) {
    etsum_eta_phi_MET.push_back(ibooker.book1D("etsum_phi_bx_MET_"+bx_obj[i],"L1T MET #phi BX="+bx_obj[i],25, -3.2, 3.2));
    etsum_eta_phi_MET.at(i)->setAxisTitle("#phi", 1);
    etsum_eta_phi_METHF.push_back(ibooker.book1D("etsum_phi_bx_METHF_"+bx_obj[i],"L1T METHF #phi BX="+bx_obj[i],25, -3.2, 3.2));
    etsum_eta_phi_METHF.at(i)->setAxisTitle("#phi", 1);
    etsum_eta_phi_MHT.push_back(ibooker.book1D("etsum_phi_bx_MHT_"+bx_obj[i],"L1T MHT #phi BX="+bx_obj[i],25, -3.2, 3.2));
    etsum_eta_phi_MHT.at(i)->setAxisTitle("#phi", 1);
    etsum_eta_phi_MHTHF.push_back(ibooker.book1D("etsum_phi_bx_MHTHF_"+bx_obj[i],"L1T MHTHF #phi BX="+bx_obj[i],25, -3.2, 3.2));
    etsum_eta_phi_MHTHF.at(i)->setAxisTitle("#phi", 1);
  }

if(algoBitFirstBxInTrain_ > -1 && algoBitLastBxInTrain_ > -1) {
  ibooker.setCurrentFolder(monitorDir_+"/L1TMuon"+"/timing"+"/Isolated_bunch");  
  for(unsigned int i=0; i<bxrange_; ++i) {
    muons_eta_phi_isolated.push_back(ibooker.book2D("muons_eta_phi_bx_isolated_"+bx_obj[i],"L1T Muon #eta vs #phi for isolated bunch BX="+bx_obj[i],25, -2.5, 2.5, 25, -3.2, 3.2));
    muons_eta_phi_isolated.at(i)->setAxisTitle("#eta", 1);
    muons_eta_phi_isolated.at(i)->setAxisTitle("#phi", 2);
  }
  
  ibooker.setCurrentFolder(monitorDir_+"/L1TJet"+"/timing"+"/Isolated_bunch");
  for(unsigned int i=0; i<bxrange_; ++i) {
    jet_eta_phi_isolated.push_back(ibooker.book2D("jet_eta_phi_bx_isolated_"+bx_obj[i],"L1T Jet #eta vs #phi for isolated bunch BX="+bx_obj[i],25, -2.5, 2.5, 25, -3.2, 3.2));
    jet_eta_phi_isolated.at(i)->setAxisTitle("#eta", 1);
    jet_eta_phi_isolated.at(i)->setAxisTitle("#phi", 2);
  }

  ibooker.setCurrentFolder(monitorDir_+"/L1TEGamma"+"/timing"+"/Isolated_bunch");
  for(unsigned int i=0; i<bxrange_; ++i) {
    egamma_eta_phi_isolated.push_back(ibooker.book2D("egamma_eta_phi_bx_isolated_"+bx_obj[i],"L1T EGamma #eta vs #phi for isolated bunch BX="+bx_obj[i],25, -2.5, 2.5, 25, -3.2, 3.2));
    egamma_eta_phi_isolated.at(i)->setAxisTitle("#eta", 1);
    egamma_eta_phi_isolated.at(i)->setAxisTitle("#phi", 2);
  }

  ibooker.setCurrentFolder(monitorDir_+"/L1TTau"+"/timing"+"/Isolated_bunch");
  for(unsigned int i=0; i<bxrange_; ++i) {
    tau_eta_phi_isolated.push_back(ibooker.book2D("tau_eta_phi_bx_isolated_"+bx_obj[i],"L1T Tau #eta vs #phi for isolated bunch BX="+bx_obj[i],25, -2.5, 2.5, 25, -3.2, 3.2));
    tau_eta_phi_isolated.at(i)->setAxisTitle("#eta", 1);
    tau_eta_phi_isolated.at(i)->setAxisTitle("#phi", 2);
  }

  ibooker.setCurrentFolder(monitorDir_+"/L1TEtSum"+"/timing"+"/Isolated_bunch");
  for(unsigned int i=0; i<bxrange_; ++i) {
    etsum_eta_phi_MET_isolated.push_back(ibooker.book1D("etsum_phi_bx_MET_isolated_"+bx_obj[i],"L1T MET #phi for isolated bunch BX="+bx_obj[i],25, -3.2, 3.2));
    etsum_eta_phi_MET_isolated.at(i)->setAxisTitle("#phi", 1);
    etsum_eta_phi_METHF_isolated.push_back(ibooker.book1D("etsum_phi_bx_METHF_isolated_"+bx_obj[i],"L1T METHF #phi for isolated bunch BX="+bx_obj[i],25, -3.2, 3.2));
    etsum_eta_phi_METHF_isolated.at(i)->setAxisTitle("#phi", 1);
    etsum_eta_phi_MHT_isolated.push_back(ibooker.book1D("etsum_phi_bx_MHT_isolated_"+bx_obj[i],"L1T MHT #phi for isolated bunch BX="+bx_obj[i],25, -3.2, 3.2));
    etsum_eta_phi_MHT_isolated.at(i)->setAxisTitle("#phi", 1);
    etsum_eta_phi_MHTHF_isolated.push_back(ibooker.book1D("etsum_phi_bx_MHTHF_isolated_"+bx_obj[i],"L1T MHTHF #phi for isolated bunch BX="+bx_obj[i],25, -3.2, 3.2));
    etsum_eta_phi_MHTHF_isolated.at(i)->setAxisTitle("#phi", 1);
  }
}
 
if(algoBitFirstBxInTrain_ > -1) {
  ibooker.setCurrentFolder(monitorDir_+"/L1TMuon"+"/timing"+"/First_bunch");
  for(unsigned int i=0; i<bxrange_-2; ++i) {
    muons_eta_phi_firstbunch.push_back(ibooker.book2D("muons_eta_phi_bx_firstbunch_"+bx_obj[i],"L1T Muon #eta vs #phi for first bunch BX="+bx_obj[i],25, -2.5, 2.5, 25, -3.2, 3.2));
    muons_eta_phi_firstbunch.at(i)->setAxisTitle("#eta", 1);
    muons_eta_phi_firstbunch.at(i)->setAxisTitle("#phi", 2);
  }
   
  ibooker.setCurrentFolder(monitorDir_+"/L1TJet"+"/timing"+"/First_bunch");
  for(unsigned int i=0; i<bxrange_-2; ++i) {
    jet_eta_phi_firstbunch.push_back(ibooker.book2D("jet_eta_phi_bx_firstbunch_"+bx_obj[i],"L1T Jet #eta vs #phi for first bunch BX="+bx_obj[i],25, -2.5, 2.5, 25, -3.2, 3.2));
    jet_eta_phi_firstbunch.at(i)->setAxisTitle("#eta", 1);
    jet_eta_phi_firstbunch.at(i)->setAxisTitle("#phi", 2);
  }
  
  ibooker.setCurrentFolder(monitorDir_+"/L1TEGamma"+"/timing"+"/First_bunch");
  for(unsigned int i=0; i<bxrange_-2; ++i) {
    egamma_eta_phi_firstbunch.push_back(ibooker.book2D("egamma_eta_phi_bx_firstbunch_"+bx_obj[i],"L1T EGamma #eta vs #phi for first bunch BX="+bx_obj[i],25, -2.5, 2.5, 25, -3.2, 3.2));
    egamma_eta_phi_firstbunch.at(i)->setAxisTitle("#eta", 1);
    egamma_eta_phi_firstbunch.at(i)->setAxisTitle("#phi", 2);
  }

  ibooker.setCurrentFolder(monitorDir_+"/L1TTau"+"/timing"+"/First_bunch");
  for(unsigned int i=0; i<bxrange_-2; ++i) {
    tau_eta_phi_firstbunch.push_back(ibooker.book2D("tau_eta_phi_bx_firstbunch_"+bx_obj[i],"L1T Tau #eta vs #phi for first bunch BX="+bx_obj[i],25, -2.5, 2.5, 25, -3.2, 3.2));
    tau_eta_phi_firstbunch.at(i)->setAxisTitle("#eta", 1);
    tau_eta_phi_firstbunch.at(i)->setAxisTitle("#phi", 2);
  }

  ibooker.setCurrentFolder(monitorDir_+"/L1TEtSum"+"/timing"+"/First_bunch");
  for(unsigned int i=0; i<bxrange_-2; ++i) {
    etsum_eta_phi_MET_firstbunch.push_back(ibooker.book1D("etsum_phi_bx_MET_firstbunch_"+bx_obj[i],"L1T MET #phi for firstbunch bunch BX="+bx_obj[i],25, -3.2, 3.2));
    etsum_eta_phi_MET_firstbunch.at(i)->setAxisTitle("#phi", 1);
    etsum_eta_phi_METHF_firstbunch.push_back(ibooker.book1D("etsum_phi_bx_METHF_firstbunch_"+bx_obj[i],"L1T METHF #phi for firstbunch bunch BX="+bx_obj[i],25, -3.2, 3.2));
    etsum_eta_phi_METHF_firstbunch.at(i)->setAxisTitle("#phi", 1);
    etsum_eta_phi_MHT_firstbunch.push_back(ibooker.book1D("etsum_phi_bx_MHT_firstbunch_"+bx_obj[i],"L1T MHT #phi for firstbunch bunch BX="+bx_obj[i],25, -3.2, 3.2));
    etsum_eta_phi_MHT_firstbunch.at(i)->setAxisTitle("#phi", 1);
    etsum_eta_phi_MHTHF_firstbunch.push_back(ibooker.book1D("etsum_phi_bx_MHTHF_firstbunch_"+bx_obj[i],"L1T MHTHF #phi for firstbunch bunch BX="+bx_obj[i],25, -3.2, 3.2));
    etsum_eta_phi_MHTHF_firstbunch.at(i)->setAxisTitle("#phi", 1);
  }
} 
  
if(algoBitLastBxInTrain_ > -1) {
  ibooker.setCurrentFolder(monitorDir_+"/L1TMuon"+"/timing"+"/Last_bunch");
  for(unsigned int i=0; i<bxrange_-2; ++i) {
    muons_eta_phi_lastbunch.push_back(ibooker.book2D("muons_eta_phi_bx_lastbunch_"+bx_obj[i+2],"L1T Muon #eta vs #phi for last bunch BX="+bx_obj[i+2],25, -2.5, 2.5, 25, -3.2, 3.2));
    muons_eta_phi_lastbunch.at(i)->setAxisTitle("#eta", 1);
    muons_eta_phi_lastbunch.at(i)->setAxisTitle("#phi", 2);
  }

  ibooker.setCurrentFolder(monitorDir_+"/L1TJet"+"/timing"+"/Last_bunch");
  for(unsigned int i=0; i<bxrange_-2; ++i) {
    jet_eta_phi_lastbunch.push_back(ibooker.book2D("jet_eta_phi_bx_lastbunch_"+bx_obj[i+2],"L1T Jet #eta vs #phi for last bunch BX="+bx_obj[i+2],25, -2.5, 2.5, 25, -3.2, 3.2));
    jet_eta_phi_lastbunch.at(i)->setAxisTitle("#eta", 1);
    jet_eta_phi_lastbunch.at(i)->setAxisTitle("#phi", 2);
  }

  ibooker.setCurrentFolder(monitorDir_+"/L1TEGamma"+"/timing"+"/Last_bunch");
  for(unsigned int i=0; i<bxrange_-2; ++i) {
    egamma_eta_phi_lastbunch.push_back(ibooker.book2D("egamma_eta_phi_bx_lastbunch_"+bx_obj[i+2],"L1T EGamma #eta vs #phi for last bunch BX="+bx_obj[i+2],25, -2.5, 2.5, 25, -3.2, 3.2));
    egamma_eta_phi_lastbunch.at(i)->setAxisTitle("#eta", 1);
    egamma_eta_phi_lastbunch.at(i)->setAxisTitle("#phi", 2);
  }

  ibooker.setCurrentFolder(monitorDir_+"/L1TTau"+"/timing"+"/Last_bunch");
  for(unsigned int i=0; i<bxrange_-2; ++i) {
    tau_eta_phi_lastbunch.push_back(ibooker.book2D("tau_eta_phi_bx_lastbunch_"+bx_obj[i+2],"L1T Tau #eta vs #phi for last bunch BX="+bx_obj[i+2],25, -2.5, 2.5, 25, -3.2, 3.2));
    tau_eta_phi_lastbunch.at(i)->setAxisTitle("#eta", 1);
    tau_eta_phi_lastbunch.at(i)->setAxisTitle("#phi", 2);
  }

  ibooker.setCurrentFolder(monitorDir_+"/L1TEtSum"+"/timing"+"/Last_bunch");
  for(unsigned int i=0; i<bxrange_-2; ++i) {
    etsum_eta_phi_MET_lastbunch.push_back(ibooker.book1D("etsum_phi_bx_MET_lastbunch_"+bx_obj[i+2],"L1T MET #phi for lastbunch bunch BX="+bx_obj[i+2],25, -3.2, 3.2));
    etsum_eta_phi_MET_lastbunch.at(i)->setAxisTitle("#phi", 1);
    etsum_eta_phi_METHF_lastbunch.push_back(ibooker.book1D("etsum_phi_bx_METHF_lastbunch_"+bx_obj[i+2],"L1T METHF #phi for lastbunch bunch BX="+bx_obj[i+2],25, -3.2, 3.2));
    etsum_eta_phi_METHF_lastbunch.at(i)->setAxisTitle("#phi", 1);
    etsum_eta_phi_MHT_lastbunch.push_back(ibooker.book1D("etsum_phi_bx_MHT_lastbunch_"+bx_obj[i+2],"L1T MHT #phi for lastbunch bunch BX="+bx_obj[i+2],25, -3.2, 3.2));
    etsum_eta_phi_MHT_lastbunch.at(i)->setAxisTitle("#phi", 1);
    etsum_eta_phi_MHTHF_lastbunch.push_back(ibooker.book1D("etsum_phi_bx_MHTHF_lastbunch_"+bx_obj[i+2],"L1T MHTHF #phi for lastbunch bunch BX="+bx_obj[i+2],25, -3.2, 3.2));
    etsum_eta_phi_MHTHF_lastbunch.at(i)->setAxisTitle("#phi", 1);
  }
} 

}

  

void L1TObjectsTiming::analyze(const edm::Event& e, const edm::EventSetup& c) {

  if (verbose_) edm::LogInfo("L1TObjectsTiming") << "L1TObjectsTiming: analyze..." << std::endl;


  // Muon Collection
  edm::Handle<l1t::MuonBxCollection> MuonBxCollection;
  e.getByToken(ugmtMuonToken_, MuonBxCollection);
  // Jet Collection
  edm::Handle<l1t::JetBxCollection> JetBxCollection;
  e.getByToken(stage2CaloLayer2JetToken_, JetBxCollection);
  // EGamma Collection
  edm::Handle<l1t::EGammaBxCollection> EGammaBxCollection;
  e.getByToken(stage2CaloLayer2EGammaToken_, EGammaBxCollection);
  // Tau Collection
  edm::Handle<l1t::TauBxCollection> TauBxCollection;
  e.getByToken(stage2CaloLayer2TauToken_, TauBxCollection);
  // EtSum Collection
  edm::Handle<l1t::EtSumBxCollection> EtSumBxCollection;
  e.getByToken(stage2CaloLayer2EtSumToken_, EtSumBxCollection);

  // Open uGT readout record
  edm::Handle<GlobalAlgBlkBxCollection> uGtAlgs;
  e.getByToken(l1tStage2uGtProducer_, uGtAlgs);
          

  // Filling eta-phi map for muons for BX=-2,-1,0,+1,+2
  for (int itBX = MuonBxCollection->getFirstBX(); itBX <= MuonBxCollection->getLastBX(); ++itBX) {
    for (l1t::MuonBxCollection::const_iterator Muon = MuonBxCollection->begin(itBX); Muon != MuonBxCollection->end(itBX); ++Muon) {
      int index = (int)itBX - std::min(0, 1 - (int)bxrange_%2 - (int)std::floor(bxrange_/2.)); // the correlation from itBX to respective index of the vector
      muons_eta_phi.at(index)->Fill(Muon->eta(), Muon->phi());
    }
  }
    
  // Filling eta-phi map for jets for BX=-2,-1,0,+1,+2
  for (int itBX = JetBxCollection->getFirstBX(); itBX <= JetBxCollection->getLastBX(); ++itBX) {
    for (l1t::JetBxCollection::const_iterator jet = JetBxCollection->begin(itBX); jet != JetBxCollection->end(itBX); ++jet) {
      int index = itBX - std::min(0, 1 - (int)bxrange_%2 - (int)std::floor(bxrange_/2.)); // the correlation from itBX to respective index of the vector
      jet_eta_phi.at(index)->Fill(jet->eta(), jet->phi());
    }
  }

  // Filling eta-phi map for egamma for BX=-2,-1,0,+1,+2
  for (int itBX = EGammaBxCollection->getFirstBX(); itBX <= EGammaBxCollection->getLastBX(); ++itBX) {
    for (l1t::EGammaBxCollection::const_iterator egamma = EGammaBxCollection->begin(itBX); egamma != EGammaBxCollection->end(itBX); ++egamma) {
      int index = itBX - std::min(0, 1 - (int)bxrange_%2 - (int)std::floor(bxrange_/2.)); // the correlation from itBX to respective index of the vector
      egamma_eta_phi.at(index)->Fill(egamma->eta(), egamma->phi());
    }
  }

  // Filling eta-phi map for tau for BX=-2,-1,0,+1,+2
  for (int itBX = TauBxCollection->getFirstBX(); itBX <= TauBxCollection->getLastBX(); ++itBX) {
    for (l1t::TauBxCollection::const_iterator tau = TauBxCollection->begin(itBX); tau != TauBxCollection->end(itBX); ++tau) {
      int index = itBX - std::min(0, 1 - (int)bxrange_%2 - (int)std::floor(bxrange_/2.)); // the correlation from itBX to respective index of the vector
      tau_eta_phi.at(index)->Fill(tau->eta(), tau->phi());
    }
  }

  // Filling eta-phi map for etsum for BX=-2,-1,0,+1,+2
  for (int itBX = EtSumBxCollection->getFirstBX(); itBX <= EtSumBxCollection->getLastBX(); ++itBX) {
    for (l1t::EtSumBxCollection::const_iterator EtSum = EtSumBxCollection->begin(itBX); EtSum != EtSumBxCollection->end(itBX); ++EtSum) {
      int index = itBX - std::min(0, 1 - (int)bxrange_%2 - (int)std::floor(bxrange_/2.)); // the correlation from itBX to respective index of the vector
      if (l1t::EtSum::EtSumType::kMissingEt == EtSum->getType()) {
        etsum_eta_phi_MET.at(index)->Fill(EtSum->phi());
      }
      else if (l1t::EtSum::EtSumType::kMissingEtHF == EtSum->getType()) {
        etsum_eta_phi_METHF.at(index)->Fill(EtSum->phi());
      }
      else if(l1t::EtSum::EtSumType::kMissingHt == EtSum->getType()) {
        etsum_eta_phi_MHT.at(index)->Fill(EtSum->phi());
      }
      else if(l1t::EtSum::EtSumType::kMissingHtHF == EtSum->getType()) {
        etsum_eta_phi_MHTHF.at(index)->Fill(EtSum->phi());
      }
    }
  }

  for(GlobalAlgBlkBxCollection::const_iterator itr = uGtAlgs->begin(0); itr != uGtAlgs->end(0); ++itr) {
    if((algoBitFirstBxInTrain_ != -1 && algoBitLastBxInTrain_ != -1) && (itr->getAlgoDecisionInitial(algoBitFirstBxInTrain_) && itr->getAlgoDecisionInitial(algoBitLastBxInTrain_))) {  
//    for(GlobalAlgBlkBxCollection::const_iterator itr = uGtAlgs->begin(0); itr != uGtAlgs->end(0); ++itr) 
      for (int itBX = MuonBxCollection->getFirstBX(); itBX <= MuonBxCollection->getLastBX(); ++itBX) {
        for (l1t::MuonBxCollection::const_iterator muon = MuonBxCollection->begin(itBX); muon != MuonBxCollection->end(itBX); ++muon) { // Starting with Muons
          int index = itBX - std::min(0, 1 - (int)bxrange_%2 - (int)std::floor(bxrange_/2.)); // the correlation from itBX to respective index of the vector
          muons_eta_phi_isolated.at(index)->Fill(muon->eta(),muon->phi());
        }
      }
      for (int itBX = JetBxCollection->getFirstBX(); itBX <= JetBxCollection->getLastBX(); ++itBX) {
        for (l1t::JetBxCollection::const_iterator jet = JetBxCollection->begin(itBX); jet != JetBxCollection->end(itBX); ++jet) {
          int index = itBX - std::min(0, 1 - (int)bxrange_%2 - (int)std::floor(bxrange_/2.)); // the correlation from itBX to respective index of the vector
          jet_eta_phi_isolated.at(index)->Fill(jet->eta(), jet->phi());
        }
      }
      for (int itBX = EGammaBxCollection->getFirstBX(); itBX <= EGammaBxCollection->getLastBX(); ++itBX) {
        for (l1t::EGammaBxCollection::const_iterator egamma = EGammaBxCollection->begin(itBX); egamma != EGammaBxCollection->end(itBX); ++egamma) {
          int index = itBX - std::min(0, 1 - (int)bxrange_%2 - (int)std::floor(bxrange_/2.)); // the correlation from itBX to respective index of the vector
          egamma_eta_phi_isolated.at(index)->Fill(egamma->eta(), egamma->phi());
        }
      }
      for (int itBX = TauBxCollection->getFirstBX(); itBX <= TauBxCollection->getLastBX(); ++itBX) {
        for (l1t::TauBxCollection::const_iterator tau = TauBxCollection->begin(itBX); tau != TauBxCollection->end(itBX); ++tau) {
          int index = itBX - std::min(0, 1 - (int)bxrange_%2 - (int)std::floor(bxrange_/2.)); // the correlation from itBX to respective index of the vector
          tau_eta_phi_isolated.at(index)->Fill(tau->eta(), tau->phi());
        }
      }
      for (int itBX = EtSumBxCollection->getFirstBX(); itBX <= EtSumBxCollection->getLastBX(); ++itBX) {
        for (l1t::EtSumBxCollection::const_iterator EtSum = EtSumBxCollection->begin(itBX); EtSum != EtSumBxCollection->end(itBX); ++EtSum) {
          int index = itBX - std::min(0, 1 - (int)bxrange_%2 - (int)std::floor(bxrange_/2.)); // the correlation from itBX to respective index of the vector
          if (l1t::EtSum::EtSumType::kMissingEt == EtSum->getType()) {
            etsum_eta_phi_MET_isolated.at(index)->Fill(EtSum->phi());
          }
          else if (l1t::EtSum::EtSumType::kMissingEtHF == EtSum->getType()) {
            etsum_eta_phi_METHF_isolated.at(index)->Fill(EtSum->phi());
          }
          else if(l1t::EtSum::EtSumType::kMissingHt == EtSum->getType()) {
            etsum_eta_phi_MHT_isolated.at(index)->Fill(EtSum->phi());
          }
          else if(l1t::EtSum::EtSumType::kMissingHtHF == EtSum->getType()) {
            etsum_eta_phi_MHTHF_isolated.at(index)->Fill(EtSum->phi());
          }
        }
      }
    }
      
    //if(itr->getAlgoDecisionInitial(algoBitFirstBxInTrain_))
    if(algoBitFirstBxInTrain_ != -1 && itr->getAlgoDecisionInitial(algoBitFirstBxInTrain_)) { // Filling eta-phi map for all objects for first bunch each BX
      for (int itBX = MuonBxCollection->getFirstBX(); itBX <= 0 ; ++itBX) {
        for (l1t::MuonBxCollection::const_iterator muon = MuonBxCollection->begin(itBX); muon != MuonBxCollection->end(itBX); ++muon) { // Starting with Muons
          int index = itBX - std::min(0, 1 - (int)bxrange_%2 - (int)std::floor(bxrange_/2.)); // the correlation from itBX to respective index of the vector 
          muons_eta_phi_firstbunch.at(index)->Fill(muon->eta(),muon->phi());
        }
      }
      for (int itBX = JetBxCollection->getFirstBX(); itBX <= 0; ++itBX) {
        for (l1t::JetBxCollection::const_iterator jet = JetBxCollection->begin(itBX); jet != JetBxCollection->end(itBX); ++jet) {
          int index = itBX - std::min(0, 1 - (int)bxrange_%2 - (int)std::floor(bxrange_/2.)); // the correlation from itBX to respective index of the vector
          jet_eta_phi_firstbunch.at(index)->Fill(jet->eta(), jet->phi());
        }
      }
      for (int itBX = EGammaBxCollection->getFirstBX(); itBX <= 0; ++itBX) {
        for (l1t::EGammaBxCollection::const_iterator egamma = EGammaBxCollection->begin(itBX); egamma != EGammaBxCollection->end(itBX); ++egamma) {
          int index = itBX - std::min(0, 1 - (int)bxrange_%2 - (int)std::floor(bxrange_/2.)); // the correlation from itBX to respective index of the vector
          egamma_eta_phi_firstbunch.at(index)->Fill(egamma->eta(), egamma->phi());
        }
      }
      for (int itBX = TauBxCollection->getFirstBX(); itBX <= 0; ++itBX) {
        for (l1t::TauBxCollection::const_iterator tau = TauBxCollection->begin(itBX); tau != TauBxCollection->end(itBX); ++tau) {
          int index = itBX - std::min(0, 1 - (int)bxrange_%2 - (int)std::floor(bxrange_/2.)); // the correlation from itBX to respective index of the vector
          tau_eta_phi_firstbunch.at(index)->Fill(tau->eta(), tau->phi());
        }
      }
      for (int itBX = EtSumBxCollection->getFirstBX(); itBX <= 0; ++itBX) {
        for (l1t::EtSumBxCollection::const_iterator EtSum = EtSumBxCollection->begin(itBX); EtSum != EtSumBxCollection->end(itBX); ++EtSum) {
          int index = itBX - std::min(0, 1 - (int)bxrange_%2 - (int)std::floor(bxrange_/2.)); // the correlation from itBX to respective index of the vector
          if (l1t::EtSum::EtSumType::kMissingEt == EtSum->getType()) {
            etsum_eta_phi_MET_firstbunch.at(index)->Fill(EtSum->phi());
          }
          else if (l1t::EtSum::EtSumType::kMissingEtHF == EtSum->getType()) {
            etsum_eta_phi_METHF_firstbunch.at(index)->Fill(EtSum->phi());
          }
          else if(l1t::EtSum::EtSumType::kMissingHt == EtSum->getType()) {
            etsum_eta_phi_MHT_firstbunch.at(index)->Fill(EtSum->phi());
          }
          else if(l1t::EtSum::EtSumType::kMissingHtHF == EtSum->getType()) {
            etsum_eta_phi_MHTHF_firstbunch.at(index)->Fill(EtSum->phi());
          }
        }
      } 
    }
    //if(itr->getAlgoDecisionInitial(algoBitLastBxInTrain_))
    if(algoBitLastBxInTrain_ != -1 && itr->getAlgoDecisionInitial(algoBitLastBxInTrain_)) { // Filling eta-phi map for all objects for last bunch each BX
      for (int itBX = 0; itBX <= MuonBxCollection->getLastBX(); ++itBX) {
        for (l1t::MuonBxCollection::const_iterator muon = MuonBxCollection->begin(itBX); muon != MuonBxCollection->end(itBX); ++muon) { // Starting with Muons
          int index = itBX - std::min(0, 1 - (int)bxrange_%2 + (int)std::floor(bxrange_/2.)); // the correlation from itBX to respective index of the vector
          muons_eta_phi_lastbunch.at(index)->Fill(muon->eta(),muon->phi());
        }
      }
      
      for (int itBX = 0; itBX <= JetBxCollection->getLastBX(); ++itBX) {
        for (l1t::JetBxCollection::const_iterator jet = JetBxCollection->begin(itBX); jet != JetBxCollection->end(itBX); ++jet) {
          int index = itBX - std::min(0, 1 - (int)bxrange_%2 + (int)std::floor(bxrange_/2.)); // the correlation from itBX to respective index of the vector
          jet_eta_phi_lastbunch.at(index)->Fill(jet->eta(), jet->phi());
        }
      }
      for (int itBX = 0; itBX <= EGammaBxCollection->getLastBX(); ++itBX) {
        for (l1t::EGammaBxCollection::const_iterator egamma = EGammaBxCollection->begin(itBX); egamma != EGammaBxCollection->end(itBX); ++egamma) {
          int index = itBX - std::min(0, 1 - (int)bxrange_%2 + (int)std::floor(bxrange_/2.)); // the correlation from itBX to respective index of the vector
          egamma_eta_phi_lastbunch.at(index)->Fill(egamma->eta(), egamma->phi());
        }
      }
      for (int itBX = 0; itBX <= TauBxCollection->getLastBX(); ++itBX) {
        for (l1t::TauBxCollection::const_iterator tau = TauBxCollection->begin(itBX); tau != TauBxCollection->end(itBX); ++tau) {
          int index = itBX - std::min(0, 1 - (int)bxrange_%2 + (int)std::floor(bxrange_/2.)); // the correlation from itBX to respective index of the vector
          tau_eta_phi_lastbunch.at(index)->Fill(tau->eta(), tau->phi());
        }
      }
      for (int itBX = 0; itBX <= EtSumBxCollection->getLastBX(); ++itBX) {
        for (l1t::EtSumBxCollection::const_iterator EtSum = EtSumBxCollection->begin(itBX); EtSum != EtSumBxCollection->end(itBX); ++EtSum) {
          int index = itBX - std::min(0, 1 - (int)bxrange_%2 + (int)std::floor(bxrange_/2.)); // the correlation from itBX to respective index of the vector
          if (l1t::EtSum::EtSumType::kMissingEt == EtSum->getType()) {
            etsum_eta_phi_MET_lastbunch.at(index)->Fill(EtSum->phi());
          }
          else if (l1t::EtSum::EtSumType::kMissingEtHF == EtSum->getType()) {
            etsum_eta_phi_METHF_lastbunch.at(index)->Fill(EtSum->phi());
          }
          else if(l1t::EtSum::EtSumType::kMissingHt == EtSum->getType()) {
            etsum_eta_phi_MHT_lastbunch.at(index)->Fill(EtSum->phi());
          }
          else if(l1t::EtSum::EtSumType::kMissingHtHF == EtSum->getType()) {
            etsum_eta_phi_MHTHF_lastbunch.at(index)->Fill(EtSum->phi());
          }
        }
      }
    } 
  }
}

