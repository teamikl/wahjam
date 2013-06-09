/*
    Copyright (C) 2012 Stefan Hajnoczi <stefanha@gmail.com>

    Wahjam is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    Wahjam is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Wahjam; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#include <math.h>
#include <portaudio.h>
#include <QFormLayout>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include "PortAudioConfigDialog.h"

PortAudioConfigDialog::PortAudioConfigDialog(QWidget *parent)
  : QDialog(parent), validateSettingsEntryCount(0)
{
  inputDeviceList = new QComboBox;
  inputDeviceList->setEditable(false);
  connect(inputDeviceList, SIGNAL(currentIndexChanged(int)),
          this, SLOT(deviceIndexChanged(int)));

  unmuteLocalChannelsBox = new QCheckBox(tr("Play back my audio"));
  unmuteLocalChannelsBox->setToolTip(tr("Disable if you play an acoustic instrument"));

  outputDeviceList = new QComboBox;
  outputDeviceList->setEditable(false);
  connect(outputDeviceList, SIGNAL(currentIndexChanged(int)),
          this, SLOT(deviceIndexChanged(int)));

  sampleRateList = new QComboBox;
  sampleRateList->setEditable(false);
  sampleRateList->addItems(QStringList() << "32000" << "44100" << "48000" << "88200" << "96000");
  connect(sampleRateList, SIGNAL(currentIndexChanged(int)),
          this, SLOT(sampleRateIndexChanged(int)));

  latencyList = new QComboBox();
  connect(latencyList, SIGNAL(currentIndexChanged(int)),
          this, SLOT(latencyIndexChanged(int)));

  hostAPIList = new QComboBox;
  hostAPIList->setEditable(false);
  connect(hostAPIList, SIGNAL(currentIndexChanged(int)),
          this, SLOT(hostAPIIndexChanged(int)));

  applyButton = new QPushButton(tr("&Apply"));
  connect(applyButton, SIGNAL(clicked()), this, SLOT(accept()));

  QPushButton *cancelButton = new QPushButton(tr("&Cancel"));
  connect(cancelButton, SIGNAL(clicked()), this, SLOT(reject()));

  QVBoxLayout *vlayout = new QVBoxLayout;
  QFormLayout *formLayout = new QFormLayout;
  formLayout->setSpacing(5);
  formLayout->setContentsMargins(2, 2, 2, 2);
  formLayout->addRow(tr("&Input device:"), inputDeviceList);
  formLayout->addRow(new QLabel, unmuteLocalChannelsBox);
  formLayout->addRow(tr("&Output device:"), outputDeviceList);
  formLayout->addRow(new QLabel); /* just a spacer */
  formLayout->addRow(tr("Sample &rate (Hz):"), sampleRateList);
  formLayout->addRow(tr("&Latency (ms):"), latencyList);
  formLayout->addRow(new QLabel); /* just a spacer */
  formLayout->addRow(new QLabel(tr("<b>Troubleshooting:</b> If you experience audio problems, try selecting another audio system.")));
  formLayout->addRow(tr("Audio &system:"), hostAPIList);
  vlayout->addLayout(formLayout);
  QHBoxLayout *hlayout = new QHBoxLayout;
  hlayout->setSpacing(0);
  hlayout->setContentsMargins(0, 0, 0, 0);
  hlayout->addWidget(applyButton);
  hlayout->addWidget(cancelButton);
  vlayout->setSpacing(2);
  vlayout->setContentsMargins(5, 5, 5, 5);
  vlayout->addLayout(hlayout);
  setLayout(vlayout);
  setWindowTitle(tr("Configure audio devices..."));

  populateHostAPIList();
}

void PortAudioConfigDialog::populateHostAPIList()
{
  PaHostApiIndex i;
  for (i = 0; i < Pa_GetHostApiCount(); i++) {
    const PaHostApiInfo *hostAPIInfo = Pa_GetHostApiInfo(i);
    if (hostAPIInfo) {
      QString name = QString::fromLocal8Bit(hostAPIInfo->name);
      hostAPIList->addItem(name, i);
    }
  }
}

void PortAudioConfigDialog::willValidateSettings()
{
  validateSettingsEntryCount++;
}

