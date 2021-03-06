#include "tracklet_em_2.h"
#include "TruthMatchEff.h"
#include "PUAnalyzer.h"
#include <algorithm>
//  for 1trk, file called "trk_output_v2.txt" should contain all event data in format:
//"Event" [2 spaces] # [3 spaces] jet pT [3 spaces] jet eta [3 spaces] jet phi  (for both jets)
// pT [3 spaces] eta [3 spaces] phi [3 spaces] z     (for each track)
//
// For example: 
// 
// Event  3   173.579330444   0.999113619328   2.01476287842
// Event  3   173.579330444   -0.999113619328   -1.12682986259
// 19.2072582245   0.990153074265   2.02103352547   -3.41288733482
// ...
// Then, next event.
//
//  for tp, file called "tp_output_v2.txt" in same format
//Arguments: argv[1]: 1 for 1trk, 2 for tp. argv[2]: number of zbins (default 64).
int main(int argc, char ** argv){
   //Numbers to specify which events to start and finish on.
	int eventstart = 1000;
	int eventend = 2000;      //if higher than number of events, will end with last event.
   
   //If second number specified it is number of zbins
   //Default 64.
	string nz = "64";
	int nzbins = 64;
	int start=0;
	int end=0;
	if(argc >= 3){
		//nz = argv[2];
		eventstart = atoi(argv[2]);
		eventend = atoi(argv[3]);
	}
	std::cout<<"Start "<<eventstart<<" End "<<eventend<<std::endl;
    //Name of input file (1st argument: 1 is 1trk, 2 is tp).	
	string filename;
        if(atoi(argv[1]) == 1){
 	    filename = "trk_output_v2.txt";
        }
        else if(atoi(argv[1]) == 2){
            filename = "tp_output_v2L1Rate.txt";
        } 
        else {
            cout << "Error: please specify 1 for 1trk or 2 for tp."<<endl;
            exit(0);
        }

	struct track_data * tracks = (struct track_data *)malloc(numtracks * sizeof(struct track_data));
	string data_in;
  //Open input file and all output files.
	ifstream in_tracks;
	in_tracks.open(filename.c_str());
	ofstream out_clusts;
	string outname;
        if(atoi(argv[1]) == 1){
             outname = "em_out_trk_" + nz + "z.txt";
        } 
        else {
             outname = "em_out_tp_" + nz + "Merged.txt";
        }
	outname=outname+argv[2];
	out_clusts.open(outname.c_str());
	ofstream plot1dat;
	string plotname = "plot1dat_" + nz + "z.dat";
	plot1dat.open(plotname.c_str());
	ofstream plot2dat;
	plotname = "plot2dat_" + nz + "z.dat";
	plot2dat.open(plotname.c_str());
	ofstream ptdat;
	plotname = "ptplot_" + nz + "z.dat";
	ptdat.open(plotname.c_str());
	ofstream traxdat;
	plotname = "ntracks_" + nz + "z.dat";
	traxdat.open(plotname.c_str());
	ofstream disdat;
	plotname = "distance_" + nz + "z.dat";
	disdat.open(plotname.c_str());
	string rootname=outname+".root";
	TFile fout(rootname.c_str(),"RECREATE");
	TH1F DeltaZ("DeltaZ", "#Delta Z (L1 PV , HTz)", 100, -5, 5);
        TH2F JetMultiplicity("JetMultiplicity", "Number of Clus ", 20, 0, 20,20, 0, 20);	
	TH1F NumClusters("NumClusters", "Number of Clus ", 100, 0, 100);
	TH1F  dRMatchFastjet(" dRMatchFastjet", "#Delta R (Cluster, Fast Jet)", 100, 0.0, 1.0);
	TH1F dRMatch("dRMatch", "#Delta R (Cluster, Gen Jet)", 100, 0.0, 1.0);
	TH1F DetaRes("DetaRes", "Delta #eta res ", 100, -0.5, 0.5);
	TH1F DphiRes("DphiRes", "Delta #phi res ", 100, -0.5, 0.5);
	TH2F DistToClus("DistToClus", "#Delta #eta/#phi (Cluster, Gen Jet)", 100, -0.5, 0.5, 100, -0.5,0.5);
	TH1F PtRatio("PtRatio", "Energy Ratio Track Jets / Gen Jets ", 100, 0.,1.);
	TH1F GenJetEffNum("GenJetEffNum", "Gen Jet Match Efficiency", 60, 0, 300);
	TH1F GenJetEffDen("GenJetEffDen", "Gen Jet Match Efficiency", 60, 0, 300);
	TH1F GenJetEtaEffNum("GenJetEtaEffNum", "Gen Jet Match Efficiency", 50, -2.5, 2.5);
	TH1F GenJetEtaEffDen("GenJetEtaEffDen", "Gen Jet Match Efficiency", 50, -2.5, 2.5);
	TH1F GenJetNtracksEffNum("GenJetNtracksEffNum", "Gen Jet Match Efficiency", 60, 0, 300);
	TH1F GenJetNtracksEffDen("GenJetNtracksEffDen", "Gen Jet Match Efficiency", 60, 0, 300);
	TH1F GenJetNtracksEtaEffNum("GenJetNtracksEtaEffNum", "Gen Jet Match Efficiency", 50, -2.5, 2.5);
	TH1F GenJetNtracksEtaEffDen("GenJetNtracksEtaEffDen", "Gen Jet Match Efficiency", 50, -2.5, 2.5);
	TH1D ClusterMHTNtracks("ClusterMHTNtracks","H_{T}",100, 0, 2000);	
	TH1D ClusterMHT("ClusterMHT","H_{T}",100, 0, 2000);	
	TH1D ClusterHTNtracks("ClusterHTNtracks","H_{T}",100, 0, 2000);	
	TH1D ClusterHT("ClusterHT","H_{T}",100, 0, 2000);	
	TH1D ClusterLeadJet("ClusterLeadJet","H_{T}",100, 0, 500);	
	TH1D ClusterDiJet("ClusterDiJet","H_{T}",100, 0, 500);	
	TH1D ClusterQuadJet("ClusterQuadJet","H_{T}",100, 0, 500);	
	TH1D ClusterLeadNtracks("ClusterLeadNtracks","H_{T}",100, 0, 500);	
	TH1D ClusterDiNtracks("ClusterDiNtracks","H_{T}",100, 0, 500);	
	TH1D ClusterQuadNtracks("ClusterQuadNtracks","H_{T}",100, 0, 500);	
	
    TH1D*hGenJetHTPass200=new TH1D("hGenJetHTPass200", "Gen H_{T}",100, 0, 2000);
    TH1D*hGenJetHTPass325=new TH1D("hGenJetHTPass325", "Gen H_{T}",100, 0, 2000);
    TH1D*hGenJetHTPass250=new TH1D("hGenJetHTPass250", "Gen H_{T}",100, 0, 2000);

    TH1D*hGenJetRecoHTPass160=new TH1D("hGenJetRecoHTPass160", "Gen H_{T}",100, 0, 2000);
    TH1D*hGenJetRecoHTPass200=new TH1D("hGenJetRecoHTPass200", "Gen H_{T}",100, 0, 2000);
    TH1D*hGenJetRecoHTPass325=new TH1D("hGenJetRecoHTPass325", "Gen H_{T}",100, 0, 2000);
    TH1D*hGenJetRecoHTPass250=new TH1D("hGenJetRecoHTPass250", "Gen H_{T}",100, 0, 2000);

    TH1D*hGenJetRecoMHTPass150=new TH1D("hGenJetRecoMHTPass150", "Gen H_{T}",100, 0, 2000);
    TH1D*hGenJetRecoMHTPass100=new TH1D("hGenJetRecoMHTPass100", "Gen H_{T}",100, 0, 2000);

    TH1D*hGenJetMHT=new TH1D("hGenJetMHT", "Gen H_{T}",100, 0, 2000);
    TH1D*hGenJetHT=new TH1D("hGenJetHT", "Gen H_{T}",100, 0, 2000);



	PUAnalyzer MinBiasEvents;
	MinBiasEvents.Loop();	
	TruthMatchEff EventClass;
	EventClass.Loop();	
	int nevents = 0;
	getline(in_tracks, data_in, ' ');
        string data;
	float distance;
        for (Long64_t jentry=eventstart; jentry<eventend;jentry++) {
//	for (Long64_t jentry=eventstart; jentry<MinBiasEvents.GetNevents();jentry++) {
		MinBiasEvents.GetEntry(jentry);
		struct mc_data * mcdat = (struct mc_data *)malloc(10*sizeof(struct mc_data));
               		int ntracks=0;
			for(unsigned int t=0; t<MinBiasEvents.trk_pt->size(); ++t){
				if(MinBiasEvents.trk_z0->at(t)!=MinBiasEvents.trk_z0->at(t))continue;
				//if(MinBiasEvents.trk_eventid->at(t)>0)continue;
				//saturate pT here	
				tracks[ntracks].pT =MinBiasEvents.trk_pt->at(t);
				if(MinBiasEvents.trk_pt->at(t)>200)tracks[ntracks].pT =200;
				tracks[ntracks].eta =MinBiasEvents.trk_eta->at(t);
				tracks[ntracks].phi =MinBiasEvents.trk_phi->at(t);
				tracks[ntracks].z =MinBiasEvents.trk_z0->at(t);	
				tracks[ntracks].bincount = 0;
				if(tracks[ntracks].pT >= 2.0 && fabs(tracks[ntracks].eta) < 2.4 && fabs(tracks[ntracks].z) < 15.0  && MinBiasEvents.trk_chi2->at(t)<5 && MinBiasEvents.trk_nstub->at(t)>4){
				++ntracks;		
				}
			}
			//std::cout<<"ntracks "<<jentry<<std::endl;
	         mcdat->ntracks = ntracks;
		//if(ntracks<1)continue;
             	struct maxzbin * mzb = L2_cluster(tracks, mcdat, nzbins,ntracks);	
		float HTSum=0;
		float HTSumNtracks=0;
		int JetsNtracks=0;
	        TLorentzVector MHT,MHTNtracks;	
		std::vector<float>clusters;
		std::vector<float>clustersNtracks;
		for(int k = 0; k < mzb->nclust; ++k){
	 		if(mzb->clusters[k].pTtot<10)continue;
			HTSum=HTSum+(mzb->clusters[k].pTtot);
			clusters.push_back(mzb->clusters[k].pTtot);
			if(mzb->clusters[k].numtracks<2)continue;
			TLorentzVector temp(mzb->clusters[k].pTtot, mzb->clusters[k].eta, mzb->clusters[k].phi, mzb->clusters[k].pTtot *cosh(mzb->clusters[k].eta));
			MHT=MHT+temp;
			HTSumNtracks=HTSumNtracks+mzb->clusters[k].pTtot;
			clustersNtracks.push_back(mzb->clusters[k].pTtot);
			MHTNtracks=MHTNtracks+temp;
		}
		std::sort(clusters.begin(), clusters.end());
		std::sort(clustersNtracks.begin(), clustersNtracks.end());
		//for(int k = 0; k < clusters.size(); ++k)std::cout<<"Pt of Jets "<<clusters[k]<<std::endl;
		if(clusters.size()>0)ClusterLeadJet.Fill(clusters[clusters.size()-1]);
		else ClusterLeadJet.Fill(0);
		if(clusters.size()>1)ClusterDiJet.Fill(clusters[clusters.size()-2]);
		else ClusterDiJet.Fill(0);
		if(clusters.size()>3)ClusterQuadJet.Fill(clusters[clusters.size()-4]);
		else ClusterQuadJet.Fill(0);
	
		if(clustersNtracks.size()>0)ClusterLeadNtracks.Fill(clustersNtracks[clustersNtracks.size()-1]);
		else ClusterLeadNtracks.Fill(0);
		if(clustersNtracks.size()>1)ClusterDiNtracks.Fill(clustersNtracks[clustersNtracks.size()-2]);
		else ClusterDiNtracks.Fill(0);
		if(clustersNtracks.size()>3)ClusterQuadNtracks.Fill(clustersNtracks[clustersNtracks.size()-4]);
		else ClusterQuadNtracks.Fill(0);
		
		if(HTSum>1200){
			std::cout<<"Difference in L1 Z "<<MinBiasEvents.pv_L1reco->at(0)-mzb->zcenter<<std::endl;
			for(int k = 0; k < mzb->nclust; ++k){
	 			if(mzb->clusters[k].pTtot<10)continue;
				std::cout<<"Clusters for high HT Event "<<mzb->clusters[k].pTtot<<", "<<mzb->clusters[k].eta<<", "<<mzb->clusters[k].phi<<std::endl;
			}
			std::cout<<"reco jet size "<<MinBiasEvents.recojet_vz->size()<<std::endl;	
		        for(unsigned int j=0; j<MinBiasEvents.recojet_vz->size(); ++j){
           			if(MinBiasEvents.recojet_pt->at(j)<10)continue;
           			if(fabs(MinBiasEvents.recojet_eta->at(j))>2.4)continue;	
				std::cout<<"Jet Min Bias for high HT Event "<<MinBiasEvents.recojet_pt->at(j)<<", "<<MinBiasEvents.recojet_eta->at(j)<<", "<<MinBiasEvents.recojet_phi->at(j)<<std::endl;
			}
		}
		ClusterMHT.Fill(MHT.Pt());
		ClusterMHTNtracks.Fill(MHTNtracks.Pt());
		ClusterHT.Fill(HTSum);
		ClusterHTNtracks.Fill(HTSumNtracks);
		free(mcdat);
          //      free(mzb->mcd);
          //      free(mzb->clusters);
                //free(mzb);
	for(int i = 0; i < nzbins-1; i++){	
	  if(&all_zbins[i] == NULL || &all_zbins[i].mcd == NULL) continue;
	    free(all_zbins[i].clusters);
	}	  
	free(all_zbins);

	}
	//for (Long64_t jentry=0; jentry<EventClass.GetNevents();jentry++) {
	for (Long64_t jentry=eventstart; jentry<eventend;jentry++){
		EventClass.GetEntry(jentry);
		struct mc_data * mcdat = (struct mc_data *)malloc(10*sizeof(struct mc_data));
		if(EventClass.MC_lep->at(0)!=0)continue;
			//std::cout<<"L1 PV z "<<EventClass.pv_L1->at(0)<<std::endl;
		int ntp=0;
		       for (int g=0; g<EventClass.genjetak4_phi->size(); ++g){
				mcdat[ntp].ogpt =EventClass.genjetak4_pt->at(g);
				mcdat[ntp].ogeta =EventClass.genjetak4_eta->at(g);
				mcdat[ntp].ogphi =EventClass.genjetak4_phi->at(g);
				if(mcdat[ntp].ogpt>30 && fabs(mcdat[ntp].ogeta)<2.2)++ntp;
			}	
	  			out_clusts << "\n****EVENT " << jentry << " *** (" << ntp << " tracking particles)" << endl;
				for(int i = 0; i < ntp; ++i){
	   				out_clusts << i << " pT: " << mcdat[i].ogpt << " eta: " << mcdat[i].ogeta << " phi: " << mcdat[i].ogphi << endl;
				}
		        for(int i = 0; i < ntp; ++i){	
				GenJetEffDen.Fill(mcdat[i].ogpt);	
				GenJetEtaEffDen.Fill(mcdat[i].ogeta);
			}
		 	if(mcdat==NULL)continue;
			int ntracks=0;
			for(unsigned int t=0; t<EventClass.trk_pt->size(); ++t){
				if(EventClass.trk_z0->at(t)!=EventClass.trk_z0->at(t))continue;
				//if(fabs(EventClass.trk_z0->at(t)-EventClass.pv_L1->at(0))>1.0)continue;
				//if(EventClass.trk_eventid->at(t)>0)continue;
				//saturate pT here	
				tracks[ntracks].pT =EventClass.trk_pt->at(t);
				if(EventClass.trk_pt->at(t)>200)tracks[ntracks].pT =200;
				tracks[ntracks].eta =EventClass.trk_eta->at(t);
				tracks[ntracks].phi =EventClass.trk_phi->at(t);
				tracks[ntracks].z =EventClass.trk_z0->at(t);	
				tracks[ntracks].bincount = 0;
				if(tracks[ntracks].pT >= 2.0 && fabs( tracks[ntracks].eta) < 2.4 && fabs(tracks[ntracks].z) < 15.0  
				&& EventClass.trk_nstub->at(t)>4  
				//&& EventClass.tp_nstublayers->at(t)>4
				&& EventClass.trk_chi2->at(t)<5){
				++ntracks;		
				}
			}
			//std::cout<<"ntracks "<<jentry<<std::endl;
	         mcdat->ntracks = ntracks;
		if(ntracks<1)continue;
             	 struct maxzbin * mzb = L2_cluster(tracks, mcdat, nzbins,ntracks);	
		std::cout<<"Event "<<jentry<<std::endl;
             	 if(mzb == NULL){
			//std::cout<<"No Clusters "<<std::endl;
                	continue;
             	}

                out_clusts << "ZBIN: " << mzb->znum << endl;
		DeltaZ.Fill(mzb->zcenter-EventClass.pv_MC->at(0));
                //cout << "ZBIN: " << mzb->zcenter << endl;
		int ClusterCount=0;
                for(int k = 0; k < mzb->nclust; ++k){
			bool matched = false;
			if(mzb->clusters[k].pTtot>10)++ClusterCount;
         		for(int b = 0; b < ntp; b++){
               		//Match clusters with correct input jet (and ignore the garbage clusters.)
               		// Only accept cluster if it is within .3 (in eta-phi space) of one of the jets.
               	    
		   		distance = sqrt(pow(mzb->mcd[b].ogphi - mzb->clusters[k].phi, 2) + pow(mzb->mcd[b].ogeta - mzb->clusters[k].eta, 2));
	   			if (distance < 0.3){
	     				plot1dat << mzb->mcd[b].ogphi << "\t" << mzb->clusters[k].phi << endl;
					plot2dat << mzb->mcd[b].ogeta << "\t" << mzb->clusters[k].eta << endl;
					ptdat << mzb->mcd[b].ogpt << "\t" << mzb->clusters[k].pTtot << endl;
					traxdat << ntracks << "\t" <<  mzb->clusters[k].numtracks << endl;
					disdat << mzb->mcd[b].ogpt << "\t" << distance << endl;
					out_clusts << "   CLUSTER " << k << "(" << mzb->clusters[k].numtracks << " tracks)\t MC data \t Matched cluster data" << endl;
					out_clusts << "   phi \t\t\t" << mzb->mcd[b].ogphi << "\t\t" << mzb->clusters[k].phi << endl;
					out_clusts << "   eta \t\t\t" << mzb->mcd[b].ogeta << "\t\t" << mzb->clusters[k].eta << endl;
					out_clusts << "   pT \t\t\t" << mzb->mcd[b].ogpt << "\t\t" << mzb->clusters[k].pTtot << endl;
					matched = true;
					PtRatio.Fill(mzb->clusters[k].pTtot/ mzb->mcd[b].ogpt);
					break;
	     	   		} 
                	}//for each input jet
			
	        	if(matched)continue;
			out_clusts << "  (Unmatched) CLUSTER " << k << "(" << mzb->clusters[k].numtracks << " tracks)" << endl;
			out_clusts << "   phi \t\t\t" << "\t\t" << mzb->clusters[k].phi << endl;
			out_clusts << "   eta \t\t\t" << mzb->clusters[k].eta << endl;
			out_clusts << "   pT \t\t\t"  << mzb->clusters[k].pTtot << endl;
	        } //for each cluster
	 int fastJetCount=0;
                        for(unsigned int i=0; i<EventClass.recojet_pt->size(); ++i)if(EventClass.recojet_pt->at(i)>10)++fastJetCount;
		                JetMultiplicity.Fill(fastJetCount,ClusterCount);	
		 NumClusters.Fill(ClusterCount);
			//float TwoLayerHT=0;	
                        for(int k = 0; k < mzb->nclust; ++k){
                                if( mzb->clusters[k].pTtot<10)continue;
				//TwoLayerHT=TwoLayerHT+mzb->clusters[k].pTtot;
                                float dRMin=9999;
                                for(unsigned int i=0; i<EventClass.recojet_pt->size(); ++i){
                                        if(EventClass.recojet_pt->at(i)<10)continue;
                                        float deta=EventClass.recojet_eta->at(i) - mzb->clusters[k].eta;
                                        float dphi=EventClass.recojet_phi->at(i) - mzb->clusters[k].phi;
                                        if(sqrt(deta*deta + dphi*dphi)<dRMin)dRMin=sqrt(deta*deta + dphi*dphi);

                                }
                                dRMatchFastjet.Fill(dRMin);
                        }	
			float GenJetHT=0;
     	TLorentzVector GenMHT;	
	   for(int b = 0; b < ntp; b++){//for each gen jet
			float mindistance=999.;
			float mineta=999.;
			float minphi=999.;
			if(fabs(mzb->mcd[b].ogeta)>2.4)continue;
			if(mzb->mcd[b].ogpt>30){
				GenJetHT=GenJetHT+mzb->mcd[b].ogpt;
                TLorentzVector temp(mzb->mcd[b].ogpt,mzb->mcd[b].ogeta, mzb->mcd[b].ogphi, mzb->mcd[b].ogpt *cosh(mzb->mcd[b].ogeta));
				GenMHT=GenMHT+temp;
			}
			for(int k = 0; k < mzb->nclust; ++k){
				distance = sqrt(pow(mzb->mcd[b].ogphi - mzb->clusters[k].phi, 2) + pow(mzb->mcd[b].ogeta - mzb->clusters[k].eta, 2));	
				if(mindistance>distance){
				mindistance=distance;
				mineta=mzb->mcd[b].ogeta - mzb->clusters[k].eta;
				minphi=mzb->mcd[b].ogphi - mzb->clusters[k].phi;
				}
				if(distance<0.3 && mzb->clusters[k].pTtot>10){ 

					if(mzb->clusters[k].numtracks>1){
					GenJetNtracksEffNum.Fill(mzb->mcd[b].ogpt);	
					GenJetNtracksEtaEffNum.Fill(mzb->mcd[b].ogeta);

					}
					GenJetEffNum.Fill(mzb->mcd[b].ogpt);	
					GenJetEtaEffNum.Fill(mzb->mcd[b].ogeta);
					break;
		    		}	
	     		}
			dRMatch.Fill(mindistance);
			DphiRes.Fill(minphi);	
			DetaRes.Fill(mineta);	
			DistToClus.Fill(mineta,minphi);
	  	}
	hGenJetMHT->Fill(GenMHT.Pt());
	hGenJetHT->Fill(GenJetHT);
	float HT=0;
	float SingleJetPt=0;
	float QuadJetPt=0;
	float DiJetPt=0;
                TLorentzVector MHT;
                std::vector<float>clusters;
                for(int k = 0; k < mzb->nclust; ++k){
                                if( mzb->clusters[k].pTtot<10)continue;
		if(mzb->clusters[k].numtracks>1){
		HT=HT+mzb->clusters[k].pTtot;
                TLorentzVector temp(mzb->clusters[k].pTtot, mzb->clusters[k].eta, mzb->clusters[k].phi, mzb->clusters[k].pTtot *cosh(mzb->clusters[k].eta));
		MHT=MHT+temp;
		clusters.push_back(mzb->clusters[k].pTtot);
		}
	}
                std::sort(clusters.begin(), clusters.end());
	if(MHT.Pt()>100)hGenJetRecoMHTPass100->Fill(GenMHT.Pt());
	if(MHT.Pt()>150)hGenJetRecoMHTPass150->Fill(GenMHT.Pt());
	if(HT>160)hGenJetRecoHTPass160->Fill(GenJetHT);
	if(HT>200)hGenJetRecoHTPass200->Fill(GenJetHT);
	if(HT>250)hGenJetRecoHTPass250->Fill(GenJetHT);
	if(HT>325)hGenJetRecoHTPass325->Fill(GenJetHT);
	for(int i = 0; i < nzbins-1; i++){	
	  if(&all_zbins[i] == NULL || &all_zbins[i].mcd == NULL) continue;
	    free(all_zbins[i].clusters);
	}	  
	free(all_zbins);

	//if(jentry%100==0)
	//free(tracks);
	}
        out_clusts << "****" << nevents << " events****" << endl;
	if(tracks!=NULL)free(tracks);
        in_tracks.close();
	out_clusts.close();
	plot1dat.close();
	plot2dat.close();
	ptdat.close();
	traxdat.close();
	disdat.close();
	cout << "5 new data files created." <<endl;
	fout.cd();
	dRMatch.Write("dRtoClus");
	DistToClus.Write("DistToClus");
	PtRatio.Write("PtRatio");
	TEfficiency effNtracks(GenJetNtracksEffNum,GenJetEffDen);
	TEfficiency eff(GenJetEffNum,GenJetEffDen);
	eff.Write("MatchingEfficiencyPt");
	effNtracks.Write("MatchingEfficiencyNtracksPt");
	TEfficiency effEtaNtracks(GenJetNtracksEtaEffNum,GenJetEtaEffDen);
	TEfficiency effEta(GenJetEtaEffNum,GenJetEtaEffDen);
	effEta.Write("MatchingEfficiencyEta");
	effEtaNtracks.Write("MatchingEfficiencyNtracksEta");
	ClusterMHT.Write("L1L2ClusterMHT");
	ClusterMHTNtracks.Write("L1L2ClusterMHTNtracks");
	ClusterHT.Write("L1L2ClusterHT");
	ClusterHTNtracks.Write("L1L2ClusterHTNtracks");
	ClusterLeadJet.Write("L1L2ClusterLeadJets");	
	ClusterDiJet.Write("L1L2ClusterDiJets");	
	ClusterQuadJet.Write("L1L2ClusterQuadJets");	
	ClusterLeadNtracks.Write("L1L2ClusterLeadJetNtracks");	
	ClusterDiNtracks.Write("L1L2ClusterDiJetNtracks");	
	ClusterQuadNtracks.Write("L1L2ClusterQuadJetNtracks");	
	        JetMultiplicity.Write("JetMultiplicityMap"); 
	NumClusters.Write("NumberofJets");
	DphiRes.Write("DeltaPhiRes");
	DetaRes.Write("DeltaEtaRes");
	DeltaZ.Write("ZVtxRes");
        dRMatchFastjet.Write("dRtoFastJet");
    TEfficiency*RecoThresh150MHT=new TEfficiency(*hGenJetRecoMHTPass150, *hGenJetMHT);
    TEfficiency*RecoThresh100MHT=new TEfficiency(*hGenJetRecoMHTPass100, *hGenJetMHT);
    TEfficiency*RecoThresh160=new TEfficiency(*hGenJetRecoHTPass160, *hGenJetHT);
    TEfficiency*RecoThresh200=new TEfficiency(*hGenJetRecoHTPass200, *hGenJetHT);
    TEfficiency*RecoThresh250=new TEfficiency(*hGenJetRecoHTPass250, *hGenJetHT);
    TEfficiency*RecoThresh325=new TEfficiency(*hGenJetRecoHTPass325, *hGenJetHT);
	RecoThresh150MHT->Write("RecoThresh150MHT");
	RecoThresh100MHT->Write("RecoThresh100MHT");
	RecoThresh160->Write("RecoThresh160HT");
	RecoThresh200->Write("RecoThresh200HT");
        RecoThresh250->Write("RecoThresh250HT");
        RecoThresh325->Write("RecoThresh325HT");
	fout.Close();

	/*
	delete GenJetEffNum;
	delete GenJetEffDen;
	delete dRMatch;
	delete DistToClus;
	delete PtRatio;
	*/
	return 0;
} //end main
