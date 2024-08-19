#include <iostream>
#include <vector>
#include <string>
#include <TFile.h>
#include <TCanvas.h>
#include <TLegend.h>
#include <TGraph.h>
#include <TMultiGraph.h>
#include <TH1.h>
#include <TDirectory.h>
#include <TString.h>

void plotROCForNTrees(std::string outputDir, std::vector<int> nTreesValues, std::string algo) {
    TCanvas *c = new TCanvas("c", "ROC Curves for Different NTrees", 800, 600);
    TLegend *legend = new TLegend(0.1, 0.1, 0.3, 0.3);
    TMultiGraph *mg = new TMultiGraph();

    // Loop over each NTrees value and plot the ROC curve
    for (size_t i = 0; i < nTreesValues.size(); ++i) {
        int NTrees = nTreesValues[i];
        TString filename = TString::Format("%s/TMVA_%s_NTrees%d.root", outputDir.c_str(), algo.c_str(), NTrees);
        
        // Open the corresponding ROOT file
        TFile *file = TFile::Open(filename);
        if (!file || file->IsZombie()) {
            std::cerr << "Error: Could not open file " << filename << std::endl;
            continue;
        }

        // Get the ROC curve from the file
        TDirectory *dir = (TDirectory*)file->Get("dataset/Method_BDT/BDT");
        if (!dir) {
            std::cerr << "Error: Could not find BDT directory in file " << filename << std::endl;
            continue;
        }

        TGraph *rocCurve = (TGraph*)dir->Get("MVA_BDT_rejBvsS");
        if (!rocCurve) {
            std::cerr << "Error: Could not find ROC curve in file " << filename << std::endl;
            continue;
        }

        rocCurve->SetLineColor(i + 1);  // Set a different color for each curve
        rocCurve->SetLineWidth(2);

        // Add ROC curve to the multi-graph
        mg->Add(rocCurve);

        // Add entry to legend
        legend->AddEntry(rocCurve, TString::Format("NTrees = %d", NTrees), "l");

        file->Close();
        delete file;
    }

    mg->Draw("AL");
    mg->SetTitle("ROC Curves for Different NTrees");
    mg->GetXaxis()->SetTitle("Signal efficiency");
    mg->GetYaxis()->SetTitle("Background rejection");
    legend->Draw();

    c->SaveAs(TString::Format("%s/ROC_Curves_Different_NTrees.png", outputDir.c_str()));

    delete c;
    delete mg;
    delete legend;
}

int main(int argc, char **argv) {
    if (argc < 3) {
        std::cerr << "Usage: " << argv[0] << " <outputDir> <algo> <NTrees1> <NTrees2> ..." << std::endl;
        return 1;
    }

    std::string outputDir = argv[1];
    std::string algo = argv[2];
    std::vector<int> nTreesValues;

    for (int i = 3; i < argc; ++i) {
        nTreesValues.push_back(atoi(argv[i]));
    }

    // Plot the ROC curves for the given NTrees values
    plotROCForNTrees(outputDir, nTreesValues, algo);

    return 0;
}
