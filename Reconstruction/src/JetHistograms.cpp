#include "JetHistograms.h"

DECLARE_COMPONENT(JetHistograms)

JetHistograms::JetHistograms(const std::string& name, ISvcLocator* svcLoc) :
		GaudiAlgorithm(name, svcLoc), m_ths(nullptr), m_E(nullptr), m_n(nullptr)

{
	declareInput("jets", m_jethandle);
}

StatusCode JetHistograms::initialize() {

	if (GaudiAlgorithm::initialize().isFailure())
		return StatusCode::FAILURE;

	if (service("THistSvc", m_ths).isFailure()) {
		error() << "Couldn't get THistSvc" << endmsg;
		return StatusCode::FAILURE;
	}

	m_E = new TH1F("JetE", "Jet Energy", 25, 1, 100);
	if (m_ths->regHist("/rec/JetE", m_E).isFailure()) {
		error() << "Couldn't register JetE" << endmsg;
	}

	m_n = new TH1F("JetN", "Number of Jets", 25, 0, 100);
	if (m_ths->regHist("/rec/JetN", m_n).isFailure()) {
		error() << "Couldn't register JetN" << endmsg;
	}

	return StatusCode::SUCCESS;
}

StatusCode JetHistograms::execute() {

	auto jets = m_jethandle.get()->getJets();

	info() << "Processing event with " << jets->size() << " jets" << endmsg;

	m_n->Fill(jets->size());

	for (const auto & jet : *jets) {
		m_E->Fill(jet.E());
	}

	return StatusCode::SUCCESS;
}

StatusCode JetHistograms::finalize() {
	if (GaudiAlgorithm::finalize().isFailure())
		return StatusCode::FAILURE;

	return StatusCode::SUCCESS;
}
