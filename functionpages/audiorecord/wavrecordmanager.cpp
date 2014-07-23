#include "wavrecordmanager.h"
#include<QDir>
#include<QUrl>
WavRecordManager::WavRecordManager(QWidget *parent) :
    QWidget(parent)
{
    initData();
    initUI();
    initConnect();
}

void WavRecordManager::initData()
{

    audioRecorder = new QAudioRecorder(this);
    probe = new QAudioProbe;
    connect(probe, SIGNAL(audioBufferProbed(QAudioBuffer)),
            this, SLOT(processBuffer(QAudioBuffer)));
    probe->setSource(audioRecorder);

    connect(audioRecorder, SIGNAL(durationChanged(qint64)), this,
            SLOT(updateProgress(qint64)));
    connect(audioRecorder, SIGNAL(statusChanged(QMediaRecorder::Status)), this,
            SLOT(updateStatus(QMediaRecorder::Status)));


}

void WavRecordManager::initUI()
{
    voiceButton = new FStateButton(QString(":/images/skin/images/fvoice.png"),\
                                                 QString(":/images/skin/images/bvoice.png"),\
                                                 0, this);
    voiceButton->setObjectName("Record");
    voiceButton->setDisabled(true);
    voiceLabel = new FShadowLabel;
    voiceLabel->setObjectName("Listening");
    wavRecord = new WavRecordWidget;

    foreach (const QString &containerName, audioRecorder->supportedContainers()) {
        wavRecord->containerComBox->addItem(containerName, QVariant(containerName));
    }

    foreach (int sampleRate, audioRecorder->supportedAudioSampleRates()) {
        wavRecord->samplingRateComBox->addItem(QString::number(sampleRate), QVariant(
                sampleRate));
    }

    QVBoxLayout* voiceLayout = new QVBoxLayout;
    voiceLayout->addWidget(wavRecord);
    voiceLayout->addWidget(voiceButton);
    voiceLayout->addWidget(voiceLabel);
    voiceLayout->addStretch();

    QHBoxLayout* mainLayout = new QHBoxLayout;
    mainLayout->addLayout(voiceLayout);
    mainLayout->addStretch();
    setLayout(mainLayout);
}

void WavRecordManager::initConnect()
{
    connect(voiceButton, SIGNAL(stateChanged()), this, SLOT(changeVoiceLabel()));
    connect(voiceButton, SIGNAL(stateChanged()), this, SLOT(toggleRecord()));
    connect(wavRecord->newWavButton, SIGNAL(clicked()), this, SLOT(setVoiceButtonDisbled()));
}

void WavRecordManager::updateProgress(qint64 duration)
{
    if (audioRecorder->error() != QMediaRecorder::NoError || duration < 2000)
        return;
    wavRecord->slider->setValue(duration / 1000);
}


void WavRecordManager::updateStatus(QMediaRecorder::Status status)
{
    switch (status) {
    case QMediaRecorder::RecordingStatus:
        if (audioLevels.count() != audioRecorder->audioSettings().channelCount()) {
            qDeleteAll(audioLevels);
            audioLevels.clear();
            for (int i = 0; i < audioRecorder->audioSettings().channelCount(); ++i) {
                QAudioLevel *level = new QAudioLevel(this);
                audioLevels.append(level);
                wavRecord->layout()->addWidget(level);
            }
        }
        break;
    case QMediaRecorder::PausedStatus:
        clearAudioLevels();
        break;
    case QMediaRecorder::UnloadedStatus:
    case QMediaRecorder::LoadedStatus:
        clearAudioLevels();
    default:
        break;
    }
}

void WavRecordManager::clearAudioLevels()
{
    for (int i = 0; i < audioLevels.size(); ++i)
        audioLevels.at(i)->setLevel(0);
}

void WavRecordManager::toggleRecord()
{
    if (audioRecorder->state() == QMediaRecorder::StoppedState) {
//        audioRecorder->setAudioInput(boxValue(ui->audioDeviceBox).toString());

        QAudioEncoderSettings settings;
        settings.setCodec("audio/PCM");
        settings.setSampleRate(8000);
        settings.setBitRate(32000);
        settings.setChannelCount(2);
        settings.setQuality(QMultimedia::EncodingQuality(3));
        settings.setEncodingMode(QMultimedia::ConstantQualityEncoding);

        QString container = "audio/x-wav";

        audioRecorder->setEncodingSettings(settings, QVideoEncoderSettings(), container);


        QString fileName = QString("%1/test.wav").arg(QDir::currentPath());
        audioRecorder->setOutputLocation(QUrl::fromLocalFile(fileName));
        qDebug(qPrintable(fileName));

        audioRecorder->record();
    }
    else {
        audioRecorder->stop();
    }
}

void WavRecordManager::changeVoiceLabel()
{
    if (voiceButton->getState() == 1)
    {
        voiceButton->setToolTip(tr("Recording on "));
        voiceLabel->setText("Listening...");
        wavRecord->setEnabled(false);
        wavRecord->newWavButton->setEnabled(false);
    }
    else if (voiceButton->getState() == 0)
    {
        voiceButton->setToolTip(tr("Recording off"));
        voiceLabel->setText("");
        wavRecord->setEnabled(true);
        wavRecord->newWavButton->setEnabled(true);
    }
}

void WavRecordManager::setVoiceButtonDisbled()
{
    wavRecord->newWavButton->setEnabled(false);
    voiceButton->setEnabled(true);
}


void WavRecordManager::processBuffer(const QAudioBuffer& buffer)
{
    QVector<qreal> levels = getBufferLevels(buffer);
    for (int i = 0; i < levels.count(); ++i)
        audioLevels.at(i)->setLevel(levels.at(i));
}
