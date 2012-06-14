
#include <QColor>
#include <QPalette>
#include <QStyle>
#include <QWidget>
#include <QDebug>

#include <QtTest>


class TestStyleSheet : public QObject
{
  Q_OBJECT

public:
  explicit TestStyleSheet(QObject *parent=0);
  virtual ~TestStyleSheet();

private slots:
  void init();
  void cleanup();
  void setupTestCase();
  void cleanupTestCase();

  void setStyleSheet();
  void setStyleSheet_data();

private:
  QWidget *widget;
};

TestStyleSheet::TestStyleSheet(QObject *parent)
    : QObject(parent), widget(NULL)
{
}

TestStyleSheet::~TestStyleSheet()
{
}

void TestStyleSheet::init()
{
  widget = new QWidget(NULL);
}

void TestStyleSheet::cleanup()
{
  delete widget;
  widget = NULL;
}

void TestStyleSheet::setupTestCase()
{
}

void TestStyleSheet::cleanupTestCase()
{
}

void TestStyleSheet::setStyleSheet()
{
  QFETCH(QString, css);
  QFETCH(QString, result);

  widget->setStyleSheet(css);

  // manual update palette.
  // It was required widget->show() to apply style sheet.
  // But, I didn't want to show widget in test.
  widget->style()->polish(widget);

  QColor color = widget->palette().text().color();

  QVERIFY(color.isValid());
  QCOMPARE(color.name(), result);
}

void TestStyleSheet::setStyleSheet_data()
{
  QTest::addColumn<QString>("css");
  QTest::addColumn<QString>("result");

  QTest::newRow("color name") << "color: red;" << "#ff0000";
  QTest::newRow("rgb values") << "color: rgb(0,255,0);" << "#00ff00";
  QTest::newRow("color code") << "color: #eeeeee;" << "#eeeeee";
}


QTEST_MAIN(TestStyleSheet);

#include "test_StyleSheet.moc"

