{

  //  std::string HVE = "HVH_50";
    std::string HVE = "HVH_20";
  //  std::string HVE = "HVL_50";
  //  std::string HVE = "HVL_20";

  TFile* inF_MiB3;
  TFile* inF_enSEE;
  TFile* inF_ZStack2;

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


  TGraphErrors* eff_MiB3 = (TGraphErrors*)inF_MiB3->Get("eff");
  TGraphErrors* eff_enSEE = (TGraphErrors*)inF_enSEE->Get("eff");
  TGraphErrors* eff_ZStack2 = (TGraphErrors*)inF_ZStack2->Get("eff");

  //settings
  eff_MiB3->SetMarkerColor(kGreen+1);
  eff_MiB3->SetLineColor(kGreen+1);
  eff_ZStack2->SetMarkerColor(kBlue);
  eff_ZStack2->SetLineColor(kBlue);
  eff_enSEE->SetMarkerColor(kRed+1);
  eff_enSEE->SetLineColor(kRed+1);
  //
  eff_MiB3->SetMarkerStyle(20);
  eff_MiB3->SetLineWidth(2);
  eff_ZStack2->SetMarkerStyle(20);
  eff_ZStack2->SetLineWidth(2);
  eff_enSEE->SetMarkerStyle(20);
  eff_enSEE->SetLineWidth(2);


  TLegend *legC = new TLegend(0.50,0.20,0.79,0.4,NULL,"brNDC");
  legC->SetTextFont(42);
  legC->SetTextSize(0.03);
  legC->SetFillColor(kWhite);
  legC->SetLineColor(kWhite);
  legC->SetShadowColor(kWhite);

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
  if(HVE == "HVH_50" || "HVL_50")  eff_MiB3->SetTitle("50 GeV");
  if(HVE == "HVH_20" || "HVL_20")  eff_MiB3->SetTitle("20 GeV");
  eff_MiB3->GetXaxis()->SetLabelSize(0.05);
  eff_MiB3->GetXaxis()->SetTitleSize(0.05);
  eff_MiB3->GetYaxis()->SetLabelSize(0.05);
  eff_MiB3->GetYaxis()->SetTitleSize(0.05);
  eff_MiB3->SetMaximum(1.05);
  eff_MiB3->SetMinimum(0.);
  eff_MiB3->Draw("aspl");
  eff_ZStack2->Draw("spl,same");
  eff_enSEE->Draw("spl,same");
  legC->Draw("same");

  //  c_HVH_50->Print("plots/X0scan/");

}