void PortAudioConfigDialog::validateSettings()
{
  /* Avoid repeating validation because Pa_IsFormatSupported() can be
   * slow and we should not block the GUI thread for too long.
   */
  if (--validateSettingsEntryCount > 1) {
    return;
  }

  double sampleRate = sampleRateList->currentText().toDouble();
  double latency = latencyList->currentText().toDouble() / 1000;

  if (sampleRate == 0 ||
      latency == 0 ||
      inputDeviceList->currentIndex() < 0 ||
      outputDeviceList->currentIndex() < 0) {
    applyButton->setEnabled(false);
    return;
  }

  PaStreamParameters inputParams;
  inputParams.device = inputDeviceList->itemData(inputDeviceList->currentIndex()).toInt(NULL);
  inputParams.channelCount = 1 /* TODO mono */;
  inputParams.sampleFormat = paFloat32 | paNonInterleaved;
  inputParams.suggestedLatency = latency;
  inputParams.hostApiSpecificStreamInfo = NULL;

  PaStreamParameters outputParams = inputParams;
  outputParams.device = outputDeviceList->itemData(outputDeviceList->currentIndex()).toInt(NULL);
  outputParams.channelCount = 1 /* TODO mono */;

  PaError error = Pa_IsFormatSupported(&inputParams, &outputParams, sampleRate);
  applyButton->setEnabled(error == paFormatIsSupported);
}

/* Return PaDeviceInfo* or NULL if not found */
static const PaDeviceInfo *lookupDeviceInfo(QComboBox *deviceList)
{
  int index = deviceList->currentIndex();
  if (index < 0) {
    return NULL;
  }
  PaDeviceIndex deviceIndex = deviceList->itemData(index).toInt(NULL);
  return Pa_GetDeviceInfo(deviceIndex);
}

void PortAudioConfigDialog::autoselectSampleRate()
{
  const PaDeviceInfo *inputDeviceInfo = lookupDeviceInfo(inputDeviceList);
  if (!inputDeviceInfo) {
    return;
  }

  const PaDeviceInfo *outputDeviceInfo = lookupDeviceInfo(outputDeviceList);
  if (!outputDeviceInfo) {
    return;
  }

  double sampleRate = qMin(inputDeviceInfo->defaultSampleRate,
                           outputDeviceInfo->defaultSampleRate);
  int index = sampleRateList->findText(QString::number(sampleRate));
  if (index != -1) {
    sampleRateList->setCurrentIndex(index);
  }
}

void PortAudioConfigDialog::autoselectLatency()
{
  const PaDeviceInfo *inputDeviceInfo = lookupDeviceInfo(inputDeviceList);
  if (!inputDeviceInfo) {
    return;
  }

  const PaDeviceInfo *outputDeviceInfo = lookupDeviceInfo(outputDeviceList);
  if (!outputDeviceInfo) {
    return;
  }

  double latency = qMax(inputDeviceInfo->defaultLowInputLatency,
                        outputDeviceInfo->defaultLowOutputLatency) * 1000;
  int i;
  for (i = 0; i < latencyList->count(); i++) {
    if (latency < latencyList->itemText(i).toDouble() - .01 /* epsilon */) {
      if (i > 0) {
        i--;
      }
      latencyList->setCurrentIndex(i);
      return;
    }
  }
}

void PortAudioConfigDialog::setupLatencyList()
{
  latencyList->clear();

  /* Enumerate latencies for power-of-2 buffers up to 4096 frames.  Start
   * at 1 millisecond since lower latencies are unlikely to produce
   * glitch-free audio.
   */
  double sampleRate = sampleRateList->currentText().toDouble();
  if (sampleRate <= 0) {
    return;
  }
  int framesPerMillisecond = 1 << (int)ceil(log2(sampleRate / 1000));
  for (int i = framesPerMillisecond; i < 4096; i *= 2) {
    latencyList->addItem(QString::number(i / sampleRate * 1000, 'g', 3));
  }
}

void PortAudioConfigDialog::deviceIndexChanged(int index)
{
  willValidateSettings();
  autoselectSampleRate();
  validateSettings();
}

void PortAudioConfigDialog::sampleRateIndexChanged(int index)
{
  willValidateSettings();
  setupLatencyList();
  autoselectLatency();
  validateSettings();
}

void PortAudioConfigDialog::latencyIndexChanged(int index)
{
  willValidateSettings();
  validateSettings();
}

