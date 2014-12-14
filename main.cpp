#include <iostream>
#include <complex>

#include "lib/ooura/fft4g.c"

#include "aquila/transform/FftFactory.h"
#include "aquila/source/WaveFile.h"
#include "aquila/source/FramesCollection.h"

#include <gsl/gsl_vector.h>
#include <gsl/gsl_statistics.h>

#include "anyoption.h"

#include <iomanip>

using namespace std;
std::vector<double> calcAmp(Aquila::FramesCollection frames)
{
    //std::cout << "calc amp" << endl;
    vector<double> ret;
    auto fft = Aquila::FftFactory::getFft(frames.getSamplesPerFrame());
    for (auto it = frames.begin(); it != frames.end(); ++it)
    {
        vector<Aquila::ComplexType> s = fft->fft(it->toArray());
        double sum = 0.0;
        for (auto it2 = s.begin(); it2 != s.end(); ++it2) {
            sum += abs(*it2);
        }
        ret.push_back(sum);
    }
    return ret;
}

inline bool exists (const std::string& name) {
    if (FILE *file = fopen(name.c_str(), "r")) {
        fclose(file);
        return true;
    } else {
        return false;
    }   
}
int main(int argc, char *argv[])
{
    AnyOption *opt = new AnyOption();
    opt->addUsage("fft");
    opt->addUsage("Usage: ");
    opt->addUsage("");
    opt->addUsage(" -h  --help");
    opt->addUsage(" --file       The main wav file");
    opt->addUsage("");

    opt->setFlag("help", 'h');
    opt->setOption("file", 'f');
    opt->processCommandArgs(argc, argv);
    if(opt->getValue("help") != NULL || opt->getValue("file") == NULL) {
        opt->printUsage();
        return 0;
    }

    const unsigned int packetSize = 1024*4;
    if(!exists(opt->getValue("file"))) {
        cout << "file not found" << endl;
        return 1;
    }
    Aquila::WaveFile small(opt->getValue("file"));
    Aquila::FramesCollection s;
    s.divideFrames(small, packetSize, 0);
    std::vector<double> small_amp = calcAmp(s);
    gsl_vector_const_view gsl_x = gsl_vector_const_view_array( &small_amp[0], small_amp.size() );
               
    double g_max = 0.0, g_secs = 0.0;
    int id = 0;
    for(int file = 0; file < 10; file++) {
        char numstr[21]; // enough to hold all numbers up to 64-bits
        sprintf(numstr, "%d", file);
        string fileName(opt->getValue("file"));
        string fileName2 = fileName + numstr + ".wav";
        if(!exists(fileName2))
            break;
        
        //cout << "reading file" << fileName2 << endl;
        Aquila::WaveFile big(fileName2);
        Aquila::FramesCollection b;
        b.divideFrames(big, packetSize, 0);
        std::vector<double> big_amp = calcAmp(b);

        std::vector<double> res;

        int sizeDiff = b.count() - s.count();

        for(int i = 0; i < sizeDiff - 1; i++) {
            gsl_vector_const_view gsl_y = gsl_vector_const_view_array( &big_amp[i], small_amp.size() );
            double pearson = gsl_stats_correlation( (double*) gsl_x.vector.data, sizeof(double),
                                                (double*) gsl_y.vector.data, sizeof(double),
                                                200 );
            res.push_back(pearson);
        }

        vector<double>::iterator pos = std::max_element(res.begin(), res.end());
        if(pos == res.end()) {
            continue;
        }
        //cout << "file " << file << "max " << g_max << "g_secs " << g_secs << endl;
        double pp = *pos;
        if(pp > g_max) {
            int start_index = std::distance(res.begin(), pos);
            double bc = b.count();
            double lc = start_index;
            double len = big.getAudioLength();
            //cout << "secs: " << (len/1000) * (lc/bc);

            g_max = pp;
            g_secs = (len/1000) * (lc/bc);
            id = file;
            
        }
    }
    double lc2 = small.getAudioLength();
    
    cout << setiosflags(ios::fixed) << setprecision(0) << g_secs << ";" << (lc2/1000) << ";" << id;
    return 0;
}

