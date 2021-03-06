#ifndef COLORPROCESSOR_H
#define COLORPROCESSOR_H

#include <OpenColorIO/OpenColorIO.h>
namespace OCIO = OCIO_NAMESPACE::v1;

#include "decoder/frame.h"

class ColorProcessor;
using ColorProcessorPtr = std::shared_ptr<ColorProcessor>;

class ColorProcessor
{
public:
  ColorProcessor(const QString &source_space, const QString &dest_space);

  ColorProcessor(const QString& source_space,
                 QString display,
                 QString view,
                 const QString& look);

  static ColorProcessorPtr Create(const QString& source_space, const QString& dest_space);

  static ColorProcessorPtr Create(const QString& source_space,
                                  const QString& display,
                                  const QString& view,
                                  const QString& look);

  OCIO::ConstProcessorRcPtr GetProcessor();

  void ConvertFrame(FramePtr f);

private:
  OCIO::ConstProcessorRcPtr processor;

};

#endif // COLORPROCESSOR_H
