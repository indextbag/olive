/***

  Olive - Non-Linear Video Editor
  Copyright (C) 2019 Olive Team

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.

***/

#include "sequence.h"

#include <QDialogButtonBox>
#include <QGroupBox>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QVBoxLayout>

#include "common/channellayout.h"
#include "common/rational.h"
#include "undo/undostack.h"

SequenceDialog::SequenceDialog(Sequence* s, Type t, QWidget* parent) :
  QDialog(parent),
  sequence_(s),
  make_undoable_(true)
{
  QVBoxLayout* layout = new QVBoxLayout(this);

  // Set up preset section
  QHBoxLayout* preset_layout = new QHBoxLayout();
  preset_layout->addWidget(new QLabel(tr("Preset:")));
  QComboBox* preset_combobox = new QComboBox();
  preset_layout->addWidget(preset_combobox);
  layout->addLayout(preset_layout);

  // Set up video section
  QGroupBox* video_group = new QGroupBox();
  video_group->setTitle(tr("Video"));
  QGridLayout* video_layout = new QGridLayout(video_group);
  video_layout->addWidget(new QLabel(tr("Width:")), 0, 0);
  video_width_field_ = new QSpinBox();
  video_width_field_->setMaximum(99999);
  video_layout->addWidget(video_width_field_, 0, 1);
  video_layout->addWidget(new QLabel(tr("Height:")), 1, 0);
  video_height_field_ = new QSpinBox();
  video_height_field_->setMaximum(99999);
  video_layout->addWidget(video_height_field_, 1, 1);
  video_layout->addWidget(new QLabel(tr("Frame Rate:")), 2, 0);
  video_frame_rate_field_ = new QComboBox();
  video_layout->addWidget(video_frame_rate_field_, 2, 1);
  layout->addWidget(video_group);

  // Set up audio section
  QGroupBox* audio_group = new QGroupBox();
  audio_group->setTitle(tr("Audio"));
  QGridLayout* audio_layout = new QGridLayout(audio_group);
  audio_layout->addWidget(new QLabel(tr("Sample Rate:")), 0, 0);
  audio_sample_rate_field_ = new QComboBox();
  // FIXME: No sample rate made
  audio_layout->addWidget(audio_sample_rate_field_, 0, 1);
  audio_layout->addWidget(new QLabel(tr("Channels:")), 1, 0);
  audio_channels_field_ = new QComboBox();
  // FIXME: No channels made
  audio_layout->addWidget(audio_channels_field_, 1, 1);
  layout->addWidget(audio_group);

  // Set up name section
  QHBoxLayout* name_layout = new QHBoxLayout();
  name_layout->addWidget(new QLabel(tr("Name:")));
  name_field_ = new QLineEdit();
  name_layout->addWidget(name_field_);
  layout->addLayout(name_layout);

  // Set up dialog buttons
  QDialogButtonBox* buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
  buttons->setCenterButtons(true);
  connect(buttons, SIGNAL(accepted()), this, SLOT(accept()));
  connect(buttons, SIGNAL(rejected()), this, SLOT(reject()));
  layout->addWidget(buttons);

  // Set window title based on type
  switch (t) {
  case kNew:
    setWindowTitle("New Sequence");
    break;
  case kExisting:
    setWindowTitle(tr("Editing \"%1\"").arg(sequence_->name()));
    break;
  }

  // Set up available frame rates
  AddFrameRate(rational(10, 1));            // 10 FPS
  AddFrameRate(rational(15, 1));            // 15 FPS
  AddFrameRate(rational(24000, 1001));      // 23.976 FPS
  AddFrameRate(rational(24, 1));            // 24 FPS
  AddFrameRate(rational(25, 1));            // 25 FPS
  AddFrameRate(rational(30000, 1001));      // 29.97 FPS
  AddFrameRate(rational(30, 1));            // 30 FPS
  AddFrameRate(rational(48000, 1001));      // 47.952 FPS
  AddFrameRate(rational(48, 1));            // 48 FPS
  AddFrameRate(rational(50, 1));            // 50 FPS
  AddFrameRate(rational(60000, 1001));      // 59.94 FPS
  AddFrameRate(rational(60, 1));            // 60 FPS

  // Set up available sample rates
  AddSampleRate(8000);         // 8000 Hz
  AddSampleRate(11025);        // 11025 Hz
  AddSampleRate(16000);        // 16000 Hz
  AddSampleRate(22050);        // 22050 Hz
  AddSampleRate(24000);        // 24000 Hz
  AddSampleRate(32000);        // 32000 Hz
  AddSampleRate(44100);        // 44100 Hz
  AddSampleRate(48000);        // 48000 Hz
  AddSampleRate(88200);        // 88200 Hz
  AddSampleRate(96000);        // 96000 Hz

  // Set up available channel layouts
  AddChannelLayout(AV_CH_LAYOUT_MONO);
  AddChannelLayout(AV_CH_LAYOUT_STEREO);
  AddChannelLayout(AV_CH_LAYOUT_5POINT1);
  AddChannelLayout(AV_CH_LAYOUT_7POINT1);

  // Set values based on input sequence
  video_width_field_->setValue(sequence_->video_params().width());
  video_height_field_->setValue(sequence_->video_params().height());

  int frame_rate_index = frame_rate_list_.indexOf(sequence_->video_params().time_base().flipped());
  video_frame_rate_field_->setCurrentIndex(frame_rate_index);

  int sample_rate_index = sample_rate_list_.indexOf(sequence_->audio_params().sample_rate());
  audio_sample_rate_field_->setCurrentIndex(sample_rate_index);

  for (int i=0;i<audio_channels_field_->count();i++) {
    if (audio_channels_field_->itemData(i).toULongLong() == sequence_->audio_params().channel_layout()) {
      audio_channels_field_->setCurrentIndex(i);
      break;
    }
  }

  name_field_->setText(sequence_->name());
}

