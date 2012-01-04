#include <RAT/Pack.hh>
#include <RAT/Log.hh>
#include <RAT/BitManip.hh>
#include <RAT/DS/Root.hh>
#include <RAT/DS/PackedEvent.hh>
#include <RAT/DS/RunStore.hh>
#include <RAT/DS/Run.hh>
#include <RAT/DS/HeaderInfo.hh>
#include <RAT/DetectorConstruction.hh>

namespace RAT {

namespace Pack {

// packing functions
DS::PackedRec* MakeTRIGHeader(DS::Root* ds)
{
  DS::TRIGInfo* trig = ds->GetHeaderInfo()->GetTRIGInfo();
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

DS::PackedRec* MakeEPEDHeader(DS::Root* ds)
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

DS::PackedRec* MakeRunHeader(DS::Root* ds)
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

DS::PackedRec* MakeAVHeader(DS::Root* ds)
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

DS::PackedRec* MakeManipHeader(DS::Root* ds)
{
  DS::ManipStat* manipstat = RAT::DS::RunStore::GetRun(ds)->GetManipStat();
  DS::CAST* rec = new DS::CAST();

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

  DS::PackedRec* PackedRec = new DS::PackedRec();
  PackedRec->RecordType = 4;
  PackedRec->Rec = rec;

  return PackedRec;
}

DS::PackedRec* PackEvent(DS::Root* ds, int iev)
{
  DS::EV* ev = ds->GetEV(iev);
  DS::PackedEvent* PackedEV = new DS::PackedEvent();

  // get event level info
  unsigned trigError = ev->GetTrigError();
  unsigned trigType = ev->GetTrigType();
  unsigned eventID = ev->GetEventID();
  unsigned long clockCount50 = ev->GetClockCount50();
  unsigned long clockCount10 = ev->GetClockCount10();
  char clockStat10 = ev->GetClockStat10();
  int nhits = ev->GetNhits();
  int npmtall = ev->GetPMTAllUnCalCount();

  // initialize header words
  std::vector<int> header(6, 0);

  // check some lengths
  Log::Assert(BitManip::CheckLength(nhits, 16), "Pack: nhit has wrong length");
  Log::Assert(BitManip::CheckLength(clockStat10, 4), "Pack: clockStat10 has wrong length");
  Log::Assert(BitManip::CheckLength(eventID, 24), "Pack: GTID has wrong length");
  Log::Assert(BitManip::CheckLength(trigError, 15), "Pack: TrigError has wrong length");

  // pack header words
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
  
  header[0] = clock10part1;                                   // 32 bits of 10MHz clock
  header[1] = clock10part2;                                   // 21 bits of ""
  header[1] = BitManip::SetBits(header[1], 21, clock50part1); // 11 bits of 50MHz clock 
  header[2] = clock50part2;                                   // 32 bits of ""

  unsigned int trig1 = BitManip::GetBits(trigType, 0, 8);
  unsigned int trig2 = BitManip::GetBits(trigType, 8, 19);
  Log::Assert(BitManip::CheckLength(trigType, 27), "Pack: TrigWord has wrong length");
  Log::Assert(BitManip::CheckLength(trig1, 8), "Pack: TrigWordP1 has wrong length");
  Log::Assert(BitManip::CheckLength(trig2, 19), "Pack: TrigWordP2 has wrong length");
  
  header[3] = eventID;                                        // 24 bits of GTID
  header[3] = BitManip::SetBits(header[3], 24, trig1);        // 8 bits of Trigger word
  header[4] = trig2;                                          // 19 bits of Trigger word
  header[5] = BitManip::SetBits(header[5], 17, trigError);    // 15 bits of TrigError word
  
  for (int i=0; i<6; i++)
    PackedEV->MTCInfo[i] = header[i];

  PackedEV->PackVer = packVer;
  PackedEV->EVOrder = eventID; //FIXME
  PackedEV->NHits = nhits;
  PackedEV->ClockStat10 = clockStat10;

  DS::Run* run = RAT::DS::RunStore::GetRun(ds);
  PackedEV->DataType = run->GetDataType();
  PackedEV->MCFlag = run->GetMCFlag();
  PackedEV->RunMask = run->GetRunType();
  PackedEV->RunID = run->GetRunID();
  PackedEV->SubRunID = run->GetSubRunID();

  // set number of pmt bundles
  PackedEV->PMTBundles.resize(npmtall);
  
  // pack pmt bundles
  for (int inh=0; inh<npmtall; inh++) {
    DS::PMTUnCal* pmt = ev->GetPMTAllUnCal(inh);
    PackedEV->PMTBundles[inh] = MakePMTBundle(pmt, eventID);
  }

  DS::PackedRec* PackedRec = new DS::PackedRec();
  PackedRec->RecordType = 1;
  PackedRec->Rec = PackedEV;

  return PackedRec;
}

// pack PMTBundles
DS::PMTBundle MakePMTBundle(DS::PMTUnCal* pmt, unsigned gtid)
{
  DS::PMTBundle bundle;

  // get pmt level info
  int lcn = pmt->GetID();
  int icrate = BitManip::GetCrate(lcn);
  int icard = BitManip::GetCard(lcn);
  int ichan = BitManip::GetChannel(lcn);
  int cell = pmt->GetCellID();

  // order of flags in chanFlags, from least sig bit upwards:
  // CGT ES16, CGT ES24, Missed Count, NC/CC Flag, LGISELECT, CMOS ES16
  char chanFlags = pmt->GetChanFlags();

  // get uncalibrated charges and time
  unsigned short qhs = pmt->GetsQHS();
  unsigned short qhl = pmt->GetsQHL();
  unsigned short qlx = pmt->GetsQLX();
  unsigned short tac = pmt->GetsPMTt();
  
  // flip last bit of Q,T to mimic the ADCs
  qhs = BitManip::FlipBit(qhs,11);
  qhl = BitManip::FlipBit(qhl,11);
  qlx = BitManip::FlipBit(qlx,11);
  tac = BitManip::FlipBit(tac,11);

  // check some lengths
  Log::Assert(BitManip::CheckLength(qlx, 12), "Pack: QLX has wrong length");
  Log::Assert(BitManip::CheckLength(qhs,12), "Pack: QHS has wrong length");
  Log::Assert(BitManip::CheckLength(qhl,12), "Pack: QH has wrong lengthL");
  Log::Assert(BitManip::CheckLength(tac,12), "Pack: TAC has wrong length");
  Log::Assert(BitManip::CheckLength(cell,4), "Pack: CellID has wrong length");
  Log::Assert(BitManip::CheckLength(icrate,5), "Pack: Crate# has wrong length");
  Log::Assert(BitManip::CheckLength(icard,4), "Pack: Card# has wrong length");
  Log::Assert(BitManip::CheckLength(ichan,5), "Pack: Channel# has wrong length");
  
  // pack this bundle
  // first word
  int word1 = BitManip::GetBits(gtid, 0, 16);      // 16 bits of GTID
  word1 = BitManip::SetBits(word1, 16, ichan);     // 5 bits of ichan
  word1 = BitManip::SetBits(word1, 21, icrate);    // 5 bits of icrate
  word1 = BitManip::SetBits(word1, 26, icard);     // 4 bits of icard
  if (BitManip::TestBit(chanFlags,0))              // 1 bit for CGT ES16
    word1 = BitManip::SetBit(word1, 30);
  if (BitManip::TestBit(chanFlags,1))              // 1 bit for CGT ES24
    word1 = BitManip::SetBit(word1, 31);
  
  // second word
  int word2 = qlx;                                 // 12 bits for QLX
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
  
  // third word
  int word3 = qhl;                                 // 12 bits for QHL
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

// unpacking functions
DS::Root* UnpackEvent(DS::PackedEvent* pev, DS::TRIGInfo* trig=NULL, DS::EPEDInfo* eped=NULL)
{
  DS::Root* ds = new DS::Root();
  DS::EV* ev = ds->AddNewEV();

  // copy header info, if provided
  DS::HeaderInfo* header = ds->GetHeaderInfo();

  if (trig) {
    DS::TRIGInfo* triginfo = header->GetTRIGInfo();
    *triginfo = *trig;
  }

  if (eped) {
    DS::EPEDInfo* epedinfo = header->GetEPEDInfo();
    *epedinfo = *eped;
  }

  // set root/ds level info
  ds->SetRunID(pev->RunID);
  ds->SetSubRunID(pev->SubRunID);

  // unpack header information: 6 MTCInfo words + clockstat10
  unsigned long clock10part1 = pev->MTCInfo[0];
  unsigned long clock10part2 = BitManip::GetBits(pev->MTCInfo[1],0,21);
  unsigned long clockCount10 = (clock10part2 << 32) + clock10part1;

  unsigned long clock50part1 = BitManip::GetBits(pev->MTCInfo[1],21,11);
  unsigned long clock50part2 = pev->MTCInfo[2];
  unsigned long clockCount50 = (clock50part2 << 11) + clock50part1;

  unsigned eventID = BitManip::GetBits(pev->MTCInfo[3],0,24);
  
  unsigned trigpart1 = BitManip::GetBits(pev->MTCInfo[3],24,8);
  unsigned trigpart2 = BitManip::GetBits(pev->MTCInfo[4],0,19);
  unsigned trigType = (trigpart2 << 8) + trigpart1;
  
  unsigned trigError = BitManip::GetBits(pev->MTCInfo[5],17,15);

  ev->SetClockStat10(pev->ClockStat10);
  ev->SetTrigError(trigError);
  ev->SetTrigType(trigType);
  ev->SetEventID(eventID);
  ev->SetClockCount50(clockCount50);
  ev->SetClockCount10(clockCount10);
    
  // set UT from 10MHz clock counts
  unsigned long Period = 100; // 10MHz period in ns
  unsigned long Total = clockCount10 * Period;
  unsigned long NNsec = Total % (unsigned long)1e9;
  unsigned long NSecs = Total / 1e9;
  unsigned long NDays = NSecs / 86400;
  NSecs = NSecs - (86400 * NDays);
  
  unsigned ndays = (unsigned) NDays;
  unsigned nsecs = (unsigned) NSecs;
  unsigned nns = (unsigned) NNsec;
  
  ev->SetUTDays(ndays);
  ev->SetUTSecs(nsecs);
  ev->SetUTNSecs(nns);

  // unpack hit information from pmt bundles
  for (size_t inh=0; inh<pev->PMTBundles.size(); inh++){
    DS::PMTBundle bundle = pev->PMTBundles[inh];

    // figure out pmt type from dqxx
    unsigned ichan = BitManip::GetBits(bundle.Word[0], 16, 5);
    unsigned icard = BitManip::GetBits(bundle.Word[0], 26, 4);
    unsigned icrate = BitManip::GetBits(bundle.Word[0], 21, 5);
    unsigned lcn = BitManip::GetLCN(icrate, icard, ichan);
    int type = DetectorConstruction::fTubeStatus[lcn];

    DS::PMTUnCal* pmt = ev->AddNewPMTUnCal(type);
    DS::PMTUnCal* pmtUnpacked = UnpackPMT(&bundle);
    *pmt = *pmtUnpacked;
    delete pmtUnpacked;
  }

  return ds;
}

DS::Run* UnpackRHDR(DS::RHDR* rhdr)
{
  DS::Run* run = new DS::Run();

  run->SetDate(rhdr->Date);
  run->SetTime(rhdr->Time);
  run->SetDAQVer(rhdr->DAQVer);
  run->SetCalibTrialID(rhdr->CalibTrialID);
  run->SetSrcMask(rhdr->SrcMask);
  run->SetRunType(rhdr->RunMask);
  run->SetCrateMask(rhdr->CrateMask);
  run->SetFirstEventID(rhdr->FirstEventID);
  run->SetValidEventID(rhdr->ValidEventID);
  run->SetRunID(rhdr->RunID);

  return run;
}

DS::AVStat* UnpackCAAC(DS::CAAC* caac)
{
  DS::AVStat* avstat = new DS::AVStat();

  for (int i=0; i<3; i++) {
    avstat->SetPosition(i, caac->AVPos[i]);
    avstat->SetRoll(i, caac->AVRoll[i]);
  }

  for (int i=0; i<7; i++)
    avstat->SetRopeLength(i, caac->AVRopeLength[i]);

  return avstat;
}

DS::ManipStat* UnpackCAST(DS::CAST* cast)
{
  DS::ManipStat* manipstat = new DS::ManipStat();

  for (int i=0; i<3; i++) {
    manipstat->SetManipPos(i, cast->ManipPos[i]);
    manipstat->SetManipDest(i, cast->ManipDest[i]);
    manipstat->SetSrcPosUnc(i, cast->SrcPosUncert2[i]);
  }

  manipstat->SetSrcID(cast->SourceID);
  manipstat->SetSrcStatus(cast->SourceStat);
  manipstat->SetNRopes(cast->NRopes);
  manipstat->SetSrcPosUnc(cast->SrcPosUncert1);
  manipstat->SetLaserballOrient(cast->LBallOrient);

  for (int i=0; i<manipstat->GetNRopes(); i++) {
    manipstat->SetRopeID(i, cast->RopeID[i]);
    manipstat->SetRopeLength(i, cast->RopeLen[i]);
    manipstat->SetRopeTargLength(i, cast->RopeTargLen[i]);
    manipstat->SetRopeVelocity(i, cast->RopeVel[i]);
    manipstat->SetRopeTension(i, cast->RopeTens[i]);
    manipstat->SetRopeErr(i, cast->RopeErr[i]);
  }

  return manipstat;
}

DS::TRIGInfo* UnpackTRIG(DS::TRIG* trig)
{
  DS::TRIGInfo* triginfo = new DS::TRIGInfo();

  triginfo->SetTrigMask(trig->TrigMask);
  triginfo->SetPulserRate(trig->PulserRate);
  triginfo->SetMTC_CSR(trig->MTC_CSR);
  triginfo->SetLockoutWidth(trig->LockoutWidth);
  triginfo->SetPrescaleFreq(trig->PrescaleFreq);
  triginfo->SetEventID(trig->EventID);
  triginfo->SetRunID(trig->RunID); 
  triginfo->SetNTrigTHold(10);
  triginfo->SetNTrigZeroOffset(10);

  for (int i=0; i<10; i++) {
    triginfo->SetTrigTHold(i, trig->Threshold[i]);
    triginfo->SetTrigZeroOffset(i, trig->TrigZeroOffset[i]);
  }

  return triginfo;
}

DS::EPEDInfo* UnpackEPED(DS::EPED* eped)
{
  DS::EPEDInfo* epedinfo = new DS::EPEDInfo();

  epedinfo->SetGTDelayCoarse(eped->GTDelayCoarse);
  epedinfo->SetGTDelayFine(eped->GTDelayFine);
  epedinfo->SetQPedAmp(eped->QPedAmp);
  epedinfo->SetQPedWidth(eped->QPedWidth);
  epedinfo->SetPatternID(eped->PatternID);
  epedinfo->SetCalType(eped->CalType);
  epedinfo->SetEventID(eped->EventID);
  epedinfo->SetRunID(eped->RunID);

  return epedinfo;
}

DS::PMTUnCal* UnpackPMT(DS::PMTBundle* bundle)
{
  DS::PMTUnCal* pmt = new DS::PMTUnCal();

  unsigned ichan = BitManip::GetBits(bundle->Word[0], 16, 5);
  unsigned icard = BitManip::GetBits(bundle->Word[0], 26, 4);
  unsigned icrate = BitManip::GetBits(bundle->Word[0], 21, 5);
  unsigned lcn = BitManip::GetLCN(icrate, icard, ichan);
  unsigned cell = BitManip::GetBits(bundle->Word[1], 12, 4);

  // 3 pmt words
  char chanflags = 0;
  if (BitManip::TestBit(bundle->Word[0], 30))
    chanflags = BitManip::SetBit(chanflags, 0);
  if (BitManip::TestBit(bundle->Word[0], 31))
    chanflags = BitManip::SetBit(chanflags, 1);
  if (BitManip::TestBit(bundle->Word[1], 28))
    chanflags = BitManip::SetBit(chanflags, 2);
  if (BitManip::TestBit(bundle->Word[1], 29))
    chanflags = BitManip::SetBit(chanflags, 3);
  if (BitManip::TestBit(bundle->Word[1], 30))
    chanflags = BitManip::SetBit(chanflags, 4);
  if (BitManip::TestBit(bundle->Word[1], 31))
    chanflags = BitManip::SetBit(chanflags, 5);
  
  unsigned short qhs = BitManip::GetBits(bundle->Word[1], 16, 12);
  unsigned short qhl = BitManip::GetBits(bundle->Word[2], 0, 12);
  unsigned short qlx = BitManip::GetBits(bundle->Word[1], 0, 12);
  unsigned short tac = BitManip::GetBits(bundle->Word[2], 16, 12);
  
  // flip last bit of Q, T (adc weirdness)
  qhs = BitManip::FlipBit(qhs,11);
  qhl = BitManip::FlipBit(qhl,11);
  qlx = BitManip::FlipBit(qlx,11);
  tac = BitManip::FlipBit(tac,11);
  
  pmt->SetID(lcn);
  pmt->SetCellID(cell);
  pmt->SetChanFlags(chanflags);
  pmt->SetsQHS(qhs);
  pmt->SetsQHL(qhl);
  pmt->SetsQLX(qlx);
  pmt->SetsPMTt(tac);

  return pmt;
}

} // namespace Pack

} // namespace RAT