void PortAudioConfigDialog::hostAPIIndexChanged(int index)
{
  inputDeviceList->clear();
  outputDeviceList->clear();

  if (index < 0) {
    return;
  }

  PaHostApiIndex apiIndex = hostAPIList->itemData(index).toInt(NULL);
  const PaHostApiInfo *hostAPIInfo = Pa_GetHostApiInfo(apiIndex);
  if (!hostAPIInfo) {
    return;
  }

  int i;
  for (i = 0; i < hostAPIInfo->deviceCount; i++) {
    PaDeviceIndex devIndex = Pa_HostApiDeviceIndexToDeviceIndex(apiIndex, i);
    const PaDeviceInfo *devInfo = Pa_GetDeviceInfo(devIndex);
    QString name = QString::fromLocal8Bit(devInfo->name);

    if (devInfo->maxInputChannels > 0) {
      inputDeviceList->addItem(name, devIndex);
    }
    if (devInfo->maxOutputChannels > 0) {
      outputDeviceList->addItem(name, devIndex);
    }
  }
}

QString PortAudioConfigDialog::hostAPI() const
{
  return hostAPIList->currentText();
}

void PortAudioConfigDialog::setHostAPI(const QString &name)
{
  int i = hostAPIList->findText(name);
  if (i != -1) {
    hostAPIList->setCurrentIndex(i);
    return;
  }

  /* Pick default host API based on this list of priorities */
  const int hostAPIPriority[] = {
    0, /* paInDevelopment */
    1, /* paDirectSound */
    0, /* paMME */
    4, /* paASIO */
    0, /* paSoundManager */
    1, /* paCoreAudio */
    0, /* <empty> */
    0, /* paOSS */
    1, /* paALSA */
    0, /* paAL */
    0, /* paBeOS */
    3, /* paWDMKS */
    2, /* paJACK */
    2, /* paWASAPI */
    0, /* paAudioScienceHPI */
  };
  const PaHostApiTypeId numTypes =
    (PaHostApiTypeId)(sizeof(hostAPIPriority) / sizeof(hostAPIPriority[0]));
  int pri = -1;

  for (int j = 0; j < hostAPIList->count(); j++) {
    PaHostApiIndex apiIndex = hostAPIList->itemData(j).toInt(NULL);
    const PaHostApiInfo *hostAPIInfo = Pa_GetHostApiInfo(apiIndex);
    if (!hostAPIInfo || hostAPIInfo->type >= numTypes) {
      continue;
    }
    if (hostAPIPriority[hostAPIInfo->type] > pri) {
      pri = hostAPIPriority[hostAPIInfo->type];
      i = j;
    }
  }
  if (i != -1) {
    hostAPIList->setCurrentIndex(i);
  }
}

QString PortAudioConfigDialog::inputDevice() const
{
  return inputDeviceList->currentText();
}

void PortAudioConfigDialog::setInputDevice(const QString &name)
{
  int i = inputDeviceList->findText(name);
  if (i >= 0) {
    inputDeviceList->setCurrentIndex(i);
  }
}

bool PortAudioConfigDialog::unmuteLocalChannels() const
{
  return unmuteLocalChannelsBox->isChecked();
}

void PortAudioConfigDialog::setUnmuteLocalChannels(bool unmute)
{
  unmuteLocalChannelsBox->setChecked(unmute);
}

QString PortAudioConfigDialog::outputDevice() const
{
  return outputDeviceList->currentText();
}

void PortAudioConfigDialog::setOutputDevice(const QString &name)
{
  int i = outputDeviceList->findText(name);
  if (i >= 0) {
    outputDeviceList->setCurrentIndex(i);
  }
}

double PortAudioConfigDialog::sampleRate() const
{
  return sampleRateList->currentText().toDouble();
}

void PortAudioConfigDialog::setSampleRate(double sampleRate)
{
  int i = sampleRateList->findText(QString::number(sampleRate));
  if (i >= 0) {
    sampleRateList->setCurrentIndex(i);
  }
}

double PortAudioConfigDialog::latency() const
{
  return latencyList->currentText().toDouble() / 1000;
}

void PortAudioConfigDialog::setLatency(double latency)
{
  int i = latencyList->findText(QString::number(latency * 1000, 'g', 3));
  if (i >= 0) {
    latencyList->setCurrentIndex(i);
  }
}