void SequenceDialog::SetUndoable(bool u)
{
  make_undoable_ = u;
}

void SequenceDialog::accept()
{
  if (name_field_->text().isEmpty()) {
    QMessageBox::critical(this, tr("Error editing Sequence"), tr("Please enter a name for this Sequence."));
    return;
  }

  // Get the rational at the combobox's index (which will be correct provided AddFrameRate() was used at all time)
  rational video_time_base = frame_rate_list_.at(video_frame_rate_field_->currentIndex()).flipped();


  // Get the rational at the combobox's index (which will be correct provided AddFrameRate() was used at all time)
  int audio_sample_rate = sample_rate_list_.at(audio_sample_rate_field_->currentIndex());

  // Get the audio channel layout value
  uint64_t channels = audio_channels_field_->currentData().toULongLong();

  // Generate video and audio parameter structs from data
  VideoParams video_params = VideoParams(video_width_field_->value(),
                                         video_height_field_->value(),
                                         video_time_base);

  AudioParams audio_params = AudioParams(audio_sample_rate,
                                         channels);

  if (make_undoable_) {

    // Make undoable command to change the parameters
    SequenceParamCommand* param_command = new SequenceParamCommand(sequence_,
                                                                   video_params,
                                                                   audio_params,
                                                                   name_field_->text());

    olive::undo_stack.push(param_command);

  } else {
    // Set sequence values directly with no undo command
    sequence_->set_video_params(video_params);
    sequence_->set_audio_params(audio_params);
    sequence_->set_name(name_field_->text());
  }

  QDialog::accept();
}

void SequenceDialog::AddFrameRate(const rational &r)
{
  frame_rate_list_.append(r);

  video_frame_rate_field_->addItem(tr("%1 FPS").arg(r.toDouble()));
}

void SequenceDialog::AddSampleRate(const int &rate)
{
  sample_rate_list_.append(rate);

  audio_sample_rate_field_->addItem(tr("%1 Hz").arg(rate));
}

void SequenceDialog::AddChannelLayout(int layout)
{
  QString layout_name;

  switch (layout) {
  case AV_CH_LAYOUT_MONO:
    layout_name = tr("Mono");
    break;
  case AV_CH_LAYOUT_STEREO:
    layout_name = tr("Stereo");
    break;
  case AV_CH_LAYOUT_5POINT1:
    layout_name = tr("5.1");
    break;
  case AV_CH_LAYOUT_7POINT1:
    layout_name = tr("7.1");
    break;
  default:
    layout_name = tr("Unknown (0x%1)").arg(layout, 1, 16);
  }

  audio_channels_field_->addItem(layout_name, layout);
}

SequenceDialog::SequenceParamCommand::SequenceParamCommand(Sequence* s,
                                                           const VideoParams& video_params,
                                                           const AudioParams& audio_params,
                                                           const QString& name,
                                                           QUndoCommand* parent) :
  QUndoCommand(parent),
  sequence_(s),
  new_video_params_(video_params),
  new_audio_params_(audio_params),
  new_name_(name),
  old_video_params_(s->video_params()),
  old_audio_params_(s->audio_params()),
  old_name_(s->name())
{
}

void SequenceDialog::SequenceParamCommand::redo()
{
  sequence_->set_video_params(new_video_params_);
  sequence_->set_audio_params(new_audio_params_);
  sequence_->set_name(new_name_);
}

void SequenceDialog::SequenceParamCommand::undo()
{
  sequence_->set_video_params(old_video_params_);
  sequence_->set_audio_params(old_audio_params_);
  sequence_->set_name(old_name_);
}
