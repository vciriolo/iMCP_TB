{

  //      std::string HVE = "EffAll";
        std::string HVE = "AngScanLow";
  //      std::string HVE = "HVL_50";
  //  std::string HVE = "HVL_20";
  //  std::string HVE = "HVH_20";
  //  std::string HVE = "EffAll";

  TFile* inF_MiB3_ON;
  TFile* inF_MiB3_OFF;
  TFile* inF_enSEE;
  TFile* inF_ZStack1_ON;
  TFile* inF_ZStack1_OFF;
  TFile* inF_ZStack2_ON;
  TFile* inF_ZStack2_OFF;

  TMultiGraph *mg = new TMultiGraph();

  TLegend *legC = new TLegend(0.50,0.20,0.79,0.4,NULL,"brNDC");
  legC->SetTextFont(42);
  legC->SetTextSize(0.03);
  legC->SetFillColor(kWhite);
  legC->SetLineColor(kWhite);
  legC->SetShadowColor(kWhite);

  TCanvas* c_HVH_50 = new TCanvas();

  if(HVE == "EffAll"){
    inF_MiB3_ON = TFile::Open("plots/time_resolution/time_MiB3_time_HV_HVScan2.root");
    inF_MiB3_OFF = TFile::Open("plots/time_resolution/time_MiB3_time_HV_HVScan7.root");
    inF_enSEE = TFile::Open("plots/time_resolution/time_enSEE_time_HV_HVScan5.root");
    inF_ZStack1_ON = TFile::Open("plots/time_resolution/time_ZStack1_time_HV_HVScan1.root");
    inF_ZStack1_OFF = TFile::Open("plots/time_resolution/time_ZStack1_time_HV_HVScan8.root");
    inF_ZStack2_ON = TFile::Open("plots/time_resolution/time_ZStack2_time_HV_HVScan2.root");
    inF_ZStack2_OFF = TFile::Open("plots/time_resolution/time_ZStack2_time_HV_HVScan6.root");

    TGraphErrors* eff_MiB3_ON = (TGraphErrors*)inF_MiB3_ON->Get("eff");
    TGraphErrors* eff_MiB3_OFF = (TGraphErrors*)inF_MiB3_OFF->Get("eff");
    TGraphErrors* eff_enSEE = (TGraphErrors*)inF_enSEE->Get("eff");
    TGraphErrors* eff_ZStack2_ON = (TGraphErrors*)inF_ZStack2_ON->Get("eff");
    TGraphErrors* eff_ZStack1_ON = (TGraphErrors*)inF_ZStack1_ON->Get("eff");
    //     TGraphErrors* eff_ZStack2_OFF = (TGraphErrors*)inF_ZStack2_OFF->Get("eff");
    TGraphErrors* eff_ZStack1_OFF = (TGraphErrors*)inF_ZStack1_OFF->Get("eff");
    TGraphErrors* eff_ZStack2_OFF = (TGraphErrors*)inF_ZStack2_OFF->Get("eff");
  

  //settings
  eff_MiB3_ON->SetMarkerColor(kGreen+1);
  eff_MiB3_ON->SetLineColor(kGreen+1);
  eff_MiB3_OFF->SetMarkerColor(kGreen+1);
  eff_MiB3_OFF->SetLineColor(kGreen+1);
  eff_ZStack2_ON->SetMarkerColor(kBlue);
  eff_ZStack2_ON->SetLineColor(kBlue);
  eff_ZStack1_ON->SetMarkerColor(kRed);
  eff_ZStack1_ON->SetLineColor(kRed);
  eff_ZStack2_OFF->SetMarkerColor(kBlue);
  eff_ZStack2_OFF->SetLineColor(kBlue);
  eff_ZStack1_OFF->SetMarkerColor(kRed);
  eff_ZStack1_OFF->SetLineColor(kRed);
  eff_enSEE->SetMarkerColor(1);
  eff_enSEE->SetLineColor(1);
  //
  eff_MiB3_ON->SetMarkerStyle(20);
  eff_MiB3_ON->SetLineWidth(2);
  eff_MiB3_OFF->SetMarkerStyle(22);
  eff_MiB3_OFF->SetLineWidth(2);
  eff_ZStack2_ON->SetMarkerStyle(20);
  eff_ZStack2_ON->SetLineWidth(2);
  eff_ZStack1_ON->SetMarkerStyle(20);
  eff_ZStack1_ON->SetLineWidth(2);
  eff_ZStack2_OFF->SetMarkerStyle(22);
  eff_ZStack2_OFF->SetLineWidth(2);
  eff_ZStack1_OFF->SetMarkerStyle(22);
  eff_ZStack1_OFF->SetLineWidth(2);
  eff_enSEE->SetMarkerStyle(22);
  eff_enSEE->SetLineWidth(2);



  legC->AddEntry(eff_MiB3_ON, "MiB3 chevron-ON", "p");
  legC->AddEntry(eff_ZStack2_ON, "ZStack2 ON", "p");
  legC->AddEntry(eff_ZStack1_ON, "ZStack1 ON", "p");
  /*  legC->AddEntry(eff_MiB3_OFF, "MiB3 chevron-OFF", "p");
  legC->AddEntry(eff_ZStack2_OFF, "ZStack2 OFF", "p");
  legC->AddEntry(eff_ZStack1_OFF, "ZStack1 OFF", "p");
  legC->AddEntry(eff_enSEE, "SEE OFF", "p");
  */
  gPad->SetTicks();
  if(HVE == "HVH_50" || "HVL_50" || "EffAll")  eff_enSEE->SetTitle("50 GeV");
  if(HVE == "HVH_20" || "HVL_20")  eff_enSEE->SetTitle("50 GeV");

  eff_MiB3_OFF->GetXaxis()->SetTitle("Angle (degrees)");
  eff_MiB3_OFF->GetYaxis()->SetTitle("Time_Res (Ps)");
  eff_enSEE->GetXaxis()->SetTitle("Angle (degrees)");
  eff_enSEE->GetYaxis()->SetTitle("Time_Res (Ps)");
  eff_ZStack1_OFF->GetXaxis()->SetTitle("Angle (degrees)");
  eff_ZStack1_OFF->GetYaxis()->SetTitle("Time_Res (Ps)");
  
  mg->Add(eff_MiB3_ON);
  mg->Add(eff_ZStack1_ON);
  mg->Add(eff_ZStack2_ON);
  /*  mg->Add(eff_MiB3_OFF);
  mg->Add(eff_ZStack1_OFF);
  mg->Add(eff_ZStack2_OFF);
  mg->Add(eff_enSEE);
  */
  mg->Draw("AP");
  mg->GetXaxis()->SetRangeUser(-0.1,5.1);
  mg->SetMaximum(200);
  mg->SetMinimum(0);
  mg->Draw("AP");  
  legC->Draw("same");
  }

  else {
  if(HVE == "AngScanHigh"){
  inF_MiB3 = TFile::Open("plots/time_resolution/time_MiB3_time_X0_AngScan_HVHigh.root");
  inF_enSEE = TFile::Open("plots/time_resolution/time_enSEE_time_X0_AngScan_HVHigh.root");
  inF_ZStack1 = TFile::Open("plots/time_resolution/time_ZStack1_time_X0_AngScan_HVHigh.root");
  }
  if(HVE == "AngScanLow"){
  inF_MiB3 = TFile::Open("plots/time_resolution/time_MiB3_time_X0_AngScan_HVLow.root");
  inF_enSEE = TFile::Open("plots/time_resolution/time_enSEE_time_X0_AngScan_HVLow.root");
  inF_ZStack1 = TFile::Open("plots/time_resolution/time_ZStack1_time_X0_AngScan_HVLow.root");
  }

  if(HVE == "HVH_50"){
  inF_MiB3 = TFile::Open("plots/time_resolution/time_MiB3_time_X0_scanX0_HVHigh50GeV.root");
  inF_enSEE = TFile::Open("plots/time_resolution/time_enSEE_time_X0_scanX0_HVHigh50GeV.root");
  inF_ZStack2 = TFile::Open("plots/time_resolution/time_ZStack2_time_X0_scanX0_HVHigh50GeV.root");
  inF_ZStack1 = TFile::Open("plots/time_resolution/time_ZStack1_time_X0_scanX0_HVHigh50GeV.root");
  }
  if(HVE == "HVH_20"){
  inF_MiB3 = TFile::Open("plots/time_resolution/time_MiB3_time_X0_scanX0_HVHigh20GeV.root");
  inF_enSEE = TFile::Open("plots/time_resolution/time_enSEE_time_X0_scanX0_HVHigh20GeV.root");
  inF_ZStack2 = TFile::Open("plots/time_resolution/time_ZStack2_time_X0_scanX0_HVHigh20GeV.root");
  inF_ZStack1 = TFile::Open("plots/time_resolution/time_ZStack1_time_X0_scanX0_HVHigh20GeV.root");
  }
  if(HVE == "HVL_50"){
  inF_MiB3 = TFile::Open("plots/time_resolution/time_MiB3_time_X0_scanX0_HVLow50GeV.root");
  inF_enSEE = TFile::Open("plots/time_resolution/time_enSEE_time_X0_scanX0_HVLow50GeV.root");
  inF_ZStack2 = TFile::Open("plots/time_resolution/time_ZStack2_time_X0_scanX0_HVLow50GeV.root");
  inF_ZStack1 = TFile::Open("plots/time_resolution/time_ZStack1_time_X0_scanX0_HVLow50GeV.root");
  }
  if(HVE == "HVL_20"){
  inF_MiB3 = TFile::Open("plots/time_resolution/time_MiB3_time_X0_scanX0_HVLow20GeV.root");
  inF_enSEE = TFile::Open("plots/time_resolution/time_enSEE_time_X0_scanX0_HVLow20GeV.root");
  inF_ZStack2 = TFile::Open("plots/time_resolution/time_ZStack2_time_X0_scanX0_HVLow20GeV.root");
  inF_ZStack1 = TFile::Open("plots/time_resolution/time_ZStack1_time_X0_scanX0_HVLow20GeV.root");
  }


    TGraphErrors* eff_MiB3 = (TGraphErrors*)inF_MiB3->Get("eff");
    TGraphErrors* eff_enSEE = (TGraphErrors*)inF_enSEE->Get("eff");
    TGraphErrors* eff_ZStack1 = (TGraphErrors*)inF_ZStack1->Get("eff");
    if (HVE=="HVL_20" || HVE=="HVH_20" || HVE=="HVH_50" || HVE=="HVL_50")
     TGraphErrors* eff_ZStack2 = (TGraphErrors*)inF_ZStack2->Get("eff");


  //settings
  eff_MiB3->SetMarkerColor(kGreen+1);
  eff_MiB3->SetLineColor(kGreen+1);
  if (HVE=="HVL_20" || HVE=="HVH_20" || HVE=="HVH_50" || HVE=="HVL_50") {
  eff_ZStack2->SetMarkerColor(kBlue);
  eff_ZStack2->SetLineColor(kBlue);
  }
  eff_ZStack1->SetMarkerColor(kRed);
  eff_ZStack1->SetLineColor(kRed);
  eff_enSEE->SetMarkerColor(1);
  eff_enSEE->SetLineColor(1);
  //
  eff_MiB3->SetMarkerStyle(20);
  eff_MiB3->SetLineWidth(2);
  if (HVE=="HVL_20" || HVE=="HVH_20" || HVE=="HVH_50" || HVE=="HVL_50") {
  eff_ZStack2->SetMarkerStyle(20);
  eff_ZStack2->SetLineWidth(2);
  }
  eff_ZStack1->SetMarkerStyle(20);
  eff_ZStack1->SetLineWidth(2);
  eff_enSEE->SetMarkerStyle(20);
  eff_enSEE->SetLineWidth(2);

  /*
  TLegend *legC = new TLegend(0.50,0.20,0.79,0.4,NULL,"brNDC");
  legC->SetTextFont(42);
  legC->SetTextSize(0.03);
  legC->SetFillColor(kWhite);
  legC->SetLineColor(kWhite);
  legC->SetShadowColor(kWhite);
  */

  if(HVE == "AngScanLow"){
  legC->AddEntry(eff_MiB3, "MiB3 chevron-OFF HV 3000", "p");
  legC->AddEntry(eff_ZStack1, "ZStack1 OFF HV 3000", "p");
  legC->AddEntry(eff_enSEE, "SEE OFF HV 1900", "p");
  }
  if(HVE == "AngScanHigh"){
  legC->AddEntry(eff_MiB3, "MiB3 chevron-OFF HV 3200", "p");
  legC->AddEntry(eff_ZStack1, "ZStack1 OFF HV 3200", "p");
  legC->AddEntry(eff_enSEE, "SEE OFF HV 2200", "p");
  }


  if(HVE == "HVH_50" || HVE == "HVH_20"){
  legC->AddEntry(eff_MiB3, "MiB3 chevron-OFF HV 3200", "p");
  legC->AddEntry(eff_ZStack1, "ZStack2 OFF HV 3200", "p");
  legC->AddEntry(eff_ZStack2, "ZStack1 OFF HV 3200", "p");
  legC->AddEntry(eff_enSEE, "SEE OFF  HV 2200", "p");
  }

  if(HVE == "HVL_50" || HVE == "HVL_20"){
  legC->AddEntry(eff_MiB3, "MiB3 chevron-OFF HV 3000", "p");
  legC->AddEntry(eff_ZStack1, "ZStack2 OFF HV 3000", "p");
  legC->AddEntry(eff_ZStack2, "ZStack1 OFF HV 3000", "p");
  legC->AddEntry(eff_enSEE, "SEE OFF  HV 1900", "p");
  }

  //  TCanvas* c_HVH_50 = new TCanvas();
  gPad->SetTicks();
  
  mg->Add(eff_MiB3);
  mg->Add(eff_ZStack1);
  mg->Add(eff_enSEE);
  if (HVE=="HVL_20" || HVE=="HVH_20" || HVE=="HVH_50" || HVE=="HVL_50") 
   mg->Add(eff_ZStack2);

  mg->Draw("AP");
    if (HVE=="HVL_20" || HVE=="HVH_20" || HVE=="HVH_50" || HVE=="HVL_50")
  mg->GetXaxis()->SetRangeUser(-0.1,7.1);
  mg->SetMaximum(200);
  mg->SetMinimum(0);
  mg->Draw("AP");

  legC->Draw("same");
  //  c_HVH_50->Print("plots/X0scan/");
  }
}
