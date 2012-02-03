#ifndef __AVALANCHE_RAT_H__
#define __AVALANCHE_RAT_H__

#include <TObject.h>
#include <RAT/DS/PackedEvent.hh>

#include "json/json.h"
#include "json/value.h"

namespace avalanche {

    /**
     * Convert a header CouchDB document (in the form of a Json::Value) into a
     * RAT::DS::PackedRec.
     *
     * You should create a pointer to this function and pass it to
     * avalanche::client::addDB like so:
     *
     * @code
avalanche::docObjectMap map = &(avalanche::docToRecord);
client.addDB("the url", "the database name", map);
     * @endcode
     *
     * Returns NULL if conversion fails.
     *
     * @param v A JSON document
     * @return A TObject*, which is really a RAT::DS::PackedRec*
     */
    static TObject* docToRecord(Json::Value &v) {
        enum { RHDR=2, CAAC=3, CAST=4, TRIG=5, EPED=6 };

        RAT::DS::PackedRec* rec = new RAT::DS::PackedRec();
        std::string type = v["type"].asString();
        if (type == "") {
            return NULL;
        }

        if (type == "rhdr") {
            rec->RecordType = RHDR;
            RAT::DS::RHDR* r = new RAT::DS::RHDR();
            r->Date = v["date"].asInt();
            r->Time = v["time"].asInt();
            r->DAQVer = v["daq_ver"].asInt();
            r->CalibTrialID = v["calib_trial_id"].asInt();
            r->SrcMask = v["source_mask"].asInt();
            r->RunMask = v["run_mask"].asInt();
            r->CrateMask = v["crate_mask"].asInt();
            r->FirstEventID = v["first_event_id"].asInt();
            r->ValidEventID = v["valid_event_id"].asInt();
            r->RunID = v["run_id"].asInt();
            rec->Rec = r;
            return rec;
        }
        else if (type == "caac") {
            rec->RecordType = CAAC;
            RAT::DS::CAAC* r = new RAT::DS::CAAC();
            for (size_t i; i<3; i++) {
                r->AVPos[i] = v["av_pos"][i].asDouble();
                r->AVRoll[i] = v["av_roll"][i].asDouble();
            }
            for (size_t i; i<7; i++) {
                r->AVRopeLength[i] = v["av_rope_length"][i].asDouble();
            }
            rec->Rec = r;
            return rec;
        }
        else if (type == "cast") {
            rec->RecordType = CAST;
            RAT::DS::CAST* r = new RAT::DS::CAST();
            r->SourceID = v["source_id"].asInt();
            r->SourceStat = v["source_stat"].asInt();
            r->NRopes = v["nropes"].asInt();
            r->SrcPosUncert1 = v["src_pos_uncert_1"].asDouble();
            r->LBallOrient = v["lball_orient"].asDouble();
            for (size_t i; i<3; i++) {
                r->ManipPos[i] = v["manip_pos"][i].asDouble();
                r->ManipDest[i] = v["manip_dest"][i].asDouble();
                r->SrcPosUncert2[i] = v["src_pos_uncert_2"][i].asDouble();
            }
            for (size_t i; i<v["rope_len"].size(); i++) {
                r->RopeID.push_back(v["rope_id"][i].asInt());
                r->RopeLen.push_back(v["rope_len"][i].asDouble());
                r->RopeTargLen.push_back(v["rope_targ_len"][i].asDouble());
                r->RopeVel.push_back(v["rope_vel"][i].asDouble());
                r->RopeTens.push_back(v["rope_tens"][i].asDouble());
                r->RopeErr.push_back(v["rope_err"][i].asDouble());
            }
            rec->Rec = r;
            return rec;
        }
        else if (type == "trig") {
            rec->RecordType = TRIG;
            RAT::DS::TRIG* r = new RAT::DS::TRIG();
            r->TrigMask = v["trigmask"].asInt();
            r->PulserRate = v["pulser_rate"].asInt();
            r->MTC_CSR = v["mtc_csr"].asInt();
            r->LockoutWidth = v["lockout_width"].asInt();
            r->PrescaleFreq = v["prescale_freq"].asInt();
            r->EventID = v["event_id"].asInt();
            r->RunID = v["run_id"].asInt();
            for (size_t i; i<10; i++) {
                r->Threshold[i] = v["threshold"][i].asInt();
                r->TrigZeroOffset[i] = v["trig_zero_offset"][i].asInt();
            }
            rec->Rec = r;
            return rec;
        }
        else if (type == "eped") {
            rec->RecordType = EPED;
            RAT::DS::EPED* r = new RAT::DS::EPED();
            r->GTDelayCoarse = v["gtdelay_coarse"].asInt();
            r->GTDelayFine = v["gtdelay_fine"].asInt();
            r->QPedAmp = v["qped_amp"].asInt();
            r->QPedWidth = v["qped_width"].asInt();
            r->PatternID = v["pattern_id"].asInt();
            r->CalType = v["caltype"].asInt();
            r->EventID = v["event_id"].asInt();
            r->RunID = v["run_id"].asInt();
            rec->Rec = r;
            return rec;
        }
        else {
            return NULL;
        }
    }

} // namespace avalanche

#endif

