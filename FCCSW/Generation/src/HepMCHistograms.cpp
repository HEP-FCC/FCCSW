#include "HepMCHistograms.h"

DECLARE_COMPONENT(HepMCHistograms)

HepMCHistograms::HepMCHistograms(const std::string& name, ISvcLocator* svcLoc) :
		GaudiAlgorithm(name, svcLoc), m_ths(nullptr), m_pt(nullptr), m_eta(
				nullptr)

{
	declareInput("hepmc", m_hepmchandle);
}

StatusCode HepMCHistograms::initialize() {

	if (GaudiAlgorithm::initialize().isFailure())
		return StatusCode::FAILURE;

	if (service("THistSvc", m_ths).isFailure()) {
		error() << "Couldn't get THistSvc" << endmsg;
		return StatusCode::FAILURE;
	}

	m_pt = new TH1F("GenPt", "Generated particles pT", 100, .1, 10);
	if (m_ths->regHist("/rec/GenPt", m_pt).isFailure()) {
		error() << "Couldn't register GenPt" << endmsg;
	}

	m_eta = new TH1F("GenEta", "Generated particles Pseudorapidity", 100, -10, 10);
	if (m_ths->regHist("/rec/GenEta", m_eta).isFailure()) {
		error() << "Couldn't register GenEta" << endmsg;
	}

	return StatusCode::SUCCESS;
}

StatusCode HepMCHistograms::execute() {

	auto evt = m_hepmchandle.get()->getEvent();

	info() << "Processing event with " << evt->particles_size() << " particles" << endmsg;

	for (auto it = evt->particles_begin(); it != evt->particles_end(); ++it) {
		auto particle = *it;

		m_eta->Fill(particle->momentum().eta());
		m_pt->Fill(particle->momentum().perp());
	}

	return StatusCode::SUCCESS;
}

StatusCode HepMCHistograms::finalize() {
	if (GaudiAlgorithm::finalize().isFailure())
		return StatusCode::FAILURE;

	return StatusCode::SUCCESS;
}
