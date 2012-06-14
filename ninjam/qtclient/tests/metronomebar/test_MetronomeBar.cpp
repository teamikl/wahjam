
#include <QObject>
#include <QTimer>
#include <QtTest>
#include "MetronomeBar.h"


class MetronomeCounter: public QObject
{
  Q_OBJECT

public:
  MetronomeCounter(int bpi_, int loop_, QObject *parent=0)
    : QObject(parent), timer(this), bpi(bpi_), loop(loop_), value(0)
  {
    Q_ASSERT(bpi > 0);
    Q_ASSERT(loop > 0);

    connect(&timer, SIGNAL(timeout()), this, SLOT(increment()));
    connect(this, SIGNAL(finished()), &timer, SLOT(stop()));
  }
  virtual ~MetronomeCounter() {};

signals:
  void valueChanged(int value);
  void finished();

private slots:
  void increment() {
    value++;
    if (value > bpi) {
      loop--;
      value = 1;

      if (loop == 0) {
        emit finished();
	return;
      }
    }
    emit valueChanged(value);
  }

public:
  QTimer timer;

private:
  int bpi;
  int loop;
  int value;
};

class TestMetronomeBar: public QObject
{
  Q_OBJECT

public:
  explicit TestMetronomeBar(QObject *parent=0);
  virtual ~TestMetronomeBar();

private slots:
  void initTestCase();
  void cleanupTestCase();
  void init();
  void cleanup();

  void setBeatsPerInterval();
  void setBeatsPerInterval_data();
  void setCurrentBeat();
  void setCurrentBeat_data();
  void viewTest();

private:
  MetronomeBar *metronome;
};

TestMetronomeBar::TestMetronomeBar(QObject *parent)
    : QObject(parent), metronome(NULL)
{
}

TestMetronomeBar::~TestMetronomeBar()
{
  qDebug() << "deleted";
}

void TestMetronomeBar::initTestCase()
{
  metronome = new MetronomeBar(NULL);
  Q_CHECK_PTR(metronome);
}

void TestMetronomeBar::cleanupTestCase()
{
  delete metronome;
  metronome = NULL;
}

void TestMetronomeBar::init()
{
}

void TestMetronomeBar::cleanup()
{
}

void TestMetronomeBar::setBeatsPerInterval()
{
  QFETCH(int, bpi);

  metronome->setBeatsPerInterval(bpi);

  QCOMPARE(metronome->beatsPerInterval(), bpi);
}

void TestMetronomeBar::setBeatsPerInterval_data()
{
  QTest::addColumn<int>("bpi");

  QTest::newRow("default setting") << 8;
  QTest::newRow("standard setting") << 16;
}

void TestMetronomeBar::setCurrentBeat()
{
  QFETCH(int, beat);

  metronome->setCurrentBeat(beat);

  QCOMPARE(metronome->currentBeat(), beat);
}

void TestMetronomeBar::setCurrentBeat_data()
{
  QTest::addColumn<int>("beat");

  QTest::newRow("standard setting") << 16;
}

void TestMetronomeBar::viewTest()
{
#ifndef VIEW_TEST
  QSKIP("skip viewTest", SkipSingle);
#endif

  int bpi = 16;
  int bpm = 120;
  int loop = 3;
  int interval = (1000/(bpm/60));

  MetronomeCounter counter(bpi, loop);

  connect(&counter, SIGNAL(valueChanged(int)),
          metronome, SLOT(setCurrentBeat(int)));
  connect(metronome, SIGNAL(destroyed()),
          &(counter.timer), SLOT(stop()));

  metronome->setAttribute(Qt::WA_DeleteOnClose);
  metronome->setCurrentBeat(1);
  metronome->setBeatsPerInterval(bpi);
  metronome->show();
  counter.timer.start(interval);
  
  // QTest::qWait(interval * (bpi * loop + 1));

  while (counter.timer.isActive())
  {
    QTest::qWait(interval);
  }
}

QTEST_MAIN(TestMetronomeBar)
#include "test_MetronomeBar.moc"

