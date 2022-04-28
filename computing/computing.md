---
layout: site
id: computing
---

FCC Computing / Storage
================================================
CERN ressources are available to be used by people being employed by an institute that has signed the Memorandum of Understanding (MoU) and by laboratories having signed the Addendum. For more information on the procedure please have a look at <a href="https://fcc.web.cern.ch/Pages/Join.aspx/"> this page. </a> 

The computing and disk ressources are controlled by <a href="https://e-groups.cern.ch/e-groups/EgroupsSearchForm.do">e-groups</a> with admin approval.

**Those receiving approval engage in using these resorces ONLY for activities related to FCC**. 

## Computing resources

A quota of resources, dedicated to FCC computing activities, are available in the HTCondor-based CERN batch farm.
Use of these resources is controlled by the <a href="https://e-groups.cern.ch/e-groups/EgroupsSearchForm.do">CERN e-group</a>
**fcc-experiments-comp** and subject to admin approval.
The associated accounting group is **group_u_FCC**; to use resources from this quota, the following needs to be
added to the job sumbmit file:
```
+AccountingGroup = "group_u_FCC.local_gen"
```
## Storage resources

A quota of storage disk space, dedicated to FCC computing activities, is available in the EOS system at CERN under the path
```
/eos/experiment/fcc
```
Access to these resources is controlled by the <a href="https://e-groups.cern.ch/e-groups/EgroupsSearchForm.do">CERN e-group</a>
**fcc-eos-access** and subject to admin approval.

### Access to storage resources from outside CERN

Users enabled to access the FCC area on EOS can do so also from outside CERN using the Kerberos credentials associated with theirr
CERN user account. To do so, theyr have to make sure to have the Kerberos credential initiator application, `kinit`, installed
and configured to enable the `CERN.CH` Kerberos realm. Typically is enough to copy the relevant sections from the /eos/krb5.conf
file accessible on lxplus (or copying the complete file, if there is none locally or if the local one is not customized).

Once this is done, the following should work:
```
$ kinit ganis@CERN.CH
ganis@CERN.CH's password:
(Enter CERN lxplus password)

$ klist
Credentials cache: API:68FEEF0D-F635-45C6-91B3-1A7A6F83E0C6
        Principal: ganis@CERN.CH

  Issued                Expires               Principal
Dec  2 16:20:16 2020  Dec  3 17:20:10 2020  krbtgt/CERN.CH@CERN.CH
```
The files can be copied locally
```
 $ xrdcp root://eospublic.cern.ch//eos/experiment/fcc/ee/generation/DelphesEvents/fcc_v02/p8_ee_ZH_ecm240/events_199832436.root .
[80.67MB/80.67MB][100%][==================================================][16.13MB/s]
```
or accessed directly via ROOT
```
$ root -l
root [0] auto f = TFile::Open("root://eospublic.cern.ch//eos/experiment/fcc/ee/generation/DelphesEvents/fcc_v02/p8_ee_ZH_ecm240/events_199832436.root")
Warning in <TClass::Init>: no dictionary for class fcc::MCParticleData is available
Warning in <TClass::Init>: no dictionary for class fcc::BareParticle is available
Warning in <TClass::Init>: no dictionary for class fcc::Point is available
Warning in <TClass::Init>: no dictionary for class fcc::LorentzVector is available
Warning in <TClass::Init>: no dictionary for class podio::ObjectID is available
Warning in <TClass::Init>: no dictionary for class fcc::GenVertexData is available
Warning in <TClass::Init>: no dictionary for class fcc::FloatValueData is available
Warning in <TClass::Init>: no dictionary for class fcc::ParticleData is available
Warning in <TClass::Init>: no dictionary for class fcc::TrackStateData is available
Warning in <TClass::Init>: no dictionary for class fcc::TaggedParticleData is available
Warning in <TClass::Init>: no dictionary for class fcc::GenJetData is available
Warning in <TClass::Init>: no dictionary for class fcc::BareJet is available
Warning in <TClass::Init>: no dictionary for class fcc::TaggedGenJetData is available
Warning in <TClass::Init>: no dictionary for class fcc::JetData is available
Warning in <TClass::Init>: no dictionary for class fcc::TaggedJetData is available
Warning in <TClass::Init>: no dictionary for class fcc::METData is available
Warning in <TClass::Init>: no dictionary for class fcc::ParticleMCParticleAssociationData is available
Warning in <TClass::Init>: no dictionary for class podio::CollectionIDTable is available
(TFile *) @0x7ffee5b6b718
root [1]
```

Note the use of the `eospublic.cern.ch` server.
A cache system, for example based on `XCache`, can make the whole setup more efficient.

Another possibility is to mount the remote directory locally using the `sshfs` utility, for example
```
$ mkdir ~/eosfcc
sshfs -o idmap=user -o follow_symlinks ganis@lxplus.cern.ch:/eos/experiment/fcc /home/ganis/eosfcc
(CERN lxplus password)
```
The local and remote user and group ID must be the same for this to work.



