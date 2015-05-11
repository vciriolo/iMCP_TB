
{

  TFile *inFile = new TFile ("ntuples/reco_HVScan1.root");
  TTree* nt = (TTree*)inFile->Get("reco_tree");
  TGraph* g = new TGraph();

  TLine* line = new TLine(-1.749, 0.1, -1.749, 0.85);


  for (int i=0; i<64; i++) {

    char h_sig_name[100]="";
    char h_trig0_name[100]="";
    sprintf(h_sig_name, "h_sig_%d", i);
    sprintf(h_trig0_name, "h_trig0_%d", i);

    TH1F* h_sig= new TH1F(h_sig_name, h_sig_name, 500, -5000, 25000);
    TH1F* h_trig0 = new TH1F(h_trig0_name, h_trig0_name, 500, -5000, 25000);
    
    char MCPCut[500]="charge_corr[0]>200 && charge_corr[2]>179 && nhodoX1<=3 && nhodoX2<=3 && nhodoY1<=3 && nhodoY2<=3 && HV[2]==2200";
    char trigCut[500]="charge_corr[0]>200 && nhodoX1<=3 && nhodoX2<=3 && nhodoY1<=3 && nhodoY2<=3 && HV[2]==2200";
    char hodoCut[200]="";
    sprintf(hodoCut, "hodoX2[%d]==1",i);

    char sigName[200] = "";
    char trigName[200] = "";
    sprintf(sigName, "charge_corr[2]>>%s",h_sig_name);
    sprintf(trigName, "charge_corr[0]>>%s",h_trig0_name);

    nt->Draw(sigName,TCut(MCPCut) && TCut(trigCut) && TCut(hodoCut),"goff");
    nt->Draw(trigName,TCut(trigCut) && TCut(hodoCut),"goff");

    std::cout<<h_sig->GetEntries()<<"/"<<(float)h_trig0->GetEntries()<<" eff: "<<h_sig->GetEntries()/(float)h_trig0->GetEntries()<<std::endl;
    if (h_sig->GetEntries()>1000)
      g->SetPoint(i,i-32,h_sig->GetEntries()/(float)h_trig0->GetEntries());

    }
  g->SetMarkerStyle(20);
  g->SetMarkerSize(0.8);
  g->SetMaximum(0.85);
  g->SetMinimum(0.1);
  g->Draw("AP");
  g->GetXaxis()->SetTitle("X hodoscope coordinate (mm)");
  g->GetYaxis()->SetTitle("Efficiency");
  line->DrawLine(-1.749, 0.1, -1.749, 0.85);
  line->SetLineColor(2);
  line->Draw("same");
  
}
