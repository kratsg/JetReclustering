# echo "cleanup xAODJetReclustering xAODJetReclustering-00-00-00 in /data/atlas/atlasdata3/burr/Multijets/trunk/Reclustering"

if test "${CMTROOT}" = ""; then
  CMTROOT=/cvmfs/atlas.cern.ch/repo/sw/software/AthAnalysisBase/x86_64-slc6-gcc49-opt/2.4.6/CMT/v1r25p20140131; export CMTROOT
fi
. ${CMTROOT}/mgr/setup.sh
cmtxAODJetReclusteringtempfile=`${CMTROOT}/${CMTBIN}/cmt.exe -quiet build temporary_name`
if test ! $? = 0 ; then cmtxAODJetReclusteringtempfile=/tmp/cmt.$$; fi
${CMTROOT}/${CMTBIN}/cmt.exe cleanup -sh -pack=xAODJetReclustering -version=xAODJetReclustering-00-00-00 -path=/data/atlas/atlasdata3/burr/Multijets/trunk/Reclustering  $* >${cmtxAODJetReclusteringtempfile}
if test $? != 0 ; then
  echo >&2 "${CMTROOT}/${CMTBIN}/cmt.exe cleanup -sh -pack=xAODJetReclustering -version=xAODJetReclustering-00-00-00 -path=/data/atlas/atlasdata3/burr/Multijets/trunk/Reclustering  $* >${cmtxAODJetReclusteringtempfile}"
  cmtcleanupstatus=2
  /bin/rm -f ${cmtxAODJetReclusteringtempfile}
  unset cmtxAODJetReclusteringtempfile
  return $cmtcleanupstatus
fi
cmtcleanupstatus=0
. ${cmtxAODJetReclusteringtempfile}
if test $? != 0 ; then
  cmtcleanupstatus=2
fi
/bin/rm -f ${cmtxAODJetReclusteringtempfile}
unset cmtxAODJetReclusteringtempfile
return $cmtcleanupstatus

