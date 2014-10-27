{

  //  std::string HVE = "HVH_50";
    std::string HVE = "EffAll";
  //  std::string HVE = "HVL_50";
  //  std::string HVE = "HVL_20";

  TFile* inF_MiB3_ON;
  TFile* inF_MiB3_OFF;
  TFile* inF_enSEE;
  TFile* inF_ZStack1_ON;
  TFile* inF_ZStack1_OFF;
  TFile* inF_ZStack2_ON;
  TFile* inF_ZStack2_OFF;

  if(HVE == "EffAll"){
    inF_MiB3_ON = TFile::Open("plots/efficiency/efficiency_MiB3_no_eff_HV_HVScan2.root");
    inF_MiB3_OFF = TFile::Open("plots/efficiency/efficiency_MiB3_no_eff_HV_HVScan3.root");
    inF_enSEE = TFile::Open("plots/efficiency/efficiency_enSEE_no_eff_HV_HVScan4.root");
    inF_ZStack1_ON = TFile::Open("plots/efficiency/efficiency_ZStack1_no_eff_HV_HVScan1.root");
    inF_ZStack1_OFF = TFile::Open("plots/efficiency/efficiency_ZStack1_no_eff_HV_HVScan5.root");
    inF_ZStack2_ON = TFile::Open("plots/efficiency/efficiency_ZStack2_no_eff_HV_HVScan2.root");
    inF_ZStack2_OFF = TFile::Open("plots/efficiency/efficiency_ZStack2_no_eff_HV_HVScan1.root");
  }

  if(HVE == "HVH_50"){
  inF_MiB3 = TFile::Open("plots/efficiency/efficiency_MiB3_no_eff_X0_H4test_X0scan_HVH_50GeV.root");
  inF_enSEE = TFile::Open("plots/efficiency/efficiency_enSEE_no_eff_X0_H4test_X0scan_HVH_50GeV.root");
  inF_ZStack2 = TFile::Open("plots/efficiency/efficiency_ZStack2_no_eff_X0_H4test_X0scan_HVH_50GeV.root");
  }
  if(HVE == "HVH_20"){
  inF_MiB3 = TFile::Open("plots/efficiency/efficiency_MiB3_no_eff_X0_H4test_X0scan_HVH_20GeV.root");
  inF_enSEE = TFile::Open("plots/efficiency/efficiency_enSEE_no_eff_X0_H4test_X0scan_HVH_20GeV.root");
  inF_ZStack2 = TFile::Open("plots/efficiency/efficiency_ZStack2_no_eff_X0_H4test_X0scan_HVH_20GeV.root");
  }
  if(HVE == "HVL_50"){
  inF_MiB3 = TFile::Open("plots/efficiency/efficiency_MiB3_no_eff_X0_H4test_X0scan_HVL_50GeV.root");
  inF_enSEE = TFile::Open("plots/efficiency/efficiency_enSEE_no_eff_X0_H4test_X0scan_HVL_50GeV.root");
  inF_ZStack2 = TFile::Open("plots/efficiency/efficiency_ZStack2_no_eff_X0_H4test_X0scan_HVL_50GeV.root");
  }
  if(HVE == "HVL_20"){
  inF_MiB3 = TFile::Open("plots/efficiency/efficiency_MiB3_no_eff_X0_H4test_X0scan_HVL_20GeV.root");
  inF_enSEE = TFile::Open("plots/efficiency/efficiency_enSEE_no_eff_X0_H4test_X0scan_HVL_20GeV.root");
  inF_ZStack2 = TFile::Open("plots/efficiency/efficiency_ZStack2_no_eff_X0_H4test_X0scan_HVL_20GeV.root");
  }


  TGraphErrors* eff_MiB3_ON = (TGraphErrors*)inF_MiB3_ON->Get("eff");
  TGraphErrors* eff_MiB3_OFF = (TGraphErrors*)inF_MiB3_OFF->Get("eff");
  TGraphErrors* eff_enSEE = (TGraphErrors*)inF_enSEE->Get("eff");
  TGraphErrors* eff_ZStack2_ON = (TGraphErrors*)inF_ZStack2_ON->Get("eff");
  TGraphErrors* eff_ZStack1_ON = (TGraphErrors*)inF_ZStack1_ON->Get("eff");
  TGraphErrors* eff_ZStack2_OFF = (TGraphErrors*)inF_ZStack2_OFF->Get("eff");
  TGraphErrors* eff_ZStack1_OFF = (TGraphErrors*)inF_ZStack1_OFF->Get("eff");

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


  TLegend *legC = new TLegend(0.50,0.20,0.79,0.4,NULL,"brNDC");
  legC->SetTextFont(42);
  legC->SetTextSize(0.03);
  legC->SetFillColor(kWhite);
  legC->SetLineColor(kWhite);
  legC->SetShadowColor(kWhite);

  if(HVE == "EffAll"){
  legC->AddEntry(eff_MiB3_OFF, "MiB3 chevron-OFF", "p");
  legC->AddEntry(eff_MiB3_ON, "MiB3 chevron-ON", "p");
  legC->AddEntry(eff_ZStack2_ON, "ZStack2 ON", "p");
  legC->AddEntry(eff_ZStack2_OFF, "ZStack2 OFF", "p");
  legC->AddEntry(eff_ZStack1_ON, "ZStack1 ON", "p");
  legC->AddEntry(eff_ZStack1_OFF, "ZStack1 OFF", "p");
  legC->AddEntry(eff_enSEE, "SEE OFF", "p");
  }


  if(HVE == "HVH_50" || HVE == "HVH_20"){
  legC->AddEntry(eff_MiB3, "MiB3 chevron-OFF HV 3100", "p");
  legC->AddEntry(eff_ZStack2, "ZStack2 OFF HV 3200", "p");
  legC->AddEntry(eff_enSEE, "SEE OFF  HV 2200", "p");
  }

  if(HVE == "HVL_50" || HVE == "HVL_20"){
  legC->AddEntry(eff_MiB3, "MiB3 chevron-OFF HV 2800", "p");
  legC->AddEntry(eff_ZStack2, "ZStack2 OFF HV 3000", "p");
  legC->AddEntry(eff_enSEE, "SEE OFF  HV 1900", "p");
  }

  TCanvas* c_HVH_50 = new TCanvas();
  gPad->SetTicks();
  if(HVE == "HVH_50" || "HVL_50" || "EffAll")  eff_MiB3_ON->SetTitle("50 GeV");
  if(HVE == "HVH_20" || "HVL_20")  eff_MiB3_ON->SetTitle("50 GeV");


  eff_MiB3_ON->GetXaxis()->SetLabelSize(0.05);
  eff_MiB3_ON->GetXaxis()->SetTitleSize(0.05);
 eff_MiB3_ON->GetYaxis()->SetLabelSize(0.05);
  eff_MiB3_ON->GetYaxis()->SetTitleSize(0.05);
  eff_MiB3_ON->SetMaximum(1.05);
  eff_MiB3_ON->SetMinimum(0.);
  eff_MiB3_ON->Draw("asp");
  eff_MiB3_OFF->Draw("sp,same");
  eff_ZStack2_ON->Draw("sp,same");
  eff_ZStack1_ON->Draw("sp,same");
  eff_ZStack2_OFF->Draw("sp,same");
  eff_ZStack1_OFF->Draw("sp,same");
  eff_enSEE->Draw("sp,same");
  legC->Draw("same");

  //  c_HVH_50->Print("plots/X0scan/");

}
