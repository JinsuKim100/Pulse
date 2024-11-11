#include <TFile.h>
#include <TTree.h>
#include <TCanvas.h>
#include <TH1F.h>
#include <TF1.h>
#include <iostream>
#include <vector>
#include <fstream>
#include <cstring>
#include <ctime>
#include <TRandom3.h> // TRandom3를 사용하기 위해 필요한 헤더 파일

// 필요한 헤더 파일을 포함합니다.
#include "LKChannelSimulator.h"
#include "LKChannelAnalyzer.h"

TRandom3 gRandomGen(0); // 전역 랜덤 객체 생성 (시드 값으로 0 설정)

int GetTbSomehow() {
    return gRandomGen.Uniform(100, 300); // 100에서 300 사이의 랜덤 값
}

int GetAmplitudeSomehow() {
    return gRandomGen.Uniform(1000, 3000); // 1000에서 3000 사이의 랜덤 값
}

void make2_dummy_pulse()
{
    bool drawAna = false;
    int numSimulations = 10000;

    gRandom->SetSeed(time(0));

    ofstream file("out.dat_10000");

    auto sim = new LKChannelSimulator();
    sim->SetPulse("pulseReference.root");
    sim->SetYMax(4096);
    sim->SetTbMax(512);
    sim->SetNumSmoothing(2);
    sim->SetSmoothingLength(2);
    sim->SetPedestalFluctuationLength(5);
    sim->SetPedestalFluctuationScale(0.4);
    sim->SetPulseErrorScale(0.2);
    sim->SetBackGroundLevel(500);

    auto ana = new LKChannelAnalyzer();

    int buffer[512] = {0};
    for (auto iSim = 0; iSim < numSimulations; ++iSim)
    {
        memset(buffer, 0, sizeof(buffer));
        sim->SetFluctuatingPedestal(buffer);
        // timebucket
        auto tbSim = GetTbSomehow();
        auto amplitudeSim = GetAmplitudeSomehow();
        sim->AddHit(buffer, tbSim, amplitudeSim);

        ana->Analyze(buffer);

        auto pedestal = 0;
        auto amplitude = 0;
        auto tb = 0;
        if (ana->GetNumHits() >= 0) {
            pedestal = ana->GetPedestal();
            amplitude = ana->GetAmplitude(0);
            tb = ana->GetTbHit(0);
        }
        file << iSim << ", " << tbSim << ", " << amplitudeSim << ", " << pedestal << ", " << tb << ", " << amplitude << endl;
        for (auto i = 0; i < 512; ++i)
            file << buffer[i] << ", ";
        file << endl;

        if (drawAna) {
            auto cvs = new TCanvas(Form("cvs%d", iSim), "", 800, 600);
            auto lat = new TLatex();
        
            ana->Draw();
            cvs->SaveAs(Form("figures/%s.png", cvs->GetName()));
        }
    }
}
