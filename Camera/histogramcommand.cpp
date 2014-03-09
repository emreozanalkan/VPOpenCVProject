#include "histogramcommand.h"

#include "histogram.h"
#include <QString>

HistogramCommand::HistogramCommand()
{
    _calculateHistogram = true;
    _histogramChannel = 0;
    _title = "Histogram - Channel 1";

    _equalizeHistogram = false;
}

HistogramCommand::HistogramCommand(bool calculateHistogram, int histogramChannel, bool equalizeHistogram)
{
    _calculateHistogram = calculateHistogram;
    _histogramChannel = histogramChannel;

    _title = QString ("Histogram - Channel %1").arg(_histogramChannel + 1).toStdString();

    _equalizeHistogram = equalizeHistogram;
}

void HistogramCommand::Execute()
{

}

void HistogramCommand::Execute(cv::Mat& image)
{
    Histogram1D h;

    if(_calculateHistogram)
    {
        if(image.channels() == 1)
            cv::imshow("Histogram - Channel 1", h.getHistogramImage(image));
        else
        {
            h.setChannel(_histogramChannel);
            cv::imshow(_title, h.getHistogramImage(image));
        }
        return;
    }
    else
    {
        if(image.channels() == 1)
            image = h.equalize(image);
        else
        {
            cv::Mat image_eq;
            image_eq.create(image.size(), CV_8UC3);
            std::vector<cv::Mat> channels, channels_eq;
            cv::split(image, channels);

            for(unsigned long i = 0; i < channels.size(); i++)
            {
                cv::Mat eq;
                cv::equalizeHist(channels[i], eq);
                channels_eq.push_back(eq);
            }

            merge(channels_eq, image_eq);

            image_eq.copyTo(image);
        }
        return;
    }
}
