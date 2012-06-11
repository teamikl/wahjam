
#include <QtTest>
#include "ChatOutput.h"

class test_ChatOutput : public QObject
{
  Q_OBJECT

public:
  test_ChatOutput();
  virtual ~test_ChatOutput();

public slots:
  void initTestCase();
  void cleanupTestCase();
  void init();
  void cleanup();

private slots:
  void verify_options();
  void add_message_data();
  void add_message();
  void url_link();
  void vote_link();

private:
  ChatOutput *chatOutput;
};

test_ChatOutput::test_ChatOutput()
  : chatOutput(NULL)
{
}

test_ChatOutput::~test_ChatOutput()
{
}

void test_ChatOutput::initTestCase()
{
  chatOutput = new ChatOutput(NULL);
  Q_CHECK_PTR(chatOutput);
}

void test_ChatOutput::cleanupTestCase()
{
  delete chatOutput;
  chatOutput = NULL;
}

void test_ChatOutput::init()
{
  Q_ASSERT(chatOutput);
}

void test_ChatOutput::cleanup()
{
  chatOutput->clear();
}

void test_ChatOutput::verify_options()
{
  QVERIFY(chatOutput->isReadOnly());
  QVERIFY(!chatOutput->isUndoRedoEnabled());
  QVERIFY(!chatOutput->tabChangesFocus());
  QVERIFY(!chatOutput->isBackwardAvailable());
  QVERIFY(!chatOutput->isForwardAvailable());
  QVERIFY(!chatOutput->openExternalLinks()); // Failed in this repo
  QVERIFY(!chatOutput->openLinks());
}

void test_ChatOutput::add_message_data()
{
  QTest::addColumn<QString>("src");
  QTest::addColumn<QString>("message");
  QTest::addColumn<QString>("expected");

  QTest::newRow("test message") << "test" << "message" << "<test> message";
  QTest::newRow("invalid name") << "<>" << "message" << "<<>> message";
  QTest::newRow("spaces name") << "    " << "message" << "<    > message";
  QTest::newRow("blank name") << "" << "message" << "<> message";

  // Who escape those letter in username ?
  // Currently, ChatOutput class does not care that.
}

void test_ChatOutput::add_message()
{
  QFETCH(QString, src);
  QFETCH(QString, message);
  QFETCH(QString, expected);

  chatOutput->addMessage(src, message);
  QCOMPARE(chatOutput->toPlainText(), expected);
}

void test_ChatOutput::url_link()
{
  chatOutput->addLine("", "http://example.com/");
  QTextCharFormat format = chatOutput->textCursor().charFormat();

  QCOMPARE(chatOutput->toPlainText(), QString("http://example.com/"));

  QVERIFY(format.isValid());
  QVERIFY(format.isAnchor());
  QCOMPARE(format.anchorHref(), QString("http://example.com/"));
}

void test_ChatOutput::vote_link()
{
  // NOTE: MainWindow class has +1 vote link logic
  // that should not be re-implemented in this test.
}

QTEST_MAIN(test_ChatOutput)
#include "test_ChatOutput.moc"

