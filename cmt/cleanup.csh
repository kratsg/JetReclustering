# echo "cleanup xAODJetReclustering xAODJetReclustering-00-00-00 in /share/home/kratsg/DualUse"

if ( $?CMTROOT == 0 ) then
  setenv CMTROOT /cvmfs/atlas.cern.ch/repo/sw/software/AthAnalysisBase/x86_64-slc6-gcc49-opt/2.3.45/CMT/v1r25p20140131
endif
source ${CMTROOT}/mgr/setup.csh
set cmtxAODJetReclusteringtempfile=`${CMTROOT}/${CMTBIN}/cmt.exe -quiet build temporary_name`
if $status != 0 then
  set cmtxAODJetReclusteringtempfile=/tmp/cmt.$$
endif
${CMTROOT}/${CMTBIN}/cmt.exe cleanup -csh -pack=xAODJetReclustering -version=xAODJetReclustering-00-00-00 -path=/share/home/kratsg/DualUse  $* >${cmtxAODJetReclusteringtempfile}
if ( $status != 0 ) then
  echo "${CMTROOT}/${CMTBIN}/cmt.exe cleanup -csh -pack=xAODJetReclustering -version=xAODJetReclustering-00-00-00 -path=/share/home/kratsg/DualUse  $* >${cmtxAODJetReclusteringtempfile}"
  set cmtcleanupstatus=2
  /bin/rm -f ${cmtxAODJetReclusteringtempfile}
  unset cmtxAODJetReclusteringtempfile
  exit $cmtcleanupstatus
endif
set cmtcleanupstatus=0
source ${cmtxAODJetReclusteringtempfile}
if ( $status != 0 ) then
  set cmtcleanupstatus=2
endif
/bin/rm -f ${cmtxAODJetReclusteringtempfile}
unset cmtxAODJetReclusteringtempfile
exit $cmtcleanupstatus

