#ifndef HISTOGRAMCOMMAND_H
#define HISTOGRAMCOMMAND_H

#include "ipcommand.h"
#include <string>

class HistogramCommand : public IPCommand
{
private:
    bool _calculateHistogram;
    int _histogramChannel;

    bool _equalizeHistogram;

    std::string _title;
public:
    HistogramCommand();
    HistogramCommand(bool, int, bool);

    void Execute();
    void Execute(cv::Mat&);
};

#endif // HISTOGRAMCOMMAND_H
