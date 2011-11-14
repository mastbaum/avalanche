#include <RAT/Pack.hh>
#include <RAT/Log.hh>
#include <RAT/BitManip.hh>
#include <RAT/DS/Root.hh>
#include <RAT/DS/PackedEvent.hh>
#include <RAT/DS/RunStore.hh>
#include <RAT/DS/Run.hh>
#include <RAT/DS/HeaderInfo.hh>

namespace RAT {

DS::PackedRec* Pack::MakeTRIGHeader(DS::Root *ds)
{
  DS::TRIGInfo *trig = ds->GetHeaderInfo()->GetTRIGInfo();
  DS::TRIG* rec = new DS::TRIG();

  rec->TrigMask = trig->GetTrigMask();
  rec->PulserRate = trig->GetPulserRate();
  rec->MTC_CSR = trig->GetMTC_CSR();
  rec->LockoutWidth = trig->GetLockoutWidth();
  rec->PrescaleFreq = trig->GetPrescaleFreq();
  rec->EventID = trig->GetEventID();
  rec->RunID = trig->GetRunID();
  
  int ntrigthold = trig->GetNTrigTHold();
  int ntrigzero = trig->GetNTrigZeroOffset();

  for(int i=0; i<10; i++) {
    rec->Threshold[i] = 0;
    rec->TrigZeroOffset[i] = 0;
    if (i < ntrigthold)
      rec->Threshold[i] = trig->GetTrigTHold(i);
    if (i < ntrigzero)
      rec->TrigZeroOffset[i] = trig->GetTrigZeroOffset(i);
  }

  DS::PackedRec* PackedRec = new DS::PackedRec();
  PackedRec->RecordType = 5;
  PackedRec->Rec = rec;

  return PackedRec;
}

DS::PackedRec* Pack::MakeEPEDHeader(DS::Root *ds)
{
  DS::EPEDInfo* eped = ds->GetHeaderInfo()->GetEPEDInfo();
  DS::EPED* rec = new DS::EPED();

  rec->GTDelayCoarse = eped->GetGTDelayCoarse();
  rec->GTDelayFine = eped->GetGTDelayFine();
  rec->QPedAmp = eped->GetQPedAmp();
  rec->QPedWidth = eped->GetQPedWidth();
  rec->PatternID = eped->GetPatternID();
  rec->CalType = eped->GetCalType();
  rec->EventID = eped->GetEventID();
  rec->RunID = eped->GetRunID();

  DS::PackedRec* PackedRec = new DS::PackedRec();
  PackedRec->RecordType = 6;
  PackedRec->Rec = rec;
  
  return PackedRec;
}

DS::PackedRec* Pack::MakeRunHeader(DS::Root *ds)
{
  DS::Run* run = RAT::DS::RunStore::GetRun(ds);
  DS::RHDR* rec = new DS::RHDR();

  rec->Date = run->GetDate();
  rec->Time = run->GetTime();
  rec->DAQVer = run->GetDAQVer();
  rec->CalibTrialID = run->GetCalibTrialID();
  rec->SrcMask = run->GetSrcMask();
  rec->RunMask = run->GetRunType();
  rec->CrateMask = run->GetCrateMask();
  rec->FirstEventID = run->GetFirstEventID();
  rec->ValidEventID = run->GetValidEventID();
  rec->RunID = run->GetRunID();

  DS::PackedRec* PackedRec = new DS::PackedRec();
  PackedRec->RecordType = 2;
  PackedRec->Rec = rec;

  return PackedRec;
}

DS::PackedRec* Pack::MakeAVHeader(DS::Root *ds)
{
  DS::AVStat* avstat = RAT::DS::RunStore::GetRun(ds)->GetAVStat();
  DS::CAAC* rec = new DS::CAAC();

  for (int i=0; i<3; i++) {
    rec->AVPos[i] = avstat->GetPosition(i);
    rec->AVRoll[i] = avstat->GetRoll(i);
  }

  for (int i=0; i<7; i++) {
    rec->AVRopeLength[i] = avstat->GetRopeLength(i);
  }

  DS::PackedRec* PackedRec = new DS::PackedRec();
  PackedRec->RecordType = 3;
  PackedRec->Rec = rec;

  return PackedRec;
}

DS::PackedRec* Pack::MakeManipHeader(DS::Root *ds)
{
  DS::ManipStat* manipstat = RAT::DS::RunStore::GetRun(ds)->GetManipStat();
  DS::CAST *rec = new DS::CAST();

  for (int i=0; i<3; i++) {
    rec->ManipPos[i] = manipstat->GetManipPos(i);
    rec->ManipDest[i] = manipstat->GetManipDest(i);
    rec->SrcPosUncert2[i] = manipstat->GetSrcPosUnc(i);
  }

  for (int i=0; i<manipstat->GetNRopes(); i++) {
    rec->RopeID.push_back(manipstat->GetRopeID(i));
    rec->RopeLen.push_back(manipstat->GetRopeLength(i));
    rec->RopeTargLen.push_back(manipstat->GetRopeTargLength(i));
    rec->RopeVel.push_back(manipstat->GetRopeVelocity(i));
    rec->RopeTens.push_back(manipstat->GetRopeTension(i));
    rec->RopeErr.push_back(manipstat->GetRopeErr(i));
  }

  rec->SourceID = manipstat->GetSrcID();
  rec->SourceStat = manipstat->GetSrcStatus();
  rec->NRopes = manipstat->GetNRopes();
  rec->SrcPosUncert1 = manipstat->GetSrcPosUnc();
  rec->LBallOrient = manipstat->GetLaserballOrient();

  DS::PackedRec *PackedRec = new DS::PackedRec();
  PackedRec->RecordType = 4;
  PackedRec->Rec = rec;

  return PackedRec;
}

DS::PackedRec* Pack::PackEvent(DS::Root *ds, int iev)
{
  DS::EV* ev = ds->GetEV(iev);
  DS::PackedEvent* PackedEV = new DS::PackedEvent();

  // Get event level info
  unsigned trigError = ev->GetTrigError();
  unsigned trigType = ev->GetTrigType();
  unsigned eventID = ev->GetEventID();
  unsigned long clockCount50 = ev->GetClockCount50();
  unsigned long clockCount10 = ev->GetClockCount10();
  char clockStat10 = ev->GetClockStat10();
  int nhits = ev->GetNhits();
  int npmtall = ev->GetPMTAllUnCalCount();

  // Init header words
  std::vector<int> header(6, 0);

  // Check some lengths:
  Log::Assert(BitManip::CheckLength(nhits, 16), "Pack: nhit has wrong length");
  Log::Assert(BitManip::CheckLength(clockStat10, 4), "Pack: clockStat10 has wrong length");
  Log::Assert(BitManip::CheckLength(eventID, 24), "Pack: GTID has wrong length");
  Log::Assert(BitManip::CheckLength(trigError, 15), "Pack: TrigError has wrong length");

  // Pack header words
  unsigned clock50part1 = (int) BitManip::GetBits((ULong64_t) clockCount50, 0, 11);
  unsigned clock50part2 = (int) BitManip::GetBits((ULong64_t) clockCount50, 11, 32);
  unsigned clock10part1 = (int) BitManip::GetBits((ULong64_t) clockCount10, 0, 32);
  unsigned clock10part2 = (int) BitManip::GetBits((ULong64_t) clockCount10, 32, 21);

  Log::Assert(BitManip::CheckLength(clock50part1, 11), "Pack: clock50part1 has wrong length");
  Log::Assert(BitManip::CheckLength(clock50part2, 32), "Pack: clock50part2 has wrong length");
  Log::Assert(BitManip::CheckLength(clock10part1, 32), "Pack: clock10part1 has wrong length");
  Log::Assert(BitManip::CheckLength(clock10part2, 21), "Pack: clock10part2 has wrong length");
 
  unsigned long p1 = clock10part1;
  unsigned long p2 = clock10part2;
  unsigned long test10 = (p2<<32) + p1;
  if (test10 != clockCount10)
    warn << dformat("Pack: test10 (%lu) != clockCount10 (%lu)\n", test10, clockCount10);
  
  header[0] = clock10part1; // 32 bits of 10MHz clock
  header[1] = clock10part2; // 21 bits of ""
  header[1] = BitManip::SetBits(header[1], 21, clock50part1); // 11 bits of 50MHz clock 
  header[2] = clock50part2; // 32 bits of ""

  unsigned int trig1 = BitManip::GetBits(trigType, 0, 8);
  unsigned int trig2 = BitManip::GetBits(trigType, 8, 19);
  Log::Assert(BitManip::CheckLength(trigType, 27), "Pack: TrigWord has wrong length");
  Log::Assert(BitManip::CheckLength(trig1, 8), "Pack: TrigWordP1 has wrong length");
  Log::Assert(BitManip::CheckLength(trig2, 19), "Pack: TrigWordP2 has wrong length");
  
  header[3] = eventID; // 24 bits of GTID
  header[3] = BitManip::SetBits(header[3], 24, trig1); // 8 bits of Trigger word
  header[4] = trig2; // 19 bits of Trigger word
  header[5] = BitManip::SetBits(header[5], 17, trigError); // 15 bits of TrigError word
  
  for (int i=0; i<6; i++)
    PackedEV->MTCInfo[i] = header[i];

  PackedEV->PackVer = fPackVer;
  PackedEV->EVOrder = eventID; //FIXME
  PackedEV->NHits = nhits;
  PackedEV->ClockStat10 = clockStat10;

  DS::Run* run = RAT::DS::RunStore::GetRun(ds);
  PackedEV->DataType = run->GetDataType();
  PackedEV->MCFlag = run->GetMCFlag();
  PackedEV->RunMask = run->GetRunType();
  PackedEV->RunID = run->GetRunID();
  PackedEV->SubRunID = run->GetSubRunID();

  // Set number of PMT bundles
  PackedEV->PMTBundles.resize(npmtall);
  
  // Pack each bundle
  for (int inh=0; inh<npmtall; inh++) {
    DS::PMTUnCal *pmt = ev->GetPMTAllUnCal(inh);
    PackedEV->PMTBundles[inh] = Pack::MakePMTBundle(pmt, eventID);
  }

  DS::PackedRec* PackedRec = new DS::PackedRec();
  PackedRec->RecordType = 1;
  PackedRec->Rec = PackedEV;

  return PackedRec;
}

// Pack the PMT Bundles
DS::PMTBundle Pack::MakePMTBundle(DS::PMTUnCal* pmt, unsigned gtid)
{
  DS::PMTBundle bundle;

  // Get PMT level info
  int lcn = pmt->GetID();
  int icrate = BitManip::GetCrate(lcn);
  int icard = BitManip::GetCard(lcn);
  int ichan = BitManip::GetChannel(lcn);
  int cell = pmt->GetCellID();

  // Order of flags in chanFlags, from least sig bit upwards:
  // CGT ES16, CGT ES24, Missed Count, NC/CC Flag, LGISELECT, CMOS ES16
  char chanFlags = pmt->GetChanFlags();

  // Get uncalibrated charges and time
  unsigned short qhs = pmt->GetsQHS();
  unsigned short qhl = pmt->GetsQHL();
  unsigned short qlx = pmt->GetsQLX();
  unsigned short tac = pmt->GetsPMTt();
  
  // Flip final bit of Q,T to mimic the ADCs
  qhs = BitManip::FlipBit(qhs,11);
  qhl = BitManip::FlipBit(qhl,11);
  qlx = BitManip::FlipBit(qlx,11);
  tac = BitManip::FlipBit(tac,11);

  // Check some lengths
  Log::Assert(BitManip::CheckLength(qlx, 12), "Pack: QLX has wrong length");
  Log::Assert(BitManip::CheckLength(qhs,12), "Pack: QHS has wrong length");
  Log::Assert(BitManip::CheckLength(qhl,12), "Pack: QH has wrong lengthL");
  Log::Assert(BitManip::CheckLength(tac,12), "Pack: TAC has wrong length");
  Log::Assert(BitManip::CheckLength(cell,4), "Pack: CellID has wrong length");
  Log::Assert(BitManip::CheckLength(icrate,5), "Pack: Crate# has wrong length");
  Log::Assert(BitManip::CheckLength(icard,4), "Pack: Card# has wrong length");
  Log::Assert(BitManip::CheckLength(ichan,5), "Pack: Channel# has wrong length");
  
  // Pack this bundle
  // First word
  int word1 = BitManip::GetBits(gtid, 0, 16);          // 16 bits of GTID
  word1 = BitManip::SetBits(word1, 16, ichan);     // 5 bits of ichan
  word1 = BitManip::SetBits(word1, 21, icrate);    // 5 bits of icrate
  word1 = BitManip::SetBits(word1, 26, icard);     // 4 bits of icard
  if (BitManip::TestBit(chanFlags,0))              // 1 bit for CGT ES16
    word1 = BitManip::SetBit(word1, 30);
  if (BitManip::TestBit(chanFlags,1))              // 1 bit for CGT ES24
    word1 = BitManip::SetBit(word1, 31);
  
  // Second word
  int word2 = qlx;                                     // 12 bits for QLX
  word2 = BitManip::SetBits(word2, 12, cell);      // 4 bits for cellID
  word2 = BitManip::SetBits(word2, 16, qhs);       // 12 bits for QHS
  if (BitManip::TestBit(chanFlags,2))              // 1 bit for `Missed count'
    word2 = BitManip::SetBit(word2, 28);
  if (BitManip::TestBit(chanFlags,3))              // 1 bit for NC/CC
    word2 = BitManip::SetBit(word2, 29);
  if (BitManip::TestBit(chanFlags,4))              // 1 bit for LGISelect
    word2 = BitManip::SetBit(word2, 30);
  if (BitManip::TestBit(chanFlags,5))              // 1 bit for CMOS ES16
    word2 = BitManip::SetBit(word2, 31);
  
  // Third word
  int word3 = qhl;                                     // 12 bits for QHL
  int gtidpart1 = BitManip::GetBits(gtid, 16, 4);
  int gtidpart2 = BitManip::GetBits(gtid, 20, 4);
  word3 = BitManip::SetBits(word3, 12, gtidpart1); // 4 bits of GTID
  word3 = BitManip::SetBits(word3, 16, tac);       // 12 bits for TAC
  word3 = BitManip::SetBits(word3, 28, gtidpart2); // 4 bits of GTID

  bundle.Word[0] = word1;
  bundle.Word[1] = word2;
  bundle.Word[2] = word3;

  return bundle;
}

} // namespace RAT

