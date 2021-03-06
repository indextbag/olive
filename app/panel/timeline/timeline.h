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

#ifndef TIMELINE_PANEL_H
#define TIMELINE_PANEL_H

#include "widget/panel/panel.h"

#include "widget/timelinewidget/timelinewidget.h"

/**
 * @brief Panel container for a TimelineWidget
 */
class TimelinePanel : public PanelWidget
{
  Q_OBJECT
public:
  TimelinePanel(QWidget* parent);

  void Clear();

  void ConnectTimelineNode(TimelineOutput* node);

  void DisconnectTimelineNode();

  void SplitAtPlayhead();

  virtual void ZoomIn() override;

  virtual void ZoomOut() override;

  virtual void SelectAll() override;

  virtual void DeselectAll() override;

  virtual void RippleToIn() override;

  virtual void RippleToOut() override;

  virtual void EditToIn() override;

  virtual void EditToOut() override;

  virtual void GoToPrevCut() override;

  virtual void GoToNextCut() override;

public slots:
  void SetTimebase(const rational& timebase);

  void SetTime(const int64_t& timestamp);

protected:
  virtual void changeEvent(QEvent* e) override;

signals:
  void TimeChanged(const int64_t& time);

private:
  void Retranslate();

  TimelineWidget* timeline_widget_;

};

#endif // TIMELINE_PANEL_H
