---
layout: site
id: computing
---

FCC Computing / Storage
================================================
CERN resources are available to be used by people being employed by an institute that has signed the Memorandum of Understanding (MoU) and by laboratories having signed the Addendum. For more information on the procedure please have a look at <a href="https://fcc-ped.web.cern.ch/content/official-documents"> this page. </a>

The computing and disk resources are controlled by <a href="https://e-groups.cern.ch/e-groups/EgroupsSearchForm.do">e-groups</a> with admin approval.

**Those receiving approval engage in using these resources ONLY for activities related to FCC**.

## Computing resources

A quota of resources, dedicated to FCC computing activities, are available in the HTCondor-based CERN batch farm.
Usage of these resources is controlled by the <a href="https://e-groups.cern.ch/e-groups/EgroupsSearchForm.do">CERN e-group</a>
**fcc-experiments-comp** and subject to admin approval.
The associated accounting group is **group_u_FCC**; to use resources from this quota, the following needs to be
added to the HTCondor job submit file:
```
+AccountingGroup = "group_u_FCC.local_gen"
```
## Storage resources

A quota of storage disk space, dedicated to FCC computing activities, is available in the EOS system at CERN under the path
```
/eos/experiment/fcc
```
Access to these resources is controlled by the <a href="https://e-groups.cern.ch/e-groups/EgroupsSearchForm.do">CERN e-group</a>
**fcc-eos-read** for read only access and **fcc-eos-write** for read and write permissions. Both are subject to admin approval.

### Access to storage resources from outside CERN

Users enabled to access the FCC area on EOS can do so also from outside CERN using the Kerberos credentials associated with their
CERN user account. To do so, they have to make sure to have the Kerberos credential initiator application, `kinit`, installed
and configured to enable the `CERN.CH` Kerberos realm. Typically is enough to copy the relevant sections from the /eos/krb5.conf
file accessible on lxplus (or copying the complete file, if there is none locally or if the local one is not customized).

Once this is done, the following should work:
```
$ kinit <CERNLOGIN>@CERN.CH
<CERNLOGIN>@CERN.CH's password:
(Enter CERN lxplus password)

$ klist
Credentials cache: API:68FEEF0D-F635-45C6-91B3-1A7A6F83E0C6
        Principal: <CERNLOGIN>@CERN.CH

  Issued                Expires               Principal
Dec  2 16:20:16 2020  Dec  3 17:20:10 2020  krbtgt/CERN.CH@CERN.CH
```
The files can be copied locally
```
 $ xrdcp root://eospublic.cern.ch//eos/experiment/fcc/ee/generation/DelphesEvents/spring2021/IDEA/p8_ee_ZH_ecm240/events_103943569.root .
[80.67MB/80.67MB][100%][==================================================][16.13MB/s]
```
or accessed directly via ROOT
```
$ root -l
root [0] auto f = TFile::Open("root://eospublic.cern.ch//eos/experiment/fcc/ee/generation/DelphesEvents/spring2021/IDEA/p8_ee_ZH_ecm240/events_103943569.root")
Warning in <TClass::Init>: no dictionary for class podio::GenericParameters is available
Warning in <TClass::Init>: no dictionary for class podio::ObjectID is available
Warning in <TClass::Init>: no dictionary for class edm4hep::ClusterData is available
Warning in <TClass::Init>: no dictionary for class edm4hep::Vector3f is available
Warning in <TClass::Init>: no dictionary for class edm4hep::MCParticleData is available
Warning in <TClass::Init>: no dictionary for class edm4hep::Vector3d is available
Warning in <TClass::Init>: no dictionary for class edm4hep::Vector2i is available
Warning in <TClass::Init>: no dictionary for class edm4hep::ReconstructedParticleData is available
Warning in <TClass::Init>: no dictionary for class edm4hep::MCRecoParticleAssociationData is available
Warning in <TClass::Init>: no dictionary for class edm4hep::ParticleIDData is available
Warning in <TClass::Init>: no dictionary for class edm4hep::TrackData is available
Warning in <TClass::Init>: no dictionary for class edm4hep::TrackState is available
Warning in <TClass::Init>: no dictionary for class edm4hep::RecoParticleRefData is available
Warning in <TClass::Init>: no dictionary for class podio::CollectionIDTable is available
Warning in <TClass::Init>: no dictionary for class pair<int,podio::GenericParameters> is available
(TFile *) @0x7ffe9e4c9890
root [1]
```

Note the use of the `eospublic.cern.ch` server.
A cache system, for example based on `XCache`, can make the whole setup more efficient.

Another possibility is to mount the remote directory locally using the `sshfs` utility, for example
```
$ mkdir ~/eosfcc
sshfs -o idmap=user -o follow_symlinks <CERNLOGIN>@lxplus.cern.ch:/eos/experiment/fcc ~/eosfcc
(CERN lxplus password)
```
The local and remote user and group ID must be the same for this to work.
